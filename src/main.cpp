#include <systemc>

#include "cpu/cpu.h"
#include "bus/bus.h"
#include "ram/ram.h"
#include "disk/disk.h"
#include "accelerator/accelerator.h"


// *** la función Test la cree para que llame en ligar el CPU y poder hacer pruebas, no es parte de main en realidad -JesúsC
// struct TestMaster : sc_core::sc_module
// {
//     tlm_utils::simple_initiator_socket<TestMaster> socket;

//     SC_CTOR(TestMaster)
//     {
//         SC_THREAD(run);
//     }

//     void send(uint64_t addr)
//     {
//         unsigned char data[4] = {0};
//         tlm::tlm_generic_payload trans;
//         sc_core::sc_time delay =
//             sc_core::SC_ZERO_TIME;
//         trans.set_command(
//             tlm::TLM_READ_COMMAND);
//         trans.set_address(addr);
//         trans.set_data_ptr(data);
//         trans.set_data_length(4);
//         socket->b_transport(
//             trans,
//             delay);
//         if(trans.is_response_error())
//         {
//             std::cout
//                 << "[MASTER] ERROR en 0x"
//                 << std::hex
//                 << addr
//                 << std::dec
//                 << std::endl;
//         }
//     }
//     void run()
//     {
//         std::cout
//             << "\n===== INICIO PRUEBAS BUS =====\n"
//             << std::endl;
//         send(0x00000000ULL);   // RAM
//         send(0x10000000ULL);   // Accelerator
//         send(0x20000000ULL);   // Disk
//         send(0x50000000ULL);   // Inválida
//         std::cout
//             << "\n===== FIN PRUEBAS BUS =====\n"
//             << std::endl;
//         sc_core::sc_stop();
//     }
// };
// *** Fin de la parte de test -JesúsC




int sc_main(int argc, char* argv[])
{
    // CPU         cpu("cpu");
    //TestMaster  cpu("cpu");
    Bus         bus("bus");
    RAM         ram("ram");
    Disk        disk("disk");
    Accelerator accelerator("accelerator");

    std::cout << "*** Modulos creados ***" << std::endl;

    // cpu.init_socket.bind(bus.target_socket);
    
    accelerator.init_socket.bind(bus.target_socket_accel);
    //cpu.socket.bind(bus.target_socket);
    bus.init_socket_ram.bind(ram.target_socket);
    bus.init_socket_accel.bind(accelerator.target_socket);
    bus.init_socket_disk.bind(disk.target_socket);

    std::cout << "*** Sockets conectados ***" << std::endl;

    sc_core::sc_start();

    std::cout << "*** Simulacion completada :) ***" << std::endl;
    return 0;
}



// #include <systemc>

// #include "cpu/cpu.h"
// #include <tlm_utils/simple_initiator_socket.h>
// #include "bus/bus.h"
// #include "ram/ram.h"
// #include "disk/disk.h"
// #include "accelerator/accelerator.h"

// struct DummyInitiator : sc_core::sc_module {
//     tlm_utils::simple_initiator_socket<DummyInitiator> init_socket;

//     DummyInitiator(sc_core::sc_module_name name)
//         : sc_module(name)
//         , init_socket("init_socket") {
//     }
// };

// int sc_main(int argc, char* argv[]) {
//     DummyInitiator dummy("dummy");
//     // CPU         cpu("cpu");
//     Bus         bus("bus");
//     RAM         ram("ram");
//     Disk        disk("disk");
//     Accelerator accelerator("accelerator");

//     dummy.init_socket.bind(bus.target_socket);
//     // cpu.init_socket.bind(bus.target_socket);
//     accelerator.init_socket.bind(bus.target_socket_accel);

//     bus.init_socket_ram.bind(ram.target_socket);
//     bus.init_socket_accel.bind(accelerator.target_socket);
//     bus.init_socket_disk.bind(disk.target_socket);

//     sc_core::sc_start();
//     return 0;
// }
