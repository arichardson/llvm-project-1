# REQUIRES: mips
## Check that we can link a shared library containing an eh_frame section without
## -z notext. This was not possible LLVM started emitting values using the
## DW_EH_PE_pcrel | DW_EH_PE_sdata4 encoding.

## It should not be possible to link code compiled without -fPIC:
# RUN: llvm-mc -filetype=obj -triple=mips64-unknown-linux %s -o %t-nopic.o
# RUN: llvm-readobj -r %t-nopic.o | FileCheck %s --check-prefix=STATIC-RELOCS
# RUN: not ld.lld -shared %t-nopic.o -o /dev/null 2>&1 | FileCheck %s --check-prefix=NOPIC-ERR
# NOPIC-ERR: ld.lld: error: can't create dynamic relocation R_MIPS_64 against local symbol in readonly segment
# STATIC-RELOCS:      .rela.eh_frame {
# STATIC-RELOCS-NEXT:   0x1C R_MIPS_64/R_MIPS_NONE/R_MIPS_NONE .text 0x0
# STATIC-RELOCS-NEXT: }

## For -fPIC, .eh_frame should contain PC-relative relocations:
# RUN: llvm-mc -filetype=obj -triple=mips64-unknown-linux --position-independent %s -o %t-pic.o
# RUN: llvm-readobj -r %t-pic.o | FileCheck %s --check-prefix=PIC-RELOCS
# RUN: ld.lld -shared %t.o -o %t-pic.so
# PIC-RELOCS:      .rela.eh_frame {
# PIC-RELOCS-NEXT:   0x1C R_MIPS_PC32/R_MIPS_NONE/R_MIPS_NONE - 0x0
# PIC-RELOCS-NEXT: }

.ent func
.global func
func:
	.cfi_startproc
	nop
	.cfi_endproc
.end func
