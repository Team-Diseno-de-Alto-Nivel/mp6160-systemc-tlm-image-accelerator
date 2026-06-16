#include "ram.h"

RAM::RAM(sc_core::sc_module_name name)
    : sc_module(name) {
    target_socket.register_b_transport(this, &RAM::b_transport);
}

void RAM::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& delay) {
    // MOCK: responde OK sin almacenar datos — implementacion real pendiente por el equipo de RAM.
    // El Accelerator procesara datos vacios/aleatorios hasta que este mock sea reemplazado.
    payload.set_response_status(tlm::TLM_OK_RESPONSE);
}
