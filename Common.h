/*
 * Copyright Bar Smith, Bryant Mairs 2012
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses.
 */

/**
 * @file   Common.h
 * @author Bryant Mairs
 * @date   August, 2012
 * @brief  Provides a common include file for datatypes and error messages.
 */
#ifndef _COMMON_H_
#define _COMMON_H_

// The fixed-size types (uint8_t, etc.) are included via this header.
#include <stdint.h>

// The standard boolean values of 'true' and 'false' are included through the stdbool.h library.
#include <stdbool.h>

// Error codes for use as fucntion return values.
enum {
    SIZE_ERROR = -1,        // Return value for an error when used with an output that is normally >= 0.
    STANDARD_ERROR = false, // Return value for an error. Semanticaly a little more clear than `false`
    SUCCESS = true          // Return value for a successful function call.
};

#endif // _COMMON_H_
