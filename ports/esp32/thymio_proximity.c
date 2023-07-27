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
#include "thymio_proximity.h"
#include "../../../../../main/stm32_spi.h"


/// \moduleref thymio
/// \class PROXIMITY - PROXIMITY object
///
/// The PROXIMITY object get data from proximity sensors.

typedef struct _thymio_proximity_obj_t {
    mp_obj_base_t base;
    mp_uint_t prox_id;
} thymio_proximity_obj_t;

STATIC const thymio_proximity_obj_t thymio_proximity_obj[] = {
    {{&thymio_proximity_type}, 0},
    {{&thymio_proximity_type}, 1},
    {{&thymio_proximity_type}, 2},
    {{&thymio_proximity_type}, 3},
    {{&thymio_proximity_type}, 4},
    {{&thymio_proximity_type}, 5},
    {{&thymio_proximity_type}, 6}                      
};
#define NUM_PROXIMITY MP_ARRAY_SIZE(thymio_proximity_obj)

void proximity_init(void) {
}

int proximity_get_value(int prox) {
    return GetProximityValue(prox);
}

/******************************************************************************/
/* MicroPython bindings                                                       */

void proximity_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    thymio_proximity_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "PROX(%u)", self->prox_id);
}

/// \classmethod \constructor(id)
/// Create an PROXIMITY object associated with the given PROXIMITY:
///
///   - `id` is the PROXIMITY number, 0-6.
STATIC mp_obj_t proximity_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // get prox number
    mp_int_t prox_id = mp_obj_get_int(args[0]);

    // check prox number
    if (prox_id > (NUM_PROXIMITY-1)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("PROX(%d) doesn't exist"), prox_id);
    }

    // return static proximity object
    return MP_OBJ_FROM_PTR(&thymio_proximity_obj[prox_id]);
}

/// \method value()
/// Get proximity value (the higher the value, the closer the object).
mp_obj_t proximity_value(mp_obj_t self_in) {
    thymio_proximity_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(proximity_get_value(self->prox_id));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(proximity_value_obj, proximity_value);

/// \method normalized_value()
/// Get proximity value (the higher the value, the closer the object).
mp_obj_t proximity_normalized_value(mp_obj_t self_in) {
    thymio_proximity_obj_t *self = MP_OBJ_TO_PTR(self_in);
    float temp = proximity_get_value(self->prox_id);
    float normalized = temp*100.0/3500.0;
    if(normalized > 100) {
        normalized = 100;
    }
    return mp_obj_new_int((int)normalized);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(proximity_normalized_value_obj, proximity_normalized_value);

STATIC const mp_rom_map_elem_t proximity_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&proximity_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_normalized_value), MP_ROM_PTR(&proximity_normalized_value_obj) },
};

STATIC MP_DEFINE_CONST_DICT(proximity_locals_dict, proximity_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    thymio_proximity_type,
    MP_QSTR_PROXIMITY,
    MP_TYPE_FLAG_NONE,
    make_new, proximity_make_new,
    print, proximity_print,
    locals_dict, &proximity_locals_dict
    );

