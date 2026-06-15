#include "ram.h"

RAM::RAM(sc_core::sc_module_name name)
    : sc_module(name) {
    target_socket.register_b_transport(this, &RAM::b_transport);
}

void RAM::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& delay) {
    // TODO: implement 64 MB byte-addressable memory read/write
}





// *** Esto de abajo es una prueba del bus, debería poder ser eliminada o comentada sin afectar el principal -JesúsC
// void RAM::b_transport(
//     tlm::tlm_generic_payload& payload,
//     sc_core::sc_time& delay)
// {
//     std::cout
//         << "[RAM] Transaccion recibida"
//         << std::endl;

//     payload.set_response_status(
//         tlm::TLM_OK_RESPONSE);
// }