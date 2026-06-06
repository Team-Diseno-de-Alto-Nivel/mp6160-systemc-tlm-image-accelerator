# Guía: Cómo Implementar un Módulo SystemC/TLM

Esta guía te enseña paso a paso cómo implementar la lógica de un módulo SystemC con comunicación TLM 2.0. La estructura de archivos ya existe — el objetivo es **completar cada stub** hasta que el módulo funcione. Usaremos el **Accelerator** como ejemplo de referencia.

---

## Paso 1: Ubicar los archivos del módulo

Cada módulo ya tiene su carpeta y archivos stub bajo `src/`:

```
src/<nombre_modulo>/
├── <nombre_modulo>.h      # Header — define sockets, constructor y métodos privados
└── <nombre_modulo>.cpp    # Implementación — completar b_transport() y la lógica

src/utils/
└── conversion.h           # Funciones auxiliares compartidas (sin dependencia SystemC)
```

**Módulos existentes:**

| Módulo | Carpeta | Estado |
|---|---|---|
| CPU | `src/cpu/` | stub — implementar `run()` |
| Bus | `src/bus/` | stub — implementar routing en `b_transport()` |
| RAM | `src/ram/` | stub — implementar read/write en `b_transport()` |
| Disk | `src/disk/` | stub — implementar I/O de archivo en `b_transport()` |
| Accelerator | `src/accelerator/` | **implementado** — usar como referencia |

---

## Paso 2: Implementar el Header (.h)

El header define la **interfaz pública** del módulo: sockets TLM, constructor, y declaraciones de métodos privados.

**Archivo:** `src/<nombre_modulo>/<nombre_modulo>.h`

### Plantilla:

```cpp
#pragma once

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <cstdint>

SC_MODULE(MiModulo) {
public:
    // Socket ENTRADA: recibe datos de otros módulos
    tlm_utils::simple_target_socket<MiModulo> target_socket;
    
    // Socket SALIDA: envía datos a otros módulos
    tlm_utils::simple_initiator_socket<MiModulo> init_socket;

    SC_CTOR(MiModulo);

private:
    void b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& delay);
    void procesar_datos(uint64_t entrada, uint64_t salida, uint64_t tamaño);
};
```

### Explicación de Sockets:

| Elemento | Tipo | Propósito |
|----------|------|----------|
| `target_socket` | Target (entrada) | Recibe transacciones de otros módulos |
| `init_socket` | Initiator (salida) | Envía transacciones a otros módulos |
| `b_transport()` | Callback | Se ejecuta cuando llega una transacción |

**Referencia real — ver [`src/accelerator/accelerator.h`](../src/accelerator/accelerator.h):**

```cpp
SC_MODULE(Accelerator) {
public:
    tlm_utils::simple_target_socket<Accelerator> target_socket;    // CPU envía config aquí
    tlm_utils::simple_initiator_socket<Accelerator> init_socket;   // Lee/escribe en RAM aquí

    SC_CTOR(Accelerator);

private:
    // Config transaction: WRITE de 24 bytes al address 0x10000000
    // Offset  +0 (8 B): src_addr    — dirección base input RGB en RAM
    // Offset  +8 (8 B): dst_addr    — dirección base output grayscale en RAM
    // Offset +16 (8 B): pixel_count — total de píxeles a procesar
    void b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& delay);
    void process_image(uint64_t src_addr, uint64_t dst_addr, uint64_t pixel_count);
    uint8_t rgb_to_gray(uint8_t r, uint8_t g, uint8_t b);
};
```

---

## Paso 3: Implementar el .cpp

El `.cpp` contiene la lógica real del módulo. El stub ya tiene el constructor y la firma de `b_transport()` — hay que completar el cuerpo.

**Archivo:** `src/<nombre_modulo>/<nombre_modulo>.cpp`

### Patrón general:

```cpp
#include "<nombre_modulo>.h"
#include <cstring>

MiModulo::MiModulo(sc_core::sc_module_name name)
    : sc_module(name) {
    target_socket.register_b_transport(this, &MiModulo::b_transport);
}

void MiModulo::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& delay) {
    // 1. Validar comando (READ o WRITE según corresponda)
    if (payload.get_command() != tlm::TLM_WRITE_COMMAND) {
        payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return;
    }

    // 2. Extraer parámetros del payload
    unsigned char* data = payload.get_data_ptr();
    uint64_t entrada = *reinterpret_cast<uint64_t*>(&data[0]);
    uint64_t salida  = *reinterpret_cast<uint64_t*>(&data[8]);
    uint64_t tamaño  = *reinterpret_cast<uint64_t*>(&data[16]);

    // 3. Lanzar proceso asincrónico
    sc_core::sc_spawn(
        sc_bind(&MiModulo::procesar_datos, this, entrada, salida, tamaño)
    );

    payload.set_response_status(tlm::TLM_OK_RESPONSE);
}

void MiModulo::procesar_datos(uint64_t entrada, uint64_t salida, uint64_t tamaño) {
    for (uint64_t i = 0; i < tamaño; ++i) {
        // LEE de RAM usando init_socket
        unsigned char buffer[4];
        tlm::tlm_generic_payload read_payload;
        read_payload.set_command(tlm::TLM_READ_COMMAND);
        read_payload.set_address(entrada + i * 4);
        read_payload.set_data_ptr(buffer);
        read_payload.set_data_length(4);
        sc_core::sc_time delay = sc_core::SC_ZERO_TIME;
        init_socket->b_transport(read_payload, delay);

        // PROCESA
        uint32_t resultado = *reinterpret_cast<uint32_t*>(buffer) * 2;

        // ESCRIBE en RAM usando init_socket
        unsigned char result_buf[4];
        *reinterpret_cast<uint32_t*>(result_buf) = resultado;
        tlm::tlm_generic_payload write_payload;
        write_payload.set_command(tlm::TLM_WRITE_COMMAND);
        write_payload.set_address(salida + i * 4);
        write_payload.set_data_ptr(result_buf);
        write_payload.set_data_length(4);
        init_socket->b_transport(write_payload, delay);
    }
}
```

