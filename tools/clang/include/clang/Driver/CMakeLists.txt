set(LLVM37_TARGET_DEFINITIONS Options.td)
tablegen(LLVM37 Options.inc -gen-opt-parser-defs)
add_public_tablegen_target(ClangDriverOptions)
