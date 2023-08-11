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
#include "thymio_imu.h"
#include "../../../../../main/accelerometer.h"
#include "../../../../../main/gyroscope.h"

T_Axis acc_temp, gyro_temp;

/// \moduleref thymio
/// \class IMU - IMU object
///
/// The IMU object get imu accelerometer and gyroscope values.

typedef struct _thymio_imu_obj_t {
    mp_obj_base_t base;
} thymio_imu_obj_t;

void imu_init(void) {
}

T_Axis imu_get_acc(void) {
    return Accelerometer_GetAcceleration();
}

T_Axis imu_get_gyro(void) {
    return Gyroscope_GetAngularVelocity();
}

/******************************************************************************/
/* MicroPython bindings                                                       */

void imu_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_printf(print, "IMU");
}

/// \classmethod \constructor()
/// Create a IMU object:
STATIC mp_obj_t imu_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    thymio_imu_obj_t *imu = m_new_obj(thymio_imu_obj_t);
    imu->base.type = &thymio_imu_type;
    return MP_OBJ_FROM_PTR(imu);
}

/// \method imu_get_acc()
/// Get accelerometer values.
mp_obj_t imu_get_acceleration(mp_obj_t self_in) {
    mp_obj_list_t *data = MP_OBJ_TO_PTR(mp_obj_new_list(3, NULL));
    acc_temp = imu_get_acc();
    data->items[0] = mp_obj_new_int(acc_temp.X);
    data->items[1] = mp_obj_new_int(acc_temp.Y);
    data->items[2] = mp_obj_new_int(acc_temp.Z);
    return MP_OBJ_FROM_PTR(data);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(imu_get_acceleration_obj, imu_get_acceleration);

/// \method imu_get_gyro()
/// Get gyroscope values.
mp_obj_t imu_get_gyroscope(mp_obj_t self_in) {
    mp_obj_list_t *data = MP_OBJ_TO_PTR(mp_obj_new_list(3, NULL));
    gyro_temp = imu_get_gyro();
    data->items[0] = mp_obj_new_int(gyro_temp.X);
    data->items[1] = mp_obj_new_int(gyro_temp.Y);
    data->items[2] = mp_obj_new_int(gyro_temp.Z);
    return MP_OBJ_FROM_PTR(data);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(imu_get_gyroscope_obj, imu_get_gyroscope);

STATIC const mp_rom_map_elem_t imu_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_get_acc), MP_ROM_PTR(&imu_get_acceleration_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_gyro), MP_ROM_PTR(&imu_get_gyroscope_obj) },
};

STATIC MP_DEFINE_CONST_DICT(imu_locals_dict, imu_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    thymio_imu_type,
    MP_QSTR_IMU,
    MP_TYPE_FLAG_NONE,
    make_new, imu_make_new,
    print, imu_print,
    locals_dict, &imu_locals_dict
    );

