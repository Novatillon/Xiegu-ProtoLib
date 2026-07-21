#include <stdint.h>
#include <stddef.h>

//precompute nibbletable, write to flash or rodata.
static const uint32_t crc32_mpeg2_nibble_table[16] = {
    0x00000000, 0x04C11DB7, 0x09823B6E, 0x0D4326D9,
    0x130476DC, 0x17C56B6B, 0x1A864DB2, 0x1E475005,
    0x2608EDB8, 0x22C9F00F, 0x2F8AD6D6, 0x2B4BCB61,
    0x350C9B64, 0x31CD86D3, 0x3C8EA00A, 0x384FBDBD
};

static uint32_t crc32_mpeg2(const uint8_t* data, size_t len) {
    
    uint32_t crc = 0xFFFFFFFF;

    for (size_t i = 0; i < len; i++)
    {
        uint8_t b = data[i ^ 3];
        
        crc = (crc << 4) ^
            crc32_mpeg2_nibble_table[((crc >> 28) ^ (b >> 4)) & 0xF];

        crc = (crc << 4) ^
            crc32_mpeg2_nibble_table[((crc >> 28) ^ (b & 0xF)) & 0xF];
    }

    return crc;
}
