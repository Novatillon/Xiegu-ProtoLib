//.\g90_tx_protocol.h

#include <stdint.h>

#ifndef G90_TX_PROTOCOL
#define G90_TX_PROTOCOL

#define G90_HEADPROTOCOL_FRAME_SIZE 96
#define G90_HEADPROTOCOL_SYNCH_WORD 0x0000AA55u //written to pkt LE, ends up 55A...

#define G90_HEADPROTOCOL_FIRST_OFFSET 4

typedef struct {
	uint8_t raw[G90_HEADPROTOCOL_FRAME_SIZE]; //stupid, byte rep of pkt, handled with offsets.
} headPacket;

typedef struct {
	uint16_t bit_offset; //macro'd from byte and bit offset.
	uint8_t  bit_length;
	uint16_t min_value;
	uint32_t max_value;
	uint16_t ram_offset;
	uint8_t ram_size;
} headField;

//TEMPLATE: X(byte_offset, bit_offset, bit_length, min_value, max_value, type, name)
#define G90_HEADPROTOCOL_FIELDS \
	/* ------------------------------------------------------------- */ \
	/* VFO block (VFO1 = index 0, VFO2 = index 1, mirrored at +0x10 stride) */ \
	/* ------------------------------------------------------------- */ \
	X(0x04, 0, 32, 500000, 30000000, uint32_t, FIELD_VFO1_FREQ) \
	X(0x08, 0, 8,  0, 2, uint8_t, FIELD_VFO1_ATT_MODE) \
	X(0x09, 0, 8,  0, 7, uint8_t, FIELD_VFO1_MOD_MODE) \
	X(0x0A, 0, 8,  0, 3, uint8_t, FIELD_VFO1_AGC_MODE) \
	/* X(0x0B, 0, 8, , , uint8_t, "FIELD_VFO1_POSSB_15M) // TODO: unconfirmed, verify */ \
	X(0x0C, 0, 8,  0, 216, uint8_t, FIELD_VFO1_FILTER_HIGH) \
	X(0x0D, 0, 8,  0, 216, uint8_t, FIELD_VFO1_FILTER_LOW) \
	X(0x13, 0, 8,  0, 1, uint8_t, FIELD_VFO1_FFT_SCALE) \
	\
	X(0x14, 0, 32, 500000, 30000000, uint32_t, FIELD_VFO2_FREQ) \
	X(0x18, 0, 8,  0, 2, uint8_t, FIELD_VFO2_ATT_MODE) \
	X(0x19, 0, 8, 0, 7, uint8_t, FIELD_VFO2_MOD_MODE) \
	/* X(0x1B, 0, 8, , , uint8_t, "FIELD_VFO2_POSSB_15M") // TODO: unconfirmed, verify */ \
	X(0x1A, 0, 8, 0, 3, uint8_t, FIELD_VFO2_AGC_MODE) \
	X(0x1C, 0, 8, 0, 216, uint8_t, FIELD_VFO2_FILTER_HIGH) \
	X(0x1D, 0, 8, 0, 216, uint8_t, FIELD_VFO2_FILTER_LOW) \
	X(0x23, 0, 8, 0, 1, uint8_t, FIELD_VFO2_FFT_SCALE) \
	\
	/* ------------------------------------------------------------- */ \
	/* byte 0x24 (36): ctrl flags */ \
	/* ------------------------------------------------------------- */ \
	X(0x24, 0, 1, 0, 1, uint8_t, FIELD_PANEL_LOCK) \
	X(0x24, 1, 1, 0, 1, uint8_t, FIELD_SPLIT_MODE) \
	X(0x24, 2, 1, 0, 1, uint8_t, FIELD_AUDIO_OUTSEL) \
	X(0x24, 3, 1, 0, 1, uint8_t, FIELD_MIC_COMP) \
	X(0x24, 4, 1, 0, 1, uint8_t, FIELD_NBFILT) \
	X(0x24, 5, 1, 0, 1, uint8_t, FIELD_TUNER_ON) \
	X(0x24, 6, 1, 0, 1, uint8_t, FIELD_VFO_ON) \
	/* mirrored flag: byte 0x24 bit 7 == byte 0x25 bit 5, always in sync. */ \
	/* Listing the primary copy only (0x24 bit 7) — the mirror struct itself doesn't map onto this flat table. */ \
	X(0x24, 7, 1, 0, 1, uint8_t, FIELD_TUNING_STATUS) \
	\
	/* byte 0x25 (37): bits 0-4, 6 unknown */ \
	X(0x25, 7, 1, 0, 1, uint8_t, FIELD_SHTWN_REQ) \
	\
	/* byte 0x26 (38-39): bits 1-4 of byte 38 unknown */ \
	X(0x26, 0, 1, 0, 1, uint8_t, FIELD_QSK_ON) \
	X(0x26, 5, 11, 0, 2000, uint16_t, FIELD_RCLK) /* 11-bit field, spans all of byte 39 */ \
	\
	/* byte 0x28 (40): unknown */ \
	/* byte 0x29 (41): unknown */ \
	/* byte 0x2A (42): bits 0-2 unknown */ \
	X(0x2A, 3, 7, 0, 255, uint8_t, FIELD_RF_GAIN) /* covers bits 0-1 of byte 0x2B too.  FIELD RANGE UNKNOWN*/ \
	\
	/* byte 0x2B (43): bits 4-5, 7 unknown */ \
	X(0x2B, 2, 1, 0, 1, uint8_t, FIELD_FULL_BANDMODE) \
	X(0x2B, 3, 1, 0, 1, uint8_t, FIELD_STARTUP_BEEP_ON) \
	X(0x2B, 6, 1, 0, 1, uint8_t, FIELD_DIGITAL_MODE) \
	\
	/* byte 0x2C (44) */ \
	X(0x2C, 0, 8, 0, 255, uint8_t, FIELD_RF_POWER) \
	\
	/* byte 0x2D (45): bits 4-7 unknown */ \
	X(0x2D, 0, 4, 0, 9, uint8_t, FIELD_SQL_LEVEL) \
	\
	/* byte 0x2E (46) */ \
	X(0x2E, 0, 4, 0, 16, uint8_t, FIELD_NB_WIDTH) \
	X(0x2E, 4, 4, 0, 16, uint8_t, FIELD_NB_LEVEL) \
	\
	/* byte 0x2F (47): padding */ \
	/* byte 0x30 (48) */ \
	X(0x30, 0, 8, 0, 28, uint8_t, FIELD_VOLUME) \
	\
	/* byte 0x31 (49) */ \
	X(0x31, 0, 8, 0, 20, uint8_t, FIELD_MIC_GAIN) \
	\
	/* byte 0x32 (50): unknown */ \
	/* byte 0x33 (51): bits 4-7 unknown */ \
	X(0x33, 0, 4, 0, 10, uint8_t, FIELD_CW_QSK_TIME) \
	\
	/* byte 0x34 (52) */ \
	X(0x34, 0, 8, 0, 4, uint8_t, FIELD_FREQKNOB_PLACEVAL) \
	\
	/* byte 0x35 (53): probably mem channel — TODO: verify */ \
	/* byte 0x36 (54): unknown */ \
	/* byte 0x37 (55): padding */ \
	/* byte 0x38 (56): unknown */ \
	/* byte 0x39 (57): unknown */ \
	/* byte 0x3A (58): unknown */ \
	/* byte 0x3B (59): unknown */ \
	/* byte 0x3C (60) */ \
	X(0x3C, 0, 8, 5, 50, uint8_t, FIELD_CW_WPM) \
	\
	/* byte 0x3D (61): unknown */ \
	/* byte 0x3E (62): unknown */ \
	/* byte 0x3F (63): unknown */ \
	/* byte 0x40 (64): unknown */ \
	/* byte 0x41 (65): unknown */ \
	/* byte 0x42 (66): unknown */ \
	/* byte 0x43 (67): unknown */ \
	/* byte 0x44 (68): unknown */ \
	/* byte 0x45 (69): unknown */ \
	/* byte 0x46 (70): unknown */ \
	/* byte 0x47 (71): unknown */ \
	/* byte 0x48 (72): unknown — possibly a counter */ \
	/* byte 0x49 (73): unknown */ \
	/* byte 0x4A (74): unknown */ \
	/* byte 0x4B (75): unknown — possibly an opcode */ \
	/* byte 0x4C (76): unknown */ \
	/* byte 0x4D (77): unknown */ \
	/* byte 0x4E (78): unknown */ \
	/* byte 0x4F (79): unknown */ \
	/* byte 0x50 (80): unknown — possibly CW tone */ \
	/* byte 0x51 (81): unknown — possibly CW volume */ \
	/* byte 0x52 (82): unknown */ \
	\
	/* byte 0x53 (83) */ \
	X(0x53, 0, 4, 0, 16, uint8_t, FIELD_AUX_IN_VOL) \
	X(0x53, 4, 4, 0, 16, uint8_t, FIELD_AUX_OUT_VOL) \
	\
	/* byte 0x54-0x55 (84-85) */ \
	X(0x54, 0, 16, 0, 1000, uint16_t, FIELD_RIT_OFFSET) /* GUESSING */ \
	\
	/* byte 0x56 (86): unknown */ \
	/* byte 0x57 (87): unknown */ \
	/* byte 0x58 (88): unknown */ \
	/* byte 0x59 (89): unknown */ \
	/* byte 0x5A (90): unknown */ \
	/* byte 0x5B (91): unknown */

