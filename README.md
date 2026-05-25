# uefi-sysprobe

![build](https://github.com/Duke-Tang/uefi-sysprobe/actions/workflows/build.yml/badge.svg)

Minimal UEFI Shell application that probes system info via three different mechanisms — built with EDK2 and tested under QEMU + OVMF.

## What it does

| Section | Mechanism | What it shows |
|---|---|---|
| CPU | `cpuid` instruction (inline x86 asm) | Vendor string, processor brand string |
| SMBIOS | EFI Configuration Table → SMBIOS entry point → walk structures | BIOS vendor/version (type 0), System manuf/product (type 1) |
| EFI Variable | `gRT->GetVariable()` | `BootCurrent` from global variable namespace |

## Demo

![screenshot](docs/screenshot.png)

## Project layout
## EDK2 libraries used

- `UefiApplicationEntryPoint` — module entry boilerplate
- `UefiBootServicesTableLib` / `UefiRuntimeServicesTableLib` — `gBS` / `gRT` access
- `UefiLib` — `EfiGetSystemConfigurationTable`, `Print`
- `BaseLib` / `BaseMemoryLib` — `CopyMem`, etc.

## Build

```bash
# Prereqs (Ubuntu 22.04 / WSL2)
sudo apt install -y build-essential uuid-dev iasl nasm acpica-tools \
                    git python3-distutils qemu-system-x86 ovmf

# EDK2 setup (once)
git clone --recurse-submodules https://github.com/tianocore/edk2.git ~/edk2
cd ~/edk2 && make -C BaseTools

# Link this module into the EDK2 tree
ln -s ~/uefi-sysprobe/src ~/edk2/MdeModulePkg/Application/SysProbe
# Add to [Components] of MdeModulePkg/MdeModulePkg.dsc:
#   MdeModulePkg/Application/SysProbe/SysProbe.inf

# Build
cd ~/edk2
source edksetup.sh
build -a X64 -t GCC -p MdeModulePkg/MdeModulePkg.dsc \
      -m MdeModulePkg/Application/SysProbe/SysProbe.inf
# → Build/MdeModule/DEBUG_GCC/X64/SysProbe.efi
```

## Run under QEMU + OVMF

```bash
mkdir -p ~/esp
cp ~/edk2/Build/MdeModule/DEBUG_GCC/X64/SysProbe.efi ~/esp/
qemu-system-x86_64 -bios /usr/share/ovmf/OVMF.fd \
                   -drive format=raw,file=fat:rw:$HOME/esp \
                   -net none -nographic
# In UEFI Shell:
#   Shell> fs0:
#   FS0:\> SysProbe.efi
```

## Why this exists

Practice project for firmware engineer interviews — exercises the
UEFI module structure, x86 instruction-level access, and the data
tables (SMBIOS / EFI variables) that real BIOS code consumes daily.

## References

- UEFI Specification 2.10 — sections on Boot/Runtime Services, EFI Variables
- SMBIOS Reference Specification 3.7.0 (DMTF DSP0134)
- Intel® 64 and IA-32 Architectures Software Developer's Manual, Vol. 2A — CPUID
- [EDK2 wiki](https://github.com/tianocore/tianocore.github.io/wiki) — module / DSC / INF structure

## License

MIT
