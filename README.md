# Xeigu-ProtoLib

<big style="color:red"> Note: This project is WIP.... full decoding has not been decoded AND implemented yet. Do not currently implement!</big>

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

#Transport
~~~cpp

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

~~~

Initialize the transport struct, pass in three function pointers to your transport pipeline. Busy can be used to indicate if the pipeline requires another packet. There is not a buffer run, and it is up to the user to implement their own architecture when it comes to how to handle dropped packets. The suggested architecture is simply sending the last transmission.

**TODO: Give example functions for emulating transport**

#Security

~~~cpp
typedef struct {
    void (*enter)(void* context);
    void (*exit)(void* context);

    void* context; 
} G90_SECURITY;
~~~

Then, what follows is the security struct. Create the exposed struct and pass in whatever security you desire. A constant state of the G90 is maintained in memory, and this should be designed to prevent mutation during transition to the transport pipeline or editing the same parameter at the same time.

This can be mutex for multi-threaded applications, or can be longer functions with DMA control, or move between levels. It is once again up to the user.

#Master Init

Then, call the global function.

```cpp 
bool g90_init(G90 *radio, G90_TRANSPORT* config_transport, G90_SECURITY* config_lockout)
```

This will merge the three into the G90 struct.

Then, call...

```cpp

while(true && watchdog_good()) //EXAMPLE: DO NOT COPY
{
    void g90_process(G90* radio)
    //remainder of stuff.
}
```
**After that you can manipulate the G90 struct with the provided helper functions, and pass the packets off to your transport pipeline, or use requests to get the state for displays. It is also up to the user to handle the storage of default configurations, and to load those configurations before startup, as this is not a full stack project.**

#Disclaimers and cool stuff

**_NOTE: ALL CODE WRITTEN HERE WAS ENTIRELY PRODUCED BY MYSELF. THERE WAS NO USE OF ANY REVERSE ENGINEERED OR AI GENERATED CODE (beside Claude which did copy my protocol map csv to the cursed X-macro you will see eventually)._**

**_OTHER COOL PROJECTS:_**

Please be aware I cannot verify the validity of either of these projects, as I have not run any of their code or implemented any of their methods, but the following are extremely cool.

[Zeroping: Initial work to RE] (https://github.com/zeroping/xiegu-g90-headprotocol). Note, while this is insanely cool, the last commit was six years ago, and several things have changed (ex. the removal of FFT scale, which is still being experimented with for fear of bricking the admittedly well priced radio units we have).

[OpenHamradioFirmware: Firmware RE Tools] (https://github.com/OpenHamradioFirmware/G90Tools). This was noticed, but not tried due to the one single security feature implemented being flash readout protection, which with enabled makes me too scared to try anything involving this, so use at your own risk.

**That is all, and 73!**
