#pragma once
#include <cstdint>

// Mapa de memoria del bus. Todos los modulos deben importar este header
// en lugar de hardcodear direcciones, para mantener consistencia.

constexpr uint64_t RAM_BASE     = 0x00000000ULL; // inicio de RAM (64 MB)
constexpr uint64_t RAM_END      = 0x03FFFFFFULL; // fin de RAM
constexpr uint64_t RAM_IMG_IN   = 0x00000000ULL; // imagen RGB de entrada (~5.9 MB)
constexpr uint64_t RAM_IMG_OUT  = 0x00600000ULL; // imagen grayscale de salida (~1.9 MB)

constexpr uint64_t ACCEL_BASE          = 0x10000000ULL; // inicio del espacio del Accelerator
constexpr uint64_t ACCEL_END           = 0x1FFFFFFFULL; // fin del espacio del Accelerator
constexpr uint64_t ACCEL_STATUS_OFFSET = 0x00000018ULL; // offset del status register del Accelerator (+24 bytes tras config)

constexpr uint64_t DISK_BASE    = 0x20000000ULL; // inicio del espacio de Disk
constexpr uint64_t DISK_END     = 0x2FFFFFFFULL; // fin del espacio de Disk
constexpr uint64_t DISK_IMG_IN  = 0x00000000ULL; // offset dentro de Disk: imagen RGB de entrada
constexpr uint64_t DISK_IMG_OUT = 0x01000000ULL; // offset dentro de Disk: imagen grayscale de salida
