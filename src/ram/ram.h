#pragma once

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>
#include <cstdint>

SC_MODULE(RAM) {
public:
    tlm_utils::simple_target_socket<RAM> target_socket;

    SC_CTOR(RAM);

private:
    void b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& delay);
};
