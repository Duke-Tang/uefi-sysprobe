;------------------------------------------------------------------------------
; Rdtsc.nasm — Read Time-Stamp Counter (RDTSC)
;
; UINT64 EFIAPI ReadTsc (VOID);
;
; RDTSC returns the 64-bit TSC value in EDX:EAX (high:low).
; Microsoft x64 ABI: return value in RAX.
;------------------------------------------------------------------------------

DEFAULT REL
SECTION .text

global ASM_PFX(ReadTsc)
ASM_PFX(ReadTsc):
    rdtsc                   ; EDX:EAX <- TSC
    shl     rdx, 32         ; merge EDX:EAX into RAX
    or      rax, rdx
    ret
