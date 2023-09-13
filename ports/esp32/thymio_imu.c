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
#include "../../../../../main/angle_controller.h"

T_Axis acc_temp, gyro_temp;
int16_t calib_temp[3];

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

int16_t imu_get_angle_deg(void) {
    return Gyroscope_GetAngleZ_deg();
}

void imu_reset_angle(void) {
    Gyroscope_ResetAngle();
}

void imu_get_gyro_calibration(int16_t *values) {
    return Gyroscope_GetCalibration(values);
}

void imu_reset_gyro_calibration(void) {
    Gyroscope_ResetCalibration();
}

void imu_calibrate_gyro(void) {
    Gyroscope_Calibrate();
}

void imu_rotate_deg(int16_t angle, int16_t speed) {
    AngleController_Start(angle, speed);
}

bool imu_rotation_is_complete(void) {
    return AngleController_Completed();
}

void imu_enable_gyro_continuous_calibration(void) {
    Gyroscope_EnableContinuousCalib();
}

void imu_disable_gyro_continuous_calibration(void) {
    Gyroscope_DisableContinuousCalib();
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
/// Get raw accelerometer values. Values between -32768 and 32767 (scale = +- 2g).
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
/// Get raw gyroscope values. Values between -32768 and 32767 (scale = +- 500 dps).
mp_obj_t imu_get_gyroscope(mp_obj_t self_in) {
    mp_obj_list_t *data = MP_OBJ_TO_PTR(mp_obj_new_list(3, NULL));
    gyro_temp = imu_get_gyro();
    data->items[0] = mp_obj_new_int(gyro_temp.X);
    data->items[1] = mp_obj_new_int(gyro_temp.Y);
    data->items[2] = mp_obj_new_int(gyro_temp.Z);
    return MP_OBJ_FROM_PTR(data);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(imu_get_gyroscope_obj, imu_get_gyroscope);

/// \method imu_get_angle_deg()
/// Get yaw angle in degrees.
mp_obj_t imu_get_angle_deg_(mp_obj_t self_in) {
    return mp_obj_new_int(imu_get_angle_deg());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(imu_get_angle_deg_obj, imu_get_angle_deg_);

/// \method imu_reset_angle()
/// Reset the angle (set degrees to 0).
mp_obj_t imu_reset_angle_(mp_obj_t self_in) {
    imu_reset_angle();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(imu_reset_angle_obj, imu_reset_angle_);

/// \method imu_get_gyro_calibration()
/// Get gyroscope calibration values.
mp_obj_t imu_get_gyro_calibration_(mp_obj_t self_in) {
    mp_obj_list_t *data = MP_OBJ_TO_PTR(mp_obj_new_list(3, NULL));
    imu_get_gyro_calibration(calib_temp);
    data->items[0] = mp_obj_new_int(calib_temp[0]);
    data->items[1] = mp_obj_new_int(calib_temp[1]);
    data->items[2] = mp_obj_new_int(calib_temp[2]);
    return MP_OBJ_FROM_PTR(data);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(imu_get_gyro_calibration_obj, imu_get_gyro_calibration_);

/// \method imu_reset_gyro_calibration()
/// Reset the gyroscope calibration values (set to 0).
mp_obj_t imu_reset_gyro_calibration_(mp_obj_t self_in) {
    imu_reset_gyro_calibration();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(imu_reset_gyro_calibration_obj, imu_reset_gyro_calibration_);

/// \method imu_calibrate_gyro()
/// Make a gyroscope calibration, make sure the automatic calibration is off.
mp_obj_t imu_calibrate_gyro_(mp_obj_t self_in) {
    imu_calibrate_gyro();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(imu_calibrate_gyro_obj, imu_calibrate_gyro_);

/// \method imu_rotate_deg()
/// Tell the robot to rotate by "angle" degrees (-360..360) at maximum speed "speed" (0..1000).
mp_obj_t imu_rotate_deg_(mp_obj_t self_in, mp_obj_t angle, mp_obj_t speed) {
    int a = mp_obj_get_int(angle);
    int s = mp_obj_get_int(speed);
    // check paramters range
    if ((a > 360) || (a < -360)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Target angle must be between -360 and 360"));
    }
    if ((s > 1000) || (s < 0)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Maximum speed must be between 0 and 1000"));
    }    
    AngleController_Start(a, s);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(imu_rotate_deg_obj, imu_rotate_deg_);

/// \method imu_rotation_is_complete()
/// Tell if the angle controller completed the rotation, i.e. reached the target position given with "rotate_deg".
/// Return true if the robot reached the target position given by "rotate_deg".
mp_obj_t imu_rotation_is_complete_(mp_obj_t self_in) {
    return mp_obj_new_bool(AngleController_Completed());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(imu_rotation_is_complete_obj, imu_rotation_is_complete_);

/// \method imu_enable_gyro_continuous_calibration()
/// Enable gyroscope automatic calibration (by default is on).
mp_obj_t imu_enable_gyro_continuous_calibration_(mp_obj_t self_in) {
    imu_enable_gyro_continuous_calibration();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(imu_enable_gyro_continuous_calibration_obj, imu_enable_gyro_continuous_calibration_);

/// \method imu_disable_gyro_continuous_calibration()
/// Disable gyroscope automatic calibration (by default is on).
mp_obj_t imu_disable_gyro_continuous_calibration_(mp_obj_t self_in) {
    imu_disable_gyro_continuous_calibration();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(imu_disable_gyro_continuous_calibration_obj, imu_disable_gyro_continuous_calibration_);

STATIC const mp_rom_map_elem_t imu_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_get_acc), MP_ROM_PTR(&imu_get_acceleration_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_gyro), MP_ROM_PTR(&imu_get_gyroscope_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_angle_deg), MP_ROM_PTR(&imu_get_angle_deg_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset_angle), MP_ROM_PTR(&imu_reset_angle_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset_gyro_calib), MP_ROM_PTR(&imu_reset_gyro_calibration_obj) },
    { MP_ROM_QSTR(MP_QSTR_calibrate_gyro), MP_ROM_PTR(&imu_calibrate_gyro_obj) },
    { MP_ROM_QSTR(MP_QSTR_rotate_deg), MP_ROM_PTR(&imu_rotate_deg_obj) },
    { MP_ROM_QSTR(MP_QSTR_rotation_completed), MP_ROM_PTR(&imu_rotation_is_complete_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable_gyro_auto_calib), MP_ROM_PTR(&imu_enable_gyro_continuous_calibration_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable_gyro_auto_calib), MP_ROM_PTR(&imu_disable_gyro_continuous_calibration_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_gyro_calib), MP_ROM_PTR(&imu_get_gyro_calibration_obj) },
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

