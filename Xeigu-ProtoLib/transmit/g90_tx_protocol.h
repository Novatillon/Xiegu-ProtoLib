//.\g90_tx_protocol.h

#ifndef G90_TX_PROTOCOL
#define G90_TX_PROTOCOL

#define G90_HEADPROTOCOL_FRAME_SIZE 96
#define G90_HEADPROTOCOL_SYNCH_WORD 0x0000AA55u //written to pkt LE, ends up 55A...

typedef struct {
	uint8_t raw[G90_HEADPROTOCOL_FRAME_SIZE]; //stupid, byte rep of pkt, handled with offsets.
} headPacket;

typedef struct {
	uint16_t bit_offset; //macro'd from byte and bit offset.
	uint8_t  bit_length;
} headField;

typedef struct {
	headField copies[2]; //worst offender appears to be 2 len, so hardcode. varidaic macro todo if worse.
} fieldMirror;

bool g90_head_writepacket(headPacket* packet, G90_STATE* radio_master_state) //only thing we need from here.


//prepare yourself for what you are about to witness, take a deep breath, and remember the magic words
//it all ends up in rodata.

//now to the person who attempted to "fix" this, a. thank you, b. please also commit the amount of time
//wasted when such a push was rolled back because it apparently hung every pointer under the sun and just
//moved the hardcoding to the master status -> packet pipeline instead of here.

// ---------------------------------------------------------------------
// VFO block (VFO1 = index 0, VFO2 = index 1, mirrored at +0x10 stride)
// ---------------------------------------------------------------------
extern const headField FIELD_VFO_FREQ[2];
extern const headField FIELD_VFO_ATT_MODE[2];
extern const headField FIELD_VFO_MOD_MODE[2];
extern const headField FIELD_VFO_AGC_MODE[2];
// extern const headField FIELD_POSSB_15M[2]; // TODO: unconfirmed, verify
extern const headField FIELD_FILTER_HIGH[2];
extern const headField FIELD_FILTER_LOW[2];
extern const headField FIELD_FFT_SCALE[2];

// ---------------------------------------------------------------------
// byte 0x24 (36): ctrl flags
// ---------------------------------------------------------------------
extern const headField FIELD_PANEL_LOCK;
extern const headField FIELD_SPLIT_MODE;
extern const headField FIELD_AUDIO_OUTSEL;
extern const headField FIELD_MIC_COMP;
extern const headField FIELD_NBFILT;
extern const headField FIELD_TUNER_ON;
extern const headField FIELD_VFO_ON;

// mirrored flag: byte 0x24 bit 7 == byte 0x25 bit 5, always in sync
extern const fieldMirror FIELD_TUNING_STATUS;

// byte 0x25 (37): bits 0-4, 6 unknown
extern const headField FIELD_SHTWN_REQ;

// byte 0x26 (38-39): bits 1-4 of byte 38 unknown
extern const headField FIELD_QSK_ON;
extern const headField FIELD_RCLK; // 11-bit field, spans all of byte 39

// byte 0x28 (40): unknown
// byte 0x29 (41): unknown
// byte 0x2A (42): bits 0-2 unknown
extern const headField FIELD_RF_GAIN; // covers bits 0-1 of byte 43 too

// byte 0x2B (43): bits 4-5, 7 unknown
extern const headField FIELD_FULL_BANDMODE;
extern const headField FIELD_STARTUP_BEEP_ON;
extern const headField FIELD_DIGITAL_MODE;

// byte 0x2C (44)
extern const headField FIELD_RF_POWER;

// byte 0x2D (45): bits 4-7 unknown
extern const headField FIELD_SQL_LEVEL;

// byte 0x2E (46)
extern const headField FIELD_NB_WIDTH;
extern const headField FIELD_NB_LEVEL;

// byte 0x2F (47): padding
// byte 0x30 (48)
extern const headField FIELD_VOLUME;

// byte 0x31 (49)
extern const headField FIELD_MIC_GAIN;

// byte 0x32 (50): unknown
// byte 0x33 (51): bits 4-7 unknown
extern const headField FIELD_CW_QSK_TIME;

// byte 0x34 (52)
extern const headField FIELD_FREQKNOB_PLACEVAL;

// byte 0x35 (53): probably mem channel — TODO: verify
// byte 0x36 (54): unknown
// byte 0x37 (55): padding
// byte 0x38 (56): unknown
// byte 0x39 (57): unknown
// byte 0x3A (58): unknown
// byte 0x3B (59): unknown
// byte 0x3C (60)
extern const headField FIELD_CW_WPM;

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
extern const headField FIELD_AUX_IN_VOL;
extern const headField FIELD_AUX_OUT_VOL;

// byte 0x54-0x55 (84-85)
extern const headField FIELD_RIT_OFFSET;

// byte 0x56 (86): unknown
// byte 0x57 (87): unknown
// byte 0x58 (88): unknown
// byte 0x59 (89): unknown
// byte 0x5A (90): unknown
// byte 0x5B (91): unknown
// bytes 0x5C-0x60 (92-96): CRC32-MPEG2


#endif