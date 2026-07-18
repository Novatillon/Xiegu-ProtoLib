#include <stdint.h>
#include <stdbool.h>

#include "g90_tx_protocol.h" //if anything else touches this file put it down immeaditily, or i will.
#include "crc.h"

//the struct for fields included in g90_tx_protocol.h

#define MIRROR(field1, field2) \
{ \
    .copies     = { field1, field2 }, \
    .num_copies = 2 \
}

#define BITFIELD(byte_pos, bit_pos, width) \
{ \
    .bit_offset = (uint16_t)((byte_pos) * 8 + (bit_pos)), \
    .bit_length = (uint8_t)(width) \
}

#define BYTEFIELD(byte_pos, byte_count) \
{ \
    .bit_offset = (uint16_t)((byte_pos) * 8), \
    .bit_length = (uint8_t)((byte_count) * 8) \
}

#define FLAGBIT(byte, bit) BITFIELD(byte, bit, 1)

// magic bytes are written in the frame-init function — no field for them.

#define VFO_STRIDE 0x10
#define VFO_FIELD(base_byte, num_bytes, vfo_index) \
    BYTEFIELD((base_byte) + (vfo_index) * VFO_STRIDE, num_bytes)

//------ im sorry, i cant think of a tree that makes sense so this goes here. ------// //TODO: resist from fixing.

// ---------------------------------------------------------------------
// VFO block (VFO1 = index 0, VFO2 = index 1, mirrored at +0x10 stride)
// ---------------------------------------------------------------------
const headField FIELD_VFO_FREQ[2] = { VFO_FIELD(0x04, 4, 0), VFO_FIELD(0x04, 4, 1) };
const headField FIELD_VFO_ATT_MODE[2] = { VFO_FIELD(0x08, 1, 0), VFO_FIELD(0x08, 1, 1) };
const headField FIELD_VFO_MOD_MODE[2] = { VFO_FIELD(0x09, 1, 0), VFO_FIELD(0x09, 1, 1) };
const headField FIELD_VFO_AGC_MODE[2] = { VFO_FIELD(0x0A, 1, 0), VFO_FIELD(0x0A, 1, 1) };
// const headField FIELD_POSSB_15M[2] = { VFO_FIELD(0x0B, 1, 0), VFO_FIELD(0x0B, 1, 1) }; // TODO: unconfirmed, verify
const headField FIELD_FILTER_HIGH[2] = { VFO_FIELD(0x0C, 1, 0), VFO_FIELD(0x0C, 1, 1) };
const headField FIELD_FILTER_LOW[2] = { VFO_FIELD(0x0D, 1, 0), VFO_FIELD(0x0D, 1, 1) };
const headField FIELD_FFT_SCALE[2] = { VFO_FIELD(0x13, 1, 0), VFO_FIELD(0x13, 1, 1) };

// ---------------------------------------------------------------------
// byte 0x24 (36): ctrl flags
// ---------------------------------------------------------------------
const headField FIELD_PANEL_LOCK = FLAGBIT(0x24, 0);
const headField FIELD_SPLIT_MODE = FLAGBIT(0x24, 1);
const headField FIELD_AUDIO_OUTSEL = FLAGBIT(0x24, 2);
const headField FIELD_MIC_COMP = FLAGBIT(0x24, 3);
const headField FIELD_NBFILT = FLAGBIT(0x24, 4);
const headField FIELD_TUNER_ON = FLAGBIT(0x24, 5);
const headField FIELD_VFO_ON = FLAGBIT(0x24, 6);

// mirrored flag: byte 0x24 bit 7 == byte 0x25 bit 5, always in sync
const FieldMirror FIELD_TUNING_STATUS = MIRROR(FLAGBIT(0x24, 7), FLAGBIT(0x25, 5));

// byte 0x25 (37): bits 0-4, 6 unknown
const headField FIELD_SHTWN_REQ = FLAGBIT(0x25, 7);

// byte 0x26 (38-39): bits 1-4 of byte 38 unknown
const headField FIELD_QSK_ON = FLAGBIT(0x26, 0);
const headField FIELD_RCLK = BITFIELD(0x26, 5, 11); // 11-bit field, spans all of byte 39

// byte 0x28 (40): unknown
// byte 0x29 (41): unknown
// byte 0x2A (42): bits 0-2 unknown
const headField FIELD_RF_GAIN = BITFIELD(0x2A, 3, 7); // covers bits 0-1 of byte 43 too

// byte 0x2B (43): bits 4-5, 7 unknown
const headField FIELD_FULL_BANDMODE = FLAGBIT(0x2B, 2);
const headField FIELD_STARTUP_BEEP_ON = FLAGBIT(0x2B, 3);
const headField FIELD_DIGITAL_MODE = FLAGBIT(0x2B, 6);

// byte 0x2C (44)
const headField FIELD_RF_POWER = BYTEFIELD(0x2C, 1);

