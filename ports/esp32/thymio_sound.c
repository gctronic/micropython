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
#include "thymio_sound.h"
#include "../../../../../main/codec.h"


/// \moduleref thymio
/// \class SOUND - SOUND object
///
/// The SOUND object play and record audio files.

typedef struct _thymio_sound_obj_t {
    mp_obj_base_t base;
} thymio_sound_obj_t;

//STATIC const thymio_sound_obj_t thymio_sound_obj;

void sound_init(void) {
}

void sound_play_mp3(int index) {
    Codec_PlayMP3File(index);
}

void sound_play_wav(int index) {
    Codec_PlayWAVFile(index);
}

void sound_record_wav(int index, int duration) {
    Codec_RecordWAVFile(index, duration);
}

/******************************************************************************/
/* MicroPython bindings                                                       */

void sound_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_printf(print, "SOUND");
}

/// \classmethod \constructor()
/// Create a SOUND object:
STATIC mp_obj_t sound_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    thymio_sound_obj_t *sound = m_new_obj(thymio_sound_obj_t);
    sound->base.type = &thymio_sound_type;
    return MP_OBJ_FROM_PTR(sound);
}

/// \method play_mp3
/// Play mp3 sound file from internal robot storage. The mp3 name must be saved in the format "number.mp3" (e.g. 0.mp3, 1.mp3, ...). The id parameter identifies the correct mp3 to play => id.mp3.
mp_obj_t sound_play_mp3_(mp_obj_t self_in, mp_obj_t ind) {
    int index = mp_obj_get_int(ind);
    sound_play_mp3(index);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(sound_play_mp3_obj, sound_play_mp3_);

/// \method play_wav
/// Play wav sound file from internal robot storage. The wav name must be saved in the format "number.wav" (e.g. 0.wav, 1.wav, ...). The id parameter identifies the correct wav to play => id.wav.
mp_obj_t sound_play_wav_(mp_obj_t self_in, mp_obj_t ind) {
    int index = mp_obj_get_int(ind);
    sound_play_wav(index);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(sound_play_wav_obj, sound_play_wav_);

/// \method record_wav
/// Record sound to internal robot storage in wav format with name id.wav.
mp_obj_t sound_record_wav_(mp_obj_t self_in, mp_obj_t ind, mp_obj_t sec) {
    int index = mp_obj_get_int(ind);
    int duration = mp_obj_get_int(sec);
    // check max duration
    sound_record_wav(index, duration);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(sound_record_wav_obj, sound_record_wav_);

STATIC const mp_rom_map_elem_t sound_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_play_mp3), MP_ROM_PTR(&sound_play_mp3_obj) },
    { MP_ROM_QSTR(MP_QSTR_play_wav), MP_ROM_PTR(&sound_play_wav_obj) },
    { MP_ROM_QSTR(MP_QSTR_record_wav), MP_ROM_PTR(&sound_record_wav_obj) },
};

STATIC MP_DEFINE_CONST_DICT(sound_locals_dict, sound_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    thymio_sound_type,
    MP_QSTR_SOUND,
    MP_TYPE_FLAG_NONE,
    make_new, sound_make_new,
    print, sound_print,
    locals_dict, &sound_locals_dict
    );

