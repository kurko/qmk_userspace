/* Copyright 2022 Alex Oliveira <@kurko>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#ifdef ENCODER_ENABLE
#    define ENCODER_RESOLUTIONS { 2 }
#    define ENCODER_RESOLUTIONS_RIGHT { 2 }
#endif

#ifdef RGBLIGHT_ENABLE
#    define RGBLIGHT_ANIMATIONS
#    define RGBLIGHT_HUE_STEP  8
#    define RGBLIGHT_SAT_STEP  8
#    define RGBLIGHT_VAL_STEP  8
#    define RGBLIGHT_LIMIT_VAL 150
#endif

// Lets you roll mod-tap keys
// IGNORE_MOD_TAP_INTERRUPT is now default behavior in QMK
// https://docs.qmk.fm/#/tap_hold?id=tapping-term
#define TAPPING_TERM 180
#define TAPPING_TERM_PER_KEY
