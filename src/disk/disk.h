#pragma once

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>

SC_MODULE(Disk) {
public:
    tlm_utils::simple_target_socket<Disk> target_socket;

    SC_CTOR(Disk);

private:
    void b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& delay);
};
