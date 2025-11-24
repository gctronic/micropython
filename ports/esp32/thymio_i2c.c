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
#include "py/obj.h"
#include "py/objstr.h"
#include "thymio_i2c.h"
#include "../../../../../main/i2c.h"


/// \moduleref thymio
/// \class I2C - I2C object
///
/// The I2C object read data from and write data to I2C bus.

typedef struct _thymio_i2c_obj_t {
    mp_obj_base_t base;
} thymio_i2c_obj_t;

void i2c_init(void) {
}

void i2c_write_reg(uint8_t slaveAddress, uint8_t registerAddress, uint8_t* data, uint16_t size)
{
    I2C_WriteToAddress(slaveAddress, registerAddress, data, size);
}

void i2c_read_reg(uint8_t slaveAddress, uint8_t registerAddress, uint8_t* data, uint16_t size)
{
    I2C_ReadFromAddress(slaveAddress, registerAddress, data, size);
}

void i2c_write_read(uint8_t slaveAddress, uint8_t* txData, uint16_t txSize, uint8_t* rxData, uint16_t rxSize)
{
    I2C_WriteAndRead(slaveAddress, txData, txSize, rxData, rxSize);
}

void i2c_read(uint8_t slaveAddress, uint8_t* data, uint16_t size)
{
    I2C_Read(slaveAddress, data, size);
}

void i2c_write(uint8_t slaveAddress, uint8_t* data, uint16_t size)
{
    I2C_Write(slaveAddress, data, size);
}

/******************************************************************************/
/* MicroPython bindings                                                       */

void i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_printf(print, "I2C0");
}

/// \classmethod \constructor()
/// Create an I2C object:
STATIC mp_obj_t i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    thymio_i2c_obj_t *i2c = m_new_obj(thymio_i2c_obj_t);
    i2c->base.type = &thymio_i2c_type;
    return MP_OBJ_FROM_PTR(i2c);
}

/// \method write_reg()
/// Write to I2C slave register.
/// Params:
/// - `device address` 7 bit slave address.
/// - `register address` this is the start address if more than one byte is written.
/// - `tx data` data to be transmitted to the slave.
/// - `tx data size` number of bytes to be transmitted
STATIC mp_obj_t thymio_i2c_write_reg(size_t n_args, const mp_obj_t *args) {
    uint8_t dev_addr = mp_obj_get_int(args[1]);
    uint8_t reg_addr = mp_obj_get_int(args[2]);
    uint16_t size = mp_obj_get_int(args[4]);
    uint8_t *data = mp_obj_str_get_data(args[3], (size_t*)&size);
    i2c_write_reg(dev_addr, reg_addr, data, size);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(thymio_i2c_write_reg_obj, 5, 5, thymio_i2c_write_reg);

/// \method read_reg()
/// Read from I2C slave register.
/// Params:
/// - `device address` 7 bit slave address.
/// - `register address` this is the start address if more than one byte is read.
/// - `rx data size` number of bytes to be read from slave
STATIC mp_obj_t thymio_i2c_read_reg(size_t n_args, const mp_obj_t *args) {
    uint8_t dev_addr = mp_obj_get_int(args[1]);
    uint8_t reg_addr = mp_obj_get_int(args[2]);
    uint16_t size = mp_obj_get_int(args[3]);
    byte *buf;
    buf = m_new(byte, size);
    i2c_read_reg(dev_addr, reg_addr, buf, size);
    return mp_obj_new_bytearray_by_ref(size, buf);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(thymio_i2c_read_reg_obj, 4, 4, thymio_i2c_read_reg);

/// \method write_and_read()
/// Raw write and read: the transmit buffer is sent to the slave and the following received data are returned.
/// Params:
/// - `device address` 7 bit slave address.
/// - `tx data` data to be transmitted to the slave.
/// - `tx data size` number of bytes to be transmitted
/// - `rx data size` number of bytes ro be read from slave.
STATIC mp_obj_t thymio_i2c_wr(size_t n_args, const mp_obj_t *args) {
    byte *rx_data;
    uint8_t dev_addr = mp_obj_get_int(args[1]);
    uint16_t tx_size = mp_obj_get_int(args[3]);
    uint8_t *tx_data = mp_obj_str_get_data(args[2], (size_t*)&tx_size);
    uint16_t rx_size = mp_obj_get_int(args[4]);
    rx_data = m_new(byte, rx_size);
    i2c_write_read(dev_addr, tx_data, tx_size, rx_data, rx_size);
    return mp_obj_new_bytearray_by_ref(rx_size, rx_data);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(thymio_i2c_wr_obj, 5, 5, thymio_i2c_wr);

/// \method write()
/// Raw write to I2C slave: the transmit buffer is sent to the slave.
/// Params:
/// - `device address` 7 bit slave address.
/// - `tx data` data to be transmitted to the slave.
/// - `tx data size` number of bytes to be transmitted
STATIC mp_obj_t thymio_i2c_write(size_t n_args, const mp_obj_t *args) {
    uint8_t dev_addr = mp_obj_get_int(args[1]);
    uint16_t size = mp_obj_get_int(args[3]);
    uint8_t *data = mp_obj_str_get_data(args[2], (size_t*)&size);
    i2c_write(dev_addr, data, size);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(thymio_i2c_write_obj, 4, 4, thymio_i2c_write);

/// \method read()
/// Raw read from I2C slave.
/// Params:
/// - `device address` 7 bit slave address.
/// - `rx data size` number of bytes to be read from slave
STATIC mp_obj_t thymio_i2c_read(size_t n_args, const mp_obj_t *args) {
    uint8_t dev_addr = mp_obj_get_int(args[1]);
    uint16_t size = mp_obj_get_int(args[2]);
    byte *buf;
    buf = m_new(byte, size);
    i2c_read(dev_addr, buf, size);
    return mp_obj_new_bytearray_by_ref(size, buf);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(thymio_i2c_read_obj, 3, 3, thymio_i2c_read);

STATIC const mp_rom_map_elem_t i2c_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_write_reg), MP_ROM_PTR(&thymio_i2c_write_reg_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_reg), MP_ROM_PTR(&thymio_i2c_read_reg_obj) },
    { MP_ROM_QSTR(MP_QSTR_wr), MP_ROM_PTR(&thymio_i2c_wr_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&thymio_i2c_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&thymio_i2c_read_obj) },
};

STATIC MP_DEFINE_CONST_DICT(i2c_locals_dict, i2c_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    thymio_i2c_type,
    MP_QSTR_I2C,
    MP_TYPE_FLAG_NONE,
    make_new, i2c_make_new,
    print, i2c_print,
    locals_dict, &i2c_locals_dict
    );