typedef enum {
#define X(byte_offset, bit_offset, bit_length, min_value, max_value, type_size, name) name,
	G90_HEADPROTOCOL_FIELDS
#undef X
	NUMBER_FIELDS
} G90_FIELDS_INDEX;

typedef enum {
	RAM_OFFSET_FIRST_ELEMENT_IS_ZERO = -1 + G90_HEADPROTOCOL_FIRST_OFFSET, // yes, this is required.
	#define X(byte_off, bit_off, bit_len, min_val, max_val, type_size, name) \
				RAM_OFFSET_##name, \
				RAM_NEXT_##name = RAM_OFFSET_##name + sizeof(type_size) - 1,
	G90_HEADPROTOCOL_FIELDS
	#undef X
	TOTAL_RAM_STATE_SIZE
} G90_FIELD_RAM_OFFSETS;

headField g90_fields[NUMBER_FIELDS] = {
	#define X(byte_off, bit_off, bit_len, min_val, max_val, type_size, name) \
    [name] = { \
        .bit_offset = (uint16_t)((byte_off) * 8 + (bit_off)), \
        .bit_length = (uint8_t)(bit_len), \
        .min_value = (uint16_t)(min_val), \
        .max_value = (uint32_t)(max_val), \
		.ram_offset = RAM_OFFSET_##name, \
		.ram_size = sizeof(type_size), \
        },

	G90_HEADPROTOCOL_FIELDS
	#undef X
};

typedef uint8_t G90_STATE[TOTAL_RAM_STATE_SIZE]; //ram

bool g90_head_writepacket(headPacket* packet, G90_STATE* radio_master_state); //only thing we need from here.

#endif