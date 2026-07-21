#include <stdint.h>
#include <stdbool.h>

#include "g90.h"
#include ".\transmit\g90_tx_protocol.h"
#include ".\receive\g90_rx_protocol.h"

//lock and unlock macro functions
static inline void g90_lock(G90* radio) {
	radio->security->enter(radio->security->context);
	radio->status_locked = true;
}

static inline void g90_unlock(G90* radio) {
	radio->security->exit(radio->security->context);
	radio->status_locked = false;
}


bool g90_init(G90 *radio, G90_TRANSPORT* config_transport, G90_SECURITY* config_lockout) {

	//do not condense, line by line will actually save us here
	//because a struct return is too much for embedded, verify before.
	if (radio == NULL || config_transport == NULL || config_lockout == NULL) {
		return false;
	}

	if (config_transport->send == NULL || config_transport->receive == NULL || config_transport->busy == NULL) {
		return false;
	}

	if (config_lockout->enter == NULL || config_lockout->exit == NULL) {
		return false;
	}

	memset(radio, 0, sizeof(*radio));

	//set the config's to the user configed structs.
	radio->transport = config_transport;
	radio->security = config_lockout;

	radio->radio_process_status = G90_STATE_STARTUP;

	radio->version = 0;
	radio->status_locked = false;
	radio->dirty = false;

	return true;
}

void g90_process(G90* radio) {

	//start rx first, always do rx no matter what.
	uint16_t length = radio->transport->receive((uint8_t*)&radio->rx_buffer,
												G90_BASEPROTOCOL_FRAME_SIZE,
												radio->transport->context);
	if (length > 0) {
		//g90_rx_handler() TODO: im sure theres a better way to do this loop too, but it should be low cost to implement later.
	}
	
	G90_GAURDIAN radio_process_status = radio->radio_process_status; //interchangable, i dont mind.

	if (radio_process_status == G90_STATE_STARTUP || radio_process_status == G90_STATE_PANIC) { 
		return; //the user has not called the helper/loaded their stored pre-config. Redo.
	}
	else if (radio_process_status == G90_STATE_PACKET_PENDING) {
		if (!radio->transport->busy(radio->transport->context)) {
			
			radio->radio_process_status = G90_STATE_SAFE;
			radio_process_status = G90_STATE_SAFE;
		}
	}

	bool transmit_now = radio->dirty && 
						radio_process_status == G90_STATE_SAFE && 
						radio->tx_enabled;

	if (transmit_now) {
		g90_lock(radio);

		if (g90_head_writepacket(&radio->tx_packet, radio->radio_master_state)) {
			radio->transport->send(radio->tx_packet.raw,
											sizeof(radio->tx_packet.raw),
											radio->transport->context);
			g90_unlock(radio);
			radio->radio_process_status = G90_STATE_PACKET_PENDING;
			radio->dirty = false;
		}
		else {
			g90_unlock(radio);
		}
	}
	
	return;
}

uint32_t g90_get_state_field(const G90 *radio, G90_FIELDS_INDEX index) {
	uint32_t value = 0;

	uint16_t ram_offset = g90_fields[index].ram_offset;

	switch (g90_fields[index].ram_size) {
		case 1:
			//no check, or we'd be finding out MUCH sooner.

			value = radio->radio_master_state[ram_offset];
			break;
		case 2:
			value = ((uint16_t)radio->radio_master_state[ram_offset]) |
					((uint16_t)radio->radio_master_state[ram_offset + 1] << 8);
			break;
		case 4:
			value = ((uint32_t)radio->radio_master_state[ram_offset]) |
					((uint32_t)radio->radio_master_state[ram_offset + 1] << 8) |
					((uint32_t)radio->radio_master_state[ram_offset + 2] << 16) |
					((uint32_t)radio->radio_master_state[ram_offset + 3] << 24);
			break;
		default:
			return UINT32_MAX;
	}

	return value;
}

bool g90_set_state_field(G90 *radio, G90_FIELDS_INDEX index, uint32_t value) {

	if (radio->status_locked == true) {
		return false;
	}

	uint16_t ram_offset = g90_fields[index].ram_offset;
	uint8_t ram_size = g90_fields[index].ram_size;

	uint32_t max_value = g90_fields[index].max_value;
	uint16_t min_value = g90_fields[index].min_value;

	if (value > max_value) {
		value = max_value;
	}

	if (value < min_value) {
		value = min_value;
	}

	switch (ram_size)
	{
	case 1:
		radio->radio_master_state[ram_offset] = value;

		radio->dirty = true;
		return true;
	case 2:
		radio->radio_master_state[ram_offset] =     (uint8_t)value;
		radio->radio_master_state[ram_offset + 1] = (uint8_t)(value >> 8);

		radio->dirty = true;
		return true;
	case 4:
		radio->radio_master_state[ram_offset] =     (uint8_t)value;
		radio->radio_master_state[ram_offset + 1] = (uint8_t)(value >> 8);
		radio->radio_master_state[ram_offset + 2] = (uint8_t)(value >> 16);
		radio->radio_master_state[ram_offset + 3] = (uint8_t)(value >> 24);

		radio->dirty = true;
		return true;
	default:
		return false;
	}
}

void g90_enable_transmission(G90* radio) {
	radio->tx_enabled = true;
	radio->radio_process_status = G90_STATE_SAFE;

	g90_unlock(radio); //We pass the user everything, someones gonna make a mistake this will solve somehow.
}

void g90_disable_transmission(G90* radio) {
	radio->tx_enabled = false;
}