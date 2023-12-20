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

#include "extmod/vfs.h"
#include "py/stream.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "thymio_sound.h"
#include "../../../../../main/codec.h"
#include "../../../../../main/aseba_esp32.h"
#include "../../../../../main/stm32_spi.h"
#include "../../../../../main/settings.h"


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

int sound_get_mic_volume(void) {
    return STM32_GetMicrophoneIntensity();
}

bool sound_clap_detected(void) {
    return IS_EVENT(EVENT_MIC);
}

void sound_clear_clap_event(void) {
    CLEAR_EVENT(EVENT_MIC);
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

/// \method record_wav
/// Record sound for "sec" seconds in wav format. The data are saved in RAM memory. Max duration is 10 seconds.
mp_obj_t sound_record_wav_(mp_obj_t self_in, mp_obj_t sec) {
    int duration = mp_obj_get_int(sec);
    if(duration > 10) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Max recording is 10 seconds"));
        return mp_const_none;
    }
    Codec_RecordWAVFile(duration);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(sound_record_wav_obj, sound_record_wav_);

/// \method get_mic_volume
/// Return the volume computed from the microphone.
mp_obj_t sound_get_mic_volume_(mp_obj_t self_in) {
    return mp_obj_new_int(sound_get_mic_volume());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(sound_get_mic_volume_obj, sound_get_mic_volume_);

/// \method clap_detected
/// Return true if a clap is detected, false otherwise. This flag must be cleared using "clear_clap_event".
mp_obj_t sound_clap_detected_(mp_obj_t self_in) {
    return mp_obj_new_bool(sound_clap_detected());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(sound_clap_detected_obj, sound_clap_detected_);

/// \method clear_clap_event
/// Clear the clap event flag. This must be used after a clap detection event in order to detect following claps.
mp_obj_t sound_clear_clap_event_(mp_obj_t self_in) {
    sound_clear_clap_event();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(sound_clear_clap_event_obj, sound_clear_clap_event_);

/// \method sound_record_is_complete()
/// Tell if the recording is completed.
/// Return true if the recording is completed.
mp_obj_t sound_record_is_complete(mp_obj_t self_in) {
    return mp_obj_new_bool(Codec_IsRecordFinished());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(sound_record_is_complete_obj, sound_record_is_complete);

/// \method sound_play_is_complete()
/// Tell if the last sound is complete.
/// Return true if the last sound played is completed. If no sound was played it returns false.
mp_obj_t sound_play_is_complete(mp_obj_t self_in) {
    return mp_obj_new_bool(Codec_IsSoundFinished());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(sound_play_is_complete_obj, sound_play_is_complete);

/// \method record_get()
/// Get the recording data.
STATIC mp_obj_t sound_record_get(mp_obj_t self_in) {
    byte *buf;
    uint32_t recSize = Codec_GetRecordSize();
    buf = m_new(byte, recSize);
    memcpy(buf, Codec_GetRecordPtr(), recSize);
    return mp_obj_new_bytearray_by_ref(recSize, buf);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(sound_record_get_obj, sound_record_get);

//! \brief  Play sound file (wav or mp3) from internal robot storage. The sound file must have either ".wav" or ".mp3" extension and in the format: 12KHz sample rate, 16 bits per sample, mono channel.
//! \param  name of the file
//! \return None if ok, RuntimeError exception if another sound or recording is already running, ValueError if file not supported.
mp_obj_t sound_play_from_file(mp_obj_t self_in, mp_obj_t name) {
    mp_obj_t file;
    mp_obj_t args[2] = {
        name,
        MP_OBJ_NEW_QSTR(MP_QSTR_rb),
    };
    uint32_t sampleRate = 0;
    uint16_t numChannels = 0;
    uint16_t bitsPerChannel = 0;
    uint16_t mp3Ver = 0;
    uint32_t id3Size = 0;
    uint32_t len;
    int errcode;
    uint32_t fileSize = 0;
    byte *buf;
    uint8_t fileType = 0;
    char* fileName = mp_obj_str_get_str(args[0]);
    int fileNameLen = strlen(fileName);
    //printf("last char filename = %c\n", fileName[fileNameLen-1]);
    if(fileName[fileNameLen-1] == 'v') { // Wav
        fileType = 1;
    } else if(fileName[fileNameLen-1] == '3') { // Mp3
        fileType = 2;
    } else {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("File format not supported"));
        return mp_const_none;
    }

    mp_obj_t stat = mp_vfs_stat(name);
    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(stat);
    // print tuple
    fileSize = mp_obj_get_int(t->items[6]);
    //for (int i = 1; i <= 9; ++i) {
    //    printf("tuple[%d]=%d\n", i, mp_obj_get_int(t->items[i]));  // dev, nlink, uid, gid, size, atime, mtime, ctime
    //}
    //printf("filesize=%d\n", fileSize);
    buf = m_new(byte, fileSize);
    file = mp_vfs_open(MP_ARRAY_SIZE(args), &args[0], (mp_map_t *)&mp_const_empty_map);
    len = mp_stream_rw(file, buf, fileSize, &errcode, MP_STREAM_RW_READ | MP_STREAM_RW_ONCE);
    mp_stream_close(file);   

    // Check sound format by interpreting the header.
    if(fileType == 1) { // Wav
        numChannels = buf[22]+(buf[23]<<8);
        sampleRate = buf[24]+(buf[25]<<8)+(buf[26]<<16)+(buf[27]<<24);        
        bitsPerChannel = buf[34]+(buf[35]<<8);
        printf("wav ch=%d, rate=%d, bits=%d\n", numChannels, sampleRate, bitsPerChannel);
        if((numChannels==1) && (sampleRate==12000) && (bitsPerChannel==16)) {
            if(Codec_PlayWAVFile(buf, fileSize) != ESP_OK) {
                mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Cannot play"));
                return mp_const_none;    
            }
        } else {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("File format not supported"));
            return mp_const_none;
        }
    } else if(fileType == 2) { // Mp3         
        if(buf[0] == 0x49) { // ID3 header detected
            id3Size = buf[9] + (buf[8]<<7) + (buf[7]<<14) + (buf[6]<<21);
            //printf("id3size = %d\n", id3Size);
            //printf("%x %x %x %x\n", buf[10+id3Size], buf[11+id3Size], buf[12+id3Size], buf[13+id3Size]);
            mp3Ver = (buf[11+id3Size]&0x18)>>3;
            numChannels = (buf[13+id3Size]&0xC0)>>6;
            sampleRate = (buf[12+id3Size]&0x0C)>>2;            
        } else if(buf[0] == 0xFF) { // Mp3 header (no ID3 included)
            //printf("%x %x %x %x\n", buf[0], buf[1], buf[2], buf[3]); // Expected something like: FF E3 84 C4
            mp3Ver = (buf[1]&0x18)>>3;
            numChannels = (buf[3]&0xC0)>>6;
            sampleRate = (buf[2]&0x0C)>>2;
        } else {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("File format not supported"));
            return mp_const_none;
        }
        //printf("mp3 ver=%d, rate=%d, ch=%d\n", mp3Ver, sampleRate, numChannels);
        if((mp3Ver==0) && (numChannels==3) && (sampleRate==1)) {
            if(Codec_PlayMP3File(buf, fileSize) != ESP_OK) {
                mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Cannot play"));
                return mp_const_none;                
            }
        } else {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("File format not supported"));
            return mp_const_none;            
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(sound_play_from_file_obj, sound_play_from_file);

//! \brief     Play the last recorded sound directly from memory.
//! \param     None
//! \return    None if ok, RuntimeError exception if another sound or recording is already running.
mp_obj_t sound_play_recorded(mp_obj_t self_in) {
    if(Codec_PlayRecorded() != ESP_OK) {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Cannot play"));
        return mp_const_none;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(sound_play_recorded_obj, sound_play_recorded);

//! \brief  Play onboard sound (the ones pre-built in the robot firmware).
//! \param  The ind parameter identifies the correct sound to play:
//!         0 = magic,
//!         1 = Tick,
//!         2 = Blop,
//!         3 = Fall,
//!         4 = Detection,
//!         5 = Bye,
//!         6 = C3,
//!         7 = D3,
//!         8 = E3,
//!         9 = F3,
//!         10 = G3,
//!         11 = A3,
//!         12 = B3,
//!         13 = Alarm,
//!         14 = Good,
//!         15 = Bad
//! \return None if ok, RuntimeError exception if another sound or recording is already running.
mp_obj_t sound_play_onboard(mp_obj_t self_in, mp_obj_t ind) {
    int index = mp_obj_get_int(ind);
    if(Codec_PlayOnboardSound(index) != ESP_OK) {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Cannot play"));
        return mp_const_none;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(sound_play_onboard_obj, sound_play_onboard);

//! \brief     Pause any running sound play (can be resumed with "resume").
//! \param     None
//! \return    None if ok, RuntimeError exception if no sound is running.
mp_obj_t sound_pause(mp_obj_t self_in) {
    if(Codec_Pause() != ESP_OK) {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Cannot pause"));
        return mp_const_none;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(sound_pause_obj, sound_pause);

//! \brief     Resume a previously paused (with "pause") sound play.
//! \param     None
//! \return    None if ok, RuntimeError exception if no sound was paused.
mp_obj_t sound_resume(mp_obj_t self_in) {
    if(Codec_Resume() != ESP_OK) {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Cannot resume"));
        return mp_const_none;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(sound_resume_obj, sound_resume);

//! \brief     Set the play volume.
//! \param     Volume Between 0 and 10.
//! \return    None
mp_obj_t sound_set_volume(mp_obj_t self_in, mp_obj_t vol) {
    int volume = mp_obj_get_int(vol);
    if((volume > 10) || (volume < 0)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Volume must be between 0 and 10"));
        return mp_const_none;
    }

    Codec_SetVolume(volume*10);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(sound_set_volume_obj, sound_set_volume);

//! \brief     Store the current volume value in the permanent settings. The volume setting will be used when the robot is turned on next times.
//! \param     None
//! \return    None
mp_obj_t sound_save_volume(mp_obj_t self_in) {
    Settings_WriteVolume(Settings_GetVolumeSettings());
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(sound_save_volume_obj, sound_save_volume);

//! \brief     Clear audio events (played and recorded).
//! \param     None
//! \return    None
mp_obj_t sound_clear_events(mp_obj_t self_in) {
    
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(sound_clear_events_obj, sound_clear_events);

//! \brief     Stop any running sound play (cannot be resumed).
//! \param     None
//! \return    None.
mp_obj_t sound_stop(mp_obj_t self_in) {
    Codec_Stop();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(sound_stop_obj, sound_stop);

STATIC const mp_rom_map_elem_t sound_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_record), MP_ROM_PTR(&sound_record_wav_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_mic_volume), MP_ROM_PTR(&sound_get_mic_volume_obj) },
    { MP_ROM_QSTR(MP_QSTR_clap_detected), MP_ROM_PTR(&sound_clap_detected_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear_clap_event), MP_ROM_PTR(&sound_clear_clap_event_obj) },
    { MP_ROM_QSTR(MP_QSTR_record_completed), MP_ROM_PTR(&sound_record_is_complete_obj) },
    { MP_ROM_QSTR(MP_QSTR_record_get), MP_ROM_PTR(&sound_record_get_obj) },
    { MP_ROM_QSTR(MP_QSTR_play_from_file), MP_ROM_PTR(&sound_play_from_file_obj) },
    { MP_ROM_QSTR(MP_QSTR_play_recorded), MP_ROM_PTR(&sound_play_recorded_obj) },
    { MP_ROM_QSTR(MP_QSTR_play_onboard), MP_ROM_PTR(&sound_play_onboard_obj) },
    { MP_ROM_QSTR(MP_QSTR_play_completed), MP_ROM_PTR(&sound_play_is_complete_obj) },
    { MP_ROM_QSTR(MP_QSTR_pause), MP_ROM_PTR(&sound_pause_obj) },    
    { MP_ROM_QSTR(MP_QSTR_resume), MP_ROM_PTR(&sound_resume_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_volume), MP_ROM_PTR(&sound_set_volume_obj) },
    { MP_ROM_QSTR(MP_QSTR_save_volume), MP_ROM_PTR(&sound_save_volume_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear_events), MP_ROM_PTR(&sound_clear_events_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&sound_stop_obj) },
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

