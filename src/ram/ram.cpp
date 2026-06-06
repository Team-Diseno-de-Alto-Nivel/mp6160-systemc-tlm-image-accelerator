#include "ram.h"

RAM::RAM(sc_core::sc_module_name name)
    : sc_module(name) {
    target_socket.register_b_transport(this, &RAM::b_transport);
}

void RAM::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& delay) {
    // TODO: implement 64 MB byte-addressable memory read/write
}
