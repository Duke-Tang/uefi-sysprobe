# uefi-sysprobe

[![build](https://github.com/Duke-Tang/uefi-sysprobe/actions/workflows/build.yml/badge.svg)](https://github.com/Duke-Tang/uefi-sysprobe/actions/workflows/build.yml)
[![EDK2](https://img.shields.io/badge/EDK2-tianocore-blue)](https://github.com/tianocore/edk2)
[![UEFI](https://img.shields.io/badge/UEFI-2.10-orange)](https://uefi.org/specifications)
[![SMBIOS](https://img.shields.io/badge/SMBIOS-3.7.0-green)](https://www.dmtf.org/standards/smbios)
[![arch](https://img.shields.io/badge/arch-x86__64-lightgrey)]()
[![license](https://img.shields.io/badge/license-MIT-blue)](LICENSE)

> Minimal UEFI Shell application that exercises three different firmware-level mechanisms — CPUID, SMBIOS table walking, and EFI Variable services. Built with EDK2, verified in CI, tested under QEMU + OVMF.

![demo](docs/screenshot.png)

## What it does

| Section | Mechanism | Spec | What it prints |
|---|---|---|---|
| **CPU** | `cpuid` instruction via **inline x86 assembly** | Intel SDM Vol. 2A | Vendor string (leaf 0), Brand string (leaf 0x80000002–4) |
| **SMBIOS** | `EfiGetSystemConfigurationTable` → entry point → walk structures | DMTF SMBIOS 3.7.0 | Type 0 (BIOS info), Type 1 (System info) |
| **EFI Variable** | `gRT->GetVariable()` | UEFI 2.10 §8.2 | `BootCurrent` from global variable namespace |

## Architecture
┌─────────────────────────────────────────────────────────────┐
│                      SysProbe.efi                            │
│                  (UEFI_APPLICATION)                          │
├─────────────────────────────────────────────────────────────┤
│  SysProbe.c          ── entry, SMBIOS walker, variable read │
│  Cpuid.c             ── inline asm wrapper for CPUID        │
│  SysProbe.inf        ── EDK2 module manifest                │
└─────────────────────────────────────────────────────────────┘
│
├── linked against MdePkg / MdeModulePkg
└── loaded by UEFI Shell on FS0:
## Build

### Prerequisites (Ubuntu 22.04 / WSL2)

```bash
sudo apt install -y build-essential uuid-dev iasl nasm acpica-tools 
git python3-distutils qemu-system-x86 ovmf
### One-time EDK2 setup

```bash
git clone --recurse-submodules https://github.com/tianocore/edk2.git ~/edk2
cd ~/edk2 && make -C BaseTools
### Wire this module in & build

```bash
Shell> fs0:
FS0:> SysProbe.efi
## CI

Every push to `main` triggers `.github/workflows/build.yml`, which on a clean
`ubuntu-22.04` runner: installs build deps → clones EDK2 → builds BaseTools →
links this module into `MdeModulePkg` → builds `SysProbe.efi` → uploads it as
a downloadable artifact. The **build badge** above reflects the latest run.

## Why this exists

Practice project targeting firmware-engineer interviews (BIOS / BMC / NB / AIO).
Touches the four pillars actually used in BIOS porting work:

- EDK2 module structure (`.inf` + `LibraryClasses`)
- UEFI services (Boot / Runtime / Configuration Tables)
- Industry-standard tables (SMBIOS, EFI Variables)
- x86 instruction-level access (CPUID inline asm, RDTSC in NASM — see `src/asm/`)

## References

- [UEFI Specification 2.10](https://uefi.org/specs/UEFI/2.10/) — §3 Boot Manager, §8 Services - Runtime, §10 Protocols - Device Path
- [SMBIOS Reference Specification 3.7.0 (DSP0134)](https://www.dmtf.org/standards/smbios)
- [Intel® 64 and IA-32 SDM Vol. 2A](https://www.intel.com/sdm) — `CPUID`, `RDTSC`
- [TianoCore EDK2 Wiki](https://github.com/tianocore/tianocore.github.io/wiki/EDK-II)

## License

MIT — see [LICENSE](LICENSE)
