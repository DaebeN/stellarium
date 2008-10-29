/*
 * Stellarium
 * Copyright (C) 2003 Fabien Chereau
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "StelApp.hpp"
#include "Navigator.hpp"
#include "StelUtils.hpp"
#include "SolarSystem.hpp"
#include "Observer.hpp"
#include "Planet.hpp"
#include "StelObjectMgr.hpp"
#include "StelCore.hpp"
#include "LocationMgr.hpp"
#include "StelModuleMgr.hpp"
#include "MovementMgr.hpp"

#include <QSettings>
#include <QStringList>
#include <QDateTime>
#include <QDebug>

////////////////////////////////////////////////////////////////////////////////
Navigator::Navigator() : timeSpeed(JD_SECOND), JDay(0.), position(NULL)
{
	localVision=Vec3d(1.,0.,0.);
	equVision=Vec3d(1.,0.,0.);
	J2000EquVision=Vec3d(1.,0.,0.);  // not correct yet...
	viewingMode = ViewHorizon;  // default
}

Navigator::~Navigator()
{
	delete position;
	position=NULL;
}

const Planet *Navigator::getHomePlanet(void) const
{
	return position->getHomePlanet();
}

void Navigator::init()
{
	QSettings* conf = StelApp::getInstance().getSettings();
	Q_ASSERT(conf);

	defaultLocationID = conf->value("init_location/location","Paris, Paris, France").toString();
	position = new Observer(StelApp::getInstance().getLocationMgr().locationForSmallString(defaultLocationID));
	
	setTimeNow();
	setLocalVision(Vec3f(1,1e-05,0.2));
	// Compute transform matrices between coordinates systems
	updateTransformMatrices();
	updateModelViewMat();
	QString tmpstr = conf->value("navigation/viewing_mode", "horizon").toString();
	if (tmpstr=="equator")
		setViewingMode(Navigator::ViewEquator);
	else
	{
		if (tmpstr=="horizon")
			setViewingMode(Navigator::ViewHorizon);
		else
		{
			qDebug() << "ERROR : Unknown viewing mode type : " << tmpstr;
			assert(0);
		}
	}
	
	initViewPos = StelUtils::strToVec3f(conf->value("navigation/init_view_pos").toString());
	setLocalVision(initViewPos);
	
	// we want to be able to handle the old style preset time, recorded as a double
	// jday, or as a more human readable string...
	bool ok;
	QString presetTimeStr = conf->value("navigation/preset_sky_time",2451545.).toString();
	presetSkyTime = presetTimeStr.toDouble(&ok);
	if (ok)
		qDebug() << "navigation/preset_sky_time is a double - treating as jday:" << presetSkyTime;
	else
	{
		qDebug() << "navigation/preset_sky_time was not a double, treating as string date:" << presetTimeStr;
		presetSkyTime = StelUtils::qDateTimeToJd(QDateTime::fromString(presetTimeStr));
	}

	// Navigation section
	setInitTodayTime(QTime::fromString(conf->value("navigation/today_time", "22:00").toString()));

	startupTimeMode = conf->value("navigation/startup_time_mode", "actual").toString().toLower();
	if (startupTimeMode=="preset")
		setJDay(presetSkyTime - StelUtils::getGMTShiftFromQT(presetSkyTime) * JD_HOUR);
	else if (startupTimeMode=="today")
		setTodayTime(getInitTodayTime());

	// we previously set the time to "now" already, so we don't need to 
	// explicitly do it if the startupTimeMode=="now".
}

// Set the location to use by default at startup
void Navigator::setDefaultLocationID(const QString& id)
{
	defaultLocationID = id;
	StelApp::getInstance().getLocationMgr().locationForSmallString(id);
	QSettings* conf = StelApp::getInstance().getSettings();
	Q_ASSERT(conf);
	conf->setValue("init_location/location", id);
}
	
//! Set stellarium time to current real world time
void Navigator::setTimeNow()
{
	setJDay(StelUtils::getJDFromSystem());
}

void Navigator::setTodayTime(const QTime& target)
{
	QDateTime dt = QDateTime::currentDateTime();
	if (target.isValid())
	{
		dt.setTime(target);
		// don't forget to adjust for timezone / daylight savings.
		setJDay(StelUtils::qDateTimeToJd(dt)-(StelUtils::getGMTShiftFromQT(StelUtils::getJDFromSystem()) * JD_HOUR));
	}
	else
	{
		qWarning() << "WARNING - time passed to Navigator::setTodayTime is not valid. The system time will be used." << target;
		setTimeNow();
	}
}

//! Get whether the current stellarium time is the real world time
bool Navigator::getIsTimeNow(void) const
{
	// cache last time to prevent to much slow system call
	static double lastJD = getJDay();
	static bool previousResult = (fabs(getJDay()-StelUtils::getJDFromSystem())<JD_SECOND);
	if (fabs(lastJD-getJDay())>JD_SECOND/4)
	{
		lastJD = getJDay();
		previousResult = (fabs(getJDay()-StelUtils::getJDFromSystem())<JD_SECOND);
	}
	return previousResult;
}

void Navigator::addSolarDays(double d)
{
	setJDay(getJDay() + d);
}

void Navigator::addSiderealDays(double d)
{
	const Planet* home = position->getHomePlanet();
	if (home->getEnglishName() != "Solar System Observer")
		d *= home->getSiderealDay();
	setJDay(getJDay() + d);
}

void Navigator::moveObserverToSelected(void)
{
	if (StelApp::getInstance().getStelObjectMgr().getWasSelected())
	{
		Planet* pl = dynamic_cast<Planet*>(StelApp::getInstance().getStelObjectMgr().getSelectedObject()[0].get());
		if (pl)
		{
			// We need to move to the selected planet. Try to generate a location from the current one
			Location loc = getCurrentLocation();
			loc.planetName = pl->getEnglishName();
			loc.name = "-";
			loc.state = "";
			moveObserverTo(loc);
		}
	}
	MovementMgr* mmgr = (MovementMgr*)GETSTELMODULE("MovementMgr");
	Q_ASSERT(mmgr);
	mmgr->setFlagTracking(false);
}

// Get the informations on the current location
const Location& Navigator::getCurrentLocation() const
{
	return position->getCurrentLocation();
}

// Smoothly move the observer to the given location
void Navigator::moveObserverTo(const Location& target, double duration, double durationIfPlanetChange)
{
	double d = (getCurrentLocation().planetName==target.planetName) ? duration : durationIfPlanetChange;
	if (d>0.)
	{
		SpaceShipObserver* newObs = new SpaceShipObserver(getCurrentLocation(), target, d);
		delete position;
		position = newObs;
	}
	else
	{
		delete position;
		position = new Observer(target);
	}
}

// Get the sideral time shifted by the observer longitude
double Navigator::getLocalSideralTime() const
{
	return (position->getHomePlanet()->getSiderealTime(JDay)+position->getCurrentLocation().longitude)*M_PI/180.;
}

void Navigator::setInitViewDirectionToCurrent(void)
{
	initViewPos = localVision;
	QString dirStr = QString("%1,%2,%3").arg(localVision[0]).arg(localVision[1]).arg(localVision[2]);
	StelApp::getInstance().getSettings()->setValue("navigation/init_view_pos", dirStr);
}

//! Increase the time speed
void Navigator::increaseTimeSpeed()
{
	double s = getTimeSpeed();
	if (s>=JD_SECOND) s*=10.;
	else if (s<-JD_SECOND) s/=10.;
	else if (s>=0. && s<JD_SECOND) s=JD_SECOND;
	else if (s>=-JD_SECOND && s<0.) s=0.;
	setTimeSpeed(s);
}

//! Decrease the time speed
void Navigator::decreaseTimeSpeed()
{
	double s = getTimeSpeed();
	if (s>JD_SECOND) s/=10.;
	else if (s<=-JD_SECOND) s*=10.;
	else if (s>-JD_SECOND && s<=0.) s=-JD_SECOND;
	else if (s>0. && s<=JD_SECOND) s=0.;
	setTimeSpeed(s);
}
	
////////////////////////////////////////////////////////////////////////////////
void Navigator::setLocalVision(const Vec3d& _pos)
{
	localVision = _pos;
	equVision=altAzToEarthEqu(localVision);
	J2000EquVision = matEarthEquToJ2000*equVision;
}

////////////////////////////////////////////////////////////////////////////////
void Navigator::setEquVision(const Vec3d& _pos)
{
	equVision = _pos;
	J2000EquVision = matEarthEquToJ2000*equVision;
	localVision = earthEquToAltAz(equVision);
}

////////////////////////////////////////////////////////////////////////////////
void Navigator::setJ2000EquVision(const Vec3d& _pos)
{
	J2000EquVision = _pos;
	equVision = matJ2000ToEarthEqu*J2000EquVision;
	localVision = earthEquToAltAz(equVision);
}

////////////////////////////////////////////////////////////////////////////////
// Increment time
void Navigator::updateTime(double deltaTime)
{
	JDay+=timeSpeed*deltaTime;

	// Fix time limits to -100000 to +100000 to prevent bugs
	if (JDay>38245309.499988) JDay = 38245309.499988;
	if (JDay<-34803211.500012) JDay = -34803211.500012;
	
	if (position->isObserverLifeOver())
	{
		// Unselect if the new home planet is the previously selected object
		StelObjectMgr &objmgr(StelApp::getInstance().getStelObjectMgr());
		if (objmgr.getWasSelected() && objmgr.getSelectedObject()[0].get()==position->getHomePlanet())
		{
			objmgr.unSelect();
		}
		Observer* newObs = position->getNextObserver();
		delete position;
		position = newObs;
	}
	position->update(deltaTime);
}

////////////////////////////////////////////////////////////////////////////////
// The non optimized (more clear version is available on the CVS : before date 25/07/2003)
// see vsop87.doc:

const Mat4d matJ2000ToVsop87(Mat4d::xrotation(-23.4392803055555555556*(M_PI/180)) * Mat4d::zrotation(0.0000275*(M_PI/180)));
const Mat4d matVsop87ToJ2000(matJ2000ToVsop87.transpose());

void Navigator::updateTransformMatrices(void)
{
	matAltAzToEarthEqu = position->getRotAltAzToEquatorial(JDay);
	matEarthEquToAltAz = matAltAzToEarthEqu.transpose();

	matEarthEquToJ2000 = matVsop87ToJ2000 * position->getRotEquatorialToVsop87();
	matJ2000ToEarthEqu = matEarthEquToJ2000.transpose();
	matJ2000ToAltAz = matEarthEquToAltAz*matJ2000ToEarthEqu;
	
	matHeliocentricEclipticToEarthEqu = matJ2000ToEarthEqu * matVsop87ToJ2000 * Mat4d::translation(-position->getCenterVsop87Pos());

	// These two next have to take into account the position of the observer on the earth
	Mat4d tmp = matJ2000ToVsop87 * matEarthEquToJ2000 * matAltAzToEarthEqu;

	matAltAzToHeliocentricEcliptic =  Mat4d::translation(position->getCenterVsop87Pos()) * tmp *
	                      Mat4d::translation(Vec3d(0.,0., position->getDistanceFromCenter()));

	matHeliocentricEclipticToAltAz =  Mat4d::translation(Vec3d(0.,0.,-position->getDistanceFromCenter())) * tmp.transpose() *
	                      Mat4d::translation(-position->getCenterVsop87Pos());
}

void Navigator::setStartupTimeMode(const QString& s)
{
	startupTimeMode = s;
}

////////////////////////////////////////////////////////////////////////////////
// Update the modelview matrices
void Navigator::updateModelViewMat(void)
{
	Vec3d f;

	if( viewingMode == ViewEquator)
	{
		// view will use equatorial coordinates, so that north is always up
		f = equVision;
	}
	else
	{
		// view will correct for horizon (always down)
		f = localVision;
	}

	f.normalize();
	Vec3d s(f[1],-f[0],0.);

	if( viewingMode == ViewEquator)
	{
		// convert everything back to local coord
		f = localVision;
		f.normalize();
		s = earthEquToAltAz( s );
	}

	Vec3d u(s^f);
	s.normalize();
	u.normalize();

	matAltAzToEye.set(s[0],u[0],-f[0],0.,
	                     s[1],u[1],-f[1],0.,
	                     s[2],u[2],-f[2],0.,
	                     0.,0.,0.,1.);

	matEarthEquToEye = matAltAzToEye*matEarthEquToAltAz;
	matHeliocentricEclipticToEye = matAltAzToEye*matHeliocentricEclipticToAltAz;
	matJ2000ToEye = matEarthEquToEye*matJ2000ToEarthEqu;
}


////////////////////////////////////////////////////////////////////////////////
// Return the observer heliocentric position
Vec3d Navigator::getObserverHelioPos(void) const
{
	static const Vec3d v(0.,0.,0.);
	return matAltAzToHeliocentricEcliptic*v;
}

void Navigator::setPresetSkyTime(QDateTime dt)
{
	setPresetSkyTime(StelUtils::qDateTimeToJd(dt));
}

////////////////////////////////////////////////////////////////////////////////
// Set type of viewing mode (align with horizon or equatorial coordinates)
void Navigator::setViewingMode(ViewingModeType viewMode)
{
	viewingMode = viewMode;

	// TODO: include some nice smoothing function trigger here to rotate between
	// the two modes
}
