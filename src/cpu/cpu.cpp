#include "cpu.h"

CPU::CPU(sc_core::sc_module_name name)
    : sc_module(name) {
    SC_THREAD(run);
}

void CPU::run() {
    // TODO: implement full flow — load image, store to RAM,
    // configure Accelerator, fetch result, save to Disk
}
