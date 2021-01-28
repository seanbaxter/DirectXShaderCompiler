//===- SampleProfWriter.h - Write LLVM37 sample profile data ----------------===//
//
//                      The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains definitions needed for writing sample profiles.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM37_PROFILEDATA_SAMPLEPROFWRITER_H
#define LLVM37_PROFILEDATA_SAMPLEPROFWRITER_H

#include "llvm37/ADT/StringRef.h"
#include "llvm37/IR/Function.h"
#include "llvm37/IR/Module.h"
#include "llvm37/ProfileData/SampleProf.h"
#include "llvm37/Support/ErrorOr.h"
#include "llvm37/Support/FileSystem.h"
#include "llvm37/Support/raw_ostream.h"

namespace llvm37 {

namespace sampleprof {

enum SampleProfileFormat { SPF_None = 0, SPF_Text, SPF_Binary, SPF_GCC };

/// \brief Sample-based profile writer. Base class.
class SampleProfileWriter {
public:
  SampleProfileWriter(StringRef Filename, std::error_code &EC,
                      sys::fs::OpenFlags Flags)
      : OS(Filename, EC, Flags) {}
  virtual ~SampleProfileWriter() {}

  /// \brief Write sample profiles in \p S for function \p FName.
  ///
  /// \returns true if the file was updated successfully. False, otherwise.
  virtual bool write(StringRef FName, const FunctionSamples &S) = 0;

  /// \brief Write sample profiles in \p S for function \p F.
  bool write(const Function &F, const FunctionSamples &S) {
    return write(F.getName(), S);
  }

  /// \brief Write all the sample profiles for all the functions in \p M.
  ///
  /// \returns true if the file was updated successfully. False, otherwise.
  bool write(const Module &M, StringMap<FunctionSamples> &P) {
    for (const auto &F : M) {
      StringRef Name = F.getName();
      if (!write(Name, P[Name]))
        return false;
    }
    return true;
  }

  /// \brief Write all the sample profiles in the given map of samples.
  ///
  /// \returns true if the file was updated successfully. False, otherwise.
  bool write(StringMap<FunctionSamples> &ProfileMap) {
    for (auto &I : ProfileMap) {
      StringRef FName = I.first();
      FunctionSamples &Profile = I.second;
      if (!write(FName, Profile))
        return false;
    }
    return true;
  }

  /// \brief Profile writer factory. Create a new writer based on the value of
  /// \p Format.
  static ErrorOr<std::unique_ptr<SampleProfileWriter>>
  create(StringRef Filename, SampleProfileFormat Format);

protected:
  /// \brief Output stream where to emit the profile to.
  raw_fd_ostream OS;
};

/// \brief Sample-based profile writer (text format).
class SampleProfileWriterText : public SampleProfileWriter {
public:
  SampleProfileWriterText(StringRef F, std::error_code &EC)
      : SampleProfileWriter(F, EC, sys::fs::F_Text) {}

  bool write(StringRef FName, const FunctionSamples &S) override;
  bool write(const Module &M, StringMap<FunctionSamples> &P) {
    return SampleProfileWriter::write(M, P);
  }
};

/// \brief Sample-based profile writer (binary format).
class SampleProfileWriterBinary : public SampleProfileWriter {
public:
  SampleProfileWriterBinary(StringRef F, std::error_code &EC);

  bool write(StringRef F, const FunctionSamples &S) override;
  bool write(const Module &M, StringMap<FunctionSamples> &P) {
    return SampleProfileWriter::write(M, P);
  }
};

} // End namespace sampleprof

} // End namespace llvm37

#endif // LLVM37_PROFILEDATA_SAMPLEPROFWRITER_H
