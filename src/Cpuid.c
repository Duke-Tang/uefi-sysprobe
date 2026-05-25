#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>

//
// 直接 inline x86 assembly 打 CPUID 指令
//
STATIC VOID InlineCpuid(
  IN  UINT32 Leaf,
  OUT UINT32 *Eax, OUT UINT32 *Ebx,
  OUT UINT32 *Ecx, OUT UINT32 *Edx
) {
  __asm__ volatile (
    "cpuid"
    : "=a"(*Eax), "=b"(*Ebx), "=c"(*Ecx), "=d"(*Edx)
    : "a"(Leaf), "c"(0)
  );
}

VOID GetCpuVendor(CHAR8 *Vendor /* size >= 13 */) {
  UINT32 Eax, Ebx, Ecx, Edx;
  InlineCpuid(0, &Eax, &Ebx, &Ecx, &Edx);
  CopyMem(Vendor,     &Ebx, 4);
  CopyMem(Vendor + 4, &Edx, 4);
  CopyMem(Vendor + 8, &Ecx, 4);
  Vendor[12] = 0;
}

VOID GetCpuBrand(CHAR8 *Brand /* size >= 49 */) {
  UINT32 Regs[4];
  for (UINT32 i = 0; i < 3; i++) {
    InlineCpuid(0x80000002 + i, &Regs[0], &Regs[1], &Regs[2], &Regs[3]);
    CopyMem(Brand + i * 16, Regs, 16);
  }
  Brand[48] = 0;
}
