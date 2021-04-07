// RUN: %clangxx -no-canonical-prefixes -x c++ %s -### -o %t.o 2>&1 \
// RUN:     --target=x86_64-unknown-linux-gnu \
// RUN:     -stdlib=libc++ \
// RUN:     -resource-dir=%S/Inputs/resource_dir_with_per_target_subdir \
// RUN:     -ccc-install-dir %S/Inputs/basic_linux_libcxx_tree/usr/bin \
// RUN:     --gcc-toolchain="" \
// RUN:     --sysroot=%S/Inputs/basic_linux_libcxx_tree \
// RUN:   | FileCheck --check-prefix=CHECK-PER-TARGET-RUNTIME %s
// CHECK-PER-TARGET-RUNTIME: "{{[^"]*}}clang{{[^"]*}}" "-cc1"
// CHECK-PER-TARGET-RUNTIME: "-resource-dir" "[[RESDIR:[^"]*]]"
// CHECK-PER-TARGET-RUNTIME: "-isysroot" "[[SYSROOT:[^"]+]]"
// CHECK-PER-TARGET-RUNTIME: "-internal-isystem" "{{.*}}/../include/c++/v1"
// CHECK-PER-TARGET-RUNTIME: "-internal-isystem" "[[SYSROOT]]/usr/local/include"
// CHECK-PER-TARGET-RUNTIME: "--sysroot=[[SYSROOT]]"
// CHECK-PER-TARGET-RUNTIME: "-L{{.*}}{{/|\\\\}}..{{/|\\\\}}lib{{/|\\\\}}x86_64-unknown-linux-gnu"

// RUN: %clang -rtlib=compiler-rt -print-libgcc-file-name 2>&1 \
// RUN:     --target=x86_64-unknown-linux-gnu \
// RUN:     -resource-dir=%S/Inputs/resource_dir_with_per_target_subdir \
// RUN:   | FileCheck --check-prefix=CHECK-CLANGRT-X8664 %s
// CHECK-CLANGRT-X8664: lib{{/|\\}}x86_64-unknown-linux-gnu{{/|\\}}libclang_rt.builtins.a

// RUN: %clang -rtlib=compiler-rt -print-file-name=libclang_rt.builtins.a 2>&1 \
// RUN:     --target=x86_64-unknown-linux-gnu \
// RUN:     -resource-dir=%S/Inputs/resource_dir_with_per_target_subdir \
// RUN:   | FileCheck --check-prefix=CHECK-FILE-NAME-X8664 %s
// CHECK-FILE-NAME-X8664: lib{{/|\\}}x86_64-unknown-linux-gnu{{/|\\}}libclang_rt.builtins.a

/// Check that we handle flags such as -m32 when searching for the builtins:
/// Previously clang would use the raw triple passed to -target to find builtins
/// and sanitizer libraries, but this will result in build errors when compiling
/// with flags such as -m32. Check that we use the adjusted triple instead:
// RUN: %clang -rtlib=compiler-rt -print-file-name=libclang_rt.builtins.a 2>&1 \
// RUN:     --target=i386-linux-gnu \
// RUN:     -resource-dir=%S/Inputs/resource_dir_with_per_target_subdir \
// RUN:   | FileCheck --check-prefix=CHECK-FILE-NAME-I386 %s
// CHECK-FILE-NAME-I386: resource_dir_with_per_target_subdir{{/|\\}}lib{{/|\\}}i386-linux-gnu{{/|\\}}libclang_rt.builtins.a
// RUN: %clang -rtlib=compiler-rt -print-file-name=libclang_rt.builtins.a 2>&1 \
// RUN:     --target=x86_64-linux-gnu -m32 \
// RUN:     -resource-dir=%S/Inputs/resource_dir_with_per_target_subdir \
// RUN:   | FileCheck --check-prefix=CHECK-FILE-NAME-X8664-M32 %s
// CHECK-FILE-NAME-X8664-M32: resource_dir_with_per_target_subdir{{/|\\}}lib{{/|\\}}i386-linux-gnu{{/|\\}}libclang_rt.builtins.a