// byte 0x2D (45): bits 4-7 unknown
const headField FIELD_SQL_LEVEL = BITFIELD(0x2D, 0, 4);

// byte 0x2E (46)
const headField FIELD_NB_WIDTH = BITFIELD(0x2E, 0, 4);
const headField FIELD_NB_LEVEL = BITFIELD(0x2E, 4, 4);

// byte 0x2F (47): padding
// byte 0x30 (48)
const headField FIELD_VOLUME = BYTEFIELD(0x30, 1);

// byte 0x31 (49)
const headField FIELD_MIC_GAIN = BYTEFIELD(0x31, 1);

// byte 0x32 (50): unknown
// byte 0x33 (51): bits 4-7 unknown
const headField FIELD_CW_QSK_TIME = BITFIELD(0x33, 0, 4);

// byte 0x34 (52)
const headField FIELD_FREQKNOB_PLACEVAL = BYTEFIELD(0x34, 1);

// byte 0x35 (53): probably mem channel — TODO: verify
// byte 0x36 (54): unknown
// byte 0x37 (55): padding
// byte 0x38 (56): unknown
// byte 0x39 (57): unknown
// byte 0x3A (58): unknown
// byte 0x3B (59): unknown
// byte 0x3C (60)
const headField FIELD_CW_WPM = BYTEFIELD(0x3C, 1);

// byte 0x3D (61): unknown
// byte 0x3E (62): unknown
// byte 0x3F (63): unknown
// byte 0x40 (64): unknown
// byte 0x41 (65): unknown
// byte 0x42 (66): unknown
// byte 0x43 (67): unknown
// byte 0x44 (68): unknown
// byte 0x45 (69): unknown
// byte 0x46 (70): unknown
// byte 0x47 (71): unknown
// byte 0x48 (72): unknown — possibly a counter
// byte 0x49 (73): unknown
// byte 0x4A (74): unknown
// byte 0x4B (75): unknown — possibly an opcode
// byte 0x4C (76): unknown
// byte 0x4D (77): unknown
// byte 0x4E (78): unknown
// byte 0x4F (79): unknown
// byte 0x50 (80): unknown — possibly CW tone
// byte 0x51 (81): unknown — possibly CW volume
// byte 0x52 (82): unknown

// byte 0x53 (83)
const headField FIELD_AUX_IN_VOL = BITFIELD(0x53, 0, 4);
const headField FIELD_AUX_OUT_VOL = BITFIELD(0x53, 4, 4);

// byte 0x54-0x55 (84-85)
const headField FIELD_RIT_OFFSET = BYTEFIELD(0x54, 2);

// byte 0x56 (86): unknown
// byte 0x57 (87): unknown
// byte 0x58 (88): unknown
// byte 0x59 (89): unknown
// byte 0x5A (90): unknown
// byte 0x5B (91): unknown
// bytes 0x5C-0x60 (92-96): CRC32-MPEG2


//------ below this line sanity begins ------- i appologize ------ //

//write CRC and synch byte helper function.
void write_32bit_le(headPacket *packet , uint32_t value, uint8_t offset) {
    packet->raw[offset    ] = value & 0xFF;
    packet->raw[offset + 1] = (value >> 8) & 0xFF;
    packet->raw[offset + 2] = (value >> 16) & 0xFF;
    packet->raw[offset + 3] = (value >> 24) & 0xFF;
}

void endian_conv_32bit(headPacket* packet) {
    //packets a multiple of four bytes, i dont care.
    for (int i = 0; i < sizeof(packet->raw); i += 4) {
        uint32_t val;

        memcpy(&val, &packet->raw[i], 4);

        val = ((val & 0xFF000000) >> 24) |
            ((val & 0x00FF0000) >> 8) |
            ((val & 0x0000FF00) << 8) |
            ((val & 0x000000FF) << 24);

        memcpy(&packet->raw[i], &val, 4);
    }
}

//securities are handled in the scheduler function, such that this can be pretty basic.
bool g90_head_writepacket(headPacket *packet, G90_STATE* radio_master_state) {
    
    //anti-seg fault.
    if (packet == NULL || radio_master_state == NULL) {
        return false; //code for unsucessful, throws panic gaurdian.
    }

    //write synch word.
    write_32bit_le(packet, G90_HEADPROTOCOL_SYNCH_WORD, 0);





    //TODO: logical implementation of struct -> packet.
    




    //I'm very concerned if I have to explain this to you, referenced in .\transmit\crc.c
    //mpeg2 expects big endian, also, i know someones going to try 
    //this but headprotocol_synch... is a macro, you do not get one instruction out of making it a reg.
    endian_conv_32bit(packet);

    uint32_t checksum = crc32_mpeg2(packet, G90_HEADPROTOCOL_SYNCH_WORD - 4);

    write_32bit_le(packet, checksum, G90_HEADPROTOCOL_SYNCH_WORD - 4);
}