#pragma once

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>

SC_MODULE(CPU) {
public:
    tlm_utils::simple_initiator_socket<CPU> init_socket;

    SC_CTOR(CPU);

private:
    void run();
};
