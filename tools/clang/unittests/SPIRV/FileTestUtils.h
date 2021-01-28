//===- FileTestUtils.h ---- Utilities For Running File Tests --------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CLANG_UNITTESTS_SPIRV_FILETESTUTILS_H
#define LLVM37_CLANG_UNITTESTS_SPIRV_FILETESTUTILS_H

#include <string>
#include <vector>

#include "dxc/Support/Global.h"
#include "dxc/Support/WinIncludes.h"
#include "dxc/Support/dxcapi.use.h"
#include "spirv-tools/libspirv.hpp"
#include "llvm37/ADT/StringRef.h"

namespace clang {
namespace spirv {
namespace utils {

/// \brief Passes the given SPIR-V binary to SPIR-V tools disassembler. The
/// SPIR-V assembly is returned via 'generatedSpirvAsm' argument.
/// Returns true on success, and false on failure.
bool disassembleSpirvBinary(std::vector<uint32_t> &binary,
                            std::string *generatedSpirvAsm,
                            bool generateHeader = false);

/// \brief Runs the SPIR-V Tools validation on the given SPIR-V binary.
/// Returns true if validation is successful; false otherwise.
bool validateSpirvBinary(spv_target_env, std::vector<uint32_t> &binary,
                         bool beforeHlslLegalization, bool glLayout,
                         bool dxLayout, bool scalarLayout,
                         std::string *message = nullptr);

/// \brief Parses the Target Profile, Entry Point, and Target Environment from
/// the Run command returns the target profile, entry point, target environment,
/// and the rest via arguments. Returns true on success, and false otherwise.
bool processRunCommandArgs(const llvm37::StringRef runCommandLine,
                           std::string *targetProfile, std::string *entryPoint,
                           spv_target_env *targetEnv,
                           std::vector<std::string> *restArgs);

/// \brief Converts an IDxcBlob into a vector of 32-bit unsigned integers which
/// is returned via the 'binaryWords' argument.
void convertIDxcBlobToUint32(const CComPtr<IDxcBlob> &blob,
                             std::vector<uint32_t> *binaryWords);

/// \brief Returns the absolute path to the input file of the test.
/// The input file is expected to be located in the directory given by the
/// testOptions::inputDataDir
std::string getAbsPathOfInputDataFile(const llvm37::StringRef filename);

/// \brief Passes the HLSL input file to the DXC compiler with SPIR-V CodeGen.
/// Returns the generated SPIR-V binary via 'generatedBinary' argument.
/// Returns true on success, and false on failure. Writes error messages to
/// errorMessages and stderr on failure.
bool runCompilerWithSpirvGeneration(const llvm37::StringRef inputFilePath,
                                    const llvm37::StringRef entryPoint,
                                    const llvm37::StringRef targetProfile,
                                    const std::vector<std::string> &restArgs,
                                    std::vector<uint32_t> *generatedBinary,
                                    std::string *errorMessages);

} // end namespace utils
} // end namespace spirv
} // end namespace clang

#endif
