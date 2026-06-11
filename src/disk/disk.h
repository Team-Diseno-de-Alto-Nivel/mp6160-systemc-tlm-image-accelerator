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
  
    // Rutas a las carpetas de entrada y salida
    static constexpr const char* INPUT_PATH  = "images/input/imagen.raw";
    static constexpr const char* OUTPUT_PATH = "images/output/salida.raw";
 
    // Offsets que el CPU usa para distinguir lectura y escritura
    static constexpr uint64_t DISK_INPUT_ADDR  = 0x00000000ULL;
    static constexpr uint64_t DISK_OUTPUT_ADDR = 0x01000000ULL;

};
