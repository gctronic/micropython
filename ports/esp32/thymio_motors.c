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


/// \moduleref thymio
/// \class MOTORS - MOTORS object
///
/// The MOTORS object set and get motors speed.

typedef struct _thymio_motors_obj_t {
    mp_obj_base_t base;
} thymio_motors_obj_t;

//STATIC const thymio_motors_obj_t thymio_motors_obj;

void motors_init(void) {
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
    return MP_OBJ_FROM_PTR(mot);
}

/// \method get_left_speed()
/// Get left motor speed.
mp_obj_t motors_left_speed(mp_obj_t self_in) {
    return mp_obj_new_int(motors_get_left_speed());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(motors_left_speed_obj, motors_left_speed);

/// \method get_right_speed()
/// Get right motor speed.
mp_obj_t motors_right_speed(mp_obj_t self_in) {
    return mp_obj_new_int(motors_get_right_speed());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(motors_right_speed_obj, motors_right_speed);

/// \method set_speed()
/// Set motors speed.
mp_obj_t motors_set_speed(mp_obj_t self_in, mp_obj_t left, mp_obj_t right) {
    int l = mp_obj_get_int(left);
    int r = mp_obj_get_int(right);
    motors_set_target_speed(l, r);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(motors_set_speed_obj, motors_set_speed);

STATIC const mp_rom_map_elem_t motors_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_get_left_speed), MP_ROM_PTR(&motors_left_speed_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_right_speed), MP_ROM_PTR(&motors_right_speed_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_speed), MP_ROM_PTR(&motors_set_speed_obj) }
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

