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
#include "thymio_motors.h"
#include "../../../../../main/stm32_spi.h"
#include "../../../../../main/settings.h"
#include "../../../../../main/timer_hw.h"

/// \moduleref thymio
/// \class MOTORS - MOTORS object
///
/// The MOTORS object set and get motors speed.

typedef struct _thymio_motors_obj_t {
    mp_obj_base_t base;
} thymio_motors_obj_t;

//STATIC const thymio_motors_obj_t thymio_motors_obj;
STATIC T_Settings Setting;

void motors_init(void) {
    Settings_GetMotorsSettings(Setting.Motors);
}

int motors_get_left_speed() {
    return GetLeftSpeed();
}

int motors_get_right_speed() {
    return GetRightSpeed();
}

void motors_set_target_speed(int left, int right) {
    SetMotorTargets(left, right);
}

void motors_get_straight_calibration(int16_t *left, int16_t *right)
{
    Settings_GetMotorsSettings(Setting.Motors);
    *left = Setting.Motors[0];
    *right = Setting.Motors[1];    
}

void motors_set_straight_calibration(int16_t corr_left, int16_t corr_right)
{
     // Bound correction to +/- 20%
    if(corr_left > 306)
    {
        corr_left = 306;
    }
    if(corr_left < 206)
    {
        corr_left = 206;
    }
    if(corr_right > 306)
    {
        corr_right = 306;
    }
    if(corr_right < 206)
    {
        corr_right = 206;
    }
    Setting.Motors[0]  = corr_left;
    Setting.Motors[1] = corr_right;
    Settings_SetMotorsSettings(Setting.Motors);    // Used to send the value to STM32
}

int motors_save_straight_calibration(void)
{
    return Settings_WriteMotors(Setting.Motors);
}

void motors_reset_straight_calibration(void)
{
    Setting.Motors[0] = DEFAULT_LEFT_MOTOR;
    Setting.Motors[1] = DEFAULT_RIGHT_MOTOR;
    Settings_SetMotorsSettings(Setting.Motors);
}

void motors_get_distance_calibration(uint64_t *fw, uint64_t *bw)
{
    Settings_GetMot15cmSettings(Setting.Mot15cm); // Timer ticks to travels 15 cm forward and backward
    *fw = Setting.Mot15cm[0];
    *bw = Setting.Mot15cm[1];
}

void motors_set_distance_calibration(uint64_t fw, uint64_t bw)
{
    Setting.Mot15cm[0] = fw;
    Setting.Mot15cm[1] = bw;
    Settings_SetMot15cmSettings(Setting.Mot15cm);
    Setting.MotFwBw = (float)Setting.Mot15cm[1]/(float)Setting.Mot15cm[0];
    Settings_SetMotFwBwSettings(Setting.MotFwBw);
}

int motors_save_distance_calibration(void)
{
    int err = 0;
    err = Settings_WriteMot15cm(Setting.Mot15cm);
    if(err < 0)
    {
        return err;
    }
    err = Settings_WriteMotFwBwFactor(Setting.MotFwBw);
    return err;
}

void motors_reset_distance_calibration(void)
{
    Setting.Mot15cm[0] = DEFAULT_MOT15CM;
    Setting.Mot15cm[1] = DEFAULT_MOT15CM;
    Settings_SetMot15cmSettings(Setting.Mot15cm);
    Setting.MotFwBw = DEFAULT_MOT_FW_TO_BW;
    Settings_SetMotFwBwSettings(Setting.MotFwBw);
}

/******************************************************************************/
/* MicroPython bindings                                                       */

void motors_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_printf(print, "MOTORS");
}

/// \classmethod \constructor()
/// Create a MOTORS object associated with the given MOTORS:
STATIC mp_obj_t motors_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    thymio_motors_obj_t *mot = m_new_obj(thymio_motors_obj_t);
    mot->base.type = &thymio_motors_type;
    motors_init();
    return MP_OBJ_FROM_PTR(mot);
}

