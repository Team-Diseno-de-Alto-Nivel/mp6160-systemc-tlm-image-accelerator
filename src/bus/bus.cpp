#include "bus.h"
#include "../utils/memory_map.h"

Bus::Bus(sc_core::sc_module_name name)
    : sc_module(name) {
    target_socket.register_b_transport(this, &Bus::b_transport);
    target_socket_accel.register_b_transport(this, &Bus::b_transport_accel);
}

void Bus::b_transport(
    tlm::tlm_generic_payload& payload,
    sc_core::sc_time& delay)
{
    uint64_t addr = payload.get_address();

    std::cout
        << "[BUS] CPU acceso a 0x"
        << std::hex
        << addr
        << std::dec
        << std::endl;

    if (addr <= RAM_END)
    {
        std::cout << "[BUS] -> RAM" << std::endl;
        init_socket_ram->b_transport(
            payload,
            delay
        );
        std::cout << "[BUS] <- RAM" << std::endl;
    }
    else if (addr == ACCEL_BASE)
    {
        std::cout << "[BUS] -> Accelerator" << std::endl;
        init_socket_accel->b_transport(
            payload,
            delay
        );
        std::cout << "[BUS] <- Accelerator" << std::endl;
    }
    else if (addr >= DISK_BASE && addr <= DISK_END)
    {
        std::cout << "[BUS] -> Disk" << std::endl;
        init_socket_disk->b_transport(
            payload,
            delay
        );
        std::cout << "[BUS] <- Disk" << std::endl;
    }
    else
    {
        payload.set_response_status(
            tlm::TLM_ADDRESS_ERROR_RESPONSE);
    }

    delay += sc_core::sc_time(
        5,
        sc_core::SC_NS
    );
}


void Bus::b_transport_accel(
    tlm::tlm_generic_payload& payload,
    sc_core::sc_time& delay)
{
    uint64_t addr = payload.get_address();

    std::cout
        << "[BUS] Accelerator acceso a 0x"
        << std::hex
        << addr
        << std::dec
        << std::endl;

    if (addr <= RAM_END)
    {
        init_socket_ram->b_transport(
            payload,
            delay
        );
    }
    else
    {
        payload.set_response_status(
            tlm::TLM_ADDRESS_ERROR_RESPONSE
        );
    }

    delay += sc_core::sc_time(
        5,
        sc_core::SC_NS
    );
}



// void Bus::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& delay) {
//     // TODO: route by address range
//     // 0x00000000 – 0x03FFFFFF → RAM
//     // 0x10000000             → Accelerator config
//     // 0x20000000+            → Disk
// }

// void Bus::b_transport_accel(tlm::tlm_generic_payload& payload, sc_core::sc_time& delay) {
//     // Comentario
//     // TODO: route Accelerator→RAM requests
// }
