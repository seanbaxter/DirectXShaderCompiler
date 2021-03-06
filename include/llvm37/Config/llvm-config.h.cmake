/*===------- llvm37/Config/llvm-config.h - llvm37 configuration -------*- C -*-===*/
/*                                                                            */
/*                     The LLVM37 Compiler Infrastructure                       */
/*                                                                            */
/* This file is distributed under the University of Illinois Open Source      */
/* License. See LICENSE.TXT for details.                                      */
/*                                                                            */
/*===----------------------------------------------------------------------===*/

/* This file enumerates variables from the LLVM37 configuration so that they
   can be in exported headers and won't override package specific directives.
   This is a C header that can be included in the llvm37-c headers. */

#ifndef LLVM37_CONFIG_H
#define LLVM37_CONFIG_H

/* Installation directory for binary executables */
#cmakedefine LLVM37_BINDIR "${LLVM37_BINDIR}"

/* Time at which LLVM37 was configured */
#cmakedefine LLVM37_CONFIGTIME "${LLVM37_CONFIGTIME}"

/* Installation directory for data files */
#cmakedefine LLVM37_DATADIR "${LLVM37_DATADIR}"

/* Target triple LLVM37 will generate code for by default */
#cmakedefine LLVM37_DEFAULT_TARGET_TRIPLE "${LLVM37_DEFAULT_TARGET_TRIPLE}"

/* Installation directory for documentation */
#cmakedefine LLVM37_DOCSDIR "${LLVM37_DOCSDIR}"

/* Define if LLVM37 is built with asserts and checks that change the layout of
   client-visible data structures.  */
#cmakedefine LLVM37_ENABLE_ABI_BREAKING_CHECKS

/* Define if threads enabled */
#cmakedefine01 LLVM37_ENABLE_THREADS

/* Installation directory for config files */
#cmakedefine LLVM37_ETCDIR "${LLVM37_ETCDIR}"

/* Has gcc/MSVC atomic intrinsics */
#cmakedefine01 LLVM37_HAS_ATOMICS

/* Host triple LLVM37 will be executed on */
#cmakedefine LLVM37_HOST_TRIPLE "${LLVM37_HOST_TRIPLE}"

/* Installation directory for include files */
#cmakedefine LLVM37_INCLUDEDIR "${LLVM37_INCLUDEDIR}"

/* Installation directory for .info files */
#cmakedefine LLVM37_INFODIR "${LLVM37_INFODIR}"

/* Installation directory for man pages */
#cmakedefine LLVM37_MANDIR "${LLVM37_MANDIR}"

/* LLVM37 architecture name for the native architecture, if available */
#cmakedefine LLVM37_NATIVE_ARCH ${LLVM37_NATIVE_ARCH}

/* LLVM37 name for the native AsmParser init function, if available */
#cmakedefine LLVM37_NATIVE_ASMPARSER LLVM37Initialize${LLVM37_NATIVE_ARCH}AsmParser

/* LLVM37 name for the native AsmPrinter init function, if available */
#cmakedefine LLVM37_NATIVE_ASMPRINTER LLVM37Initialize${LLVM37_NATIVE_ARCH}AsmPrinter

/* LLVM37 name for the native Disassembler init function, if available */
#cmakedefine LLVM37_NATIVE_DISASSEMBLER LLVM37Initialize${LLVM37_NATIVE_ARCH}Disassembler

/* LLVM37 name for the native Target init function, if available */
#cmakedefine LLVM37_NATIVE_TARGET LLVM37Initialize${LLVM37_NATIVE_ARCH}Target

/* LLVM37 name for the native TargetInfo init function, if available */
#cmakedefine LLVM37_NATIVE_TARGETINFO LLVM37Initialize${LLVM37_NATIVE_ARCH}TargetInfo

/* LLVM37 name for the native target MC init function, if available */
#cmakedefine LLVM37_NATIVE_TARGETMC LLVM37Initialize${LLVM37_NATIVE_ARCH}TargetMC

/* Define if this is Unixish platform */
#cmakedefine LLVM37_ON_UNIX ${LLVM37_ON_UNIX}

/* Define if this is Win32ish platform */
#cmakedefine LLVM37_ON_WIN32 ${LLVM37_ON_WIN32}

/* Installation prefix directory */
#cmakedefine LLVM37_PREFIX "${LLVM37_PREFIX}"

/* Define if we have the Intel JIT API runtime support library */
#cmakedefine LLVM37_USE_INTEL_JITEVENTS 1

/* Define if we have the oprofile JIT-support library */
#cmakedefine LLVM37_USE_OPROFILE 1

/* Major version of the LLVM37 API */
#define LLVM37_VERSION_MAJOR ${LLVM37_VERSION_MAJOR}

/* Minor version of the LLVM37 API */
#define LLVM37_VERSION_MINOR ${LLVM37_VERSION_MINOR}

/* Patch version of the LLVM37 API */
#define LLVM37_VERSION_PATCH ${LLVM37_VERSION_PATCH}

/* LLVM37 version string */
#define LLVM37_VERSION_STRING "${PACKAGE_VERSION}"

/* Define if we link Polly to the tools */
#cmakedefine LINK_POLLY_INTO_TOOLS

#endif
