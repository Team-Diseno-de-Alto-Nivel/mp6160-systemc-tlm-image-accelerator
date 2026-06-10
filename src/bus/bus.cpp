#include "bus.h"

Bus::Bus(sc_core::sc_module_name name)
    : sc_module(name) {
    target_socket.register_b_transport(this, &Bus::b_transport);
    target_socket_accel.register_b_transport(this, &Bus::b_transport_accel);
}

void Bus::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& delay) {
    // TODO: route by address range
    // 0x00000000 – 0x03FFFFFF → RAM
    // 0x10000000             → Accelerator config
    // 0x20000000+            → Disk
}

void Bus::b_transport_accel(tlm::tlm_generic_payload& payload, sc_core::sc_time& delay) {
    // Comentario
    // TODO: route Accelerator→RAM requests
}
