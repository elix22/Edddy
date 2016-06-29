/* Edddy
// Copyright (C) 2016 LucKey Productions (luckeyproductions.nl)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "intvector3.h"

const IntVector3 IntVector3::ZERO;
const IntVector3 IntVector3::LEFT(  -1, 0, 0);
const IntVector3 IntVector3::RIGHT(  1, 0, 0);
const IntVector3 IntVector3::UP(     0, 1, 0);
const IntVector3 IntVector3::DOWN(   0,-1, 0);
const IntVector3 IntVector3::FORWARD(0, 0, 1);
const IntVector3 IntVector3::BACK(   0, 0,-1);
const IntVector3 IntVector3::ONE(    1, 1, 1);