### Transacción TLM — las 4 partes:

```cpp
tlm::tlm_generic_payload payload;
payload.set_command(tlm::TLM_WRITE_COMMAND);  // READ o WRITE
payload.set_address(0x00000000);               // dirección destino
payload.set_data_ptr(data);                    // buffer de datos
payload.set_data_length(24);                   // bytes a transferir
init_socket->b_transport(payload, delay);
```

**Referencia real — ver [`src/accelerator/accelerator.cpp`](../src/accelerator/accelerator.cpp)**

---

## Paso 3b: Agregar logging con SC_REPORT_INFO

Cada módulo reporta sus propias acciones usando `SC_REPORT_INFO`. No se usa una clase Logger separada — SystemC ya provee timestamps y nombre del módulo automáticamente.

### Patrón básico:

```cpp
SC_REPORT_INFO(name(), "mensaje descriptivo");
```

Para mensajes con variables, usar `snprintf`:

```cpp
char msg[128];
snprintf(msg, sizeof(msg), "Leyendo %u bytes desde 0x%lX", len, (unsigned long)addr);
SC_REPORT_INFO(name(), msg);
```

### Qué loguear por tipo de módulo:

| Módulo | Qué loguear |
|---|---|
| CPU | Cada uno de los 6 pasos del flujo |
| Disk | Apertura de archivo, bytes leídos/escritos |
| RAM | Solo transferencias bulk (evitar log por píxel) |
| Bus | Decisión de routing para transferencias bulk |
| Accelerator | Config recibida, inicio, progreso cada 10%, fin |

### Evitar flooding en loops de millones de iteraciones:

Si el módulo procesa millones de elementos (como el Accelerator con 2M píxeles), loguear por porcentaje, no por elemento:

```cpp
const uint64_t step = (total >= 10) ? (total / 10) : 0;
for (uint64_t i = 0; i < total; ++i) {
    if (step > 0 && i > 0 && i % step == 0) {
        char msg[32];
        snprintf(msg, sizeof(msg), "%lu%% completado", (unsigned long)(i * 100 / total));
        SC_REPORT_INFO(name(), msg);
    }
    // ... lógica
}
SC_REPORT_INFO(name(), "Procesamiento completo");
```

**Referencia real — ver [`src/accelerator/accelerator.cpp`](../src/accelerator/accelerator.cpp)**

---

## Paso 4: Funciones Auxiliares (Sin SystemC)

Si tu módulo necesita funciones matemáticas o de procesamiento, ponlas en un `.h` separado **sin dependencia de SystemC** para simplificar la depuración.

**Archivo:** `src/utils/conversion.h`

```cpp
#pragma once
#include <cstdint>
#include <cmath>

inline uint8_t rgb_to_gray(uint8_t r, uint8_t g, uint8_t b) {
    float gray = 0.299f * r + 0.587f * g + 0.114f * b;
    return static_cast<uint8_t>(std::round(gray));
}
```

En el `.cpp`, usala con:
```cpp
#include "../utils/conversion.h"

uint8_t Accelerator::rgb_to_gray(uint8_t r, uint8_t g, uint8_t b) {
    return ::rgb_to_gray(r, g, b);
}
```

**Referencia real — ver [`src/utils/conversion.h`](../src/utils/conversion.h)**

---

## Paso 5: Verificar CMakeLists.txt

El `.cpp` de tu módulo debe estar en el bloque `add_executable(sim ...)`.

**Archivo:** `CMakeLists.txt`

```cmake
add_executable(sim
    src/main.cpp
    src/cpu/cpu.cpp
    src/bus/bus.cpp
    src/ram/ram.cpp
    src/disk/disk.cpp
    src/accelerator/accelerator.cpp
    # si agregás un módulo nuevo, añadí su .cpp aquí
)
```

---

## Paso 6: Verificar main.cpp

