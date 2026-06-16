#include "ram.h"

#include <cstring>
#include <iostream>

RAM::RAM(sc_core::sc_module_name name)
    : sc_module(name),
      target_socket("target_socket"),
      memory(RAM_SIZE, 0)
{
    target_socket.register_b_transport(
        this,
        &RAM::b_transport);

    std::cout
        << "[RAM] Initialized with "
        << RAM_SIZE / (1024 * 1024)
        << " MB"
        << std::endl;
}

void RAM::b_transport(
    tlm::tlm_generic_payload& payload,
    sc_core::sc_time& delay)
{
    uint64_t addr = payload.get_address();

    unsigned char* data =
        payload.get_data_ptr();

    unsigned int len =
        payload.get_data_length();

    tlm::tlm_command cmd =
        payload.get_command();

    // Verificar que la dirección sea válida
    if ((addr + len) > memory.size())
    {
        std::cerr
            << "[RAM] Address error. Addr=0x"
            << std::hex << addr
            << " Len=" << std::dec << len
            << std::endl;

        payload.set_response_status(
            tlm::TLM_ADDRESS_ERROR_RESPONSE);

        return;
    }

    // WRITE
    if (cmd == tlm::TLM_WRITE_COMMAND)
    {
        std::memcpy(
            &memory[addr],
            data,
            len);

        std::cout
            << "[RAM] WRITE  Addr=0x"
            << std::hex << addr
            << " Size=" << std::dec << len
            << " bytes"
            << std::endl;
    }
    // READ
    else if (cmd == tlm::TLM_READ_COMMAND)
    {
        std::memcpy(
            data,
            &memory[addr],
            len);

        std::cout
            << "[RAM] READ   Addr=0x"
            << std::hex << addr
            << " Size=" << std::dec << len
            << " bytes"
            << std::endl;
    }
    else
    {
        payload.set_response_status(
            tlm::TLM_COMMAND_ERROR_RESPONSE);

        return;
    }

    // Latencia simulada
    delay += sc_core::sc_time(10, sc_core::SC_NS);

    payload.set_response_status(
        tlm::TLM_OK_RESPONSE);
}