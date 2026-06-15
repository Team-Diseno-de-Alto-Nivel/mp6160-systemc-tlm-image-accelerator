#include "disk.h"

Disk::Disk(sc_core::sc_module_name name)
    : sc_module(name) {
    target_socket.register_b_transport(this, &Disk::b_transport);
}

void Disk::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& delay) {
    // TODO: implement filesystem read/write to images/input/ and images/output/
}





// *** Esto de abajo es una prueba del bus, debería poder ser eliminada o comentada sin afectar el principal -JesúsC
// void Disk::b_transport(
//     tlm::tlm_generic_payload& payload,
//     sc_core::sc_time& delay)
// {
//     std::cout
//         << "[DISK] Transaccion recibida"
//         << std::endl;

//     payload.set_response_status(
//         tlm::TLM_OK_RESPONSE);
// }