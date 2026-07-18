cmake_minimum_required (VERSION 3.12)
project (Xeigu-ProtoLib C)

# Add source to this project's executable
add_executable (Xeigu-ProtoLib "tests/masterTest.c" "Xeigu-ProtoLib.h" "transmit/checksum/crc.c" "transmit/checksum/crc.h" "g90.h" "transmit/g90_tx_protocol.h" "transmit/g90_tx_protocol.c" "Xeigu-ProtoLib_inpl.c" "receive/g90_rx_protocol.h")

# Enforce the C11 standard globally for this target
set_target_properties(Xeigu-ProtoLib PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF
)
