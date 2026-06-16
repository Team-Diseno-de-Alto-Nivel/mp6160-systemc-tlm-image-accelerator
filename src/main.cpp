#include <systemc>

#include "cpu/cpu.h"
#include "bus/bus.h"
#include "ram/ram.h"
#include "disk/disk.h"
#include "accelerator/accelerator.h"

int sc_main(int argc, char* argv[])
{
    CPU         cpu("cpu");
    Bus         bus("bus");
    RAM         ram("ram");
    Disk        disk("disk");
    Accelerator accelerator("accelerator");

    std::cout << "*** Modulos creados ***" << std::endl;

    // CPU → Bus (ruta principal del pipeline)
    cpu.init_socket.bind(bus.target_socket);

    // Accelerator → Bus (ruta de acceso a RAM durante el procesamiento de imagen)
    accelerator.init_socket.bind(bus.target_socket_accel);

    // Bus → periféricos
    bus.init_socket_ram.bind(ram.target_socket);
    bus.init_socket_accel.bind(accelerator.target_socket);
    bus.init_socket_disk.bind(disk.target_socket);

    std::cout << "*** Sockets conectados ***" << std::endl;

    sc_core::sc_start();

    std::cout << "*** Simulacion completada :) ***" << std::endl;
    return 0;
}
