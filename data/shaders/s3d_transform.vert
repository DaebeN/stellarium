/*
 * Stellarium Scenery3d Plug-in
 *
 * Copyright (C) 2014 Simon Parzer, Peter Neubauer, Georg Zotti, Andrei Borza, Florian Schaukowitsch
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
 
 
/*
This is a shader for MVP transformation only. Used to fill depth maps.
*/
 
#version 110
#define ALPHATEST 1

//matrices
uniform mat4 u_mMVP;

attribute vec4 a_vertex;
#if ALPHATEST
attribute vec2 a_texcoord;
varying vec2 v_texcoord;
#endif

void main(void)
{
#if ALPHATEST
	v_texcoord = a_texcoord;
#endif
	gl_Position = u_mMVP * a_vertex;
}