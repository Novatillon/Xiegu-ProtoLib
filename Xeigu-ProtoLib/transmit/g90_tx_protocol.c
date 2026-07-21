#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "g90_tx_protocol.h" //if anything else touches this file put it down immeaditily, or i will.
#include ".\checksum\crc.h"

//write CRC and synch byte helper function.
static inline void write_32bit_le(headPacket *packet , uint32_t value, size_t offset) {
    packet->raw[offset    ] = value & 0xFF;
    packet->raw[offset + 1] = (value >> 8) & 0xFF;
    packet->raw[offset + 2] = (value >> 16) & 0xFF;
    packet->raw[offset + 3] = (value >> 24) & 0xFF;
}

//generic code, found online.
static inline void write_bits_to_buffer(uint8_t* buffer, uint32_t bit_offset, uint8_t bit_length, uint32_t value) {

    if (bit_length == 0 || bit_length > 32) {
        return;
    }

    uint32_t mask = (bit_length == 32) ? 0xFFFFFFFF : ((1UL << bit_length) - 1);

    value &= mask;

    for (uint8_t i = 0; i < bit_length; i++) {
        uint32_t target_bit = bit_offset + i;
        uint32_t byte_idx = target_bit / 8;
        uint8_t  bit_shift = target_bit % 8; 

        uint8_t bit_val = (value >> i) & 1;

        buffer[byte_idx] &= ~(1U << bit_shift);

        buffer[byte_idx] |= (bit_val << bit_shift);
    }
}

static inline void write_field_to_packet(const G90_STATE radio_state, headPacket* packet, G90_FIELDS_INDEX index) {
    if (radio_state == NULL || packet == NULL || index >= NUMBER_FIELDS) {
        return;
    }

    uint16_t ram_offset = g90_fields[index].ram_offset;
    uint8_t  ram_size = g90_fields[index].ram_size;
    uint16_t bit_offset = g90_fields[index].bit_offset;
    uint8_t  bit_length = g90_fields[index].bit_length;

    // Bounds check on RAM array
    if (ram_offset + ram_size > TOTAL_RAM_STATE_SIZE) {
        return;
    }

    uint32_t raw_val = 0;

    switch (ram_size) {
        case sizeof(uint8_t) :
            raw_val = radio_state[ram_offset];
            break;

        case sizeof(uint16_t) :
            raw_val = (uint32_t)radio_state[ram_offset] |
            ((uint32_t)radio_state[ram_offset + 1] << 8);
            break;

        case sizeof(uint32_t) :
            raw_val = (uint32_t)radio_state[ram_offset] |
            ((uint32_t)radio_state[ram_offset + 1] << 8) |
            ((uint32_t)radio_state[ram_offset + 2] << 16) |
            ((uint32_t)radio_state[ram_offset + 3] << 24);
            break;

        default:
            return; //i'd be greatly concerned if this ever existed.
    }

    // Write directly to the packet buffer
    write_bits_to_buffer(packet->raw, bit_offset, bit_length, raw_val);
}

//securities are handled in the scheduler function, such that this can be pretty basic.
bool g90_head_writepacket(headPacket *packet, G90_STATE* radio_master_state) {

    //anti-seg fault.
    if (packet == NULL || radio_master_state == NULL) {
        return false; //code for unsucessful, throws panic gaurdian.
    }

    memset(packet, 0, G90_HEADPROTOCOL_FRAME_SIZE);

    //write synch word.
    write_32bit_le(packet, G90_HEADPROTOCOL_SYNCH_WORD, 0);

    for (int i = 0; i < NUMBER_FIELDS; i++) {
        write_field_to_packet(radio_master_state, packet, i);
    }

    uint32_t checksum = crc32_mpeg2(packet, G90_HEADPROTOCOL_FRAME_SIZE - 4);

    write_32bit_le(packet, checksum, G90_HEADPROTOCOL_FRAME_SIZE - 4);

    return true;
}

