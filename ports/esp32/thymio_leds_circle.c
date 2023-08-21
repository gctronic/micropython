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
#include "thymio_leds_circle.h"
#include "../../../../../main/leds.h"


/// \moduleref thymio
/// \class LED - LED object
///
/// The LED object controls an individual LED (Light Emitting Diode).

typedef struct _thymio_leds_circle_obj_t {
    mp_obj_base_t base;
    mp_uint_t led_id;
} thymio_leds_circle_obj_t;

STATIC const thymio_leds_circle_obj_t thymio_leds_circle_obj[] = {
    {{&thymio_leds_circle_type}, 0},
    {{&thymio_leds_circle_type}, 1},
    {{&thymio_leds_circle_type}, 2},
    {{&thymio_leds_circle_type}, 3},
    {{&thymio_leds_circle_type}, 4},
    {{&thymio_leds_circle_type}, 5},
    {{&thymio_leds_circle_type}, 6},
    {{&thymio_leds_circle_type}, 7},                        
};
#define NUM_LEDS_CIRCLE MP_ARRAY_SIZE(thymio_leds_circle_obj)

void leds_circle_init(void) {
    /* Turn off LEDs and initialize */
    Leds_SetCircleBrightness(0, 0, 0, 0, 0, 0, 0, 0);
}

int leds_circle_get_intensity(int led) {
    return Leds_GetBrightness(E_Led_Circle_N + led);
}

void leds_circle_set_intensity(int led, int intensity) {
    if (led > (NUM_LEDS_CIRCLE-1)) {
        return;
    }
    if(intensity > MAX_BRIGHTNESS) {
        intensity = MAX_BRIGHTNESS;
    }
    Leds_SetSingleBrightness(E_Led_Circle_N + led, intensity);
}

/******************************************************************************/
/* MicroPython bindings                                                       */

void leds_circle_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    thymio_leds_circle_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "LED CIRCLE(%u)", self->led_id);
}

/// \classmethod \constructor(id)
/// Create an LED object associated with the given LED:
///
///   - `id` is the LED number, 0-7.
STATIC mp_obj_t leds_circle_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // get led number
    mp_int_t led_id = mp_obj_get_int(args[0]);

    // check led number
    if (led_id > (NUM_LEDS_CIRCLE-1)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("LED CIRCLE(%d) doesn't exist"), led_id);
    }

    // return static led object
    return MP_OBJ_FROM_PTR(&thymio_leds_circle_obj[led_id]);
}

/// \method on()
/// Turn the LED on at maximum brightness.
mp_obj_t leds_circle_obj_on(mp_obj_t self_in) {
    thymio_leds_circle_obj_t *self = MP_OBJ_TO_PTR(self_in);
    leds_circle_set_intensity(self->led_id, MAX_BRIGHTNESS);
    return mp_const_none;
}

/// \method off()
/// Turn the LED off.
mp_obj_t leds_circle_obj_off(mp_obj_t self_in) {
    thymio_leds_circle_obj_t *self = MP_OBJ_TO_PTR(self_in);
    leds_circle_set_intensity(self->led_id, 0);
    return mp_const_none;
}

/// \method intensity([value])
/// Get or set the LED intensity.  Intensity ranges between 0 (off) and 16 (full on).
/// If no argument is given, return the current LED intensity.
/// If an argument is given, set the LED intensity and return `None`.
mp_obj_t leds_circle_obj_intensity(size_t n_args, const mp_obj_t *args) {
    thymio_leds_circle_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        return mp_obj_new_int(leds_circle_get_intensity(self->led_id));
    } else {
        leds_circle_set_intensity(self->led_id, mp_obj_get_int(args[1]));
        return mp_const_none;
    }
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(leds_circle_obj_on_obj, leds_circle_obj_on);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(leds_circle_obj_off_obj, leds_circle_obj_off);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(leds_circle_obj_intensity_obj, 1, 2, leds_circle_obj_intensity);

STATIC const mp_rom_map_elem_t leds_circle_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&leds_circle_obj_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&leds_circle_obj_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_intensity), MP_ROM_PTR(&leds_circle_obj_intensity_obj) },
};

STATIC MP_DEFINE_CONST_DICT(leds_circle_locals_dict, leds_circle_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    thymio_leds_circle_type,
    MP_QSTR_LEDS_CIRCLE,
    MP_TYPE_FLAG_NONE,
    make_new, leds_circle_obj_make_new,
    print, leds_circle_obj_print,
    locals_dict, &leds_circle_locals_dict
    );

