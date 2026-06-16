#include "disk.h"

Disk::Disk(sc_core::sc_module_name name)
    : sc_module(name) {
    target_socket.register_b_transport(this, &Disk::b_transport);
}

void Disk::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& delay) {
    // MOCK: responde OK sin acceder al filesystem — implementacion real pendiente por el equipo de Disk.
    // Sin imagen de entrada ni salida real hasta que este mock sea reemplazado.
    std::cout << "[DISK] Transaccion recibida en 0x"
              << std::hex << payload.get_address() << std::dec
              << " (" << payload.get_data_length() << " bytes)" << std::endl;
    payload.set_response_status(tlm::TLM_OK_RESPONSE);
}
