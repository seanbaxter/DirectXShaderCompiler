//===-- llvm37/LinkTimeOptimizer.h - LTO Public C Interface -------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This header provides a C API to use the LLVM37 link time optimization
// library. This is intended to be used by linkers which are C-only in
// their implementation for performing LTO.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_C_LINKTIMEOPTIMIZER_H
#define LLVM37_C_LINKTIMEOPTIMIZER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LLVM37CLinkTimeOptimizer Link Time Optimization
 * @ingroup LLVM37C
 *
 * @{
 */

  /// This provides a dummy type for pointers to the LTO object.
  typedef void* llvm37_lto_t;

  /// This provides a C-visible enumerator to manage status codes.
  /// This should map exactly onto the C++ enumerator LTOStatus.
  typedef enum llvm37_lto_status {
    LLVM37_LTO_UNKNOWN,
    LLVM37_LTO_OPT_SUCCESS,
    LLVM37_LTO_READ_SUCCESS,
    LLVM37_LTO_READ_FAILURE,
    LLVM37_LTO_WRITE_FAILURE,
    LLVM37_LTO_NO_TARGET,
    LLVM37_LTO_NO_WORK,
    LLVM37_LTO_MODULE_MERGE_FAILURE,
    LLVM37_LTO_ASM_FAILURE,

    //  Added C-specific error codes
    LLVM37_LTO_NULL_OBJECT
  } llvm37_lto_status_t;

  /// This provides C interface to initialize link time optimizer. This allows
  /// linker to use dlopen() interface to dynamically load LinkTimeOptimizer.
  /// extern "C" helps, because dlopen() interface uses name to find the symbol.
  extern llvm37_lto_t llvm37_create_optimizer(void);
  extern void llvm37_destroy_optimizer(llvm37_lto_t lto);

  extern llvm37_lto_status_t llvm37_read_object_file
    (llvm37_lto_t lto, const char* input_filename);
  extern llvm37_lto_status_t llvm37_optimize_modules
    (llvm37_lto_t lto, const char* output_filename);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
