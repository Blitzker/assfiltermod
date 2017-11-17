/*
 *   Copyright(C) 2016-2017 Blitzker
 *
 *   This program is free software : you can redistribute it and / or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#define ASSF_VERSION_MAJOR     0
#define ASSF_VERSION_MINOR     3
#define ASSF_VERSION_REVISION  0

#include "../version_rev.h"

/////////////////////////////////////////////////////////
#ifndef ISPP_INCLUDED

#define DO_MAKE_STR(x) #x
#define MAKE_STR(x) DO_MAKE_STR(x)

#define ASSF_VERSION ASSF_VERSION_MAJOR.ASSF_VERSION_MINOR.ASSF_VERSION_REVISION
#define ASSF_VERSION_TAG ASSF_VERSION_MAJOR, ASSF_VERSION_MINOR, ASSF_VERSION_REVISION

#define ASSF_VERSION_STR MAKE_STR(ASSF_VERSION)
#define ASSF_HASH_STR MAKE_STR(ASSF_VERSION_HASH)

#endif
