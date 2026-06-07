#include <systemc>
#include "cpu/cpu.h"
#include <tlm_utils/simple_initiator_socket.h>
#include "bus/bus.h"
#include "ram/ram.h"
#include "disk/disk.h"
#include "accelerator/accelerator.h"

struct DummyInitiator : sc_core::sc_module {
    tlm_utils::simple_initiator_socket<DummyInitiator> init_socket;

    DummyInitiator(sc_core::sc_module_name name)
        : sc_module(name)
        , init_socket("init_socket") {
    }
};

int sc_main(int argc, char* argv[]) {
    DummyInitiator dummy("dummy");
    // CPU         cpu("cpu");
    Bus         bus("bus");
    RAM         ram("ram");
    Disk        disk("disk");
    Accelerator accelerator("accelerator");

    dummy.init_socket.bind(bus.target_socket);
    // cpu.init_socket.bind(bus.target_socket);
    accelerator.init_socket.bind(bus.target_socket_accel);

    bus.init_socket_ram.bind(ram.target_socket);
    bus.init_socket_accel.bind(accelerator.target_socket);
    bus.init_socket_disk.bind(disk.target_socket);

    sc_core::sc_start();
    return 0;
}
