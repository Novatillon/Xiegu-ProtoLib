#include <stdint.h>
#include <stdbool.h>

#include "g90.h"
#include ".\transmit\g90_tx_protocol.h"
#include ".\receive\g90_rx_protocol.h"

//lock and unlock macro functions
static inline void g90_lock(G90* radio) {
	radio->security->enter(radio->security->context);
	radio->status_locked == true;
}

static inline void g90_unlock(G90* radio) {
	radio->security->exit(radio->security->context);
	radio->status_locked == false;
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
			bool sent = radio->transport->send(radio->tx_packet.raw,
											sizeof(radio->tx_packet.raw),
											radio->transport->context);
			g90_unlock(radio);
		} 
	}
	

}