Todos los módulos ya están instanciados y conectados en `src/main.cpp`. Al implementar la lógica de un módulo no es necesario modificar este archivo, salvo que se agregue un módulo completamente nuevo.

**Referencia — ver [`src/main.cpp`](../src/main.cpp):**

```cpp
cpu.init_socket.bind(bus.target_socket);
accelerator.init_socket.bind(bus.target_socket_accel);

bus.init_socket_ram.bind(ram.target_socket);
bus.init_socket_accel.bind(accelerator.target_socket);
bus.init_socket_disk.bind(disk.target_socket);
```

### Topología:

```
CPU (initiator)        Accelerator (initiator)
        ↓                       ↓
   bus.target_socket   bus.target_socket_accel
        └──────── Bus ──────────┘
                  ↓↓↓
         RAM    Accel    Disk
```

**Regla:** initiator → target. Cada socket se bindea exactamente una vez.

---

## Paso 7: Definir Direcciones de Memoria (Memory Map)

Cada módulo ocupa un rango de direcciones en el bus. Documentar en `README.md` → sección "Memory Map".

| Región | Base Address | Size | Módulo |
|---|---|---|---|
| Input RGB | `0x00000000` | ~5.9 MB | RAM |
| Output Grayscale | `0x00600000` | ~1.9 MB | RAM |
| Accelerator config | `0x10000000` | 24 B | Accelerator |
| Disk | `0x20000000` | — | Disk |

El Bus usa estas direcciones para **enrutar transacciones**. Tiene **dos target sockets**:
- `target_socket` — recibe transacciones del CPU → enruta por address range
- `target_socket_accel` — recibe transacciones del Accelerator → siempre va a RAM

Implementar en `src/bus/bus.cpp`:

```cpp
void Bus::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& delay) {
    uint64_t addr = payload.get_address();

    if (addr < 0x10000000) {
        init_socket_ram->b_transport(payload, delay);
    } else if (addr < 0x20000000) {
        init_socket_accel->b_transport(payload, delay);
    } else {
        init_socket_disk->b_transport(payload, delay);
    }
}

void Bus::b_transport_accel(tlm::tlm_generic_payload& payload, sc_core::sc_time& delay) {
    // El Accelerator solo accede a RAM
    init_socket_ram->b_transport(payload, delay);
}
```

---

## Paso 8: Actualizar README — Organización de Módulos

Cuando la interfaz pública del módulo esté definida, actualizar la sección **Module Organization** en `README.md`.

**Archivo:** `README.md` → sección "Module Organization"

### classDiagram — agregar o actualizar el bloque del módulo:

    ```mermaid
    classDiagram
        class MiModulo {
            +tlm_target_socket target_socket
            +b_transport() void
        }
    ```

Reflejar los sockets públicos reales y los métodos que forman parte de la interfaz observable. No documentar métodos privados aquí.

### Tabla de módulos — verificar que la fila del módulo sea precisa:

| Module | File(s) | TLM role | Responsibility |
|---|---|---|---|
| **MiModulo** | `src/<nombre_modulo>/` | Target / Initiator | Descripción concisa de su responsabilidad |

### Resultados — actualizar al finalizar la implementación completa:

Una vez que el sistema corre de punta a punta, llenar la sección **Results** en `README.md` con:
- Imagen de entrada y salida (side-by-side o paths)
- Log de salida de `./build/sim`
- Tiempo de simulación de SystemC observado

---

## Paso 9: Documentar Formato de Configuración

Documentar el formato de la transacción WRITE en el header del módulo:

```cpp
// Config transaction: WRITE de 24 bytes al address 0x10000000
// Offset  +0 (8 B): src_addr    — dirección base input en RAM
// Offset  +8 (8 B): dst_addr    — dirección base output en RAM
// Offset +16 (8 B): pixel_count — total de elementos a procesar
```

---

## Checklist: Resumen Completo

```
✓ Paso 1: Ubicar archivos en src/<nombre_modulo>/
✓ Paso 2: Implementar header <nombre_modulo>.h con sockets y constructor
✓ Paso 3: Implementar <nombre_modulo>.cpp con b_transport() y lógica
✓ Paso 3b: Agregar SC_REPORT_INFO en los puntos clave del módulo
✓ Paso 4: (Opcional) Implementar conversion.h para lógica sin SystemC
✓ Paso 5: Verificar CMakeLists.txt incluye <nombre_modulo>.cpp
✓ Paso 6: Verificar conexiones en src/main.cpp
✓ Paso 7: Actualizar README.md — Memory Map
✓ Paso 8: Actualizar README.md — Module Organization + Results
✓ Paso 9: Documentar formato de configuración en header
```

---

## Compilar y Probar

```bash
make      # compila
make run  # ejecuta la simulación
```

---

## Referencias

- [SystemC Documentation](https://accellera.org/activities/systemc/)
- [TLM 2.0 Specification](https://accellera.org/activities/tlm/)
- Módulo implementado de referencia: [`src/accelerator/`](../src/accelerator/)
