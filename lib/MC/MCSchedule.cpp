//===- MCSchedule.cpp - Scheduling ------------------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the default scheduling model.
//
//===----------------------------------------------------------------------===//

#include "llvm37/MC/MCSchedule.h"
#include <type_traits>

using namespace llvm37;

static_assert(std::is_pod<MCSchedModel>::value,
              "We shouldn't have a static constructor here");
const MCSchedModel MCSchedModel::Default = {DefaultIssueWidth,
                                            DefaultMicroOpBufferSize,
                                            DefaultLoopMicroOpBufferSize,
                                            DefaultLoadLatency,
                                            DefaultHighLatency,
                                            DefaultMispredictPenalty,
                                            false,
                                            true,
                                            0,
                                            nullptr,
                                            nullptr,
                                            0,
                                            0,
                                            nullptr};
