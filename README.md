# SystemC Image Processing — TLM 2.0

> Academic project — Diseño de Alto Nivel, 2C 2026 · Due 2026-06-18

An electronic system-level model of an embedded platform that converts 1080p RAW RGB images to grayscale using **SystemC** and **TLM 2.0**.

![Build](https://github.com/Team-Diseno-de-Alto-Nivel/mp6160-systemc-tlm-image-accelerator/actions/workflows/build.yml/badge.svg)

---

## Requirements & Build Instructions

### System prerequisites

| Dependency | Minimum version | Notes |
|---|---|---|
| C++ compiler | GCC ≥ 9 or Clang ≥ 10 | Must support C++17 |
| CMake | ≥ 3.16 | Used for both building and fetching SystemC |
| Git | any | To clone the repository |
| Internet access | — | Required on first build to download SystemC (skipped if `$SYSTEMC_HOME` is set) |

**Linux (Ubuntu/Debian):**
```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake git
```

**macOS:**
```bash
xcode-select --install       # provides clang and make
brew install cmake git
```

### Local Build

```bash
git clone <repository-url>
cd <repository>
make      # configure + download SystemC (first run ~1-2 min) + compile
make run  # build and run the simulation
make clean  # delete build directory
```

On the first run, CMake downloads and compiles SystemC 2.3.4 automatically. Subsequent builds use the cached version inside `build/`.

If SystemC is already installed, set `$SYSTEMC_HOME` before running `make` to skip the download:

```bash
export SYSTEMC_HOME=/opt/systemc   # adjust to your install path
make run
```

### Development Container

The repository ships a ready-to-use dev container that pre-installs all dependencies (including a compiled SystemC 2.3.4) so every team member gets an identical Linux build environment regardless of their host OS. This is the recommended alternative to a local build.

#### VS Code (recommended)

1. Install the [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode.remote-containers) extension (`ms-vscode.remote-containers`)
2. Open the repository in VS Code
3. When prompted, click **Reopen in Container** (or run the command `Dev Containers: Reopen in Container`)
4. The container builds once (~3–5 min on first run), then `make` runs automatically to verify the setup

The following extensions are pre-installed inside the container:
- **C/C++** (`ms-vscode.cpptools`) — IntelliSense, debugging
- **CMake Tools** (`ms-vscode.cmake-tools`) — build integration

#### Docker CLI (without VS Code)

```bash
docker build -t systemc-tlm .devcontainer/
docker run -it --rm -v $(pwd):/workspace -w /workspace systemc-tlm make run
```

#### GitHub Codespaces

The same `devcontainer.json` works on [GitHub Codespaces](https://github.com/features/codespaces) — click **Code → Codespaces → Create codespace** for a cloud-hosted environment with no local install required.

### CI / CD

Every pull request triggers a GitHub Actions workflow ([build.yml](.github/workflows/build.yml)) that builds SystemC (cached), compiles the project, and runs `./build/sim`. To enforce this on `master`, enable branch protection and require the `build` check to pass before merging.

---

## Repository Organization

```
.
├── .devcontainer/
│   ├── Dockerfile                 # Linux image with SystemC pre-built
│   └── devcontainer.json          # VS Code / Codespaces dev container config
├── .github/
│   └── workflows/
│       ├── build.yml              # CI: compiles and runs on every pull request
│       └── results.yml            # CI: re-runs on push to main, publishes images + results.md as a release
├── docs/
│   ├── CrearModuloSystemC.md      # Step-by-step guide for implementing a new module
│   └── Enunciado.md               # Assignment specification (Spanish)
├── images/
│   ├── input/                     # Input RAW RGB images (place here before running)
│   └── output/                    # Grayscale output images (written here by sim)
├── scripts/
│   ├── prepare_input.py           # Converts/generates images/input/image.raw for the sim
│   ├── raw_to_jpg.py              # Converts a headerless RAW file back to JPG/PNG for viewing
│   ├── jpg_to_raw.py              # Converts a JPG/PNG into headerless RAW for the sim
│   └── generate_results.py        # Extracts sim time, byte counts, and BT.601 pixel checks for CI
├── src/
│   ├── modules/
│   │   ├── accelerator/
│   │   │   ├── accelerator.h
│   │   │   └── accelerator.cpp
│   │   ├── bus/
│   │   │   ├── bus.h
│   │   │   └── bus.cpp
│   │   ├── cpu/
│   │   │   ├── cpu.h
│   │   │   └── cpu.cpp
│   │   ├── disk/
│   │   │   ├── disk.h
│   │   │   └── disk.cpp
│   │   └── ram/
│   │       ├── ram.h
│   │       └── ram.cpp
│   ├── config/
│   │   ├── memory_map.h           # Bus address map constants
│   │   └── image_config.h         # Image dimension constants
│   ├── utils/
│   │   └── conversion.h           # Pure BT.601 helper (no SystemC dependency)
│   ├── infra/
│   │   └── logger.h               # Centralized console logging
│   └── sc_main.cpp                # sc_main — top-level instantiation and sc_start()
├── AGENTS.md                      # AI assistant instructions
├── CMakeLists.txt                 # Build system; auto-fetches SystemC if needed
├── CLAUDE.md                      # Context file for Claude Code
├── Makefile                       # Thin CMake wrapper
└── README.md
```

---

## Module Organization

```mermaid
classDiagram
    class CPU {
        +tlm_initiator_socket init_socket
        +run() void
    }
    class Bus {
        +tlm_target_socket target_socket
        +tlm_target_socket target_socket_accel
        +tlm_initiator_socket init_socket_ram
        +tlm_initiator_socket init_socket_accel
        +tlm_initiator_socket init_socket_disk
        +b_transport() void
        +b_transport_accel() void
    }
    class RAM {
        +tlm_target_socket target_socket
        +uint8_t mem[64MB]
        +b_transport() void
    }
    class Disk {
        +tlm_target_socket target_socket
        +b_transport() void
    }
    class Accelerator {
        +tlm_target_socket target_socket
        +tlm_initiator_socket init_socket
        +b_transport() void
    }

    CPU --> Bus : initiator (target_socket)
    Accelerator --> Bus : initiator (target_socket_accel)
    Bus --> RAM : target
    Bus --> Disk : target
    Bus --> Accelerator : target
```

| Module | File(s) | TLM role | Responsibility |
|---|---|---|---|
| **CPU** | `src/modules/cpu/` | Initiator | Orchestrates the full flow: load → store → configure → fetch → save |
| **Bus** | `src/modules/bus/` | Target + Initiator | Routes transactions to the correct target by address range |
| **RAM** | `src/modules/ram/` | Target | 64 MB byte-addressable memory; holds input RGB and output grayscale |
| **Disk** | `src/modules/disk/` | Target | Maps READ/WRITE transactions to local filesystem file I/O |
| **Accelerator** | `src/modules/accelerator/` | Target | On WRITE to config registers, reads RGB from RAM and writes grayscale back |

---

## Block Diagram

```mermaid
graph LR
    CPU <-->|TLM 2.0| Bus
    Bus <-->|TLM 2.0| RAM
    Bus <-->|TLM 2.0| Accelerator
    Bus <-->|TLM 2.0| Disk
```

### CPU

The CPU orchestrates the entire processing pipeline. It initiates all TLM transactions: loading the raw image from Disk into RAM, configuring the Accelerator with the source address, destination address, and pixel count, then polling the Accelerator's status register every 100 ns until processing is done, and finally fetching the processed image back from RAM and saving it to Disk. The CPU holds no image data locally — RAM is the only intermediate buffer.

### Bus

The Bus has two TLM target sockets: `target_socket`, for transactions coming from the CPU, and `target_socket_accel`, for transactions coming from the Accelerator. The CPU-facing socket decodes the address and routes it to RAM (`0x00000000`–`0x03FFFFFF`), Accelerator (`0x10000000`–`0x1FFFFFFF`), or Disk (`0x20000000`–`0x2FFFFFFF`); the Accelerator-facing socket only forwards to RAM, since the Accelerator only ever reads/writes pixel data there. All inter-module communication passes through the Bus.

### RAM

A 64 MB byte-addressable memory array. It holds the input RGB image starting at `0x00000000` and the output grayscale image starting at `0x00600000`. All inter-module data exchange passes through RAM — neither the CPU nor the Accelerator transfer image data directly to each other.

### Disk

Models the persistent file system as a SystemC module. A TLM READ transaction causes it to open the specified image file and return its bytes; a TLM WRITE transaction creates or overwrites a file with the provided data. It is the only module that performs actual filesystem I/O, keeping the rest of the system independent of the host OS.

### Accelerator

On receiving a 24-byte WRITE to its configuration register at `0x10000000`, the Accelerator spawns an asynchronous process (`sc_spawn`) that reads the source RGB pixels from RAM, converts each pixel to grayscale, and writes the result back to RAM at the destination address — the configuration WRITE itself returns immediately, it does not block until processing is done. While processing, a status register at `0x10000018` reads `0`; once the loop finishes, it's set to `1` so the polling CPU can proceed. It uses the **BT.601 luminosity formula**:

```
Gray = 0.299 × R + 0.587 × G + 0.114 × B
```

**Input:** 3 bytes per pixel (RGB, values 0–255)  
**Output:** 1 byte per pixel (Grayscale, 0–255, rounded)

This formula reflects human eye sensitivity to different color channels:
- Green (58.7%): highest sensitivity
- Red (29.9%): medium sensitivity
- Blue (11.4%): lowest sensitivity

**Example:** RGB(100, 150, 200) → 0.299×100 + 0.587×150 + 0.114×200 = 140.75 ≈ 141 (grayscale)

See [Roboflow Image Convert Grayscale](https://inference.roboflow.com/workflows/blocks/image_convert_grayscale/) for reference.

---

## Sequence Diagram

```mermaid
sequenceDiagram
    participant CPU
    participant Bus
    participant Disk
    participant RAM
    participant Accelerator

    CPU->>Bus: READ @ disk_src (load image)
    Bus->>Disk: b_transport(READ, disk_src, 6220800 B)
    Disk-->>CPU: RAW RGB data

    CPU->>Bus: WRITE @ 0x00000000 (store image in RAM)
    Bus->>RAM: b_transport(WRITE, 0x00000000, 6220800 B)

    CPU->>Bus: WRITE @ accel_cfg (src_addr=0x0, dst_addr=0x600000, pixels=2073600)
    Bus->>Accelerator: b_transport(WRITE, accel_cfg, 24 B)
    Accelerator-->>CPU: TLM_OK_RESPONSE (processing spawned asynchronously)

    par Accelerator processes the image in the background
        loop per pixel
            Accelerator->>Bus: READ @ ram_src (via target_socket_accel)
            Bus->>RAM: b_transport(READ)
            RAM-->>Accelerator: RGB bytes
            Accelerator->>Bus: WRITE @ ram_dst (grayscale byte)
            Bus->>RAM: b_transport(WRITE)
        end
        Accelerator->>Accelerator: status_register = 1
    and CPU polls for completion
        loop until status == 1
            CPU->>Bus: READ @ accel_status (0x10000018)
            Bus->>Accelerator: b_transport(READ, accel_status, 4 B)
            Accelerator-->>CPU: status (0 = busy, 1 = done)
            Note over CPU: wait(100 ns) if status == 0
        end
    end

    CPU->>Bus: READ @ 0x00600000 (fetch result)
    Bus->>RAM: b_transport(READ, 0x00600000, 2073600 B)
    RAM-->>CPU: grayscale data

    CPU->>Bus: WRITE @ disk_dst (save result)
    Bus->>Disk: b_transport(WRITE, disk_dst, 2073600 B)
```

---

## Transaction Format

All inter-module communication uses the TLM 2.0 generic payload (`tlm::tlm_generic_payload`).

| Field | Type | Description |
|---|---|---|
| `command` | `tlm_command` | `TLM_READ_COMMAND` or `TLM_WRITE_COMMAND` |
| `address` | `uint64_t` | Absolute byte address on the bus (Bus resolves to target) |
| `data_ptr` | `unsigned char*` | Pointer to the data buffer |
| `data_length` | `unsigned int` | Transfer size in bytes |
| `response_status` | `tlm_response_status` | `TLM_OK_RESPONSE` on success, `TLM_GENERIC_ERROR_RESPONSE` on failure |

### Accelerator configuration transaction

When the CPU configures the Accelerator, it issues a single 24-byte WRITE to the Accelerator's base address:

| Offset | Size | Field |
|---|---|---|
| `+0` | 8 B | Source base address in RAM (input RGB) |
| `+8` | 8 B | Destination base address in RAM (output grayscale) |
| `+16` | 8 B | Total pixel count |

### Accelerator status transaction

The CPU does not block on the configuration WRITE — the Accelerator spawns the pixel-processing loop asynchronously (`sc_spawn`) and returns immediately. To know when processing finished, the CPU polls a 4-byte status register at `accel_base + 0x18` with a READ, waiting 100 ns between polls while the value is `0`:

| Offset | Size | Field | Values |
|---|---|---|---|
| `+24` (`0x18`) | 4 B | Status register | `0` = processing, `1` = done |

---

## Memory Map

The Bus routes transactions based on address range.

| Region | Base Address | Size | Module |
|---|---|---|---|
| Input RGB image | `0x00000000` | 6,220,800 B (~5.9 MB) | RAM |
| Output grayscale image | `0x00600000` | 2,073,600 B (~1.9 MB) | RAM |
| *(free RAM)* | `0x007F9C00` | ~56 MB remaining | RAM |
| Accelerator config | `0x10000000` | 24 B | Accelerator |
| Accelerator status | `0x10000018` | 4 B | Accelerator |
| Disk | `0x20000000` | — | Disk |

RAM total capacity: 64 MB (`0x00000000` – `0x03FFFFFF`).

---

## Results

> Images and simulation data are generated automatically by CI ([results.yml](.github/workflows/results.yml)) on every push to `main` that touches `src/` or the input image, and published to the [`simulation-results` release](https://github.com/Team-Diseno-de-Alto-Nivel/mp6160-systemc-tlm-image-accelerator/releases/tag/simulation-results).

### Output image

| Input (RGB) | Output (Grayscale) |
|:-----------:|:------------------:|
| ![Input RGB](https://github.com/Team-Diseno-de-Alto-Nivel/mp6160-systemc-tlm-image-accelerator/releases/download/simulation-results/image.jpg) | ![Output Grayscale](https://github.com/Team-Diseno-de-Alto-Nivel/mp6160-systemc-tlm-image-accelerator/releases/download/simulation-results/output.jpg) |

### Simulation log

Full log, final `sc_time_stamp()`, byte counts, and a BT.601 conversion check on sample pixels (generated by [scripts/generate_results.py](scripts/generate_results.py)):

📄 **[results.md](https://github.com/Team-Diseno-de-Alto-Nivel/mp6160-systemc-tlm-image-accelerator/releases/download/simulation-results/results.md)**

### Discussion

The output image is visually correct: every feature visible in the RGB input — the mountain silhouettes, the sun, the lake and its reflection, the tree line, and the golden and blue vegetation in the foreground — remains clearly distinguishable in the grayscale output, with the expected relative tonal shifts. The blue-toned vegetation (high blue + green contribution) renders lighter than the golden vegetation (high red, low blue) of similar visual brightness, consistent with BT.601's channel weighting.

Conversion correctness was checked at the pixel level rather than just by eye: `scripts/generate_results.py` samples 4 pixels from the actual committed `images/input/image.raw` / `images/output/output.raw` and recomputes BT.601 independently of the C++ implementation (`src/utils/conversion.h`). All 4 match exactly:

| Pixel # | RGB | Expected gray (BT.601) | Actual gray | Match |
|---|---|---|---|---|
| 0 | (139, 118, 209) | 135 | 135 | ✅ |
| 518,400 | (192, 173, 205) | 182 | 182 | ✅ |
| 1,036,800 | (59, 56, 47) | 56 | 56 | ✅ |
| 2,073,599 | (45, 48, 101) | 53 | 53 | ✅ |

The full, per-run table is published in [results.md](https://github.com/Team-Diseno-de-Alto-Nivel/mp6160-systemc-tlm-image-accelerator/releases/download/simulation-results/results.md) on every push to `main`.

The simulation log shows the CPU's 5 logged phases in order, each corresponding to one or more of the assignment's 6 conceptual steps: `[1/5] Loading image from disk` covers step 1 (CPU loads the image from persistent storage); `[2/5] Storing image in RAM` covers step 3 (CPU stores the image in RAM); `[3/5] Configuring accelerator` covers step 4 (CPU indicates src/dst/pixel count to the Accelerator); `[4/5] Processing image` covers step 5 (the Accelerator reads RGB, converts, and writes grayscale, while the CPU just polls its status register); and `[5/5] Saving result to disk` covers step 6 (CPU reads the processed image from RAM and writes it to disk). No deviations were observed — the order is fixed by the CPU's `run()` method, which executes these steps sequentially with no branching.

`sc_time_stamp()` reports **100 ns** when the simulation stops, and this does not reflect real execution time — the real run took on the order of milliseconds to a few seconds for the full 2,073,600-pixel image, depending on host CPU speed. The reason is that each `b_transport` call annotates a local delay (CPU 10 ns, Bus 5 ns, RAM 10 ns, Disk 100 ns), but none of these annotations are ever consumed with `wait()` — they're computed and then discarded. The only call that actually advances simulated time is the single `wait(100 ns)` inside `CPU::wait_accelerator_ready()`'s polling loop. This is a loosely-timed TLM model: functionally accurate (data moves correctly and in the right order) but not timing-accurate, since per-transaction delays are annotated without being honored.

The data volume matches the expected values exactly, by direct calculation from the image dimensions. For a 1920×1080 image, the pixel count is 1920 × 1080 = 2,073,600 pixels. The RGB input uses 3 bytes per pixel, so the expected input size is 2,073,600 × 3 = **6,220,800 B**, which is exactly what the system transfers from Disk into RAM. The grayscale output uses 1 byte per pixel, so the expected output size is 2,073,600 × 1 = **2,073,600 B**, which is exactly what the system transfers back from RAM into Disk. Total bytes moved through the system: 6,220,800 + 2,073,600 = **8,294,400 B**. These are real measured file sizes, not assumed constants — `scripts/generate_results.py` reads `images/input/image.raw` and `images/output/output.raw` directly and confirms the match on every CI run.

---

## AI-Assisted Development

Declared as required by course policy — see [docs/Enunciado.md](docs/Enunciado.md).

> **Using Claude Code?** Run `/log-ai` in any Claude Code session inside this repo to append a row to the table below automatically. The command asks for model, type of use, and prompt description.


| Model | Type of use | Prompt |
|---|---|---|
| Claude Sonnet 4.6 ([Claude Code](https://claude.ai/code)) | Concept lookup, code generation, documentation generation, diagram generation | *"Create the base repo: readme, gitignore, C++ SystemC template, makefile, cmake with auto-fetch of SystemC, and CI/CD pipeline for PRs. Generate a complete README with all sections required by the assignment spec; include Mermaid diagrams (block, sequence), transaction format, and memory map as base templates to be updated once the system is implemented."* |
| Claude Sonnet 4.6 ([Claude Code](https://claude.ai/code)) | Documentation generation, code generation | *"Create docs/CrearModuloSystemC.md: a step-by-step guide for implementing a SystemC/TLM module in this project, using the Accelerator as a reference example. Covers header, cpp, conversion.h, CMakeLists, main.cpp wiring, memory map, and config format documentation."* |
| Claude Sonnet 4.6 ([Claude Code](https://claude.ai/code)) | code generation, concept lookup | *"Create a Claude Code skill following Anthropic's official skill guide to automate AI usage logging (/log-ai), placed at .claude/skills/log-ai/SKILL.md, with context inference so it derives model, type of use, and prompt from the conversation automatically."* |
