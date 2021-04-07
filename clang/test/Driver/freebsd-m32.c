/// Check that building with -m32 links with i386-freebsd12.2-ld/ instead of
/// x86_64-freebsd12.2-ld and that we select the right sanitizer runtime.

/// We should select x86_64-unknown-freebsd12.2-ld since it matches the triple argument
/// Note: The paths specified by -B are not searched for triple-prefixed tools, so
/// we also have to set $PATH.
// RUN: env PATH=%S/Inputs/basic_freebsd64_tree/usr/bin %clang -no-canonical-prefixes \
// RUN:   -target x86_64-unknown-freebsd12.2 %s \
// RUN:   -B%S/Inputs/basic_freebsd64_tree/usr/bin -### 2>&1 | FileCheck %s --check-prefix=PREFIXED-64
// PREFIXED-64: "-cc1" "-triple" "x86_64-unknown-freebsd12.2"
// PREFIXED-64-NEXT: "{{.+}}Inputs{{/|\\\\}}basic_freebsd64_tree{{/|\\\\}}usr{{/|\\\\}}bin{{/|\\\\}}x86_64-unknown-freebsd12.2-ld" "--eh-frame-hdr"
// Should not be passing an explicit linker emulation for the 64-bit case
// PREFIXED-64-NOT: "-m"
// RUN: env PATH=/this/path/does/not/exist %clang -no-canonical-prefixes \
// RUN:   -target x86_64-unknown-freebsd12.2 %s \
// RUN:   -B%S/Inputs/basic_freebsd64_tree/usr/bin -### 2>&1 | FileCheck %s --check-prefix=MINUS-B-NO-TRIPLE-PREFIX
// MINUS-B-NO-TRIPLE-PREFIX: "-cc1" "-triple" "x86_64-unknown-freebsd12.2"
// MINUS-B-NO-TRIPLE-PREFIX-NEXT: "ld" "--eh-frame-hdr"
// MINUS-B-NO-TRIPLE-PREFIX-NOT: "-m"

/// The triple passed to clang -cc1 should be normalized, but the prefix when searching
/// for ld should not be normalized. Since there is no x86_64--freebsd12.2-ld, passing
/// -target x86_64--freebsd12.2 should not find a a valid linker:
// RUN: env PATH=%S/Inputs/basic_freebsd64_tree/usr/bin %clang -no-canonical-prefixes \
// RUN:   -target x86_64--freebsd12.2 %s \
// RUN:   -B%S/Inputs/basic_freebsd64_tree/usr/bin -### 2>&1 | FileCheck %s --check-prefix=NO-NORMALIZE-LD-PREFIX-64
// NO-NORMALIZE-LD-PREFIX-64: "-cc1" "-triple" "x86_64-unknown-freebsd12.2"
// NO-NORMALIZE-LD-PREFIX-64-NEXT: "ld" "--eh-frame-hdr"
// NO-NORMALIZE-LD-PREFIX-NOT: "-m"

/// We should search for i386-unknown-freebsd12.2-ld when -m32 is passed (and also pass -m elf_i386_fbsd):
// RUN: env PATH=%S/Inputs/basic_freebsd64_tree/usr/bin %clang -no-canonical-prefixes \
// RUN:   -target x86_64-unknown-freebsd12.2 %s -m32 \
// RUN:   -B%S/Inputs/basic_freebsd64_tree/usr/bin -### 2>&1  | FileCheck %s --check-prefix=PREFIXED-M32
// PREFIXED-M32: "-cc1" "-triple" "i386-unknown-freebsd12.2"
// PREFIXED-M32-NEXT: "{{.+}}Inputs{{/|\\\\}}basic_freebsd64_tree{{/|\\\\}}usr{{/|\\\\}}bin{{/|\\\\}}i386-unknown-freebsd12.2-ld" "--eh-frame-hdr"
// PREFIXED-M32-SAME: "-m" "elf_i386_fbsd"
/// Only the -cc1 triple should be normalized when adjusted by -m32:
// RUN: env PATH=%S/Inputs/basic_freebsd64_tree/usr/bin %clang -no-canonical-prefixes \
// RUN:   -target x86_64-freebsd13 %s -m32 \
// RUN:   -B%S/Inputs/basic_freebsd64_tree/usr/bin -### 2>&1 | FileCheck %s --check-prefix=NO-NORMALIZE-LD-PREFIX-M32
// NO-NORMALIZE-LD-PREFIX-M32: "-cc1" "-triple" "i386-unknown-freebsd13"
// NO-NORMALIZE-LD-PREFIX-M32-NEXT: "ld" "--eh-frame-hdr"
// NO-NORMALIZE-LD-PREFIX-M32-SAME: "-m" "elf_i386_fbsd"

