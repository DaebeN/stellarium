/*
 * Stellarium
 * Copyright (C) 2002 Fabien Chereau
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

#include "stel_ui.h"
#include "StelUtils.hpp"
#include "StelApp.hpp"
#include "StelCore.hpp"
#include "StarMgr.hpp"
#include "ConstellationMgr.hpp"
#include "SolarSystem.hpp"
#include "NebulaMgr.hpp"
#include "stel_command_interface.h"
#include "StelTextureMgr.hpp"
#include "LandscapeMgr.hpp"
#include "GridLinesMgr.hpp"
#include "MovementMgr.hpp"
#include "StelObjectMgr.hpp"
#include "MeteorMgr.hpp"
#include "StelModuleMgr.hpp"
#include "StelFontMgr.hpp"
#include "StelLocaleMgr.hpp"
#include "StelSkyCultureMgr.hpp"
#include "InitParser.hpp"
#include "Observer.hpp"
#include "Navigator.hpp"
#include "StelFileMgr.hpp"

using namespace s_gui;

Component* StelUI::createConfigWindow(SFont& courierFont)
{
	StelApp::getInstance().getTextureManager().setDefaultParams();
	
	StarMgr* smgr = (StarMgr*)StelApp::getInstance().getModuleMgr().getModule("stars");
	NebulaMgr* nmgr = (NebulaMgr*)StelApp::getInstance().getModuleMgr().getModule("nebulas");
	LandscapeMgr* lmgr = (LandscapeMgr*)StelApp::getInstance().getModuleMgr().getModule("landscape");
	
	config_win = new StdBtWin(_("Configuration"));
	//config_win->setOpaque(opaqueGUI);
	config_win->reshape(300,200,500,450);
	config_win->setVisible(FlagConfig);

	config_tab_ctr = new TabContainer();
	config_tab_ctr->setSize(config_win->getSize());

	// The current drawing position
	int x,y;
	x=70; y=15;

	// Rendering options
	FilledContainer* tab_render = new FilledContainer();
	tab_render->setSize(config_tab_ctr->getSize());

	STextureSP starp = StelApp::getInstance().getTextureManager().createTexture("halo.png");
	Picture* pstar = new Picture(starp, x-50, y+5, 32, 32);
	tab_render->addComponent(pstar);

	stars_cbx = new LabeledCheckBox(smgr->getFlagStars(), _("Stars"));
	stars_cbx->setOnPressCallback(callback<void>(this, &StelUI::updateConfigVariables));
	tab_render->addComponent(stars_cbx);
	stars_cbx->setPos(x,y); y+=15;

	star_names_cbx = new LabeledCheckBox(smgr->getFlagNames(), _("Star Names. Up to mag :"));
	star_names_cbx->setOnPressCallback(callback<void>(this, &StelUI::updateConfigVariables));
	tab_render->addComponent(star_names_cbx);
	star_names_cbx->setPos(x,y);

	max_mag_star_name = new FloatIncDec(&courierFont, tex_up, tex_down, -1.5, 9, smgr->getMaxMagName(), 0.5);
	max_mag_star_name->setOnPressCallback(callback<void>(this, &StelUI::updateConfigVariables));
	tab_render->addComponent(max_mag_star_name);
	max_mag_star_name->setPos(x + 320,y);

	y+=15;

	star_twinkle_cbx = new LabeledCheckBox(smgr->getFlagTwinkle(), _("Star Twinkle. Amount :"));
	star_twinkle_cbx->setOnPressCallback(callback<void>(this, &StelUI::updateConfigVariables));
	tab_render->addComponent(star_twinkle_cbx);
	star_twinkle_cbx->setPos(x,y);

	star_twinkle_amount = new FloatIncDec(&courierFont, tex_up, tex_down, 0, 0.6, smgr->getTwinkleAmount(), 0.1);
	star_twinkle_amount->setOnPressCallback(callback<void>(this, &StelUI::updateConfigVariables));
	tab_render->addComponent(star_twinkle_amount);
	star_twinkle_amount->setPos(x + 320,y);

	y+=30;

	STextureSP constellp = StelApp::getInstance().getTextureManager().createTexture("bt_constellations.png");
	Picture* pconstell = new Picture(constellp, x-50, y+10, 32, 32);
	tab_render->addComponent(pconstell);

	constellation_cbx = new LabeledCheckBox(false, _("Constellations Lines"));
	constellation_cbx->setOnPressCallback(callback<void>(this, &StelUI::updateConfigVariables));
	tab_render->addComponent(constellation_cbx);
	constellation_cbx->setPos(x,y); y+=15;

	constellation_name_cbx = new LabeledCheckBox(false, _("Constellations Names"));
	constellation_name_cbx->setOnPressCallback(callback<void>(this, &StelUI::updateConfigVariables));
	tab_render->addComponent(constellation_name_cbx);
	constellation_name_cbx->setPos(x,y); y+=15;

	constellation_boundaries_cbx = new LabeledCheckBox(false, _("Constellations Boundaries"));
	constellation_boundaries_cbx->setOnPressCallback(callback<void>(this, &StelUI::updateConfigVariables));
	tab_render->addComponent(constellation_boundaries_cbx);
	constellation_boundaries_cbx->setPos(x,y); y+=15;

	sel_constellation_cbx = new LabeledCheckBox(false, _("Selected Constellation Only"));
	sel_constellation_cbx->setOnPressCallback(callback<void>(this, &StelUI::updateConfigVariables));
	tab_render->addComponent(sel_constellation_cbx);
	sel_constellation_cbx->setPos(x,y);

	y+=30;

	STextureSP nebp = StelApp::getInstance().getTextureManager().createTexture("bt_nebula.png");
	Picture* pneb = new Picture(nebp, x-50, y-13, 32, 32);
	tab_render->addComponent(pneb);

	nebulas_names_cbx = new LabeledCheckBox(false, _("Nebulas Names. Up to mag :"));
	nebulas_names_cbx->setOnPressCallback(callback<void>(this, &StelUI::updateConfigVariables));
	tab_render->addComponent(nebulas_names_cbx);
	nebulas_names_cbx->setPos(x,y);

	max_mag_nebula_name = new FloatIncDec(&courierFont, tex_up, tex_down, 0, 12,
		nmgr->getMaxMagHints(), 0.5);
	max_mag_nebula_name->setOnPressCallback(callback<void>(this, &StelUI::updateConfigVariables));
	tab_render->addComponent(max_mag_nebula_name);
	max_mag_nebula_name->setPos(x + 320,y);

	y+=15;
	
	nebulas_no_texture_cbx = new LabeledCheckBox(false, _("Also display Nebulas without textures"));
	nebulas_no_texture_cbx->setOnPressCallback(callback<void>(this, &StelUI::updateConfigVariables));
	tab_render->addComponent(nebulas_no_texture_cbx);
	nebulas_no_texture_cbx->setPos(x,y);

	y+=30;

	STextureSP planp = StelApp::getInstance().getTextureManager().createTexture("bt_planet.png");
	Picture* pplan = new Picture(planp, x-50, y-7, 32, 32);
	tab_render->addComponent(pplan);

	planets_cbx = new LabeledCheckBox(false, _("Planets"));
	planets_cbx->setOnPressCallback(callback<void>(this, &StelUI::updateConfigVariables2));
	tab_render->addComponent(planets_cbx);
	planets_cbx->setPos(x,y);

	moon_x4_cbx = new LabeledCheckBox(false, _("Moon Scale"));
	moon_x4_cbx->setOnPressCallback(callback<void>(this, &StelUI::updateConfigVariables));
	tab_render->addComponent(moon_x4_cbx);
	moon_x4_cbx->setPos(x + 220,y);

	y+=15;

	planets_hints_cbx = new LabeledCheckBox(false, _("Planets Hints"));
	planets_hints_cbx->setOnPressCallback(callback<void>(this, &StelUI::updateConfigVariables));
	tab_render->addComponent(planets_hints_cbx);
	planets_hints_cbx->setPos(x,y);
	
	
	y+=30;

	STextureSP gridp = StelApp::getInstance().getTextureManager().createTexture("bt_eqgrid.png");
	Picture* pgrid = new Picture(gridp, x-50, y-4, 32, 32);
	tab_render->addComponent(pgrid);

	equator_grid_cbx = new LabeledCheckBox(false, _("Equatorial Grid"));
	equator_grid_cbx->setOnPressCallback(callback<void>(this, &StelUI::updateConfigVariables));
	tab_render->addComponent(equator_grid_cbx);
	equator_grid_cbx->setPos(x,y); y+=15;

	azimuth_grid_cbx = new LabeledCheckBox(false, _("Azimuthal Grid"));
	azimuth_grid_cbx->setOnPressCallback(callback<void>(this, &StelUI::updateConfigVariables));
	tab_render->addComponent(azimuth_grid_cbx);
	azimuth_grid_cbx->setPos(x,y); y-=15;

	equator_cbx = new LabeledCheckBox(false, _("Equator Line"));
	equator_cbx->setOnPressCallback(callback<void>(this, &StelUI::updateConfigVariables));
	tab_render->addComponent(equator_cbx);
	equator_cbx->setPos(x + 220,y); y+=15;

	ecliptic_cbx = new LabeledCheckBox(false, _("Ecliptic Line"));
	ecliptic_cbx->setOnPressCallback(callback<void>(this, &StelUI::updateConfigVariables));
	tab_render->addComponent(ecliptic_cbx);
	ecliptic_cbx->setPos(x + 220,y);

	y+=30;

	STextureSP groundp = StelApp::getInstance().getTextureManager().createTexture("bt_ground.png");
	Picture* pground = new Picture(groundp, x-50, y-4, 32, 32);
	tab_render->addComponent(pground);

	ground_cbx = new LabeledCheckBox(false, _("Ground"));
	ground_cbx->setOnPressCallback(callback<void>(this, &StelUI::updateConfigVariables));
	tab_render->addComponent(ground_cbx);
	ground_cbx->setPos(x,y);

	cardinal_cbx = new LabeledCheckBox(false, _("Cardinal Points"));
	cardinal_cbx->setOnPressCallback(callback<void>(this, &StelUI::updateConfigVariables));
	tab_render->addComponent(cardinal_cbx);
	cardinal_cbx->setPos(x + 220,y); y+=15;

	atmosphere_cbx = new LabeledCheckBox(false, _("Atmosphere"));
	atmosphere_cbx->setOnPressCallback(callback<void>(this, &StelUI::updateConfigVariables));
	tab_render->addComponent(atmosphere_cbx);
	atmosphere_cbx->setPos(x,y);

	fog_cbx = new LabeledCheckBox(false, _("Fog"));
	fog_cbx->setOnPressCallback(callback<void>(this, &StelUI::updateConfigVariables));
	tab_render->addComponent(fog_cbx);
	fog_cbx->setPos(x + 220,y);

	y+=30;
	
	meteorlbl = new Label(L"-");
	meteorlbl->setPos(x,y);
	tab_render->addComponent(meteorlbl);

	y+=20;
	
	meteor_rate_10 = new LabeledCheckBox(false, L"10");
	meteor_rate_10->setOnPressCallback(callback<void>(this, &StelUI::updateConfigVariables));
	tab_render->addComponent(meteor_rate_10);
	meteor_rate_10->setPos(x,y);
	meteor_rate_80 = new LabeledCheckBox(false, L"80");
	meteor_rate_80->setOnPressCallback(callback<void>(this, &StelUI::updateConfigVariables));
	tab_render->addComponent(meteor_rate_80);
	meteor_rate_80->setPos(x+40,y);
	meteor_rate_10000 = new LabeledCheckBox(false, L"10000");
	meteor_rate_10000->setOnPressCallback(callback<void>(this, &StelUI::updateConfigVariables));
	tab_render->addComponent(meteor_rate_10000);
	meteor_rate_10000->setPos(x+80,y);
	meteor_rate_144000 = new LabeledCheckBox(false, L"144000");
	meteor_rate_144000->setOnPressCallback(callback<void>(this, &StelUI::updateConfigVariables));
	tab_render->addComponent(meteor_rate_144000);
	meteor_rate_144000->setPos(x+144,y);

	LabeledButton* render_save_bt = new LabeledButton(_("Save as default"));
	render_save_bt->setOnPressCallback(callback<void>(this, &StelUI::saveRenderOptions));
	tab_render->addComponent(render_save_bt);
	render_save_bt->setPos(tab_render->getSizex()/2-render_save_bt->getSizex()/2,tab_render->getSizey()-70);
	y+=20;

	// Date & Time options
	FilledContainer* tab_time = new FilledContainer();
	tab_time->setSize(config_tab_ctr->getSize());

	x=10; y=10;

	Label* tclbl = new Label(wstring(L"\u2022 ")+_("Current Time :"));
	tclbl->setPos(x,y); y+=20;
	tab_time->addComponent(tclbl);

	time_current = new Time_item(&courierFont, tex_up, tex_down);
	time_current->setOnChangeTimeCallback(callback<void>(this, &StelUI::setCurrentTimeFromConfig));
	tab_time->addComponent(time_current);
	time_current->setPos(50,y); y+=80;

	Label* tzbl = new Label(wstring(L"\u2022 ")+_("Time Zone :"));
	tzbl->setPos(x,y); y+=20;
	tab_time->addComponent(tzbl);

	Label* system_tz_lbl = new Label(wstring(L"\u2022 ")+_("Using System Default Time Zone"));
	tab_time->addComponent(system_tz_lbl);
	system_tz_lbl->setPos(50 ,y); y+=20;
	wstring tmpl(L"(ERROR)");
	system_tz_lbl2 = new Label(tmpl);
	tab_time->addComponent(system_tz_lbl2);
	
	system_tz_lbl2->setPos(70 ,y); y+=30;

	Label* time_speed_lbl = new Label(wstring(L"\u2022 ")+_("Time speed : "));
	tab_time->addComponent(time_speed_lbl);
	time_speed_lbl->setPos(x ,y); y+=20;

	time_speed_lbl2 = new Label();
	tab_time->addComponent(time_speed_lbl2);
	time_speed_lbl2->setPos(50 ,y); y+=30;

	TextLabel* ts_lbl = new TextLabel(_("Use key J and L to decrease and increase\n   time speed.\nUse key K to return to real time speed."));
	tab_time->addComponent(ts_lbl);
	ts_lbl->setPos(50 ,y); y+=30;

	// Location options
	FilledContainer* tab_location = new FilledContainer();
	tab_location->setSize(config_tab_ctr->getSize());

	x=5; y=5;
	STextureSP earth = core->getObservatory()->getHomePlanet()->getMapTexture();
	STextureSP pointertex = StelApp::getInstance().getTextureManager().createTexture("pointeur1.png");
	STextureSP citytex = StelApp::getInstance().getTextureManager().createTexture("city.png");
	earth_map = new MapPicture(earth, pointertex, citytex, x,y,tab_location->getSizex()-10, 250);
	earth_map->setOnPressCallback(callback<void>(this, &StelUI::setObserverPositionFromMap));
	earth_map->setOnNearestCityCallback(callback<void>(this, &StelUI::setCityFromMap));
	tab_location->addComponent(earth_map);
	y+=earth_map->getSizey();
	earth_map->set_font(&(StelApp::getInstance().getFontManager().getStandardFont(StelApp::getInstance().getLocaleMgr().getAppLanguage(), 9.5)));
	load_cities(core->getObservatory()->getHomePlanetEnglishName());
	
	y += 5;
	Label * lblcursor = new Label(_("Cursor : "));
	lblcursor->setPos(20, y+1);
	lblMapLocation = new Label();
	lblMapLocation->setPos(130, y+1);
	
	Label * lblloc = new Label(_("Selected : "));
	lblloc->setPos(20, y+21);
	lblMapPointer = new Label(L"ERROR");
	lblMapPointer->setPos(130, y+21);
	
	Label * lbllong = new Label(_("Longitude : "));
	lbllong->setPos(20, y+41);
	long_incdec	= new FloatIncDec(&courierFont, tex_up, tex_down, -180, 180, 0, 1.f/60);
	long_incdec->setSizex(135);
	long_incdec->setFormat(FORMAT_LONGITUDE);
	long_incdec->setOnPressCallback(callback<void>(this, &StelUI::setObserverPositionFromIncDec));
	long_incdec->setPos(130,y+40);

	Label * lbllat = new Label(_("Latitude : "));
	lbllat->setPos(20, y+61);
	lat_incdec	= new FloatIncDec(&courierFont, tex_up, tex_down, -90, 90, 0, 1.f/60);
	lat_incdec->setFormat(FORMAT_LATITUDE);
	lat_incdec->setSizex(135);
	lat_incdec->setOnPressCallback(callback<void>(this, &StelUI::setObserverPositionFromIncDec));
	lat_incdec->setPos(130,y+60);

	Label * lblalt = new Label(_("Altitude : "));
	lblalt->setPos(20, y+81);
	alt_incdec	= new IntIncDec(&courierFont, tex_up, tex_down, 0, 2000, 0, 10);
	alt_incdec->setSizex(135);
	alt_incdec->setOnPressCallback(callback<void>(this, &StelUI::setObserverPositionFromIncDec));
	alt_incdec->setPos(130,y+80);

	LabeledButton* location_save_bt = new LabeledButton(_("Save location"));
	location_save_bt->setOnPressCallback(callback<void>(this, &StelUI::saveObserverPosition));
	location_save_bt->setPos(280,y+70);
	//location_save_bt->setSize(120,25);

	tab_location->addComponent(lblcursor);
	tab_location->addComponent(lblloc);
	tab_location->addComponent(lblMapLocation);
	tab_location->addComponent(lblMapPointer);
	tab_location->addComponent(lbllong);
	tab_location->addComponent(lbllat);
	tab_location->addComponent(lblalt);
	tab_location->addComponent(long_incdec);
	tab_location->addComponent(lat_incdec);
	tab_location->addComponent(alt_incdec);
	tab_location->addComponent(location_save_bt);

	// Video Options
	FilledContainer* tab_video = new FilledContainer();
	tab_video->setSize(config_tab_ctr->getSize());

	x=30; y=10;
	Label * lblvideo1 = new Label(wstring(L"\u2022 ")+_("Projection :"));
	lblvideo1->setPos(x, y);
	tab_video->addComponent(lblvideo1);

	y+=20;

	projection_sl = new StringList();
	projection_sl->addItem("perspective");
	projection_sl->addItem("orthographic");
	projection_sl->addItem("equal_area");
	projection_sl->addItem("fisheye");
	projection_sl->addItem("stereographic");
	projection_sl->addItem("cylinder");
	projection_sl->adjustSize();
	projection_sl->setValue(core->getProjection()->getCurrentProjection());
	projection_sl->setOnPressCallback(callback<void>(this, &StelUI::updateVideoVariables));
	tab_video->addComponent(projection_sl);
	projection_sl->setPos(x+20,y); y+=140;
	
	disk_viewport_cbx = new LabeledCheckBox(false, _("Disk Viewport"));
	disk_viewport_cbx->setOnPressCallback(callback<void>(this, &StelUI::updateVideoVariables));
	tab_video->addComponent(disk_viewport_cbx);
	disk_viewport_cbx->setPos(x,y); y+=35;

	viewport_distorter_cbx = new LabeledCheckBox(false, _("Viewport Distorter"));
	viewport_distorter_cbx->setOnPressCallback(callback<void>(this, &StelUI::updateVideoVariables));
	tab_video->addComponent(viewport_distorter_cbx);
	viewport_distorter_cbx->setPos(x,y); y+=35;

	x=220; y=10;
	Label * lblvideo2 = new Label(wstring(L"\u2022 ")+_("Screen Resolution :"));
	lblvideo2->setPos(x+10, y);
	tab_video->addComponent(lblvideo2); y+=24;

	Label * lblvideo3 = new Label(_("Restart program for"));
	Label * lblvideo4 = new Label(_("change to apply."));
	lblvideo3->setPos(x+10, y+145);
	lblvideo4->setPos(x+10, y+160);
	tab_video->addComponent(lblvideo3);
	tab_video->addComponent(lblvideo4);

	screen_size_sl = new ListBox(6);
	screen_size_sl->setPos(x+20,y);
	screen_size_sl->setSizex(200);
	screen_size_sl->addItemList(StelUtils::stringToWstring(app->getVideoModeList()));
	char vs[1000];
	sprintf(vs, "%dx%d", core->getProjection()->getViewportWidth(), core->getProjection()->getViewportHeight());
	screen_size_sl->setCurrent(StelUtils::stringToWstring(vs));
	tab_video->addComponent(screen_size_sl);

	Label * lblvideo5 = new Label(_("For unlisted screen resolution, edit the file :"));
	Label * lblvideo6 = new Label(StelUtils::stringToWstring(app->getConfigFilePath()));
	lblvideo5->setPos(30, tab_video->getSizey()-125);
	lblvideo6->setPos(30, tab_video->getSizey()-110);
	tab_video->addComponent(lblvideo5);
	tab_video->addComponent(lblvideo6);

	LabeledButton* video_save_bt = new LabeledButton(_("Save as default"));
	video_save_bt->setOnPressCallback(callback<void>(this, &StelUI::setVideoOption));
	tab_video->addComponent(video_save_bt);
	video_save_bt->setPos(tab_video->getSizex()/2-video_save_bt->getSizex()/2,tab_video->getSizey()-70);

	// Landscapes option
	FilledContainer* tab_landscapes = new FilledContainer();
	tab_landscapes->setSize(config_tab_ctr->getSize());

	x=10; y=10;
	Label * lbllandscapes1 = new Label(wstring(L"\u2022 ")+_("Choose landscapes:"));
	lbllandscapes1->setPos(x, y);
	tab_landscapes->addComponent(lbllandscapes1);

	x=30; y+=24;

	landscape_sl = new StringList();
	landscape_sl->setPos(x,y);
	landscape_sl->addItemList(lmgr->getLandscapeNames());
	landscape_sl->adjustSize();
	sprintf(vs, "%s", StelUtils::wstringToString(lmgr->getLandscapeName()).c_str());
	landscape_sl->setValue(vs);
	landscape_sl->setOnPressCallback(callback<void>(this, &StelUI::setLandscape));
	tab_landscapes->addComponent(landscape_sl);

	landscape_authorlb = new Label(_("Author: ") +lmgr->getLandscapeAuthorName());
	landscape_authorlb->setPos(x+landscape_sl->getSizex()+20, y); 
	landscape_authorlb->adjustSize();
	tab_landscapes->addComponent(landscape_authorlb);
	
	landscapePlanetLb = new Label(_("Planet: ") + lmgr->getLandscapePlanetName());
	landscapePlanetLb->setPos(x+landscape_sl->getSizex()+20, y+25); 
	landscapePlanetLb->adjustSize();
	tab_landscapes->addComponent(landscapePlanetLb);

	landscapeLocationLb = new Label(_("Location: ") + lmgr->getLandscapeLocationDescription());
	landscapeLocationLb->setPos(x+landscape_sl->getSizex()+20, y+50); 
	landscapeLocationLb->adjustSize();
	tab_landscapes->addComponent(landscapeLocationLb);

	locationFromLandscapeCheck = new LabeledCheckBox(lmgr->getFlagLandscapeSetsLocation(),_("Setting landscape updates the location"));
	locationFromLandscapeCheck->setOnPressCallback(callback<void>(this, &StelUI::setLandscapeUpdatesLocation));
	locationFromLandscapeCheck->setPos(x+landscape_sl->getSizex()+20, y+80); 
	tab_landscapes->addComponent(locationFromLandscapeCheck);
	
	landscape_descriptionlb = new TextLabel(lmgr->getLandscapeDescription());
	landscape_descriptionlb->setPos(x+landscape_sl->getSizex()+20, y+110); 
	landscape_descriptionlb->adjustSize();
	tab_landscapes->addComponent(landscape_descriptionlb);	
	y+=250;	

	LabeledButton* landscape_save_bt = new LabeledButton(_("Save as default"));
	landscape_save_bt->setOnPressCallback(callback<void>(this, &StelUI::saveLandscapeOptions));
	tab_landscapes->addComponent(landscape_save_bt);
	landscape_save_bt->setPos(tab_landscapes->getSizex()/2-landscape_save_bt->getSizex()/2,tab_landscapes->getSizey()-70);

	// Language options
	FilledContainer* tab_language = new FilledContainer();
	tab_language->setSize(config_tab_ctr->getSize());

	x=10; y=10;
	Label * lbllanguage = new Label(wstring(L"\u2022 ")+_("Program Language: "));
	lbllanguage->setPos(x, y);
	tab_language->addComponent(lbllanguage);

	y+=25;

	language_lb = new ListBox(6);
	language_lb->setPos(x+10,y);
	language_lb->setSizex(200);
	language_lb->addItemList(Translator::getAvailableLanguagesNamesNative(app->getFileMgr().getLocaleDir()));
	language_lb->setOnChangeCallback(callback<void>(this, &StelUI::setAppLanguage));
	language_lb->setCurrent(StelUtils::stringToWstring(app->getLocaleMgr().getAppLanguage()));
	tab_language->addComponent(language_lb);
	
	x=260; y=10;
	
	Label * lbllanguage2 = new Label(wstring(L"\u2022 ")+_("Sky Language: "));
	lbllanguage2->setPos(x, y);
	tab_language->addComponent(lbllanguage2);

	y+=25;

	languageSky_lb = new ListBox(6);
	languageSky_lb->setPos(x+10,y);
	languageSky_lb->setSizex(200);
	languageSky_lb->addItemList(Translator::getAvailableLanguagesNamesNative(app->getFileMgr().getLocaleDir()));
	languageSky_lb->setOnChangeCallback(callback<void>(this, &StelUI::setSkyLanguage));
	languageSky_lb->setCurrent(StelUtils::stringToWstring(app->getLocaleMgr().getSkyLanguage()));
	tab_language->addComponent(languageSky_lb);

	x=150;
	y+=languageSky_lb->getSizey();
	y+=30;
	
	Label * lbllanguage3 = new Label(wstring(L"\u2022 ")+_("Sky Culture: "));
	lbllanguage3->setPos(x, y);
	tab_language->addComponent(lbllanguage3);
	
	y+=25;
	
	skyculture_lb = new ListBox(5);
	skyculture_lb->setSizex(200);
	skyculture_lb->setPos(x,y);
	skyculture_lb->addItemList(app->getSkyCultureMgr().getSkyCultureListI18());
	skyculture_lb->setOnChangeCallback(callback<void>(this, &StelUI::setSkyCulture));
	skyculture_lb->setCurrent(app->getSkyCultureMgr().getSkyCulture());
	tab_language->addComponent(skyculture_lb);

	LabeledButton* language_save_bt = new LabeledButton(_("Save as default"));
	language_save_bt->setOnPressCallback(callback<void>(this, &StelUI::saveLanguageOptions));
	tab_language->addComponent(language_save_bt);
	language_save_bt->setPos(tab_language->getSizex()/2-language_save_bt->getSizex()/2,tab_language->getSizey()-70);

	// Global window
	config_tab_ctr->setTexture(flipBaseTex);
	config_tab_ctr->addTab(tab_language, _("Language"));	
	config_tab_ctr->addTab(tab_time, _("Date & Time"));
	config_tab_ctr->addTab(tab_location, _("Location"));
	config_tab_ctr->addTab(tab_landscapes, _("Landscapes"));
	config_tab_ctr->addTab(tab_video, _("Video"));
	config_tab_ctr->addTab(tab_render, _("Rendering"));
	config_win->addComponent(config_tab_ctr);
	config_win->setOnHideBtCallback(callback<void>(this, &StelUI::config_win_hideBtCallback));

	return config_win;
}

void StelUI::dialogCallback(void)
{
	string lastID = dialog_win->getLastID();
	int lastButton = dialog_win->getLastButton();
	string lastInput = StelUtils::wstringToString(dialog_win->getLastInput());
	int lastType = dialog_win->getLastType();

	if (lastID == "observatory name")
	{
		if (lastButton != BT_OK || lastInput == "")
			lastInput = UNKNOWN_OBSERVATORY;
		doSaveObserverPosition(lastInput);
		setCityFromMap();
	}
	else if (lastID != "")
	{
		string msg = lastID;
		msg += " returned btn: ";
	
		if (lastButton == BT_OK) msg += "BT_OK";
		else if (lastButton == BT_YES) msg += "BT_YES";
		else if (lastButton == BT_NO) msg += "BT_NO";
		else if (lastButton == BT_CANCEL) msg += "BT_CANCEL";
	
		if (lastType == STDDLGWIN_MSG) dialog_win->MessageBox(L"Stellarium",StelUtils::stringToWstring(msg), BT_OK);
		else if (lastType == STDDLGWIN_INPUT) dialog_win->MessageBox(L"Stellarium",StelUtils::stringToWstring(msg) + L" inp: " + StelUtils::stringToWstring(lastInput), BT_OK);
	}		
}

void StelUI::setSkyLanguage(void)
{
	app->getLocaleMgr().setSkyLanguage(Translator::nativeLanguageNameCodeToIso639_1(languageSky_lb->getCurrent()));
}

void StelUI::setAppLanguage(void)
{
	app->getLocaleMgr().setAppLanguage(Translator::nativeLanguageNameCodeToIso639_1(language_lb->getCurrent()));
}

void StelUI::setSkyCulture(void)
{
	app->getSkyCultureMgr().setSkyCulture(skyculture_lb->getCurrent());
}

void StelUI::load_cities(const string& planetEnglishName)
{
	// Clear the old cities
	earth_map->clearCities();
	
	string fileName;
	try
	{
		fileName = StelApp::getInstance().getFileMgr().findFile("data/cities_" + planetEnglishName + ".fab");
	}
	catch(exception& e)
	{
		cerr << "INFO StelUI::load_cities " << e.what() << endl;
		return;
	}

	float time;

	char linetmp[200];
	char cname[50],cstate[50],ccountry[50],clat[20],clon[20],ctime[20];
	int showatzoom;
	int alt;
	char tmpstr[2000];
	int total = 0;

	cout << "Loading Cities data for planet " << planetEnglishName << "...";
	FILE *fic = fopen(fileName.c_str(), "r");
	if (!fic)
	{
		cerr << "Can't open " << fileName << endl;
		return;
	}

	// determine total number to be loaded for percent complete display
	while (fgets(tmpstr, 2000, fic)) {++total;}
	rewind(fic);

	int line = 0;
	while (!feof(fic))
	{
		fgets(linetmp, 100, fic);
		if (linetmp[0] != '#')
		{
			if (sscanf(linetmp, "%s %s %s %s %s %d %s %d\n", cname, cstate, ccountry, clat, 
				clon, &alt, ctime, &showatzoom) != 8)
			{
				if (feof(fic))
				{
					// Empty city file
					fclose(fic);
					return;		
				}
				cerr << "ERROR while loading city data in line " << line << endl;
				assert(0);
			}
			
			string name, state, country;
			name = cname;
		    for (string::size_type i=0;i<name.length();++i)
			{
				if (name[i]=='_') name[i]=' ';
			}	
			state = cstate;
		    for (string::size_type i=0;i<state.length();++i)
			{
				if (state[i]=='_') state[i]=' ';
			}
			country = ccountry;
		    for (string::size_type i=0;i<country.length();++i)
			{
				if (country[i]=='_') country[i]=' ';
			}	
			if (ctime[0] == 'x')
				time = 0;
			else 
				sscanf(ctime,"%f",&time);
			earth_map->addCity(name, 
				state, 
				country, 
				StelUtils::get_dec_angle(clon), 
				StelUtils::get_dec_angle(clat), time, showatzoom, alt);
		}
		line++;
	}
	fclose(fic);
	cout << "(" << line << " cities loaded)" << endl;
}

// Create Search window widgets
Component* StelUI::createSearchWindow(void)
{
	int x=10; 
	int y=10;

     // Bring up dialog
	search_win = new StdBtWin(_("Object Search"));
	search_win->reshape(300,200,400,100);
	search_win->setVisible(FlagSearch);

	lblSearchMessage = new Label(L"");
	lblSearchMessage->setPos(15, search_win->getSizey()-25);

	Label * lblstars1 = new Label(_("Search for (eg. Saturn, Polaris, HP6218, Orion, M31):"));
	lblstars1->setPos(x, y);
	search_win->addComponent(lblstars1);

	y+=30;

	StelApp::getInstance().getTextureManager().setDefaultParams();
	STextureSP searchp = StelApp::getInstance().getTextureManager().createTexture("bt_search.png");
	Picture* psearch = new Picture(searchp, x, y+1, 24, 24);
	search_win->addComponent(psearch);
	
	star_edit = new EditBox();
	star_edit->setOnReturnKeyCallback(callback<void>(this, &StelUI::gotoSearchedObject));
	star_edit->setOnKeyCallback(callback<void>(this, &StelUI::autoCompleteSearchedObject));
	search_win->addComponent(star_edit);
	star_edit->setPos(x+30,y);
	star_edit->setSize(230,25);
	
	LabeledButton* gobutton = new LabeledButton(_("GO"));
	gobutton->setPos(300, y-2);
	gobutton->setJustification(JUSTIFY_CENTER);
	gobutton->setOnPressCallback(callback<void>(this, &StelUI::gotoSearchedObject));
	
	search_win->addComponent(gobutton);
	search_win->addComponent(lblSearchMessage);
	search_win->setOnHideBtCallback(callback<void>(this, &StelUI::search_win_hideBtCallback));

	return search_win;
}


void StelUI::autoCompleteSearchedObject(void)
{
    wstring objectName = star_edit->getText();
    star_edit->setAutoCompleteOptions(StelApp::getInstance().getStelObjectMgr().listMatchingObjectsI18n(objectName, 5));
    lblSearchMessage->setLabel(star_edit->getAutoCompleteOptions());
}

void StelUI::gotoSearchedObject(void)
{
	MovementMgr* mvmgr = (MovementMgr*)StelApp::getInstance().getModuleMgr().getModule("movements");
	
	if (StelApp::getInstance().getStelObjectMgr().findAndSelectI18n(star_edit->getText()))
	{
		const std::vector<StelObjectP> newSelected = StelApp::getInstance().getStelObjectMgr().getSelectedObject();
		if (!newSelected.empty())
		{
			star_edit->clearText();
			mvmgr->moveTo(newSelected[0]->get_earth_equ_pos(core->getNavigation()),mvmgr->getAutoMoveDuration());
			mvmgr->setFlagTracking(true);
			lblSearchMessage->setLabel(L"");
			  // johannes
			search_win->setVisible(false);
		}
	}
	else
	{
		lblSearchMessage->setLabel(star_edit->getText()+L" is unknown!");
	}
}

void StelUI::updateConfigVariables(void)
{
	NebulaMgr* nmgr = (NebulaMgr*)StelApp::getInstance().getModuleMgr().getModule("nebulas");
	MeteorMgr* metmgr = (MeteorMgr*)StelApp::getInstance().getModuleMgr().getModule("meteors");

	app->commander->execute_command("flag stars ", stars_cbx->getState());
	app->commander->execute_command("flag star_names ", star_names_cbx->getState());
	app->commander->execute_command("set max_mag_star_name ", max_mag_star_name->getValue());
	app->commander->execute_command("flag star_twinkle ", star_twinkle_cbx->getState());
	app->commander->execute_command("set star_twinkle_amount ", star_twinkle_amount->getValue());
	app->commander->execute_command("flag constellation_drawing ", constellation_cbx->getState());
	app->commander->execute_command("flag constellation_names ", constellation_name_cbx->getState());
	app->commander->execute_command("flag constellation_boundaries ", constellation_boundaries_cbx->getState());
	app->commander->execute_command("flag constellation_pick ", sel_constellation_cbx->getState());
	app->commander->execute_command("flag nebula_names ", nebulas_names_cbx->getState());
	app->commander->execute_command("set max_mag_nebula_name ", max_mag_nebula_name->getValue());
	nmgr->setFlagDisplayNoTexture(nebulas_no_texture_cbx->getState());
	app->commander->execute_command("flag planet_names ", planets_hints_cbx->getState());
	app->commander->execute_command("flag moon_scaled ", moon_x4_cbx->getState());
	app->commander->execute_command("flag equatorial_grid ", equator_grid_cbx->getState());
	app->commander->execute_command("flag azimuthal_grid ", azimuth_grid_cbx->getState());
	app->commander->execute_command("flag equator_line ", equator_cbx->getState());
	app->commander->execute_command("flag ecliptic_line ", ecliptic_cbx->getState());
	app->commander->execute_command("flag landscape ", ground_cbx->getState());
	app->commander->execute_command("flag cardinal_points ", cardinal_cbx->getState());
	app->commander->execute_command("flag atmosphere ", atmosphere_cbx->getState());
	app->commander->execute_command("flag fog ", fog_cbx->getState());
	app->commander->execute_command("flag landscape_sets_location ", locationFromLandscapeCheck->getState());
	if (meteor_rate_10->getState() && metmgr->getZHR()!=10)
		app->commander->execute_command("meteors zhr 10");
	else if (meteor_rate_80->getState() && metmgr->getZHR()!=80)
		app->commander->execute_command("meteors zhr 80");
	else if (meteor_rate_10000->getState() && metmgr->getZHR()!=10000)
		app->commander->execute_command("meteors zhr 10000");
	else if (meteor_rate_144000->getState() && metmgr->getZHR()!=144000)
		app->commander->execute_command("meteors zhr 144000");
}

void StelUI::updateConfigVariables2(void)
{
	app->commander->execute_command("flag planets ", planets_cbx->getState());
}

void StelUI::setCurrentTimeFromConfig(void)
{
	//	core->navigation->set_JDay(time_current->getJDay() - core->observatory->get_GMT_shift()*JD_HOUR);
	app->commander->execute_command(string("date local " + time_current->getDateString()));
}

void StelUI::setObserverPositionFromMap(void)
{
	std::ostringstream oss;
	oss << "moveto lat " << earth_map->getPointerLatitude() << " lon " << earth_map->getPointerLongitude()
		<< " alt " << earth_map->getPointerAltitude();
	app->commander->execute_command(oss.str());
}

void StelUI::setCityFromMap(void)
{
	waitOnLocation = false;
	lblMapLocation->setLabel(StelUtils::stringToWstring(earth_map->getCursorString()));
	lblMapPointer->setLabel(StelUtils::stringToWstring(earth_map->getPositionString()));	
}

void StelUI::setObserverPositionFromIncDec(void)
{
	std::ostringstream oss;
	oss.setf(ios::fixed);
	oss.precision(10);
	oss << "moveto lat " << lat_incdec->getValue() << " lon " << long_incdec->getValue()
		<< " alt " << alt_incdec->getValue();
	app->commander->execute_command(oss.str());
}

void StelUI::doSaveObserverPosition(const string& name)
{
	string location = name;
    for (string::size_type i=0;i<location.length();++i)
	{
		if (location[i]==' ') location[i]='_';
	}

	std::ostringstream oss;
	oss << "moveto lat " << lat_incdec->getValue() << " lon " << long_incdec->getValue()
		<< " name " << location;
	app->commander->execute_command(oss.str());

	core->getObservatory()->save(app->getConfigFilePath(), "init_location");
	app->ui->setTitleObservatoryName(app->ui->getTitleWithAltitude());
}

void StelUI::saveObserverPosition(void)
{
	string location = earth_map->getPositionString();

	if (location == UNKNOWN_OBSERVATORY)	
		dialog_win->InputBox(L"Stellarium",_("Enter observatory name"), "observatory name");
	else
		doSaveObserverPosition(location);

}

void StelUI::saveLandscapeOptions(void)
{
	cout << "Saving landscape name in file " << app->getConfigFilePath() << endl;
	InitParser conf;
	conf.load(app->getConfigFilePath());
	LandscapeMgr* lmgr = (LandscapeMgr*)app->getModuleMgr().getModule("landscape");
	conf.set_str("init_location:landscape_name", lmgr->getLandscapeId());
	conf.set_boolean("landscape:flag_landscape_sets_location", lmgr->getFlagLandscapeSetsLocation());
	conf.save(app->getConfigFilePath());
}

void StelUI::setLandscapeUpdatesLocation(void)
{
	LandscapeMgr* lmgr = (LandscapeMgr*)app->getModuleMgr().getModule("landscape");
	lmgr->setFlagLandscapeSetsLocation(locationFromLandscapeCheck->getState());
	InitParser conf;
	conf.load(app->getConfigFilePath());
	conf.set_boolean("landscape:flag_landscape_sets_location", lmgr->getFlagLandscapeSetsLocation());
	if (lmgr->getFlagLandscapeSetsLocation())
	{
		cout << "Landscape changes will now update the location" << endl;
	}
	else
	{
		cout << "Landscape changes not update the location" << endl;
	}
}

void StelUI::saveLanguageOptions(void)
{
	cout << "Saving language in file " << app->getConfigFilePath() << endl;
	InitParser conf;
	conf.load(app->getConfigFilePath());
	conf.set_str("localization:sky_locale", app->getLocaleMgr().getSkyLanguage());
	conf.set_str("localization:app_locale", app->getLocaleMgr().getAppLanguage());
	conf.set_str("localization:sky_culture", app->getSkyCultureMgr().getSkyCultureDir());
	conf.save(app->getConfigFilePath());
}

void StelUI::saveRenderOptions(void)
{
	cout << "Saving rendering options in file " << app->getConfigFilePath() << endl;

	InitParser conf;
	conf.load(app->getConfigFilePath());

	StarMgr* smgr = (StarMgr*)StelApp::getInstance().getModuleMgr().getModule("stars");
	ConstellationMgr* cmgr = (ConstellationMgr*)StelApp::getInstance().getModuleMgr().getModule("constellations");
	NebulaMgr* nmgr = (NebulaMgr*)StelApp::getInstance().getModuleMgr().getModule("nebulas");
	SolarSystem* ssmgr = (SolarSystem*)StelApp::getInstance().getModuleMgr().getModule("ssystem");
	LandscapeMgr* lmgr = (LandscapeMgr*)StelApp::getInstance().getModuleMgr().getModule("landscape");
	GridLinesMgr* grlmgr = (GridLinesMgr*)StelApp::getInstance().getModuleMgr().getModule("gridlines");
	MeteorMgr* metmgr = (MeteorMgr*)StelApp::getInstance().getModuleMgr().getModule("meteors");
	
	conf.set_boolean("astro:flag_stars", smgr->getFlagStars());
	conf.set_boolean("astro:flag_star_name", smgr->getFlagNames());
	conf.set_double("stars:max_mag_star_name", smgr->getMaxMagName());
	conf.set_boolean("stars:flag_star_twinkle", smgr->getFlagTwinkle());
	conf.set_double("stars:star_twinkle_amount", smgr->getTwinkleAmount());
	
	conf.set_boolean("viewing:flag_constellation_drawing", cmgr->getFlagLines());
	conf.set_boolean("viewing:flag_constellation_name", cmgr->getFlagNames());
	conf.set_boolean("viewing:flag_constellation_boundaries", cmgr->getFlagBoundaries());
	conf.set_boolean("viewing:flag_constellation_pick", cmgr->getFlagIsolateSelected());
	
	conf.set_boolean("astro:flag_nebula", nmgr->getFlagShow());
	conf.set_boolean("astro:flag_nebula_name", nmgr->getFlagHints());
	conf.set_double("astro:max_mag_nebula_name", nmgr->getMaxMagHints());
	conf.set_boolean("astro:flag_nebula_display_no_texture", nmgr->getFlagDisplayNoTexture());
	
	conf.set_boolean("astro:flag_planets", ssmgr->getFlagPlanets());
	conf.set_boolean("astro:flag_planets_hints", ssmgr->getFlagHints());
	conf.set_double("viewing:moon_scale", ssmgr->getMoonScale());
	conf.set_boolean("viewing:flag_moon_scaled", ssmgr->getFlagMoonScale());
	conf.set_boolean("viewing:flag_night", app->getVisionModeNight());
	conf.set_boolean("viewing:flag_equatorial_grid", grlmgr->getFlagEquatorGrid());
	conf.set_boolean("viewing:flag_azimutal_grid", grlmgr->getFlagAzimutalGrid());
	conf.set_boolean("viewing:flag_equator_line", grlmgr->getFlagEquatorLine());
	conf.set_boolean("viewing:flag_ecliptic_line", grlmgr->getFlagEclipticLine());
	conf.set_boolean("landscape:flag_landscape", lmgr->getFlagLandscape());
	conf.set_boolean("viewing:flag_cardinal_points", lmgr->getFlagCardinalsPoints());
	conf.set_boolean("landscape:flag_atmosphere", lmgr->getFlagAtmosphere());
	conf.set_boolean("landscape:flag_fog", lmgr->getFlagFog());
	conf.set_int("astro:meteor_rate", metmgr->getZHR());
	conf.save(app->getConfigFilePath());
}

void StelUI::setVideoOption(void)
{
	string s = StelUtils::wstringToString(screen_size_sl->getCurrent());
	int i = s.find("x");
	int w = atoi(s.substr(0,i).c_str());
	int h = atoi(s.substr(i+1,s.size()).c_str());

        // cheap hack to prevent bug #1483662 - MNG, 20060508
	cout << "Saving video settings: projection=" << core->getProjection()->getCurrentProjection()
	     << ", distorter=" << app->getViewPortDistorterType();
	if ( w && h ) cout << ", res=" << w << "x" << h;
	cout << " in file " << app->getConfigFilePath() << endl;

	InitParser conf;
	conf.load(app->getConfigFilePath());

	conf.set_str("projection:type", core->getProjection()->getCurrentProjection());
	conf.set_str("video:distorter", app->getViewPortDistorterType());


	if (core->getProjection()->getViewportMaskDisk()) conf.set_str("projection:viewport", "disk");
	else conf.set_str("projection:viewport", "maximized");

	if ( w && h ) {   
		conf.set_int("video:screen_w", w);
		conf.set_int("video:screen_h", h);
	}

	conf.save(app->getConfigFilePath());
}

void StelUI::setLandscape(void)
{
	LandscapeMgr* lmgr = (LandscapeMgr*)StelApp::getInstance().getModuleMgr().getModule("landscape");
	lmgr->setLandscape(lmgr->nameToKey(landscape_sl->getValue()));
	landscape_authorlb->setLabel(_("Author: ") + lmgr->getLandscapeAuthorName());
	landscape_descriptionlb->setLabel(_("Info: ") + lmgr->getLandscapeDescription());	
	landscapePlanetLb->setLabel(_("Planet: ") + lmgr->getLandscapePlanetName());
	landscapeLocationLb->setLabel(_("Location: ") + lmgr->getLandscapeLocationDescription());
}

void StelUI::updateVideoVariables(void)
{
	core->getProjection()->setCurrentProjection(projection_sl->getValue());
    
	if (disk_viewport_cbx->getState() && !core->getProjection()->getViewportMaskDisk())
	{
		core->getProjection()->setViewportMaskDisk();
	}
	if (!disk_viewport_cbx->getState() && core->getProjection()->getViewportMaskDisk())
	{
		core->getProjection()->setViewportMaskNone();
	}

	app->setViewPortDistorterType(viewport_distorter_cbx->getState()
	                              ? "fisheye_to_spheric_mirror" : "none");
}

void StelUI::updateConfigForm(void)
{
	// Stars
	StarMgr* smgr = (StarMgr*)StelApp::getInstance().getModuleMgr().getModule("stars");
	ConstellationMgr* cmgr = (ConstellationMgr*)StelApp::getInstance().getModuleMgr().getModule("constellations");
	NebulaMgr* nmgr = (NebulaMgr*)StelApp::getInstance().getModuleMgr().getModule("nebulas");
	SolarSystem* ssmgr = (SolarSystem*)StelApp::getInstance().getModuleMgr().getModule("ssystem");
	LandscapeMgr* lmgr = (LandscapeMgr*)StelApp::getInstance().getModuleMgr().getModule("landscape");
	GridLinesMgr* grlmgr = (GridLinesMgr*)StelApp::getInstance().getModuleMgr().getModule("gridlines");
	MeteorMgr* metmgr = (MeteorMgr*)StelApp::getInstance().getModuleMgr().getModule("meteors");
	
	stars_cbx->setState(smgr->getFlagStars());
	star_names_cbx->setState(smgr->getFlagNames());
	max_mag_star_name->setValue(smgr->getMaxMagName());
	star_twinkle_cbx->setState(smgr->getFlagTwinkle());
	star_twinkle_amount->setValue(smgr->getTwinkleAmount());
	
	// Constellations
	constellation_cbx->setState(cmgr->getFlagLines());
	constellation_name_cbx->setState(cmgr->getFlagNames());
	constellation_boundaries_cbx->setState(cmgr->getFlagBoundaries());
	sel_constellation_cbx->setState(cmgr->getFlagIsolateSelected());
	
	// Nebulas
	nebulas_names_cbx->setState(nmgr->getFlagHints());
	max_mag_nebula_name->setValue(nmgr->getMaxMagHints());
	nebulas_no_texture_cbx->setState(nmgr->getFlagDisplayNoTexture());
	
	// Planets
	planets_cbx->setState(ssmgr->getFlagPlanets());
	planets_hints_cbx->setState(ssmgr->getFlagHints());
	moon_x4_cbx->setState(ssmgr->getFlagMoonScale());
	equator_grid_cbx->setState(grlmgr->getFlagEquatorGrid());
	azimuth_grid_cbx->setState(grlmgr->getFlagAzimutalGrid());
	equator_cbx->setState(grlmgr->getFlagEquatorLine());
	ecliptic_cbx->setState(grlmgr->getFlagEclipticLine());
	ground_cbx->setState(lmgr->getFlagLandscape());
	cardinal_cbx->setState(lmgr->getFlagCardinalsPoints());
	atmosphere_cbx->setState(lmgr->getFlagAtmosphere());
	fog_cbx->setState(lmgr->getFlagFog());

	wstring meteorRate;
	if (metmgr->getZHR()==10)
	{
		meteorRate = _(": Normal rate");
		meteor_rate_10->setState(true);
	}
	else
		meteor_rate_10->setState(false);
	if (metmgr->getZHR()==80)
	{
		meteorRate = _(": Standard Perseids rate");
		meteor_rate_80->setState(true);
	}
	else
		meteor_rate_80->setState(false);
	if (metmgr->getZHR()==10000)
	{
		meteorRate = _(": Exceptional Leonid rate");
		meteor_rate_10000->setState(true);
	}
	else
		meteor_rate_10000->setState(false);
	if (metmgr->getZHR()==144000)
	{
		meteorRate = _(": Highest rate ever (1966 Leonids)");
		meteor_rate_144000->setState(true);
	}
	else
		meteor_rate_144000->setState(false);
	meteorlbl->setLabel(_("Meteor zenith hourly rate")+meteorRate);

	earth_map->setPointerLongitude(core->getObservatory()->get_longitude());
	earth_map->setPointerLatitude(core->getObservatory()->get_latitude());
	long_incdec->setValue(core->getObservatory()->get_longitude());
	lat_incdec->setValue(core->getObservatory()->get_latitude());
	alt_incdec->setValue(core->getObservatory()->get_altitude());
	lblMapLocation->setLabel(StelUtils::stringToWstring(earth_map->getCursorString()));
	if (!waitOnLocation)
		lblMapPointer->setLabel(StelUtils::stringToWstring(earth_map->getPositionString()));
	else
	{
		earth_map->findPosition(core->getObservatory()->get_longitude(),core->getObservatory()->get_latitude());
		lblMapPointer->setLabel(StelUtils::stringToWstring(earth_map->getPositionString()));
		waitOnLocation = false;
	}
	
	const Planet* p = core->getObservatory()->getHomePlanet();
	if (p != mapLastPlanet) {
		updatePlanetMap(core->getObservatory()->getHomePlanetEnglishName());
		mapLastPlanet = const_cast<Planet*>(p);
	}

	time_current->setJDay(core->getNavigation()->getJDay() + app->getLocaleMgr().get_GMT_shift(core->getNavigation()->getJDay())*JD_HOUR);
	system_tz_lbl2->setLabel(L"(" + get_time_zone_name_from_system(core->getNavigation()->getJDay()) + L")");

	time_speed_lbl2->setLabel(wstring(L"\u2022 ")+_("Current Time Speed is x") + StelUtils::doubleToWstring(core->getNavigation()->getTimeSpeed()/JD_SECOND));

	projection_sl->setValue(core->getProjection()->getCurrentProjection());
	disk_viewport_cbx->setState(core->getProjection()->getViewportMaskDisk());
	viewport_distorter_cbx->setState(app->getViewPortDistorterType()!="none");
	
	locationFromLandscapeCheck->setState(lmgr->getFlagLandscapeSetsLocation());
}

void StelUI::config_win_hideBtCallback(void)
{
	FlagConfig = false;
	config_win->setVisible(false);
	// for MapPicture - when the dialog appears, this tells the system
	// not to show the city until MapPicture has located the name
	// from the lat and long.
	waitOnLocation = true;
	bt_flag_config->setState(0);
}

void StelUI::search_win_hideBtCallback(void)
{
	FlagSearch = false;
	search_win->setVisible(false);
	bt_flag_search->setState(0);
}

void StelUI::updatePlanetMap(const string& englishName)
{
	SolarSystem* ssystem = (SolarSystem*)StelApp::getInstance().getModuleMgr().getModule("ssystem");
	Planet* planetObject = (Planet*)(ssystem->searchByEnglishName(englishName));
	if (!planetObject)
		return;
	
	STextureSP newTex = planetObject->getMapTexture();
	if (newTex)
		earth_map->setMapTexture(newTex);
	else
		cerr << "WARNING StelUI::updatePlanetMap no texture found for body: " << englishName << endl;
	
	load_cities(englishName);	
}
