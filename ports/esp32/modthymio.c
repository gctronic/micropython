/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 "Eric Poulsen" <eric@zyxod.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <string.h>

#include <time.h>
#include <sys/time.h>
#include "soc/rtc_cntl_reg.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_heap_caps.h"

#include "py/nlr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "shared/timeutils/timeutils.h"
#include "modmachine.h"
#include "thymio_leds_circle.h"
#include "thymio_proximity.h"
#include "thymio_ground.h"
#include "thymio_motors.h"
#include "thymio_filesystem.h"
#include "thymio_sound.h"
#include "thymio_leds_lego_front.h"
#include "thymio_leds_lego_back.h"
#include "thymio_leds_rgb.h"
#include "thymio_rc5.h"
#include "thymio_imu.h"
#include "../../../../../main/mode.h"
#include "../../../../../main/utility.h"

STATIC mp_obj_t disable_behaviors(void) {
    enter_micropython_mode();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(disable_behaviors_obj, disable_behaviors);

STATIC mp_obj_t enable_behaviors(void) {
    exit_micropython_mode();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(enable_behaviors_obj, enable_behaviors);

STATIC mp_obj_t turn_off_all(void) {
    turnOffAllSensors();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(turn_off_all_obj, turn_off_all);

STATIC const mp_rom_map_elem_t thymio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_thymio) },

    { MP_ROM_QSTR(MP_QSTR_disable_behaviors), MP_ROM_PTR(&disable_behaviors_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_behaviors), MP_ROM_PTR(&enable_behaviors_obj) },
    { MP_ROM_QSTR(MP_QSTR_turn_off_all), MP_ROM_PTR(&turn_off_all_obj) },

    { MP_ROM_QSTR(MP_QSTR_LEDS_CIRCLE), MP_ROM_PTR(&thymio_leds_circle_type) },
    { MP_ROM_QSTR(MP_QSTR_PROXIMITY), MP_ROM_PTR(&thymio_proximity_type) },
    { MP_ROM_QSTR(MP_QSTR_GROUND), MP_ROM_PTR(&thymio_ground_type) },
    { MP_ROM_QSTR(MP_QSTR_MOTORS), MP_ROM_PTR(&thymio_motors_type) },
    { MP_ROM_QSTR(MP_QSTR_FILESYSTEM), MP_ROM_PTR(&thymio_filesystem_type) },
    { MP_ROM_QSTR(MP_QSTR_SOUND), MP_ROM_PTR(&thymio_sound_type) },
    { MP_ROM_QSTR(MP_QSTR_LEDS_LEGO_FRONT), MP_ROM_PTR(&thymio_leds_lego_front_type) },
    { MP_ROM_QSTR(MP_QSTR_LEDS_LEGO_BACK), MP_ROM_PTR(&thymio_leds_lego_back_type) },
    { MP_ROM_QSTR(MP_QSTR_LEDS_RGB), MP_ROM_PTR(&thymio_leds_rgb_type) },
    { MP_ROM_QSTR(MP_QSTR_RC5), MP_ROM_PTR(&thymio_rc5_type) },
    { MP_ROM_QSTR(MP_QSTR_IMU), MP_ROM_PTR(&thymio_imu_type) },
};

STATIC MP_DEFINE_CONST_DICT(tyhmio_module_globals, thymio_module_globals_table);

const mp_obj_module_t thymio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&tyhmio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_thymio, thymio_module);
