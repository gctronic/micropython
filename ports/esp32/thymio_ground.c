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
#include "thymio_ground.h"
#include "../../../../../main/stm32_spi.h"


/// \moduleref thymio
/// \class GROUND - GROUND object
///
/// The GROUND object get data from ground sensors.

typedef struct _thymio_ground_obj_t {
    mp_obj_base_t base;
    mp_uint_t ground_id;
} thymio_ground_obj_t;

STATIC const thymio_ground_obj_t thymio_ground_obj[] = {
    {{&thymio_ground_type}, 0},
    {{&thymio_ground_type}, 1}                   
};
#define NUM_GROUND MP_ARRAY_SIZE(thymio_ground_obj)

void ground_init(void) {
}

int ground_get_value(int ground) {
    return GetGroundValue(ground);
}

/******************************************************************************/
/* MicroPython bindings                                                       */

void ground_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    thymio_ground_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "GROUND(%u)", self->ground_id);
}

/// \classmethod \constructor(id)
/// Create an GROUND object associated with the given GROUND:
///
///   - `id` is the GROUND number, 0-6.
STATIC mp_obj_t ground_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // get ground number
    mp_int_t ground_id = mp_obj_get_int(args[0]);

    // check ground number
    if (ground_id > (NUM_GROUND-1)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("GROUND(%d) doesn't exist"), ground_id);
    }

    // return static ground object
    return MP_OBJ_FROM_PTR(&thymio_ground_obj[ground_id]);
}

/// \method value()
/// Get ground sensor value (the lower the value, the darker the object).
mp_obj_t ground_value(mp_obj_t self_in) {
    thymio_ground_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(ground_get_value(self->ground_id));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ground_value_obj, ground_value);

/// \method normalized_value()
/// Get normalized (between 0 and 100) ground sensor value (the lower the value, the darker the object).
mp_obj_t ground_normalized_value(mp_obj_t self_in) {
    thymio_ground_obj_t *self = MP_OBJ_TO_PTR(self_in);
    float temp = ground_get_value(self->ground_id);
    float normalized = temp*100.0/1024.0;
    if(normalized > 100) {
        normalized = 100;
    }
    return mp_obj_new_int((int)normalized);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ground_normalized_value_obj, ground_normalized_value);

STATIC const mp_rom_map_elem_t ground_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&ground_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_normalized_value), MP_ROM_PTR(&ground_normalized_value_obj) },
};

STATIC MP_DEFINE_CONST_DICT(ground_locals_dict, ground_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    thymio_ground_type,
    MP_QSTR_GROUND,
    MP_TYPE_FLAG_NONE,
    make_new, ground_make_new,
    print, ground_print,
    locals_dict, &ground_locals_dict
    );

