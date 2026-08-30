# Xeigu-ProtoLib

The current implementation of me (@Novatillion) and @Zindswini's attempt to reverse engineer the Xeigu G90 head and base protocol.

A little while ago separate projects emerged, and a mutual friend James came to ask if I had made any progress on anything related. @Zindswini, starting with "ham-bridge" had already attempted to convert the G90 into a remote station via a custom head and a separate serial interface, which is when the idea for me to pick this project back up emerged. Some previous work had already been done by others, but a few separate attempts were made, cumulating in the verification of much of the packet structure using sniffing (credit @Zindswini for actually getting that done) and the analysis in Ghidra of a few publicly provided binary's, which I primarily handled.

Sadly, due to the accidental security of Xeigu engineers and one intentional enabling of FRP on the actual G90 microcontroller, and due to the fact the actual firmware inserts values into a G90 struct which appears to without protection be memcpy'd into the transport pipeline, breaking down the GSOC binary did little than give us more names to search for (due to the inclusion of literally every string), and validate specific things about the transport pipeline and byte ranges.

This project then started to seem necessary, regarding the fact we still had no way to do packet synthesis.

What you will find in this REPO is the **currently incomplete** light weight architecture agnostic implementation of emulating a G90 head state.

**_NOTE: ALL CODE WRITTEN HERE WAS ENTIRELY PRODUCED BY MYSELF. THERE WAS NO USE OF ANY REVERSE ENGINEERED OR AI GENERATED CODE (beside Claude which did copy my protocol map csv to the cursed X-macro you will see eventually._**
