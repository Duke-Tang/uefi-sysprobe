#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/PrintLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Guid/SmBios.h>
#include <Guid/GlobalVariable.h>
#include <IndustryStandard/SmBios.h>

extern VOID GetCpuVendor(CHAR8 *Vendor);
extern VOID GetCpuBrand(CHAR8 *Brand);
extern UINT64 EFIAPI ReadTsc (VOID);

STATIC VOID Header(CONST CHAR16 *T) { Print(L"\n=== %s ===\n", T); }

STATIC CONST CHAR8 *SmbiosString(SMBIOS_STRUCTURE *Hdr, UINT8 N) {
  if (N == 0) return "N/A";
  CHAR8 *P = (CHAR8 *)Hdr + Hdr->Length;
  while (--N && *P) { while (*P) P++; P++; }
  return (*P) ? P : "N/A";
}

STATIC VOID PrintCpu(VOID) {
  CHAR8 Vendor[13] = {0}, Brand[49] = {0};
  GetCpuVendor(Vendor);
  GetCpuBrand(Brand);
  Header(L"CPU (via CPUID)");
  Print(L"  Vendor: %a\n", Vendor);
  Print(L"  Brand : %a\n", Brand);

  UINT64 Tsc1 = ReadTsc();
  gBS->Stall(1000);  // sleep 1ms
  UINT64 Tsc2 = ReadTsc();
  Print(L"  TSC   : %lu cycles (delta in 1ms = %lu)\n", Tsc2, Tsc2 - Tsc1);
}

STATIC VOID PrintSmbios(VOID) {
  SMBIOS_TABLE_ENTRY_POINT *Ep = NULL;
  Header(L"SMBIOS");

  if (EFI_ERROR(EfiGetSystemConfigurationTable(
        &gEfiSmbiosTableGuid, (VOID **)&Ep)) || Ep == NULL) {
    Print(L"  SMBIOS table not found.\n");
    return;
  }

  Print(L"  Version: %d.%d\n", Ep->MajorVersion, Ep->MinorVersion);

  UINT8 *Cur = (UINT8 *)(UINTN)Ep->TableAddress;
  UINT8 *End = Cur + Ep->TableLength;

  while (Cur < End) {
    SMBIOS_STRUCTURE *Hdr = (SMBIOS_STRUCTURE *)Cur;
    if (Hdr->Type == 127) break;

    if (Hdr->Type == 0) {
      SMBIOS_TABLE_TYPE0 *T0 = (SMBIOS_TABLE_TYPE0 *)Hdr;
      Print(L"  [BIOS] vendor=%a  version=%a\n",
        SmbiosString(Hdr, T0->Vendor),
        SmbiosString(Hdr, T0->BiosVersion));
    } else if (Hdr->Type == 1) {
      SMBIOS_TABLE_TYPE1 *T1 = (SMBIOS_TABLE_TYPE1 *)Hdr;
      Print(L"  [SYS ] manuf=%a  product=%a\n",
        SmbiosString(Hdr, T1->Manufacturer),
        SmbiosString(Hdr, T1->ProductName));
    }

    UINT8 *P = Cur + Hdr->Length;
    while (P[0] || P[1]) P++;
    Cur = P + 2;
  }
}

STATIC VOID PrintBootVar(VOID) {
  UINT16 BootCurrent = 0;
  UINTN  Size = sizeof(BootCurrent);
  EFI_STATUS Status;

  Header(L"EFI Variable");
  Status = gRT->GetVariable(L"BootCurrent", &gEfiGlobalVariableGuid,
                            NULL, &Size, &BootCurrent);
  if (EFI_ERROR(Status)) Print(L"  BootCurrent: not available\n");
  else                   Print(L"  BootCurrent: Boot%04x\n", BootCurrent);
}

EFI_STATUS EFIAPI SysProbeMain(
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
) {
  Print(L"SysProbe v0.1 - UEFI System Probe\n");
  PrintCpu();
  PrintSmbios();
  PrintBootVar();
  Print(L"\nDone.\n");
  return EFI_SUCCESS;
}
