#include "disk.h"

#include <fstream>
#include <iostream> 
#include <cstring>
#include <sys/stat.h>   // para crear directorios

Disk::Disk(sc_core::sc_module_name name)
    : sc_module(name) {
    target_socket.register_b_transport(this, &Disk::b_transport);
}

void Disk::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& delay) {
    
    const uint64_t     addr = payload.get_address();
    unsigned char*     ptr  = payload.get_data_ptr();
    const unsigned int len  = payload.get_data_length();
 
    if (payload.get_command() == tlm::TLM_READ_COMMAND) {

        // El CPU quiere leer la imagen de entrada desde disco
        if (addr == DISK_INPUT_ADDR) {
            std::ifstream file(INPUT_PATH, std::ios::binary);
 
            if (!file.is_open()) {
                SC_REPORT_ERROR("Disk", ("No se pudo abrir: " + std::string(INPUT_PATH)).c_str());
                payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
                return;
            }
 
            file.read(reinterpret_cast<char*>(ptr), len);
 
            if (!file) {
                SC_REPORT_ERROR("Disk", "Error leyendo imagen de entrada");
                payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
                return;
            }
 
            std::cout << sc_core::sc_time_stamp()
                      << " Disk: leidos " << len
                      << " bytes desde " << INPUT_PATH << std::endl;
        }
        else {
            SC_REPORT_ERROR("Disk", "Direccion de lectura desconocida");
            payload.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
            return;
        }
    }
    else if (payload.get_command() == tlm::TLM_WRITE_COMMAND) {
        
        // El CPU quiere guardar la imagen procesada en disco
        if (addr == DISK_OUTPUT_ADDR) {
            // Crear carpeta de salida si no existe
            mkdir("images",        0755);
            mkdir("images/output", 0755);
 
            std::ofstream file(OUTPUT_PATH, std::ios::binary);
 
            if (!file.is_open()) {
                SC_REPORT_ERROR("Disk", ("No se pudo crear: " + std::string(OUTPUT_PATH)).c_str());
                payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
                return;
            }
 
            file.write(reinterpret_cast<char*>(ptr), len);
 
            std::cout << sc_core::sc_time_stamp()
                      << " Disk: escritos " << len
                      << " bytes en " << OUTPUT_PATH << std::endl;
        }
        else {
            SC_REPORT_ERROR("Disk", "Direccion de escritura desconocida");
            payload.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
            return;
        }
    }
 
    // Latencia simulada de disco: mayor que la RAM
    delay += sc_core::sc_time(100, sc_core::SC_NS);
    payload.set_response_status(tlm::TLM_OK_RESPONSE);
    
}
