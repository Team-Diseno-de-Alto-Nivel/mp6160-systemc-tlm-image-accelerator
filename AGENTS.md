# AGENTS.md

Project instructions for all AI coding assistants working in this repository.

See @README.md for architecture overview, build instructions, diagrams, memory map, and transaction format.
See @docs/Enunciado.md for the full assignment specification (Spanish) — read it to understand what must be implemented and what deliverables are required.

## Build

`$SYSTEMC_HOME` is optional. If unset, CMake downloads and compiles SystemC 2.3.4 automatically on first configure (requires internet, ~1–2 min).

```bash
make      # configure + compile
make run  # build and run the simulation
```

## Non-obvious constraints

- **Separation of concerns is graded**: grayscale logic lives only in Accelerator; all inter-module data transfer via TLM `b_transport`, never direct calls; RAM is the only intermediate buffer; Disk is the only module that touches the filesystem.
- **Accelerator config is a 24-byte TLM WRITE**: 3 × `uint64_t` — src address in RAM, dst address in RAM, pixel count. CPU sends this before the Accelerator starts processing.
- **Image format**: RAW binary, no header, 3 bytes/pixel RGB, row-major, 1920 × 1080. Total: 6,220,800 bytes input, 2,073,600 bytes output.
- **AI usage must be declared** in the README (prompts + type of use). Omitting it is treated as plagiarism per @docs/Enunciado.md.

## Coding conventions

- One `.h` + `.cpp` pair per module under `src/<module>/`
- `#pragma once` in all headers, C++17
- `SC_THREAD` for all module processes
- Initiator sockets on CPU and Bus upstream; target sockets on RAM, Disk, Accelerator, and Bus downstream

## Diagrams

All diagrams use Mermaid fenced blocks. GitHub renders them natively. Block diagram: `graph LR`. Sequence diagram: `sequenceDiagram`.

## AI Usage Declaration

When your AI assistant helps with this project, run `/log-ai` (available in Claude Code and GitHub Copilot Chat) to append a row to the `## AI-Assisted Development` table in `README.md`. Columns: **Model**, **Type of use**, **Prompt**. Required by the course — omitting it counts as plagiarism.
