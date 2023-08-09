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
#include <string.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "thymio_filesystem.h"
#include "../../../../../main/file_system.h"


/// \moduleref thymio
/// \class FILESYSTEM - FILESYSTEM object
///
/// The FILESYSTEM object read data from and write data to the internal robot storage.

typedef struct _thymio_filesystem_obj_t {
    mp_obj_base_t base;
} thymio_filesystem_obj_t;

void filesystem_init(void) {
}

void filesystem_write_file(char* filename, void* buf, long int size) {
    FileSystem_Write(filename, buf, size);
}

int filesystem_read_file(char* filename, void* buf) {
    return FileSystem_Read3(filename, buf);
}

long int filesystem_get_file_size(char* filename) {
    return FileSystem_GetFileSize(filename); 
}

void filesystem_remove_file(char* filename) {
    FileSystem_EraseFile(filename);
}

void filesystem_ls(void) {
    listDir();
}

/******************************************************************************/
/* MicroPython bindings                                                       */

void filesystem_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    //thymio_filesystem_obj_t *self = MP_OBJ_TO_PTR(self_in);
    filesystem_ls();
    //mp_printf(print, "PROX(%u)", self->prox_id);
}

/// \classmethod \constructor()
/// Create an FILESYSTEM object:
STATIC mp_obj_t filesystem_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    thymio_filesystem_obj_t *fs = m_new_obj(thymio_filesystem_obj_t);
    fs->base.type = &thymio_filesystem_type;
    return MP_OBJ_FROM_PTR(fs);
}

/// \method write()
/// Write a file to the robot internal storage.
STATIC mp_obj_t filesystem_write(size_t n_args, const mp_obj_t *args) {
    char fname[28]; // max file length check!
    //printf("strlen = %d\n", strlen(mp_obj_str_get_str(args[1])));
    if(strlen(mp_obj_str_get_str(args[1])) > 22) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("File name is too long, maximum is 22 chars"));
        return mp_const_none;
    }
    strcpy(fname, "/spiffs/");
    strcpy(&fname[8], mp_obj_str_get_str(args[1]));
    long int fsize = mp_obj_get_int(args[3]);
    //printf("fsize = %ld\n", fsize);
    char *fdata = mp_obj_str_get_data(args[2], (size_t*)&fsize);
    filesystem_write_file(fname, fdata, fsize);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(filesystem_write_obj, 4, 4, filesystem_write);

/// \method read()
/// Read a file from the robot internal storage.
STATIC mp_obj_t filesystem_read(mp_obj_t self_in, mp_obj_t filename) {
    char fname[28]; // max file length check!
    int err = 0;
    byte *buf;
    //printf("strlen = %d\n", strlen(mp_obj_str_get_str(args[1])));
    if(strlen(mp_obj_str_get_str(filename)) > 22) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("File name is too long, maximum is 22 chars"));
        return mp_const_none;
    }
    strcpy(fname, "/spiffs/");
    strcpy(&fname[8], mp_obj_str_get_str(filename));

    long int fsize = filesystem_get_file_size(fname);
    //printf("read fsize = %ld\n", fsize);

    buf = m_new(byte, fsize);
    err = filesystem_read_file(fname, buf);

    if(err == -1) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Cannot open file"));
        return mp_const_none;
    } else if(err == -2) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Cannot get file size"));
        return mp_const_none;
    } else if(err == -3) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Cannot read file"));
        return mp_const_none;
    }

    return mp_obj_new_bytes(buf, fsize);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(filesystem_read_obj, filesystem_read);

/// \method delete()
/// Delete a file from the robot internal storage.
STATIC mp_obj_t filesystem_remove_file_(mp_obj_t self_in, mp_obj_t filename) {
    char fname[28]; // max file length check!
    //printf("strlen = %d\n", strlen(mp_obj_str_get_str(args[1])));
    if(strlen(mp_obj_str_get_str(filename)) > 22) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("File name is too long, maximum is 22 chars"));
        return mp_const_none;
    }
    strcpy(fname, "/spiffs/");
    strcpy(&fname[8], mp_obj_str_get_str(filename));

    filesystem_remove_file(fname);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(filesystem_remove_file_obj, filesystem_remove_file_);

STATIC const mp_rom_map_elem_t filesystem_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&filesystem_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&filesystem_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove_file), MP_ROM_PTR(&filesystem_remove_file_obj) },
};

STATIC MP_DEFINE_CONST_DICT(filesystem_locals_dict, filesystem_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    thymio_filesystem_type,
    MP_QSTR_FILESYSTEM,
    MP_TYPE_FLAG_NONE,
    make_new, filesystem_make_new,
    print, filesystem_print,
    locals_dict, &filesystem_locals_dict
    );

