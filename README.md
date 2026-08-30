# Xeigu-ProtoLib

The current implementation of me (@Novatillion) and @Zindswini's attempt to reverse engineer the Xeigu G90 head and base protocol.

#The Story Stuff

A little while ago separate projects emerged, and a mutual friend James came to ask if I had made any progress on anything related. @Zindswini, starting with "ham-bridge" had already attempted to convert the G90 into a remote station via a custom head and a separate serial interface, which is when the idea for me to pick this project back up emerged. Some previous work had already been done by others, but a few separate attempts were made, cumulating in the verification of much of the packet structure using sniffing (credit @Zindswini for actually getting that done) and the analysis in Ghidra of a few publicly provided binary's, which I primarily handled.

Sadly, due to the accidental security implementations of Xeigu engineers and one intentional enabling of FRP on the actual G90 microcontroller, and due to the fact the actual firmware inserts values into a G90 struct which appears to without protection be memcpy'd into the transport pipeline, breaking down the GSOC binary did little than give us more names to search for (due to the inclusion of literally every string), and validate specific things about the transport pipeline and byte ranges.

This project then started to seem necessary, regarding the fact we still had no way to do packet synthesis.

#The Project

What you will find in this REPO is the **currently incomplete** light weight architecture agnostic implementation of emulating a G90 head state.

The project should currently consume less than 300 bytes of RAM and comparable amounts of rodata. This was kept intentionally minimal to allow for the use of anything from serious processing units to very small microcontrollers.

#Implementation

The current code is written entirely in C99, which can theoretically be compiled in C89 if you're about that life. The entire implementation methodology folds out to the following.

**CURRENT IMPLEMENTATION SEQUENCE**

- Create transport struct
- Create security struct
- Initialize the master G90 struct
- Merge using g90_init()
- Pass off g90_process() with the G90 struct to a event handler or managed loop for continuous running. (Note: This is decently inefficient right now, but due to the nature of this project, it may persist).
- Make calls to either enable transmission, set or get state fields, or manage emergency modes for the emulated head.

**WALKTHROUGH**

'''
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
'''


**_NOTE: ALL CODE WRITTEN HERE WAS ENTIRELY PRODUCED BY MYSELF. THERE WAS NO USE OF ANY REVERSE ENGINEERED OR AI GENERATED CODE (beside Claude which did copy my protocol map csv to the cursed X-macro you will see eventually)._**
