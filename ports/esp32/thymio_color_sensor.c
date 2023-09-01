/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2016 Damien P. George
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "thymio_color_sensor.h"

T_HSV hsv_temp;
T_RawColor raw_temp;
T_RawColor calib_temp;

/// \moduleref thymio
/// \class COLOR_SENSOR - COLOR_SENSOR object
///
/// The COLOR_SENSOR object get color_sensor accelerometer and gyroscope values.

typedef struct _thymio_color_sensor_obj_t {
    mp_obj_base_t base;
} thymio_color_sensor_obj_t;

void color_sensor_init(void) {
}

T_HSV color_sensor_get_hsv(void) {
    return ColorSensor_GetHsv();
}

T_RawColor color_sensor_get_raw(void) {
    return ColorSensor_GetRaw();
}

T_RawColor color_sensor_get_calib_white(void) {
    return ColorSensor_GetWhiteCalibration();
}

T_RawColor color_sensor_get_calib_black(void) {
    return ColorSensor_GetBlackCalibration();
}


/******************************************************************************/
/* MicroPython bindings                                                       */

void color_sensor_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_printf(print, "COLOR_SENSOR");
}

/// \classmethod \constructor()
/// Create a COLOR_SENSOR object:
STATIC mp_obj_t color_sensor_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    thymio_color_sensor_obj_t *color_sensor = m_new_obj(thymio_color_sensor_obj_t);
    color_sensor->base.type = &thymio_color_sensor_type;
    return MP_OBJ_FROM_PTR(color_sensor);
}

/// \method get_hsv()
/// Get HSV values.
mp_obj_t color_sensor_get_hsv_values(mp_obj_t self_in) {
    mp_obj_list_t *data = MP_OBJ_TO_PTR(mp_obj_new_list(3, NULL));
    hsv_temp = color_sensor_get_hsv();
    data->items[0] = mp_obj_new_int(hsv_temp.Hue);
    data->items[1] = mp_obj_new_int(hsv_temp.Saturation);
    data->items[2] = mp_obj_new_int(hsv_temp.Value);
    return MP_OBJ_FROM_PTR(data);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(color_sensor_get_hsv_values_obj, color_sensor_get_hsv_values);

/// \method get_raw()
/// Get RGB raw values.
mp_obj_t color_sensor_get_raw_values(mp_obj_t self_in) {
    mp_obj_list_t *data = MP_OBJ_TO_PTR(mp_obj_new_list(3, NULL));
    raw_temp = color_sensor_get_raw();
    data->items[0] = mp_obj_new_int(raw_temp.Red);
    data->items[1] = mp_obj_new_int(raw_temp.Green);
    data->items[2] = mp_obj_new_int(raw_temp.Blue);
    return MP_OBJ_FROM_PTR(data);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(color_sensor_get_raw_values_obj, color_sensor_get_raw_values);

/// \method get_calibration()
/// Get calibration values (calibration done in both white and black surfaces): [red white, green white, blue white, red black, green black, blue black].
mp_obj_t color_sensor_get_calibration(mp_obj_t self_in) {
    mp_obj_list_t *data = MP_OBJ_TO_PTR(mp_obj_new_list(6, NULL));
    calib_temp = color_sensor_get_calib_white();
    data->items[0] = mp_obj_new_int(calib_temp.Red);
    data->items[1] = mp_obj_new_int(calib_temp.Green);
    data->items[2] = mp_obj_new_int(calib_temp.Blue);
    calib_temp = color_sensor_get_calib_black();
    data->items[3] = mp_obj_new_int(calib_temp.Red);
    data->items[4] = mp_obj_new_int(calib_temp.Green);
    data->items[5] = mp_obj_new_int(calib_temp.Blue);    
    return MP_OBJ_FROM_PTR(data);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(color_sensor_get_calibration_obj, color_sensor_get_calibration);

STATIC const mp_rom_map_elem_t color_sensor_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_get_hsv), MP_ROM_PTR(&color_sensor_get_hsv_values_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_raw), MP_ROM_PTR(&color_sensor_get_raw_values_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_calibration), MP_ROM_PTR(&color_sensor_get_calibration_obj) },
};

STATIC MP_DEFINE_CONST_DICT(color_sensor_locals_dict, color_sensor_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    thymio_color_sensor_type,
    MP_QSTR_COLOR_SENSOR,
    MP_TYPE_FLAG_NONE,
    make_new, color_sensor_make_new,
    print, color_sensor_print,
    locals_dict, &color_sensor_locals_dict
    );

