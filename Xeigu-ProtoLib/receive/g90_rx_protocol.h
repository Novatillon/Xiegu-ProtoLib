#ifndef G90_RX_PROTOCOL
#define G90_RX_PROTOCOL

#define G90_BASEPROTOCOL_FRAME_SIZE 376

typedef struct {
	raw[G90_BASEPROTOCOL_FRAME_SIZE];
} basePacket;

#endif // !G90_RX_PROTOCOL
