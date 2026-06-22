# ⚙️ win32_gamecore — Low-Level 2D Software Renderer Baseline

A lightweight, zero-dependency 2D software rendering engine built from scratch using the raw **Win32 API** and **GDI (Graphics Device Interface)**. This repository serves as a low-level foundational base for custom game engines, completely bypassing modern third-party wrappers (like SDL, SFML, or GLFW) to communicate directly with the Windows kernel subsystem.

The project structure and memory-mapping paradigms are deeply inspired by Casey Muratori's **Handmade Hero** series, focused on writing clean, high-performance, and platform-aware C/C++ code.

---

## 🛠️ Core Engineering & Learning Objectives

This project was built to master the inner workings of operating systems, hardware memory layouts, and real-time application pipelines:

* **Platform Windowing Subsystem:** Deep-dive into registering window classes (`WNDCLASS`), managing window procedures (`WindowProc` callbacks), and handling core OS messages (`WM_PAINT`, `WM_SIZE`, `WM_DESTROY`).
* **Manual Virtual Memory Mapping:** Bypassing standard heap allocators (`malloc`/`new`) to request raw virtual memory pages directly from the OS kernel using `VirtualAlloc` and handling lifecycle releases with `VirtualFree`.
* **Real-Time Message Pumping:** Transitioning from blocking architectures (`GetMessage`) to active, high-throughput polling loops (`PeekMessage`) to guarantee uninterrupted frame rendering independent of OS event queues.
* **Bitwise Software Rasterization:** Implementing a direct-to-RAM software pixel pipeline, factoring in hardware byte alignment, pitch stride calculation, and **Little-Endian** byte ordering architecture.

---

## 🖥️ The Software-to-Hardware Rendering Pipeline

The rendering system bypasses the modern GPU stack, writing pixel color data directly into a dedicated block of system RAM, which is then blitted to the screen coordinate system:

1. **`VirtualAlloc` Allocation:** Allocates a contiguous, page-aligned pixel buffer in system memory (`BitmapMemory`) sized exactly to the window dimensions $\text{Width} \times \text{Height} \times 4\text{ bytes per pixel}$.
2. **`RenderWeirdGradient` Compute:** Loops through the allocated buffer using 32-bit wskaźniki (`uint32*`). It manipulates raw memory registers using bit-shifting operations to map vertical and horizontal values to color channels.
3. **`StretchDIBits` Blitting:** GDI passes the raw buffer straight to the Device Context (`HDC`), translating the bitmap memory straight into the window's client rectangle area.

> ⚠️ **Memory Layout Precision:** Colors are structured in memory as **BB GG RR xx** (Blue, Green, Red, Alpha-padded), which on Little-Endian CPU registers translates directly to the hex format `0x00RRGGBB`.

---

## 💻 Technical Stack Matrix

| Layer | Component / Technology |
| :--- | :--- |
| **Languages** | C++ / C (Procedural paradigm) |
| **Operating System API** | Native Win32 API (Core User32 & Kernel32 subsystems) |
| **Graphics Subsystem** | GDI (Graphics Device Interface — Windows legacy blitting) |
| **Memory Management** | Kernel page allocation (`VirtualAlloc` / `VirtualFree`) |
| **Supported Compilers** | MSVC (cl.exe), Clang, MinGW |

---

## 📂 Codebase Architecture Breakdown

The entire core is neatly sectioned within a single source file to maintain strict cache-locality and simple compilation:

* **Type Aliases & Macros:** Explicit width-defined integer types (`uint8`, `uint32`) and semantic local re-definitions for the `static` keyword (`internal`, `local_persist`, `global_variable`).
* **`Win32ResizeDIBSection`:** Handles dynamic window resizing by freeing old structures and re-allocating a Device Independent Bitmap section to match new coordinate bounds.
* **`Win32MainWindowCallback`:** The primary window message processing center, dictating application behavior upon user sizing or termination signals.
* **`WinMain` Entry Point:** Initializes the platform window, instantiates the active `PeekMessage` loop, updates offsets, and ticks the software renderer frame-by-frame.

---

## 💡 Inspiration

This project is part of a low-level educational journey guided by [Handmade Hero](https://handmadehero.org). It represents a deliberate choice to understand the computer at a hardware and operating-system level before building higher abstracted gameplay layers.
