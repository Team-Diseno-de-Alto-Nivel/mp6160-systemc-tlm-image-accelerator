#include "accelerator.h"
#include "../utils/conversion.h"
#include <cstdio>

Accelerator::Accelerator(sc_core::sc_module_name name)
     : sc_module(name) {
     target_socket.register_b_transport(this, &Accelerator::b_transport);
 }

uint8_t Accelerator::rgb_to_gray(uint8_t r, uint8_t g, uint8_t b) {
    return ::rgb_to_gray(r, g, b);
}

void Accelerator::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& delay) {
    if (payload.get_command() != tlm::TLM_WRITE_COMMAND) {
        payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return;
    }

    if (payload.get_data_length() != 24) {
        payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return;
    }

    unsigned char* data = payload.get_data_ptr();
    uint64_t src_addr = *reinterpret_cast<uint64_t*>(&data[0]);
    uint64_t dst_addr = *reinterpret_cast<uint64_t*>(&data[8]);
    uint64_t pixel_count = *reinterpret_cast<uint64_t*>(&data[16]);

    char cfg_msg[128];
    snprintf(cfg_msg, sizeof(cfg_msg),
             "Configuración recibida — src=0x%lX, dst=0x%lX, pixeles=%lu",
             (unsigned long)src_addr, (unsigned long)dst_addr, (unsigned long)pixel_count);
    SC_REPORT_INFO(name(), cfg_msg);

    sc_core::sc_spawn(
        sc_bind(&Accelerator::process_image, this, src_addr, dst_addr, pixel_count)
    );

    payload.set_response_status(tlm::TLM_OK_RESPONSE);
}

void Accelerator::process_image(uint64_t src_addr, uint64_t dst_addr, uint64_t pixel_count) {
    SC_REPORT_INFO(name(), "Procesamiento iniciado");
    const uint64_t step = (pixel_count >= 10) ? (pixel_count / 10) : 0;
    for (uint64_t i = 0; i < pixel_count; ++i) {
        if (step > 0 && i > 0 && i % step == 0) {
            char prog_msg[32];
            snprintf(prog_msg, sizeof(prog_msg), "%lu%% completado", (unsigned long)(i * 100 / pixel_count));
            SC_REPORT_INFO(name(), prog_msg);
        }
        uint64_t src_offset = src_addr + (i * 3);
        uint64_t dst_offset = dst_addr + i;

        unsigned char rgb_data[3];
        tlm::tlm_generic_payload read_payload;
        read_payload.set_command(tlm::TLM_READ_COMMAND);
        read_payload.set_address(src_offset);
        read_payload.set_data_ptr(rgb_data);
        read_payload.set_data_length(3);
        sc_core::sc_time delay = sc_core::SC_ZERO_TIME;
        init_socket->b_transport(read_payload, delay);

        uint8_t r = rgb_data[0];
        uint8_t g = rgb_data[1];
        uint8_t b = rgb_data[2];
        uint8_t gray = rgb_to_gray(r, g, b);

        unsigned char gray_data[1] = {gray};
        tlm::tlm_generic_payload write_payload;
        write_payload.set_command(tlm::TLM_WRITE_COMMAND);
        write_payload.set_address(dst_offset);
        write_payload.set_data_ptr(gray_data);
        write_payload.set_data_length(1);
        init_socket->b_transport(write_payload, delay);
    }
    SC_REPORT_INFO(name(), "Procesamiento completo");
}



// *** Esto de abajo es una prueba del bus, debería poder ser eliminada o comentada sin afectar el principal -JesúsC
// void Accelerator::b_transport(
//     tlm::tlm_generic_payload& payload,
//     sc_core::sc_time& delay)
// {
//     std::cout
//         << "[ACCEL] Transaccion recibida"
//         << std::endl;

//     payload.set_response_status(
//         tlm::TLM_OK_RESPONSE);
// }