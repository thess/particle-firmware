/*
 * Copyright (c) 2019 Particle Industries, Inc.  All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CELLULAR_SIG_MAPPING_HAL_H
#define CELLULAR_SIG_MAPPING_HAL_H

#include "logging.h"
#include "net_hal.h"
#include "string.h"
#include <stdlib.h>
#include <limits>

#ifdef __cplusplus
extern "C" {
#endif

/*
*   Convert strength in dBm into percetage
*/
int get_scaled_strn(int strn_value, hal_net_access_tech_t access_tech);

/*
*   Conver quality in dBm into percetage
*/
int get_scaled_qual(int qual_value, hal_net_access_tech_t access_tech);

#ifdef __cplusplus
}
#endif

#endif // CELLULAR_SIG_MAPPING_HAL_H