/*************************************************************************************
 * Copyright (C) 2018 by Charly Lamothe                                              *
 *                                                                                   *
 * This file is part of LibSharedMemorySlot.                                         *
 *                                                                                   *
 *   LibSharedMemorySlot is free software: you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by            *
 *   the Free Software Foundation, either version 3 of the License, or               *
 *   (at your option) any later version.                                             *
 *                                                                                   *
 *   LibSharedMemorySlot is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of                  *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                   *
 *   GNU General Public License for more details.                                    *
 *                                                                                   *
 *   You should have received a copy of the GNU General Public License               *
 *   along with LibSharedMemorySlot.  If not, see <http://www.gnu.org/licenses/>.    *
 ************************************************************************************/

/**
 *  @file      bool.h
 *  @brief     Portable way to represent bool type.
 *  @author    Charly Lamothe
 *  @copyright GNU Public License.
 */

#ifndef SHAREDMEMORYOBJECT_BOOL_H
#define SHAREDMEMORYOBJECT_BOOL_H

/* C99 */
#if __STDC_VERSION__ >= 199901L

#include <stdbool.h>

#elif defined(_MSC_VER)

#define bool _Bool
#define true 1
#define false 0
#define __bool_true_false_are_defined 1

#else /* Not C99 */

typedef enum {
    false,
    true
} bool;

#endif

#endif /* BOOL_H */
