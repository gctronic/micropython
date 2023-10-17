/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#ifndef MICROPY_INCLUDED_THYMIO_IMU_H
#define MICROPY_INCLUDED_THYMIO_IMU_H

#include "../../../../../main/imu_common.h"

void imu_init(void);
T_Axis imu_get_acc(void);
T_Axis imu_get_gyro(void);
int16_t imu_get_angle_deg(void);
void imu_reset_angle(void);
void imu_get_gyro_calibration(int16_t *values);
void imu_reset_gyro_calibration(void);
void imu_calibrate_gyro(void);
void imu_rotate_deg(int16_t angle, int16_t speed);
bool imu_rotation_is_complete(void);
void imu_enable_gyro_continuous_calibration(void);
void imu_disable_gyro_continuous_calibration(void);
bool imu_freefall_detected(void);
void imu_clear_freefall_event(void);
bool imu_tap_detected(void);
void imu_clear_tap_event(void);

extern const mp_obj_type_t thymio_imu_type;

#endif // MICROPY_INCLUDED_THYMIO_IMU_H