/// \method get_left_speed()
/// Get measured left motor speed.
mp_obj_t motors_left_speed(mp_obj_t self_in) {
    return mp_obj_new_int(motors_get_left_speed());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(motors_left_speed_obj, motors_left_speed);

/// \method get_right_speed()
/// Get measured right motor speed.
mp_obj_t motors_right_speed(mp_obj_t self_in) {
    return mp_obj_new_int(motors_get_right_speed());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(motors_right_speed_obj, motors_right_speed);

/// \method set_speed()
/// Set motors speed. Range is between -1000 and 1000.
mp_obj_t motors_set_speed(mp_obj_t self_in, mp_obj_t left, mp_obj_t right) {
    int l = mp_obj_get_int(left);
    int r = mp_obj_get_int(right);
    motors_set_target_speed(l, r);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(motors_set_speed_obj, motors_set_speed);

/// \method get_left_pwm_duty()
/// Get left PWM duty cycle.
mp_obj_t motors_left_pwm(mp_obj_t self_in) {
    return mp_obj_new_int(STM32_GetLeftMotorPwm());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(motors_left_pwm_obj, motors_left_pwm);

/// \method get_right_pwm_duty()
/// Get right PWM duty cycle.
mp_obj_t motors_right_pwm(mp_obj_t self_in) {
    return mp_obj_new_int(STM32_GetRightMotorPwm());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(motors_right_pwm_obj, motors_right_pwm);

/// \method get_straight_calibration()
/// Get straight calibration raw values: [left, right].
mp_obj_t motors_get_straight_calib(mp_obj_t self_in) {
    int16_t left = 0, right = 0;
    motors_get_straight_calibration(&left, &right);
    mp_obj_t items[2];
    items[0] = mp_obj_new_int(left);
    items[1] = mp_obj_new_int(right);
    return mp_obj_new_list(2, items);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(motors_get_straight_calib_obj, motors_get_straight_calib);

/// \method get_distance_calibration()
/// Get distance calibration raw values: [forward, backward].
mp_obj_t motors_get_distance_calib(mp_obj_t self_in) {
    uint64_t fw = 0, bw = 0;
    motors_get_distance_calibration(&fw, &bw);
    mp_obj_t items[2];
    items[0] = mp_obj_new_int(fw);
    items[1] = mp_obj_new_int(bw);
    return mp_obj_new_list(2, items);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(motors_get_distance_calib_obj, motors_get_distance_calib);

/// \method reset_straight_calibration()
/// Reset straight calibration values to default.
mp_obj_t motors_reset_straight_calib(mp_obj_t self_in) {
    motors_reset_straight_calibration();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(motors_reset_straight_calib_obj, motors_reset_straight_calib);

/// \method reset_distance_calibration()
/// Reset distance calibration values to default.
mp_obj_t motors_reset_distance_calib(mp_obj_t self_in) {
    motors_reset_distance_calibration();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(motors_reset_distance_calib_obj, motors_reset_distance_calib);

/// \method set_straight_calibration()
/// Set motors straight calibration values. Range is between -50 and +50 (corresponds to -/+ 20%).
/// Positive values mean compensating left motor (increase left motor and decrease right motor), negative values mean compensating right motor.
/// These values will be used until power off.
mp_obj_t motors_set_straight_calib(mp_obj_t self_in, mp_obj_t corr_left, mp_obj_t corr_right) {
    motors_set_straight_calibration(mp_obj_get_int(corr_left), mp_obj_get_int(corr_right));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(motors_set_straight_calib_obj, motors_set_straight_calib);

/// \method set_distance_calibration()
/// Set motors distance calibration values. Range is given in microseconds. Can be different for forward and backward motions.
/// These values will be used until power off.
mp_obj_t motors_set_distance_calib(mp_obj_t self_ins, mp_obj_t fw, mp_obj_t bw) {
    motors_set_distance_calibration(mp_obj_get_int(fw), mp_obj_get_int(bw));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(motors_set_distance_calib_obj, motors_set_distance_calib);

/// \method save_straight_calibration()
/// Save straight calibration values to flash. The last values set with "set_straight_calibration" will be saved.
mp_obj_t motors_save_straight_calib(mp_obj_t self_in) {
    if(motors_save_straight_calibration() < 0)
    {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Cannot save straight calibration"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(motors_save_straight_calib_obj, motors_save_straight_calib);

/// \method save_distance_calibration()
/// Save distance calibration values to flash. The last values set with "set_distance_calibration" will be saved.
mp_obj_t motors_save_distance_calib(mp_obj_t self_in) {
    if(motors_save_distance_calibration() < 0)
    {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Cannot save distance calibration"));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(motors_save_distance_calib_obj, motors_save_distance_calib);

/// \method motors_distance_calib_timer_start()
/// Start the internal timer counter used for distance motion.
mp_obj_t motors_distance_calib_timer_start(mp_obj_t self_in) {
    TimerHw_Start(1, 1);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(motors_distance_calib_timer_start_obj, motors_distance_calib_timer_start);

/// \method motors_distance_calib_timer_reset()
/// Reset the internal timer counter used for distance motion.
mp_obj_t motors_distance_calib_timer_reset(mp_obj_t self_in) {
    TimerHw_Reset_Counter(1, 1);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(motors_distance_calib_timer_reset_obj, motors_distance_calib_timer_reset);

/// \method motors_distance_calib_timer_get()
/// Get the internal timer counter used for distance motion.
mp_obj_t motors_distance_calib_timer_get(mp_obj_t self_in) {
    return mp_obj_new_int(TimerHw_Get_Counter(1, 1));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(motors_distance_calib_timer_get_obj, motors_distance_calib_timer_get);

/// \method motors_distance_calib_timer_set()
/// Set the internal timer counter alarm used for distance motion. Alarm given in microseconds.
mp_obj_t motors_distance_calib_timer_set(mp_obj_t self_in, mp_obj_t ticks) {
    TimerHw_Set_Alarm_Ticks(1, 1, mp_obj_get_int(ticks));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(motors_distance_calib_timer_set_obj, motors_distance_calib_timer_set);

/// \method motors_distance_calib_timer_pause()
/// Pause the internal timer counter used for distance motion.
mp_obj_t motors_distance_calib_timer_pause(mp_obj_t self_in) {
    TimerHw_Stop(1, 1);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(motors_distance_calib_timer_pause_obj, motors_distance_calib_timer_pause);

STATIC const mp_rom_map_elem_t motors_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_get_left_speed), MP_ROM_PTR(&motors_left_speed_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_right_speed), MP_ROM_PTR(&motors_right_speed_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_speed), MP_ROM_PTR(&motors_set_speed_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_left_pwm_duty), MP_ROM_PTR(&motors_left_pwm_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_right_pwm_duty), MP_ROM_PTR(&motors_right_pwm_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_straight_calibration), MP_ROM_PTR(&motors_get_straight_calib_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_distance_calibration), MP_ROM_PTR(&motors_get_distance_calib_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset_straight_calibration), MP_ROM_PTR(&motors_reset_straight_calib_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset_distance_calibration), MP_ROM_PTR(&motors_reset_distance_calib_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_straight_calibration), MP_ROM_PTR(&motors_set_straight_calib_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_distance_calibration), MP_ROM_PTR(&motors_set_distance_calib_obj) },
    { MP_ROM_QSTR(MP_QSTR_save_straight_calibration), MP_ROM_PTR(&motors_save_straight_calib_obj) },
    { MP_ROM_QSTR(MP_QSTR_save_distance_calibration), MP_ROM_PTR(&motors_save_distance_calib_obj) },
    { MP_ROM_QSTR(MP_QSTR_distance_calib_timer_reset), MP_ROM_PTR(&motors_distance_calib_timer_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_distance_calib_timer_get), MP_ROM_PTR(&motors_distance_calib_timer_get_obj) },
    { MP_ROM_QSTR(MP_QSTR_distance_calib_timer_start), MP_ROM_PTR(&motors_distance_calib_timer_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_distance_calib_timer_set), MP_ROM_PTR(&motors_distance_calib_timer_set_obj) },
    { MP_ROM_QSTR(MP_QSTR_distance_calib_timer_pause), MP_ROM_PTR(&motors_distance_calib_timer_pause_obj) },
};

STATIC MP_DEFINE_CONST_DICT(motors_locals_dict, motors_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    thymio_motors_type,
    MP_QSTR_MOTORS,
    MP_TYPE_FLAG_NONE,
    make_new, motors_make_new,
    print, motors_print,
    locals_dict, &motors_locals_dict
    );

