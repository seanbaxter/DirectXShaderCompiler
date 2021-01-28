#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace llvm37
{
  class CallInst;
  class ConstantInt;
  class Function;
  class FunctionType;
  class Instruction;
  class LLVM37Context;
  class Module;
  class Pass;
}

std::vector<llvm37::CallInst*> getCallsToFunction(llvm37::Function* callee, const llvm37::Function* caller = nullptr);

llvm37::Function* getOrCreateFunction(const std::string& name, llvm37::Module* module, llvm37::FunctionType* funcType, std::map<llvm37::FunctionType*, llvm37::Function*>& typeToFuncMap);

llvm37::ConstantInt* makeInt32(int val, llvm37::LLVM37Context& context);

llvm37::Instruction* getInstructionAfter(llvm37::Instruction* inst);

std::unique_ptr<llvm37::Module> loadModuleFromAsmFile(llvm37::LLVM37Context& context, const std::string& filename);
std::unique_ptr<llvm37::Module> loadModuleFromAsmString(llvm37::LLVM37Context& context, const std::string& str);
void saveModuleToAsmFile(const llvm37::Module* module, const std::string& filename);

void dumpCFG(const llvm37::Function* F, const std::string& suffix);

void runPasses(llvm37::Function*, const std::vector<llvm37::Pass*>& passes);
