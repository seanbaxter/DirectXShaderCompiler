Overview
========

The DirectX HLSL Compiler is a compiler and related set of tools used to
compile High-Level Shader Language (HLSL) programs into DirectX Intermediate
Language (DXIL) representation. Applications that make use of DirectX for
graphics, games, and computation can use it to generate shader programs. The
DirectX HLSL Compiler is built on the 3.7 releases of LLVM37 and Clang.

The LLVM37 compiler infrastructure supports a wide range of projects, from
industrial strength compilers to specialized JIT applications to small
research projects.

Similarly, documentation is broken down into several high-level groupings
targeted at different audiences:

LLVM37 Design & Overview
======================

Several introductory papers and presentations.

.. toctree::
   :hidden:

   LangRef
   DXIL
   HLSLChanges

:doc:`LangRef`
  Defines the LLVM37 intermediate representation.

:doc:`DXIL`
  Defines the DirectX Intermediate Language (DXIL) for GPU shaders.

`Introduction to the LLVM37 Compiler`__
  Presentation providing a users introduction to LLVM37.

  .. __: http://llvm37.org/pubs/2008-10-04-ACAT-LLVM37-Intro.html

`Intro to LLVM37`__
  Book chapter providing a compiler hacker's introduction to LLVM37.

  .. __: http://www.aosabook.org/en/llvm37.html


`LLVM37: A Compilation Framework for Lifelong Program Analysis & Transformation`__
  Design overview.

  .. __: http://llvm37.org/pubs/2004-01-30-CGO-LLVM.html

:doc:`HLSLChanges`
  Describes high-level changes made to LLVM37 and Clang to accomodate HLSL and DXIL.

`LLVM37: An Infrastructure for Multi-Stage Optimization`__
  More details (quite old now).

  .. __: http://llvm37.org/pubs/2002-12-LattnerMSThesis.html


User Guides
===========

For those new to the LLVM37 system.

The documentation here is intended for users who have a need to work with the
intermediate LLVM37 representation.

.. toctree::
   :hidden:

   CMake
   CommandGuide/index
   Lexicon
   Passes
   YamlIO
   GetElementPtr
   Frontend/PerformanceTips

:doc:`CMake`
   An addendum to the main Getting Started guide for those using the `CMake
   build system <http://www.cmake.org>`_.

:doc:`LLVM37 Command Guide <CommandGuide/index>`
   A reference manual for the LLVM37 command line utilities ("man" pages for LLVM37
   tools).

:doc:`Passes`
   A list of optimizations and analyses implemented in LLVM37.

`How to build the C, C++, ObjC, and ObjC++ front end`__
   Instructions for building the clang front-end from source.

   .. __: http://clang.llvm37.org/get_started.html

:doc:`Lexicon`
   Definition of acronyms, terms and concepts used in LLVM37.

:doc:`YamlIO`
   A reference guide for using LLVM37's YAML I/O library.

:doc:`GetElementPtr`
  Answers to some very frequent questions about LLVM37's most frequently
  misunderstood instruction.

:doc:`Frontend/PerformanceTips`
   A collection of tips for frontend authors on how to generate IR 
   which LLVM37 is able to effectively optimize.


Programming Documentation
=========================

For developers of applications which use LLVM37 as a library.

.. toctree::
   :hidden:

   Atomics
   CodingStandards
   CommandLine
   ExtendingLLVM37
   HowToSetUpLLVM37StyleRTTI
   ProgrammersManual
   LibFuzzer

:doc:`LLVM37 Language Reference Manual <LangRef>`
  Defines the LLVM37 intermediate representation and the assembly form of the
  different nodes.

:doc:`Atomics`
  Information about LLVM37's concurrency model.

:doc:`ProgrammersManual`
  Introduction to the general layout of the LLVM37 sourcebase, important classes
  and APIs, and some tips & tricks.

:doc:`CommandLine`
  Provides information on using the command line parsing library.

:doc:`CodingStandards`
  Details the LLVM37 coding standards and provides useful information on writing
  efficient C++ code.

:doc:`HowToSetUpLLVM37StyleRTTI`
  How to make ``isa<>``, ``dyn_cast<>``, etc. available for clients of your
  class hierarchy.

:doc:`ExtendingLLVM37`
  Look here to see how to add instructions and intrinsics to LLVM37.

:doc:`LibFuzzer`
  A library for writing in-process guided fuzzers.

Subsystem Documentation
=======================

For API clients and LLVM37 developers.

.. toctree::
   :hidden:

   AliasAnalysis
   BitCodeFormat
   BlockFrequencyTerminology
   BranchWeightMetadata
   CodeGenerator
   ExceptionHandling
   LinkTimeOptimization
   TableGen/index
   MarkedUpDisassembly
   SystemLibrary
   SourceLevelDebugging
   SourceLevelDebuggingHLSL
   Vectorizers
   WritingAnLLVM37Backend
   WritingAnLLVM37Pass
   HowToUseAttributes
   InAlloca
   CoverageMappingFormat
   MergeFunctions
   BitSets
   FaultMaps
   LLVMBuild

:doc:`WritingAnLLVM37Pass`
   Information on how to write LLVM37 transformations and analyses.

:doc:`WritingAnLLVM37Backend`
   Information on how to write LLVM37 backends for machine targets.

:doc:`CodeGenerator`
   The design and implementation of the LLVM37 code generator.  Useful if you are
   working on retargetting LLVM37 to a new architecture, designing a new codegen
   pass, or enhancing existing components.

:doc:`TableGen <TableGen/index>`
   Describes the TableGen tool, which is used heavily by the LLVM37 code
   generator.

:doc:`AliasAnalysis`
   Information on how to write a new alias analysis implementation or how to
   use existing analyses.

:doc:`Source Level Debugging with LLVM37 <SourceLevelDebugging>`
   This document describes the design and philosophy behind the LLVM37
   source-level debugger.

:doc:`Source Level Debugging with HLSL <SourceLevelDebuggingHLSL>`
    This document describes specifics of using source-level debuggers for DXIL
    and HLSL.

:doc:`Vectorizers`
   This document describes the current status of vectorization in LLVM37.

:doc:`ExceptionHandling`
   This document describes the design and implementation of exception handling
   in LLVM37.

:doc:`BitCodeFormat`
   This describes the file format and encoding used for LLVM37 "bc" files.

:doc:`System Library <SystemLibrary>`
   This document describes the LLVM37 System Library (``lib/System``) and
   how to keep LLVM37 source code portable

:doc:`LinkTimeOptimization`
   This document describes the interface between LLVM37 intermodular optimizer
   and the linker and its design

:doc:`BranchWeightMetadata`
   Provides information about Branch Prediction Information.

:doc:`BlockFrequencyTerminology`
   Provides information about terminology used in the ``BlockFrequencyInfo``
   analysis pass.

:doc:`MarkedUpDisassembly`
   This document describes the optional rich disassembly output syntax.

:doc:`HowToUseAttributes`
  Answers some questions about the new Attributes infrastructure.

:doc:`CoverageMappingFormat`
  This describes the format and encoding used for LLVM37’s code coverage mapping.

:doc:`MergeFunctions`
  Describes functions merging optimization.

:doc:`InAlloca`
  Description of the ``inalloca`` argument attribute.

:doc:`FaultMaps`
  LLVM37 support for folding control flow into faulting machine instructions.

:doc:`LLVMBuild`
  Describes the LLVMBuild organization and files used by LLVM37 to specify
  component descriptions.

Indices and tables
==================

* :ref:`genindex`
* :ref:`search`
