#include "ram.h"

RAM::RAM(sc_core::sc_module_name name)
    : sc_module(name) {
    target_socket.register_b_transport(this, &RAM::b_transport);
}

void RAM::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& delay) {
    // MOCK: responds OK without storing data — real implementation pending from the RAM team.
    // The Accelerator will process empty/random data until this mock is replaced.
    payload.set_response_status(tlm::TLM_OK_RESPONSE);
}