/// Check that -m32 also affects the library name for the sanitizer runtime.
/// This previous caused build issues since we were selecting "./lib/clang/13.0.0/lib/x86_64-unknown-freebsd12.2/libclang_rt.asan.a"
// RUN: env PATH=%S/Inputs/basic_freebsd64_tree/usr/bin %clang -no-canonical-prefixes \
// RUN:   -target x86_64-unknown-freebsd12.2 %s -fsanitize=address \
// RUN:   -B%S/Inputs/basic_freebsd64_tree/usr/bin -### 2>&1 | FileCheck %s --check-prefix=ASAN-64
// ASAN-64: "-cc1" "-triple" "x86_64-unknown-freebsd12.2"
// ASAN-64-NEXT: "{{.+}}Inputs{{/|\\\\}}basic_freebsd64_tree{{/|\\\\}}usr{{/|\\\\}}bin{{/|\\\\}}x86_64-unknown-freebsd12.2-ld" "--eh-frame-hdr"
// ASAN-64-NOT: "-m"
// ASAN-64-SAME: lib/freebsd/libclang_rt.asan-x86_64.a"
// ASAN-64-NOT: "-m"
// RUN: env PATH=%S/Inputs/basic_freebsd64_tree/usr/bin %clang -no-canonical-prefixes \
// RUN:   -target x86_64-unknown-freebsd12.2 %s -fsanitize=address -m32 \
// RUN:   -B%S/Inputs/basic_freebsd64_tree/usr/bin -### 2>&1 | FileCheck %s --check-prefix=ASAN-M32
// ASAN-M32: "-cc1" "-triple" "i386-unknown-freebsd12.2"
// ASAN-M32-NEXT: "{{.+}}Inputs{{/|\\\\}}basic_freebsd64_tree{{/|\\\\}}usr{{/|\\\\}}bin{{/|\\\\}}i386-unknown-freebsd12.2-ld" "--eh-frame-hdr"
// ASAN-M32-SAME: "-m" "elf_i386_fbsd"
// ASAN-M32-SAME: lib/freebsd/libclang_rt.asan-i386.a"

/// If there is no ld matching the -m32 triple, we should use the 64-bit linker as we
/// can assume that it is also able to link 32-bit files. However, we still need to
/// pass 32-bit flags and library paths:
// Note: This also checks that the -m32 triple isn't normalized since there is a linker
// that matches the normalized triple (i386-unknown-freebsd12.2-ld), but not one that
// matches the adjusted triple (i386-freebsd12.2-ld).
// RUN: env PATH=%S/Inputs/basic_freebsd64_tree/usr/bin %clangxx -no-canonical-prefixes \
// RUN:   -target x86_64-freebsd12.2 %s -fsanitize=address -m32 \
// RUN:   -B%S/Inputs/basic_freebsd64_tree/usr/bin -### 2>&1 | FileCheck %s --check-prefix=ASAN-M32-64BIT-LINKER
// ASAN-M32-64BIT-LINKER: "-cc1" "-triple" "i386-unknown-freebsd12.2"
// ASAN-M32-64BIT-LINKER-NEXT: "{{.+}}Inputs{{/|\\\\}}basic_freebsd64_tree{{/|\\\\}}usr{{/|\\\\}}bin{{/|\\\\}}x86_64-freebsd12.2-ld" "--eh-frame-hdr"
// ASAN-M32-64BIT-LINKER-SAME: "-m" "elf_i386_fbsd"
// ASAN-M32-64BIT-LINKER-SAME: lib/freebsd/libclang_rt.asan-i386.a"
