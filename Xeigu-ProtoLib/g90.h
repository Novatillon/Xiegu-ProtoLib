#ifndef G90_MAIN_HEADER
#define G90_MAIN_HEADER

#include "./transmit/g90_tx_protocol.h" //
#include "./receive/g90_rx_protocol.h"

//protocol specifications: on both sides the goal of this 
//library is to allow the user to modify a state with git like functions
//allowing cloning and merging by checkout version applied on top for ordering.
//implementations will be made to ensure the system does not either do a simultanious
//status struct write or write to the status struct during packet copy.

//user modifiabile transport struct for modulatiry
//function pointers. Merged into a G90 struct on init.
typedef struct {
    bool (*send)(
        const uint8_t *data,
        uint16_t length,
        void *context
    );

    bool (*busy)(
        void *context
    );

    uint16_t (*receive)(
        uint8_t *buffer,
        uint16_t max_length,
        void *context
    );

    void *context;

} G90_TRANSPORT;

//the end goal is no two pointers or operations 
//are to happen on the same state struct at one time.
typedef enum {
    G90_STATE_STARTUP, //startup, no tx, user must move out of startup after setting config.
    G90_STATE_SAFE, //general safe to write
    G90_STATE_PACKET_WORKING, //do not touch, state being formatted for packet.
    G90_STATE_PACKET_PENDING, //the packet is being sent, it has not left yet, there is no need for another.
    G90_STATE_PANIC //emergency use only.
} G90_GAURDIAN;

typedef struct {
    G90_GAURDIAN status;
    uint32_t version;
} G90_STATUS_SNAPSHOT;

//debugging struct only
//for user to request bc optimistic archetecture.
typedef struct {
    G90_GAURDIAN status;
    uint32_t version;
    bool tx_enabled; 
    bool dirty;
} G90_DIAG_STATUS; 

//another user configuration struct, passed into radio struct on init() call, provides
//the user flexability on what type of locking they want to do.
typedef struct {
    void (*enter)(void* context);
    void (*exit)(void* context);

    void* context; //up to user, could be disable interupts, or &pthread_mutex_t;
} G90_SECURITY;

typedef struct {
    G90_STATE radio_master_state;

    const G90_TRANSPORT* transport; //merged at init, pointer to user defined.
    const G90_SECURITY* security;

    G90_GAURDIAN radio_process_status;
    bool status_locked;
    uint32_t version;
    bool tx_enabled; //its needed

    bool dirty; //decides if rederivation is required on next tx request, if not the user simply must retransmit the last packet.
    headPacket tx_packet; //written by g90_head_writepacket() I cannot think of anything else to own this, thank god for pointers.
    basePacket rx_buffer;
} G90;


#endif