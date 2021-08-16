set(LLVM_LINK_COMPONENTS
  Support
  )

add_lld_tool(lld
  lld.cpp

  SUPPORT_PLUGINS
  )
export_executable_symbols_for_plugins(lld)

target_link_libraries(lld
  PRIVATE
  lldCommon
  )
set(LLD_DEFAULT_SYMLINKS)
if (LLD_ENABLE_COFF_LINKER)
  target_link_libraries(lld PRIVATE lldCOFF lldMinGW)
  list(APPEND LLD_DEFAULT_SYMLINKS ld.lld lld-link)
endif()
if (LLD_ENABLE_ELF_LINKER)
  target_link_libraries(lld PRIVATE lldELF)
  list(APPEND LLD_DEFAULT_SYMLINKS ld.lld)
endif()
if (LLD_ENABLE_MACHO_LINKER)
  target_link_libraries(lld PRIVATE lldDriver lldMachO)
  list(APPEND LLD_DEFAULT_SYMLINKS ld64.lld ld64.lld.darwinnew ld64.lld.darwinold)
endif()
if (LLD_ENABLE_WASM_LINKER)
  target_link_libraries(lld PRIVATE lldWasm)
  list(APPEND LLD_DEFAULT_SYMLINKS wasm-ld)
endif()
list(REMOVE_DUPLICATES LLD_DEFAULT_SYMLINKS)

install(TARGETS lld
  RUNTIME DESTINATION bin)

if(NOT LLD_SYMLINKS_TO_CREATE)
  set(LLD_SYMLINKS_TO_CREATE ${LLD_DEFAULT_SYMLINKS})
endif()

foreach(link ${LLD_SYMLINKS_TO_CREATE})
  add_lld_symlink(${link} lld)
endforeach()
