/*
 * Copyright (C) 2007 Fabien Chereau
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

#ifndef _STELGRIDOBJECT_HPP_
#define _STELGRIDOBJECT_HPP_

#include <QSharedPointer>
#include "VecMath.hpp"

//! Simple abstract class defining the method getPositionForGrid() used
//! by the grid algorithms to get a permanent (fixed) position.
class StelGridObject
{
	public:
		virtual ~StelGridObject(void) {;}

		//! This method is used by the grid algorithms to get a permanent position for an object
		//! @return a unit vector giving a permanent direction in a 3d coordinate system.
		virtual Vec3d getPositionForGrid() const=0;
};

//! @typedef StelGridObjectP
//! Shared pointer on a StelGridObject with smart pointers
typedef QSharedPointer<StelGridObject> StelGridObjectP;

#endif // _STELGRIDOBJECT_HPP_
