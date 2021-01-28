//===-- Core.cpp ----------------------------------------------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the common infrastructure (including the C bindings)
// for libLLVM37Core.a, which implements the LLVM37 intermediate representation.
//
//===----------------------------------------------------------------------===//

#include "llvm37-c/Core.h"
#include "llvm37/Bitcode/ReaderWriter.h"
#include "llvm37/IR/Attributes.h"
#include "llvm37/IR/CallSite.h"
#include "llvm37/IR/Constants.h"
#include "llvm37/IR/DerivedTypes.h"
#include "llvm37/IR/DiagnosticInfo.h"
#include "llvm37/IR/DiagnosticPrinter.h"
#include "llvm37/IR/GlobalAlias.h"
#include "llvm37/IR/GlobalVariable.h"
#include "llvm37/IR/IRBuilder.h"
#include "llvm37/IR/InlineAsm.h"
#include "llvm37/IR/IntrinsicInst.h"
#include "llvm37/IR/LLVMContext.h"
#include "llvm37/IR/LegacyPassManager.h"
#include "llvm37/IR/Module.h"
#include "llvm37/Support/Debug.h"
#include "llvm37/Support/ErrorHandling.h"
#include "llvm37/Support/FileSystem.h"
#include "llvm37/Support/ManagedStatic.h"
#include "llvm37/Support/MemoryBuffer.h"
#include "llvm37/Support/Threading.h"
#include "llvm37/Support/raw_ostream.h"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <system_error>

using namespace llvm37;

#define DEBUG_TYPE "ir"

void llvm37::initializeCore(PassRegistry &Registry) {
  initializeDominatorTreeWrapperPassPass(Registry);
  initializePrintModulePassWrapperPass(Registry);
  initializePrintFunctionPassWrapperPass(Registry);
  initializePrintBasicBlockPassPass(Registry);
  initializeVerifierLegacyPassPass(Registry);
}

void LLVM37InitializeCore(LLVM37PassRegistryRef R) {
  initializeCore(*unwrap(R));
}

void LLVM37Shutdown() {
  llvm37_shutdown();
}

// HLSL Change: use ISO _strdup rather than strdup, which compiled but fails to link
#ifdef _MSC_VER
#define strdup _strdup
#endif

/*===-- Error handling ----------------------------------------------------===*/

char *LLVM37CreateMessage(const char *Message) {
  return strdup(Message);
}

void LLVM37DisposeMessage(char *Message) {
  free(Message);
}


/*===-- Operations on contexts --------------------------------------------===*/

LLVM37ContextRef LLVM37ContextCreate() {
  return wrap(new LLVM37Context());
}

LLVM37ContextRef LLVM37GetGlobalContext() {
  return wrap(&getGlobalContext());
}

void LLVM37ContextSetDiagnosticHandler(LLVM37ContextRef C,
                                     LLVM37DiagnosticHandler Handler,
                                     void *DiagnosticContext) {
  unwrap(C)->setDiagnosticHandler(
      LLVM37_EXTENSION reinterpret_cast<LLVM37Context::DiagnosticHandlerTy>(Handler),
      DiagnosticContext);
}

void LLVM37ContextSetYieldCallback(LLVM37ContextRef C, LLVM37YieldCallback Callback,
                                 void *OpaqueHandle) {
  auto YieldCallback =
    LLVM37_EXTENSION reinterpret_cast<LLVM37Context::YieldCallbackTy>(Callback);
  unwrap(C)->setYieldCallback(YieldCallback, OpaqueHandle);
}

void LLVM37ContextDispose(LLVM37ContextRef C) {
  delete unwrap(C);
}

unsigned LLVM37GetMDKindIDInContext(LLVM37ContextRef C, const char* Name,
                                  unsigned SLen) {
  return unwrap(C)->getMDKindID(StringRef(Name, SLen));
}

unsigned LLVM37GetMDKindID(const char* Name, unsigned SLen) {
  return LLVM37GetMDKindIDInContext(LLVM37GetGlobalContext(), Name, SLen);
}

char *LLVM37GetDiagInfoDescription(LLVM37DiagnosticInfoRef DI) {
  std::string MsgStorage;
  raw_string_ostream Stream(MsgStorage);
  DiagnosticPrinterRawOStream DP(Stream);

  unwrap(DI)->print(DP);
  Stream.flush();

  return LLVM37CreateMessage(MsgStorage.c_str());
}

LLVM37DiagnosticSeverity LLVM37GetDiagInfoSeverity(LLVM37DiagnosticInfoRef DI){
    LLVM37DiagnosticSeverity severity;

    switch(unwrap(DI)->getSeverity()) {
    default:
      severity = LLVM37DSError;
      break;
    case DS_Warning:
      severity = LLVM37DSWarning;
      break;
    case DS_Remark:
      severity = LLVM37DSRemark;
      break;
    case DS_Note:
      severity = LLVM37DSNote;
      break;
    }

    return severity;
}




/*===-- Operations on modules ---------------------------------------------===*/

LLVM37ModuleRef LLVM37ModuleCreateWithName(const char *ModuleID) {
  return wrap(new Module(ModuleID, getGlobalContext()));
}

LLVM37ModuleRef LLVM37ModuleCreateWithNameInContext(const char *ModuleID,
                                                LLVM37ContextRef C) {
  return wrap(new Module(ModuleID, *unwrap(C)));
}

void LLVM37DisposeModule(LLVM37ModuleRef M) {
  delete unwrap(M);
}

/*--.. Data layout .........................................................--*/
const char * LLVM37GetDataLayout(LLVM37ModuleRef M) {
  return unwrap(M)->getDataLayoutStr().c_str();
}

void LLVM37SetDataLayout(LLVM37ModuleRef M, const char *Triple) {
  unwrap(M)->setDataLayout(Triple);
}

/*--.. Target triple .......................................................--*/
const char * LLVM37GetTarget(LLVM37ModuleRef M) {
  return unwrap(M)->getTargetTriple().c_str();
}

void LLVM37SetTarget(LLVM37ModuleRef M, const char *Triple) {
  unwrap(M)->setTargetTriple(Triple);
}

void LLVM37DumpModule(LLVM37ModuleRef M) {
  unwrap(M)->dump();
}

LLVM37Bool LLVM37PrintModuleToFile(LLVM37ModuleRef M, const char *Filename,
                               char **ErrorMessage) {
  std::error_code EC;
  raw_fd_ostream dest(Filename, EC, sys::fs::F_Text);
  if (EC) {
    *ErrorMessage = strdup(EC.message().c_str());
    return true;
  }

  unwrap(M)->print(dest, nullptr);

  dest.close();

  if (dest.has_error()) {
    *ErrorMessage = strdup("Error printing to file");
    return true;
  }

  return false;
}

char *LLVM37PrintModuleToString(LLVM37ModuleRef M) {
  std::string buf;
  raw_string_ostream os(buf);

  unwrap(M)->print(os, nullptr);
  os.flush();

  return strdup(buf.c_str());
}

/*--.. Operations on inline assembler ......................................--*/
void LLVM37SetModuleInlineAsm(LLVM37ModuleRef M, const char *Asm) {
  unwrap(M)->setModuleInlineAsm(StringRef(Asm));
}


/*--.. Operations on module contexts ......................................--*/
LLVM37ContextRef LLVM37GetModuleContext(LLVM37ModuleRef M) {
  return wrap(&unwrap(M)->getContext());
}


/*===-- Operations on types -----------------------------------------------===*/

/*--.. Operations on all types (mostly) ....................................--*/

LLVM37TypeKind LLVM37GetTypeKind(LLVM37TypeRef Ty) {
  switch (unwrap(Ty)->getTypeID()) {
  case Type::VoidTyID:
    return LLVM37VoidTypeKind;
  case Type::HalfTyID:
    return LLVM37HalfTypeKind;
  case Type::FloatTyID:
    return LLVM37FloatTypeKind;
  case Type::DoubleTyID:
    return LLVM37DoubleTypeKind;
  case Type::X86_FP80TyID:
    return LLVM37X86_FP80TypeKind;
  case Type::FP128TyID:
    return LLVM37FP128TypeKind;
  case Type::PPC_FP128TyID:
    return LLVM37PPC_FP128TypeKind;
  case Type::LabelTyID:
    return LLVM37LabelTypeKind;
  case Type::MetadataTyID:
    return LLVM37MetadataTypeKind;
  case Type::IntegerTyID:
    return LLVM37IntegerTypeKind;
  case Type::FunctionTyID:
    return LLVM37FunctionTypeKind;
  case Type::StructTyID:
    return LLVM37StructTypeKind;
  case Type::ArrayTyID:
    return LLVM37ArrayTypeKind;
  case Type::PointerTyID:
    return LLVM37PointerTypeKind;
  case Type::VectorTyID:
    return LLVM37VectorTypeKind;
  case Type::X86_MMXTyID:
    return LLVM37X86_MMXTypeKind;
  }
  llvm37_unreachable("Unhandled TypeID.");
}

LLVM37Bool LLVM37TypeIsSized(LLVM37TypeRef Ty)
{
    return unwrap(Ty)->isSized();
}

LLVM37ContextRef LLVM37GetTypeContext(LLVM37TypeRef Ty) {
  return wrap(&unwrap(Ty)->getContext());
}

void LLVM37DumpType(LLVM37TypeRef Ty) {
  return unwrap(Ty)->dump();
}

char *LLVM37PrintTypeToString(LLVM37TypeRef Ty) {
  std::string buf;
  raw_string_ostream os(buf);

  if (unwrap(Ty))
    unwrap(Ty)->print(os);
  else
    os << "Printing <null> Type";

  os.flush();

  return strdup(buf.c_str());
}

/*--.. Operations on integer types .........................................--*/

LLVM37TypeRef LLVM37Int1TypeInContext(LLVM37ContextRef C)  {
  return (LLVM37TypeRef) Type::getInt1Ty(*unwrap(C));
}
LLVM37TypeRef LLVM37Int8TypeInContext(LLVM37ContextRef C)  {
  return (LLVM37TypeRef) Type::getInt8Ty(*unwrap(C));
}
LLVM37TypeRef LLVM37Int16TypeInContext(LLVM37ContextRef C) {
  return (LLVM37TypeRef) Type::getInt16Ty(*unwrap(C));
}
LLVM37TypeRef LLVM37Int32TypeInContext(LLVM37ContextRef C) {
  return (LLVM37TypeRef) Type::getInt32Ty(*unwrap(C));
}
LLVM37TypeRef LLVM37Int64TypeInContext(LLVM37ContextRef C) {
  return (LLVM37TypeRef) Type::getInt64Ty(*unwrap(C));
}
LLVM37TypeRef LLVM37Int128TypeInContext(LLVM37ContextRef C) {
  return (LLVM37TypeRef) Type::getInt128Ty(*unwrap(C));
}
LLVM37TypeRef LLVM37IntTypeInContext(LLVM37ContextRef C, unsigned NumBits) {
  return wrap(IntegerType::get(*unwrap(C), NumBits));
}

LLVM37TypeRef LLVM37Int1Type(void)  {
  return LLVM37Int1TypeInContext(LLVM37GetGlobalContext());
}
LLVM37TypeRef LLVM37Int8Type(void)  {
  return LLVM37Int8TypeInContext(LLVM37GetGlobalContext());
}
LLVM37TypeRef LLVM37Int16Type(void) {
  return LLVM37Int16TypeInContext(LLVM37GetGlobalContext());
}
LLVM37TypeRef LLVM37Int32Type(void) {
  return LLVM37Int32TypeInContext(LLVM37GetGlobalContext());
}
LLVM37TypeRef LLVM37Int64Type(void) {
  return LLVM37Int64TypeInContext(LLVM37GetGlobalContext());
}
LLVM37TypeRef LLVM37Int128Type(void) {
  return LLVM37Int128TypeInContext(LLVM37GetGlobalContext());
}
LLVM37TypeRef LLVM37IntType(unsigned NumBits) {
  return LLVM37IntTypeInContext(LLVM37GetGlobalContext(), NumBits);
}

unsigned LLVM37GetIntTypeWidth(LLVM37TypeRef IntegerTy) {
  return unwrap<IntegerType>(IntegerTy)->getBitWidth();
}

/*--.. Operations on real types ............................................--*/

LLVM37TypeRef LLVM37HalfTypeInContext(LLVM37ContextRef C) {
  return (LLVM37TypeRef) Type::getHalfTy(*unwrap(C));
}
LLVM37TypeRef LLVM37FloatTypeInContext(LLVM37ContextRef C) {
  return (LLVM37TypeRef) Type::getFloatTy(*unwrap(C));
}
LLVM37TypeRef LLVM37DoubleTypeInContext(LLVM37ContextRef C) {
  return (LLVM37TypeRef) Type::getDoubleTy(*unwrap(C));
}
LLVM37TypeRef LLVM37X86FP80TypeInContext(LLVM37ContextRef C) {
  return (LLVM37TypeRef) Type::getX86_FP80Ty(*unwrap(C));
}
LLVM37TypeRef LLVM37FP128TypeInContext(LLVM37ContextRef C) {
  return (LLVM37TypeRef) Type::getFP128Ty(*unwrap(C));
}
LLVM37TypeRef LLVM37PPCFP128TypeInContext(LLVM37ContextRef C) {
  return (LLVM37TypeRef) Type::getPPC_FP128Ty(*unwrap(C));
}
LLVM37TypeRef LLVM37X86MMXTypeInContext(LLVM37ContextRef C) {
  return (LLVM37TypeRef) Type::getX86_MMXTy(*unwrap(C));
}

LLVM37TypeRef LLVM37HalfType(void) {
  return LLVM37HalfTypeInContext(LLVM37GetGlobalContext());
}
LLVM37TypeRef LLVM37FloatType(void) {
  return LLVM37FloatTypeInContext(LLVM37GetGlobalContext());
}
LLVM37TypeRef LLVM37DoubleType(void) {
  return LLVM37DoubleTypeInContext(LLVM37GetGlobalContext());
}
LLVM37TypeRef LLVM37X86FP80Type(void) {
  return LLVM37X86FP80TypeInContext(LLVM37GetGlobalContext());
}
LLVM37TypeRef LLVM37FP128Type(void) {
  return LLVM37FP128TypeInContext(LLVM37GetGlobalContext());
}
LLVM37TypeRef LLVM37PPCFP128Type(void) {
  return LLVM37PPCFP128TypeInContext(LLVM37GetGlobalContext());
}
LLVM37TypeRef LLVM37X86MMXType(void) {
  return LLVM37X86MMXTypeInContext(LLVM37GetGlobalContext());
}

/*--.. Operations on function types ........................................--*/

LLVM37TypeRef LLVM37FunctionType(LLVM37TypeRef ReturnType,
                             LLVM37TypeRef *ParamTypes, unsigned ParamCount,
                             LLVM37Bool IsVarArg) {
  ArrayRef<Type*> Tys(unwrap(ParamTypes), ParamCount);
  return wrap(FunctionType::get(unwrap(ReturnType), Tys, IsVarArg != 0));
}

LLVM37Bool LLVM37IsFunctionVarArg(LLVM37TypeRef FunctionTy) {
  return unwrap<FunctionType>(FunctionTy)->isVarArg();
}

LLVM37TypeRef LLVM37GetReturnType(LLVM37TypeRef FunctionTy) {
  return wrap(unwrap<FunctionType>(FunctionTy)->getReturnType());
}

unsigned LLVM37CountParamTypes(LLVM37TypeRef FunctionTy) {
  return unwrap<FunctionType>(FunctionTy)->getNumParams();
}

void LLVM37GetParamTypes(LLVM37TypeRef FunctionTy, LLVM37TypeRef *Dest) {
  FunctionType *Ty = unwrap<FunctionType>(FunctionTy);
  for (FunctionType::param_iterator I = Ty->param_begin(),
                                    E = Ty->param_end(); I != E; ++I)
    *Dest++ = wrap(*I);
}

/*--.. Operations on struct types ..........................................--*/

LLVM37TypeRef LLVM37StructTypeInContext(LLVM37ContextRef C, LLVM37TypeRef *ElementTypes,
                           unsigned ElementCount, LLVM37Bool Packed) {
  ArrayRef<Type*> Tys(unwrap(ElementTypes), ElementCount);
  return wrap(StructType::get(*unwrap(C), Tys, Packed != 0));
}

LLVM37TypeRef LLVM37StructType(LLVM37TypeRef *ElementTypes,
                           unsigned ElementCount, LLVM37Bool Packed) {
  return LLVM37StructTypeInContext(LLVM37GetGlobalContext(), ElementTypes,
                                 ElementCount, Packed);
}

LLVM37TypeRef LLVM37StructCreateNamed(LLVM37ContextRef C, const char *Name)
{
  return wrap(StructType::create(*unwrap(C), Name));
}

const char *LLVM37GetStructName(LLVM37TypeRef Ty)
{
  StructType *Type = unwrap<StructType>(Ty);
  if (!Type->hasName())
    return nullptr;
  return Type->getName().data();
}

void LLVM37StructSetBody(LLVM37TypeRef StructTy, LLVM37TypeRef *ElementTypes,
                       unsigned ElementCount, LLVM37Bool Packed) {
  ArrayRef<Type*> Tys(unwrap(ElementTypes), ElementCount);
  unwrap<StructType>(StructTy)->setBody(Tys, Packed != 0);
}

unsigned LLVM37CountStructElementTypes(LLVM37TypeRef StructTy) {
  return unwrap<StructType>(StructTy)->getNumElements();
}

void LLVM37GetStructElementTypes(LLVM37TypeRef StructTy, LLVM37TypeRef *Dest) {
  StructType *Ty = unwrap<StructType>(StructTy);
  for (StructType::element_iterator I = Ty->element_begin(),
                                    E = Ty->element_end(); I != E; ++I)
    *Dest++ = wrap(*I);
}

LLVM37TypeRef LLVM37StructGetTypeAtIndex(LLVM37TypeRef StructTy, unsigned i) {
  StructType *Ty = unwrap<StructType>(StructTy);
  return wrap(Ty->getTypeAtIndex(i));
}

LLVM37Bool LLVM37IsPackedStruct(LLVM37TypeRef StructTy) {
  return unwrap<StructType>(StructTy)->isPacked();
}

LLVM37Bool LLVM37IsOpaqueStruct(LLVM37TypeRef StructTy) {
  return unwrap<StructType>(StructTy)->isOpaque();
}

LLVM37TypeRef LLVM37GetTypeByName(LLVM37ModuleRef M, const char *Name) {
  return wrap(unwrap(M)->getTypeByName(Name));
}

/*--.. Operations on array, pointer, and vector types (sequence types) .....--*/

LLVM37TypeRef LLVM37ArrayType(LLVM37TypeRef ElementType, unsigned ElementCount) {
  return wrap(ArrayType::get(unwrap(ElementType), ElementCount));
}

LLVM37TypeRef LLVM37PointerType(LLVM37TypeRef ElementType, unsigned AddressSpace) {
  return wrap(PointerType::get(unwrap(ElementType), AddressSpace));
}

LLVM37TypeRef LLVM37VectorType(LLVM37TypeRef ElementType, unsigned ElementCount) {
  return wrap(VectorType::get(unwrap(ElementType), ElementCount));
}

LLVM37TypeRef LLVM37GetElementType(LLVM37TypeRef Ty) {
  return wrap(unwrap<SequentialType>(Ty)->getElementType());
}

unsigned LLVM37GetArrayLength(LLVM37TypeRef ArrayTy) {
  return unwrap<ArrayType>(ArrayTy)->getNumElements();
}

unsigned LLVM37GetPointerAddressSpace(LLVM37TypeRef PointerTy) {
  return unwrap<PointerType>(PointerTy)->getAddressSpace();
}

unsigned LLVM37GetVectorSize(LLVM37TypeRef VectorTy) {
  return unwrap<VectorType>(VectorTy)->getNumElements();
}

/*--.. Operations on other types ...........................................--*/

LLVM37TypeRef LLVM37VoidTypeInContext(LLVM37ContextRef C)  {
  return wrap(Type::getVoidTy(*unwrap(C)));
}
LLVM37TypeRef LLVM37LabelTypeInContext(LLVM37ContextRef C) {
  return wrap(Type::getLabelTy(*unwrap(C)));
}

LLVM37TypeRef LLVM37VoidType(void)  {
  return LLVM37VoidTypeInContext(LLVM37GetGlobalContext());
}
LLVM37TypeRef LLVM37LabelType(void) {
  return LLVM37LabelTypeInContext(LLVM37GetGlobalContext());
}

/*===-- Operations on values ----------------------------------------------===*/

/*--.. Operations on all values ............................................--*/

LLVM37TypeRef LLVM37TypeOf(LLVM37ValueRef Val) {
  return wrap(unwrap(Val)->getType());
}

const char *LLVM37GetValueName(LLVM37ValueRef Val) {
  return unwrap(Val)->getName().data();
}

void LLVM37SetValueName(LLVM37ValueRef Val, const char *Name) {
  unwrap(Val)->setName(Name);
}

void LLVM37DumpValue(LLVM37ValueRef Val) {
  unwrap(Val)->dump();
}

char* LLVM37PrintValueToString(LLVM37ValueRef Val) {
  std::string buf;
  raw_string_ostream os(buf);

  if (unwrap(Val))
    unwrap(Val)->print(os);
  else
    os << "Printing <null> Value";

  os.flush();

  return strdup(buf.c_str());
}

void LLVM37ReplaceAllUsesWith(LLVM37ValueRef OldVal, LLVM37ValueRef NewVal) {
  unwrap(OldVal)->replaceAllUsesWith(unwrap(NewVal));
}

int LLVM37HasMetadata(LLVM37ValueRef Inst) {
  return unwrap<Instruction>(Inst)->hasMetadata();
}

LLVM37ValueRef LLVM37GetMetadata(LLVM37ValueRef Inst, unsigned KindID) {
  auto *I = unwrap<Instruction>(Inst);
  assert(I && "Expected instruction");
  if (auto *MD = I->getMetadata(KindID))
    return wrap(MetadataAsValue::get(I->getContext(), MD));
  return nullptr;
}

// MetadataAsValue uses a canonical format which strips the actual MDNode for
// MDNode with just a single constant value, storing just a ConstantAsMetadata
// This undoes this canonicalization, reconstructing the MDNode.
static MDNode *extractMDNode(MetadataAsValue *MAV) {
  Metadata *MD = MAV->getMetadata();
  assert((isa<MDNode>(MD) || isa<ConstantAsMetadata>(MD)) &&
      "Expected a metadata node or a canonicalized constant");

  if (MDNode *N = dyn_cast<MDNode>(MD))
    return N;

  return MDNode::get(MAV->getContext(), MD);
}

void LLVM37SetMetadata(LLVM37ValueRef Inst, unsigned KindID, LLVM37ValueRef Val) {
  MDNode *N = Val ? extractMDNode(unwrap<MetadataAsValue>(Val)) : nullptr;

  unwrap<Instruction>(Inst)->setMetadata(KindID, N);
}

/*--.. Conversion functions ................................................--*/

#define LLVM37_DEFINE_VALUE_CAST(name)                                       \
  LLVM37ValueRef LLVM37IsA##name(LLVM37ValueRef Val) {                           \
    return wrap(static_cast<Value*>(dyn_cast_or_null<name>(unwrap(Val)))); \
  }

LLVM37_FOR_EACH_VALUE_SUBCLASS(LLVM37_DEFINE_VALUE_CAST)

LLVM37ValueRef LLVM37IsAMDNode(LLVM37ValueRef Val) {
  if (auto *MD = dyn_cast_or_null<MetadataAsValue>(unwrap(Val)))
    if (isa<MDNode>(MD->getMetadata()) ||
        isa<ValueAsMetadata>(MD->getMetadata()))
      return Val;
  return nullptr;
}

LLVM37ValueRef LLVM37IsAMDString(LLVM37ValueRef Val) {
  if (auto *MD = dyn_cast_or_null<MetadataAsValue>(unwrap(Val)))
    if (isa<MDString>(MD->getMetadata()))
      return Val;
  return nullptr;
}

/*--.. Operations on Uses ..................................................--*/
LLVM37UseRef LLVM37GetFirstUse(LLVM37ValueRef Val) {
  Value *V = unwrap(Val);
  Value::use_iterator I = V->use_begin();
  if (I == V->use_end())
    return nullptr;
  return wrap(&*I);
}

LLVM37UseRef LLVM37GetNextUse(LLVM37UseRef U) {
  Use *Next = unwrap(U)->getNext();
  if (Next)
    return wrap(Next);
  return nullptr;
}

LLVM37ValueRef LLVM37GetUser(LLVM37UseRef U) {
  return wrap(unwrap(U)->getUser());
}

LLVM37ValueRef LLVM37GetUsedValue(LLVM37UseRef U) {
  return wrap(unwrap(U)->get());
}

/*--.. Operations on Users .................................................--*/

static LLVM37ValueRef getMDNodeOperandImpl(LLVM37Context &Context, const MDNode *N,
                                         unsigned Index) {
  Metadata *Op = N->getOperand(Index);
  if (!Op)
    return nullptr;
  if (auto *C = dyn_cast<ConstantAsMetadata>(Op))
    return wrap(C->getValue());
  return wrap(MetadataAsValue::get(Context, Op));
}

LLVM37ValueRef LLVM37GetOperand(LLVM37ValueRef Val, unsigned Index) {
  Value *V = unwrap(Val);
  if (auto *MD = dyn_cast<MetadataAsValue>(V)) {
    if (auto *L = dyn_cast<ValueAsMetadata>(MD->getMetadata())) {
      assert(Index == 0 && "Function-local metadata can only have one operand");
      return wrap(L->getValue());
    }
    return getMDNodeOperandImpl(V->getContext(),
                                cast<MDNode>(MD->getMetadata()), Index);
  }

  return wrap(cast<User>(V)->getOperand(Index));
}

LLVM37UseRef LLVM37GetOperandUse(LLVM37ValueRef Val, unsigned Index) {
  Value *V = unwrap(Val);
  return wrap(&cast<User>(V)->getOperandUse(Index));
}

void LLVM37SetOperand(LLVM37ValueRef Val, unsigned Index, LLVM37ValueRef Op) {
  unwrap<User>(Val)->setOperand(Index, unwrap(Op));
}

int LLVM37GetNumOperands(LLVM37ValueRef Val) {
  Value *V = unwrap(Val);
  if (isa<MetadataAsValue>(V))
    return LLVM37GetMDNodeNumOperands(Val);

  return cast<User>(V)->getNumOperands();
}

/*--.. Operations on constants of any type .................................--*/

LLVM37ValueRef LLVM37ConstNull(LLVM37TypeRef Ty) {
  return wrap(Constant::getNullValue(unwrap(Ty)));
}

LLVM37ValueRef LLVM37ConstAllOnes(LLVM37TypeRef Ty) {
  return wrap(Constant::getAllOnesValue(unwrap(Ty)));
}

LLVM37ValueRef LLVM37GetUndef(LLVM37TypeRef Ty) {
  return wrap(UndefValue::get(unwrap(Ty)));
}

LLVM37Bool LLVM37IsConstant(LLVM37ValueRef Ty) {
  return isa<Constant>(unwrap(Ty));
}

LLVM37Bool LLVM37IsNull(LLVM37ValueRef Val) {
  if (Constant *C = dyn_cast<Constant>(unwrap(Val)))
    return C->isNullValue();
  return false;
}

LLVM37Bool LLVM37IsUndef(LLVM37ValueRef Val) {
  return isa<UndefValue>(unwrap(Val));
}

LLVM37ValueRef LLVM37ConstPointerNull(LLVM37TypeRef Ty) {
  return
      wrap(ConstantPointerNull::get(unwrap<PointerType>(Ty)));
}

/*--.. Operations on metadata nodes ........................................--*/

LLVM37ValueRef LLVM37MDStringInContext(LLVM37ContextRef C, const char *Str,
                                   unsigned SLen) {
  LLVM37Context &Context = *unwrap(C);
  return wrap(MetadataAsValue::get(
      Context, MDString::get(Context, StringRef(Str, SLen))));
}

LLVM37ValueRef LLVM37MDString(const char *Str, unsigned SLen) {
  return LLVM37MDStringInContext(LLVM37GetGlobalContext(), Str, SLen);
}

LLVM37ValueRef LLVM37MDNodeInContext(LLVM37ContextRef C, LLVM37ValueRef *Vals,
                                 unsigned Count) {
  LLVM37Context &Context = *unwrap(C);
  SmallVector<Metadata *, 8> MDs;
  for (auto *OV : makeArrayRef(Vals, Count)) {
    Value *V = unwrap(OV);
    Metadata *MD;
    if (!V)
      MD = nullptr;
    else if (auto *C = dyn_cast<Constant>(V))
      MD = ConstantAsMetadata::get(C);
    else if (auto *MDV = dyn_cast<MetadataAsValue>(V)) {
      MD = MDV->getMetadata();
      assert(!isa<LocalAsMetadata>(MD) && "Unexpected function-local metadata "
                                          "outside of direct argument to call");
    } else {
      // This is function-local metadata.  Pretend to make an MDNode.
      assert(Count == 1 &&
             "Expected only one operand to function-local metadata");
      return wrap(MetadataAsValue::get(Context, LocalAsMetadata::get(V)));
    }

    MDs.push_back(MD);
  }
  return wrap(MetadataAsValue::get(Context, MDNode::get(Context, MDs)));
}

LLVM37ValueRef LLVM37MDNode(LLVM37ValueRef *Vals, unsigned Count) {
  return LLVM37MDNodeInContext(LLVM37GetGlobalContext(), Vals, Count);
}

const char *LLVM37GetMDString(LLVM37ValueRef V, unsigned* Len) {
  if (const auto *MD = dyn_cast<MetadataAsValue>(unwrap(V)))
    if (const MDString *S = dyn_cast<MDString>(MD->getMetadata())) {
      *Len = S->getString().size();
      return S->getString().data();
    }
  *Len = 0;
  return nullptr;
}

unsigned LLVM37GetMDNodeNumOperands(LLVM37ValueRef V)
{
  auto *MD = cast<MetadataAsValue>(unwrap(V));
  if (isa<ValueAsMetadata>(MD->getMetadata()))
    return 1;
  return cast<MDNode>(MD->getMetadata())->getNumOperands();
}

void LLVM37GetMDNodeOperands(LLVM37ValueRef V, LLVM37ValueRef *Dest)
{
  auto *MD = cast<MetadataAsValue>(unwrap(V));
  if (auto *MDV = dyn_cast<ValueAsMetadata>(MD->getMetadata())) {
    *Dest = wrap(MDV->getValue());
    return;
  }
  const auto *N = cast<MDNode>(MD->getMetadata());
  const unsigned numOperands = N->getNumOperands();
  LLVM37Context &Context = unwrap(V)->getContext();
  for (unsigned i = 0; i < numOperands; i++)
    Dest[i] = getMDNodeOperandImpl(Context, N, i);
}

unsigned LLVM37GetNamedMetadataNumOperands(LLVM37ModuleRef M, const char* name)
{
  if (NamedMDNode *N = unwrap(M)->getNamedMetadata(name)) {
    return N->getNumOperands();
  }
  return 0;
}

void LLVM37GetNamedMetadataOperands(LLVM37ModuleRef M, const char* name, LLVM37ValueRef *Dest)
{
  NamedMDNode *N = unwrap(M)->getNamedMetadata(name);
  if (!N)
    return;
  LLVM37Context &Context = unwrap(M)->getContext();
  for (unsigned i=0;i<N->getNumOperands();i++)
    Dest[i] = wrap(MetadataAsValue::get(Context, N->getOperand(i)));
}

void LLVM37AddNamedMetadataOperand(LLVM37ModuleRef M, const char* name,
                                 LLVM37ValueRef Val)
{
  NamedMDNode *N = unwrap(M)->getOrInsertNamedMetadata(name);
  if (!N)
    return;
  if (!Val)
    return;
  N->addOperand(extractMDNode(unwrap<MetadataAsValue>(Val)));
}

/*--.. Operations on scalar constants ......................................--*/

LLVM37ValueRef LLVM37ConstInt(LLVM37TypeRef IntTy, unsigned long long N,
                          LLVM37Bool SignExtend) {
  return wrap(ConstantInt::get(unwrap<IntegerType>(IntTy), N, SignExtend != 0));
}

LLVM37ValueRef LLVM37ConstIntOfArbitraryPrecision(LLVM37TypeRef IntTy,
                                              unsigned NumWords,
                                              const uint64_t Words[]) {
    IntegerType *Ty = unwrap<IntegerType>(IntTy);
    return wrap(ConstantInt::get(Ty->getContext(),
                                 APInt(Ty->getBitWidth(),
                                       makeArrayRef(Words, NumWords))));
}

LLVM37ValueRef LLVM37ConstIntOfString(LLVM37TypeRef IntTy, const char Str[],
                                  uint8_t Radix) {
  return wrap(ConstantInt::get(unwrap<IntegerType>(IntTy), StringRef(Str),
                               Radix));
}

LLVM37ValueRef LLVM37ConstIntOfStringAndSize(LLVM37TypeRef IntTy, const char Str[],
                                         unsigned SLen, uint8_t Radix) {
  return wrap(ConstantInt::get(unwrap<IntegerType>(IntTy), StringRef(Str, SLen),
                               Radix));
}

LLVM37ValueRef LLVM37ConstReal(LLVM37TypeRef RealTy, double N) {
  return wrap(ConstantFP::get(unwrap(RealTy), N));
}

LLVM37ValueRef LLVM37ConstRealOfString(LLVM37TypeRef RealTy, const char *Text) {
  return wrap(ConstantFP::get(unwrap(RealTy), StringRef(Text)));
}

LLVM37ValueRef LLVM37ConstRealOfStringAndSize(LLVM37TypeRef RealTy, const char Str[],
                                          unsigned SLen) {
  return wrap(ConstantFP::get(unwrap(RealTy), StringRef(Str, SLen)));
}

unsigned long long LLVM37ConstIntGetZExtValue(LLVM37ValueRef ConstantVal) {
  return unwrap<ConstantInt>(ConstantVal)->getZExtValue();
}

long long LLVM37ConstIntGetSExtValue(LLVM37ValueRef ConstantVal) {
  return unwrap<ConstantInt>(ConstantVal)->getSExtValue();
}

double LLVM37ConstRealGetDouble(LLVM37ValueRef ConstantVal, LLVM37Bool *LosesInfo) {
  ConstantFP *cFP = unwrap<ConstantFP>(ConstantVal) ;
  Type *Ty = cFP->getType();

  if (Ty->isFloatTy()) {
    *LosesInfo = false;
    return cFP->getValueAPF().convertToFloat();
  }

  if (Ty->isDoubleTy()) {
    *LosesInfo = false;
    return cFP->getValueAPF().convertToDouble();
  }

  bool APFLosesInfo;
  APFloat APF = cFP->getValueAPF();
  APF.convert(APFloat::IEEEdouble, APFloat::rmNearestTiesToEven, &APFLosesInfo);
  *LosesInfo = APFLosesInfo;
  return APF.convertToDouble();
}

/*--.. Operations on composite constants ...................................--*/

LLVM37ValueRef LLVM37ConstStringInContext(LLVM37ContextRef C, const char *Str,
                                      unsigned Length,
                                      LLVM37Bool DontNullTerminate) {
  /* Inverted the sense of AddNull because ', 0)' is a
     better mnemonic for null termination than ', 1)'. */
  return wrap(ConstantDataArray::getString(*unwrap(C), StringRef(Str, Length),
                                           DontNullTerminate == 0));
}
LLVM37ValueRef LLVM37ConstStructInContext(LLVM37ContextRef C,
                                      LLVM37ValueRef *ConstantVals,
                                      unsigned Count, LLVM37Bool Packed) {
  Constant **Elements = unwrap<Constant>(ConstantVals, Count);
  return wrap(ConstantStruct::getAnon(*unwrap(C), makeArrayRef(Elements, Count),
                                      Packed != 0));
}

LLVM37ValueRef LLVM37ConstString(const char *Str, unsigned Length,
                             LLVM37Bool DontNullTerminate) {
  return LLVM37ConstStringInContext(LLVM37GetGlobalContext(), Str, Length,
                                  DontNullTerminate);
}

LLVM37ValueRef LLVM37GetElementAsConstant(LLVM37ValueRef c, unsigned idx) {
  return wrap(static_cast<ConstantDataSequential*>(unwrap(c))->getElementAsConstant(idx));
}

LLVM37Bool LLVM37IsConstantString(LLVM37ValueRef c) {
  return static_cast<ConstantDataSequential*>(unwrap(c))->isString();
}

const char *LLVM37GetAsString(LLVM37ValueRef c, size_t* Length) {
  StringRef str = static_cast<ConstantDataSequential*>(unwrap(c))->getAsString();
  *Length = str.size();
  return str.data();
}

LLVM37ValueRef LLVM37ConstArray(LLVM37TypeRef ElementTy,
                            LLVM37ValueRef *ConstantVals, unsigned Length) {
  ArrayRef<Constant*> V(unwrap<Constant>(ConstantVals, Length), Length);
  return wrap(ConstantArray::get(ArrayType::get(unwrap(ElementTy), Length), V));
}

LLVM37ValueRef LLVM37ConstStruct(LLVM37ValueRef *ConstantVals, unsigned Count,
                             LLVM37Bool Packed) {
  return LLVM37ConstStructInContext(LLVM37GetGlobalContext(), ConstantVals, Count,
                                  Packed);
}

LLVM37ValueRef LLVM37ConstNamedStruct(LLVM37TypeRef StructTy,
                                  LLVM37ValueRef *ConstantVals,
                                  unsigned Count) {
  Constant **Elements = unwrap<Constant>(ConstantVals, Count);
  StructType *Ty = cast<StructType>(unwrap(StructTy));

  return wrap(ConstantStruct::get(Ty, makeArrayRef(Elements, Count)));
}

LLVM37ValueRef LLVM37ConstVector(LLVM37ValueRef *ScalarConstantVals, unsigned Size) {
  return wrap(ConstantVector::get(makeArrayRef(
                            unwrap<Constant>(ScalarConstantVals, Size), Size)));
}

/*-- Opcode mapping */

static LLVM37Opcode map_to_llvm37opcode(int opcode)
{
    switch (opcode) {
      default: llvm37_unreachable("Unhandled Opcode.");
#define HANDLE_INST(num, opc, clas) case num: return LLVM37##opc;
#include "llvm37/IR/Instruction.def"
#undef HANDLE_INST
    }
}

static int map_from_llvm37opcode(LLVM37Opcode code)
{
    switch (code) {
#define HANDLE_INST(num, opc, clas) case LLVM37##opc: return num;
#include "llvm37/IR/Instruction.def"
#undef HANDLE_INST
    }
    llvm37_unreachable("Unhandled Opcode.");
}

/*--.. Constant expressions ................................................--*/

LLVM37Opcode LLVM37GetConstOpcode(LLVM37ValueRef ConstantVal) {
  return map_to_llvm37opcode(unwrap<ConstantExpr>(ConstantVal)->getOpcode());
}

LLVM37ValueRef LLVM37AlignOf(LLVM37TypeRef Ty) {
  return wrap(ConstantExpr::getAlignOf(unwrap(Ty)));
}

LLVM37ValueRef LLVM37SizeOf(LLVM37TypeRef Ty) {
  return wrap(ConstantExpr::getSizeOf(unwrap(Ty)));
}

LLVM37ValueRef LLVM37ConstNeg(LLVM37ValueRef ConstantVal) {
  return wrap(ConstantExpr::getNeg(unwrap<Constant>(ConstantVal)));
}

LLVM37ValueRef LLVM37ConstNSWNeg(LLVM37ValueRef ConstantVal) {
  return wrap(ConstantExpr::getNSWNeg(unwrap<Constant>(ConstantVal)));
}

LLVM37ValueRef LLVM37ConstNUWNeg(LLVM37ValueRef ConstantVal) {
  return wrap(ConstantExpr::getNUWNeg(unwrap<Constant>(ConstantVal)));
}


LLVM37ValueRef LLVM37ConstFNeg(LLVM37ValueRef ConstantVal) {
  return wrap(ConstantExpr::getFNeg(unwrap<Constant>(ConstantVal)));
}

LLVM37ValueRef LLVM37ConstNot(LLVM37ValueRef ConstantVal) {
  return wrap(ConstantExpr::getNot(unwrap<Constant>(ConstantVal)));
}

LLVM37ValueRef LLVM37ConstAdd(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant) {
  return wrap(ConstantExpr::getAdd(unwrap<Constant>(LHSConstant),
                                   unwrap<Constant>(RHSConstant)));
}

LLVM37ValueRef LLVM37ConstNSWAdd(LLVM37ValueRef LHSConstant,
                             LLVM37ValueRef RHSConstant) {
  return wrap(ConstantExpr::getNSWAdd(unwrap<Constant>(LHSConstant),
                                      unwrap<Constant>(RHSConstant)));
}

LLVM37ValueRef LLVM37ConstNUWAdd(LLVM37ValueRef LHSConstant,
                             LLVM37ValueRef RHSConstant) {
  return wrap(ConstantExpr::getNUWAdd(unwrap<Constant>(LHSConstant),
                                      unwrap<Constant>(RHSConstant)));
}

LLVM37ValueRef LLVM37ConstFAdd(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant) {
  return wrap(ConstantExpr::getFAdd(unwrap<Constant>(LHSConstant),
                                    unwrap<Constant>(RHSConstant)));
}

LLVM37ValueRef LLVM37ConstSub(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant) {
  return wrap(ConstantExpr::getSub(unwrap<Constant>(LHSConstant),
                                   unwrap<Constant>(RHSConstant)));
}

LLVM37ValueRef LLVM37ConstNSWSub(LLVM37ValueRef LHSConstant,
                             LLVM37ValueRef RHSConstant) {
  return wrap(ConstantExpr::getNSWSub(unwrap<Constant>(LHSConstant),
                                      unwrap<Constant>(RHSConstant)));
}

LLVM37ValueRef LLVM37ConstNUWSub(LLVM37ValueRef LHSConstant,
                             LLVM37ValueRef RHSConstant) {
  return wrap(ConstantExpr::getNUWSub(unwrap<Constant>(LHSConstant),
                                      unwrap<Constant>(RHSConstant)));
}

LLVM37ValueRef LLVM37ConstFSub(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant) {
  return wrap(ConstantExpr::getFSub(unwrap<Constant>(LHSConstant),
                                    unwrap<Constant>(RHSConstant)));
}

LLVM37ValueRef LLVM37ConstMul(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant) {
  return wrap(ConstantExpr::getMul(unwrap<Constant>(LHSConstant),
                                   unwrap<Constant>(RHSConstant)));
}

LLVM37ValueRef LLVM37ConstNSWMul(LLVM37ValueRef LHSConstant,
                             LLVM37ValueRef RHSConstant) {
  return wrap(ConstantExpr::getNSWMul(unwrap<Constant>(LHSConstant),
                                      unwrap<Constant>(RHSConstant)));
}

LLVM37ValueRef LLVM37ConstNUWMul(LLVM37ValueRef LHSConstant,
                             LLVM37ValueRef RHSConstant) {
  return wrap(ConstantExpr::getNUWMul(unwrap<Constant>(LHSConstant),
                                      unwrap<Constant>(RHSConstant)));
}

LLVM37ValueRef LLVM37ConstFMul(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant) {
  return wrap(ConstantExpr::getFMul(unwrap<Constant>(LHSConstant),
                                    unwrap<Constant>(RHSConstant)));
}

LLVM37ValueRef LLVM37ConstUDiv(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant) {
  return wrap(ConstantExpr::getUDiv(unwrap<Constant>(LHSConstant),
                                    unwrap<Constant>(RHSConstant)));
}

LLVM37ValueRef LLVM37ConstSDiv(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant) {
  return wrap(ConstantExpr::getSDiv(unwrap<Constant>(LHSConstant),
                                    unwrap<Constant>(RHSConstant)));
}

LLVM37ValueRef LLVM37ConstExactSDiv(LLVM37ValueRef LHSConstant,
                                LLVM37ValueRef RHSConstant) {
  return wrap(ConstantExpr::getExactSDiv(unwrap<Constant>(LHSConstant),
                                         unwrap<Constant>(RHSConstant)));
}

LLVM37ValueRef LLVM37ConstFDiv(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant) {
  return wrap(ConstantExpr::getFDiv(unwrap<Constant>(LHSConstant),
                                    unwrap<Constant>(RHSConstant)));
}

LLVM37ValueRef LLVM37ConstURem(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant) {
  return wrap(ConstantExpr::getURem(unwrap<Constant>(LHSConstant),
                                    unwrap<Constant>(RHSConstant)));
}

LLVM37ValueRef LLVM37ConstSRem(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant) {
  return wrap(ConstantExpr::getSRem(unwrap<Constant>(LHSConstant),
                                    unwrap<Constant>(RHSConstant)));
}

LLVM37ValueRef LLVM37ConstFRem(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant) {
  return wrap(ConstantExpr::getFRem(unwrap<Constant>(LHSConstant),
                                    unwrap<Constant>(RHSConstant)));
}

LLVM37ValueRef LLVM37ConstAnd(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant) {
  return wrap(ConstantExpr::getAnd(unwrap<Constant>(LHSConstant),
                                   unwrap<Constant>(RHSConstant)));
}

LLVM37ValueRef LLVM37ConstOr(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant) {
  return wrap(ConstantExpr::getOr(unwrap<Constant>(LHSConstant),
                                  unwrap<Constant>(RHSConstant)));
}

LLVM37ValueRef LLVM37ConstXor(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant) {
  return wrap(ConstantExpr::getXor(unwrap<Constant>(LHSConstant),
                                   unwrap<Constant>(RHSConstant)));
}

LLVM37ValueRef LLVM37ConstICmp(LLVM37IntPredicate Predicate,
                           LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant) {
  return wrap(ConstantExpr::getICmp(Predicate,
                                    unwrap<Constant>(LHSConstant),
                                    unwrap<Constant>(RHSConstant)));
}

LLVM37ValueRef LLVM37ConstFCmp(LLVM37RealPredicate Predicate,
                           LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant) {
  return wrap(ConstantExpr::getFCmp(Predicate,
                                    unwrap<Constant>(LHSConstant),
                                    unwrap<Constant>(RHSConstant)));
}

LLVM37ValueRef LLVM37ConstShl(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant) {
  return wrap(ConstantExpr::getShl(unwrap<Constant>(LHSConstant),
                                   unwrap<Constant>(RHSConstant)));
}

LLVM37ValueRef LLVM37ConstLShr(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant) {
  return wrap(ConstantExpr::getLShr(unwrap<Constant>(LHSConstant),
                                    unwrap<Constant>(RHSConstant)));
}

LLVM37ValueRef LLVM37ConstAShr(LLVM37ValueRef LHSConstant, LLVM37ValueRef RHSConstant) {
  return wrap(ConstantExpr::getAShr(unwrap<Constant>(LHSConstant),
                                    unwrap<Constant>(RHSConstant)));
}

LLVM37ValueRef LLVM37ConstGEP(LLVM37ValueRef ConstantVal,
                          LLVM37ValueRef *ConstantIndices, unsigned NumIndices) {
  ArrayRef<Constant *> IdxList(unwrap<Constant>(ConstantIndices, NumIndices),
                               NumIndices);
  return wrap(ConstantExpr::getGetElementPtr(
      nullptr, unwrap<Constant>(ConstantVal), IdxList));
}

LLVM37ValueRef LLVM37ConstInBoundsGEP(LLVM37ValueRef ConstantVal,
                                  LLVM37ValueRef *ConstantIndices,
                                  unsigned NumIndices) {
  Constant* Val = unwrap<Constant>(ConstantVal);
  ArrayRef<Constant *> IdxList(unwrap<Constant>(ConstantIndices, NumIndices),
                               NumIndices);
  return wrap(ConstantExpr::getInBoundsGetElementPtr(nullptr, Val, IdxList));
}

LLVM37ValueRef LLVM37ConstTrunc(LLVM37ValueRef ConstantVal, LLVM37TypeRef ToType) {
  return wrap(ConstantExpr::getTrunc(unwrap<Constant>(ConstantVal),
                                     unwrap(ToType)));
}

LLVM37ValueRef LLVM37ConstSExt(LLVM37ValueRef ConstantVal, LLVM37TypeRef ToType) {
  return wrap(ConstantExpr::getSExt(unwrap<Constant>(ConstantVal),
                                    unwrap(ToType)));
}

LLVM37ValueRef LLVM37ConstZExt(LLVM37ValueRef ConstantVal, LLVM37TypeRef ToType) {
  return wrap(ConstantExpr::getZExt(unwrap<Constant>(ConstantVal),
                                    unwrap(ToType)));
}

LLVM37ValueRef LLVM37ConstFPTrunc(LLVM37ValueRef ConstantVal, LLVM37TypeRef ToType) {
  return wrap(ConstantExpr::getFPTrunc(unwrap<Constant>(ConstantVal),
                                       unwrap(ToType)));
}

LLVM37ValueRef LLVM37ConstFPExt(LLVM37ValueRef ConstantVal, LLVM37TypeRef ToType) {
  return wrap(ConstantExpr::getFPExtend(unwrap<Constant>(ConstantVal),
                                        unwrap(ToType)));
}

LLVM37ValueRef LLVM37ConstUIToFP(LLVM37ValueRef ConstantVal, LLVM37TypeRef ToType) {
  return wrap(ConstantExpr::getUIToFP(unwrap<Constant>(ConstantVal),
                                      unwrap(ToType)));
}

LLVM37ValueRef LLVM37ConstSIToFP(LLVM37ValueRef ConstantVal, LLVM37TypeRef ToType) {
  return wrap(ConstantExpr::getSIToFP(unwrap<Constant>(ConstantVal),
                                      unwrap(ToType)));
}

LLVM37ValueRef LLVM37ConstFPToUI(LLVM37ValueRef ConstantVal, LLVM37TypeRef ToType) {
  return wrap(ConstantExpr::getFPToUI(unwrap<Constant>(ConstantVal),
                                      unwrap(ToType)));
}

LLVM37ValueRef LLVM37ConstFPToSI(LLVM37ValueRef ConstantVal, LLVM37TypeRef ToType) {
  return wrap(ConstantExpr::getFPToSI(unwrap<Constant>(ConstantVal),
                                      unwrap(ToType)));
}

LLVM37ValueRef LLVM37ConstPtrToInt(LLVM37ValueRef ConstantVal, LLVM37TypeRef ToType) {
  return wrap(ConstantExpr::getPtrToInt(unwrap<Constant>(ConstantVal),
                                        unwrap(ToType)));
}

LLVM37ValueRef LLVM37ConstIntToPtr(LLVM37ValueRef ConstantVal, LLVM37TypeRef ToType) {
  return wrap(ConstantExpr::getIntToPtr(unwrap<Constant>(ConstantVal),
                                        unwrap(ToType)));
}

LLVM37ValueRef LLVM37ConstBitCast(LLVM37ValueRef ConstantVal, LLVM37TypeRef ToType) {
  return wrap(ConstantExpr::getBitCast(unwrap<Constant>(ConstantVal),
                                       unwrap(ToType)));
}

LLVM37ValueRef LLVM37ConstAddrSpaceCast(LLVM37ValueRef ConstantVal,
                                    LLVM37TypeRef ToType) {
  return wrap(ConstantExpr::getAddrSpaceCast(unwrap<Constant>(ConstantVal),
                                             unwrap(ToType)));
}

LLVM37ValueRef LLVM37ConstZExtOrBitCast(LLVM37ValueRef ConstantVal,
                                    LLVM37TypeRef ToType) {
  return wrap(ConstantExpr::getZExtOrBitCast(unwrap<Constant>(ConstantVal),
                                             unwrap(ToType)));
}

LLVM37ValueRef LLVM37ConstSExtOrBitCast(LLVM37ValueRef ConstantVal,
                                    LLVM37TypeRef ToType) {
  return wrap(ConstantExpr::getSExtOrBitCast(unwrap<Constant>(ConstantVal),
                                             unwrap(ToType)));
}

LLVM37ValueRef LLVM37ConstTruncOrBitCast(LLVM37ValueRef ConstantVal,
                                     LLVM37TypeRef ToType) {
  return wrap(ConstantExpr::getTruncOrBitCast(unwrap<Constant>(ConstantVal),
                                              unwrap(ToType)));
}

LLVM37ValueRef LLVM37ConstPointerCast(LLVM37ValueRef ConstantVal,
                                  LLVM37TypeRef ToType) {
  return wrap(ConstantExpr::getPointerCast(unwrap<Constant>(ConstantVal),
                                           unwrap(ToType)));
}

LLVM37ValueRef LLVM37ConstIntCast(LLVM37ValueRef ConstantVal, LLVM37TypeRef ToType,
                              LLVM37Bool isSigned) {
  return wrap(ConstantExpr::getIntegerCast(unwrap<Constant>(ConstantVal),
                                           unwrap(ToType), isSigned));
}

LLVM37ValueRef LLVM37ConstFPCast(LLVM37ValueRef ConstantVal, LLVM37TypeRef ToType) {
  return wrap(ConstantExpr::getFPCast(unwrap<Constant>(ConstantVal),
                                      unwrap(ToType)));
}

LLVM37ValueRef LLVM37ConstSelect(LLVM37ValueRef ConstantCondition,
                             LLVM37ValueRef ConstantIfTrue,
                             LLVM37ValueRef ConstantIfFalse) {
  return wrap(ConstantExpr::getSelect(unwrap<Constant>(ConstantCondition),
                                      unwrap<Constant>(ConstantIfTrue),
                                      unwrap<Constant>(ConstantIfFalse)));
}

LLVM37ValueRef LLVM37ConstExtractElement(LLVM37ValueRef VectorConstant,
                                     LLVM37ValueRef IndexConstant) {
  return wrap(ConstantExpr::getExtractElement(unwrap<Constant>(VectorConstant),
                                              unwrap<Constant>(IndexConstant)));
}

LLVM37ValueRef LLVM37ConstInsertElement(LLVM37ValueRef VectorConstant,
                                    LLVM37ValueRef ElementValueConstant,
                                    LLVM37ValueRef IndexConstant) {
  return wrap(ConstantExpr::getInsertElement(unwrap<Constant>(VectorConstant),
                                         unwrap<Constant>(ElementValueConstant),
                                             unwrap<Constant>(IndexConstant)));
}

LLVM37ValueRef LLVM37ConstShuffleVector(LLVM37ValueRef VectorAConstant,
                                    LLVM37ValueRef VectorBConstant,
                                    LLVM37ValueRef MaskConstant) {
  return wrap(ConstantExpr::getShuffleVector(unwrap<Constant>(VectorAConstant),
                                             unwrap<Constant>(VectorBConstant),
                                             unwrap<Constant>(MaskConstant)));
}

LLVM37ValueRef LLVM37ConstExtractValue(LLVM37ValueRef AggConstant, unsigned *IdxList,
                                   unsigned NumIdx) {
  return wrap(ConstantExpr::getExtractValue(unwrap<Constant>(AggConstant),
                                            makeArrayRef(IdxList, NumIdx)));
}

LLVM37ValueRef LLVM37ConstInsertValue(LLVM37ValueRef AggConstant,
                                  LLVM37ValueRef ElementValueConstant,
                                  unsigned *IdxList, unsigned NumIdx) {
  return wrap(ConstantExpr::getInsertValue(unwrap<Constant>(AggConstant),
                                         unwrap<Constant>(ElementValueConstant),
                                           makeArrayRef(IdxList, NumIdx)));
}

LLVM37ValueRef LLVM37ConstInlineAsm(LLVM37TypeRef Ty, const char *AsmString,
                                const char *Constraints,
                                LLVM37Bool HasSideEffects,
                                LLVM37Bool IsAlignStack) {
  return wrap(InlineAsm::get(dyn_cast<FunctionType>(unwrap(Ty)), AsmString,
                             Constraints, HasSideEffects, IsAlignStack));
}

LLVM37ValueRef LLVM37BlockAddress(LLVM37ValueRef F, LLVM37BasicBlockRef BB) {
  return wrap(BlockAddress::get(unwrap<Function>(F), unwrap(BB)));
}

/*--.. Operations on global variables, functions, and aliases (globals) ....--*/

LLVM37ModuleRef LLVM37GetGlobalParent(LLVM37ValueRef Global) {
  return wrap(unwrap<GlobalValue>(Global)->getParent());
}

LLVM37Bool LLVM37IsDeclaration(LLVM37ValueRef Global) {
  return unwrap<GlobalValue>(Global)->isDeclaration();
}

LLVM37Linkage LLVM37GetLinkage(LLVM37ValueRef Global) {
  switch (unwrap<GlobalValue>(Global)->getLinkage()) {
  case GlobalValue::ExternalLinkage:
    return LLVM37ExternalLinkage;
  case GlobalValue::AvailableExternallyLinkage:
    return LLVM37AvailableExternallyLinkage;
  case GlobalValue::LinkOnceAnyLinkage:
    return LLVM37LinkOnceAnyLinkage;
  case GlobalValue::LinkOnceODRLinkage:
    return LLVM37LinkOnceODRLinkage;
  case GlobalValue::WeakAnyLinkage:
    return LLVM37WeakAnyLinkage;
  case GlobalValue::WeakODRLinkage:
    return LLVM37WeakODRLinkage;
  case GlobalValue::AppendingLinkage:
    return LLVM37AppendingLinkage;
  case GlobalValue::InternalLinkage:
    return LLVM37InternalLinkage;
  case GlobalValue::PrivateLinkage:
    return LLVM37PrivateLinkage;
  case GlobalValue::ExternalWeakLinkage:
    return LLVM37ExternalWeakLinkage;
  case GlobalValue::CommonLinkage:
    return LLVM37CommonLinkage;
  }

  llvm37_unreachable("Invalid GlobalValue linkage!");
}

void LLVM37SetLinkage(LLVM37ValueRef Global, LLVM37Linkage Linkage) {
  GlobalValue *GV = unwrap<GlobalValue>(Global);

  switch (Linkage) {
  case LLVM37ExternalLinkage:
    GV->setLinkage(GlobalValue::ExternalLinkage);
    break;
  case LLVM37AvailableExternallyLinkage:
    GV->setLinkage(GlobalValue::AvailableExternallyLinkage);
    break;
  case LLVM37LinkOnceAnyLinkage:
    GV->setLinkage(GlobalValue::LinkOnceAnyLinkage);
    break;
  case LLVM37LinkOnceODRLinkage:
    GV->setLinkage(GlobalValue::LinkOnceODRLinkage);
    break;
  case LLVM37LinkOnceODRAutoHideLinkage:
    DEBUG(errs() << "LLVM37SetLinkage(): LLVM37LinkOnceODRAutoHideLinkage is no "
                    "longer supported.");
    break;
  case LLVM37WeakAnyLinkage:
    GV->setLinkage(GlobalValue::WeakAnyLinkage);
    break;
  case LLVM37WeakODRLinkage:
    GV->setLinkage(GlobalValue::WeakODRLinkage);
    break;
  case LLVM37AppendingLinkage:
    GV->setLinkage(GlobalValue::AppendingLinkage);
    break;
  case LLVM37InternalLinkage:
    GV->setLinkage(GlobalValue::InternalLinkage);
    break;
  case LLVM37PrivateLinkage:
    GV->setLinkage(GlobalValue::PrivateLinkage);
    break;
  case LLVM37LinkerPrivateLinkage:
    GV->setLinkage(GlobalValue::PrivateLinkage);
    break;
  case LLVM37LinkerPrivateWeakLinkage:
    GV->setLinkage(GlobalValue::PrivateLinkage);
    break;
  case LLVM37DLLImportLinkage:
    DEBUG(errs()
          << "LLVM37SetLinkage(): LLVM37DLLImportLinkage is no longer supported.");
    break;
  case LLVM37DLLExportLinkage:
    DEBUG(errs()
          << "LLVM37SetLinkage(): LLVM37DLLExportLinkage is no longer supported.");
    break;
  case LLVM37ExternalWeakLinkage:
    GV->setLinkage(GlobalValue::ExternalWeakLinkage);
    break;
  case LLVM37GhostLinkage:
    DEBUG(errs()
          << "LLVM37SetLinkage(): LLVM37GhostLinkage is no longer supported.");
    break;
  case LLVM37CommonLinkage:
    GV->setLinkage(GlobalValue::CommonLinkage);
    break;
  }
}

const char *LLVM37GetSection(LLVM37ValueRef Global) {
  return unwrap<GlobalValue>(Global)->getSection();
}

void LLVM37SetSection(LLVM37ValueRef Global, const char *Section) {
  unwrap<GlobalObject>(Global)->setSection(Section);
}

LLVM37Visibility LLVM37GetVisibility(LLVM37ValueRef Global) {
  return static_cast<LLVM37Visibility>(
    unwrap<GlobalValue>(Global)->getVisibility());
}

void LLVM37SetVisibility(LLVM37ValueRef Global, LLVM37Visibility Viz) {
  unwrap<GlobalValue>(Global)
    ->setVisibility(static_cast<GlobalValue::VisibilityTypes>(Viz));
}

LLVM37DLLStorageClass LLVM37GetDLLStorageClass(LLVM37ValueRef Global) {
  return static_cast<LLVM37DLLStorageClass>(
      unwrap<GlobalValue>(Global)->getDLLStorageClass());
}

void LLVM37SetDLLStorageClass(LLVM37ValueRef Global, LLVM37DLLStorageClass Class) {
  unwrap<GlobalValue>(Global)->setDLLStorageClass(
      static_cast<GlobalValue::DLLStorageClassTypes>(Class));
}

LLVM37Bool LLVM37HasUnnamedAddr(LLVM37ValueRef Global) {
  return unwrap<GlobalValue>(Global)->hasUnnamedAddr();
}

void LLVM37SetUnnamedAddr(LLVM37ValueRef Global, LLVM37Bool HasUnnamedAddr) {
  unwrap<GlobalValue>(Global)->setUnnamedAddr(HasUnnamedAddr);
}

/*--.. Operations on global variables, load and store instructions .........--*/

unsigned LLVM37GetAlignment(LLVM37ValueRef V) {
  Value *P = unwrap<Value>(V);
  if (GlobalValue *GV = dyn_cast<GlobalValue>(P))
    return GV->getAlignment();
  if (AllocaInst *AI = dyn_cast<AllocaInst>(P))
    return AI->getAlignment();
  if (LoadInst *LI = dyn_cast<LoadInst>(P))
    return LI->getAlignment();
  if (StoreInst *SI = dyn_cast<StoreInst>(P))
    return SI->getAlignment();

  llvm37_unreachable(
      "only GlobalValue, AllocaInst, LoadInst and StoreInst have alignment");
}

void LLVM37SetAlignment(LLVM37ValueRef V, unsigned Bytes) {
  Value *P = unwrap<Value>(V);
  if (GlobalObject *GV = dyn_cast<GlobalObject>(P))
    GV->setAlignment(Bytes);
  else if (AllocaInst *AI = dyn_cast<AllocaInst>(P))
    AI->setAlignment(Bytes);
  else if (LoadInst *LI = dyn_cast<LoadInst>(P))
    LI->setAlignment(Bytes);
  else if (StoreInst *SI = dyn_cast<StoreInst>(P))
    SI->setAlignment(Bytes);
  else
    llvm37_unreachable(
        "only GlobalValue, AllocaInst, LoadInst and StoreInst have alignment");
}

/*--.. Operations on global variables ......................................--*/

LLVM37ValueRef LLVM37AddGlobal(LLVM37ModuleRef M, LLVM37TypeRef Ty, const char *Name) {
  return wrap(new GlobalVariable(*unwrap(M), unwrap(Ty), false,
                                 GlobalValue::ExternalLinkage, nullptr, Name));
}

LLVM37ValueRef LLVM37AddGlobalInAddressSpace(LLVM37ModuleRef M, LLVM37TypeRef Ty,
                                         const char *Name,
                                         unsigned AddressSpace) {
  return wrap(new GlobalVariable(*unwrap(M), unwrap(Ty), false,
                                 GlobalValue::ExternalLinkage, nullptr, Name,
                                 nullptr, GlobalVariable::NotThreadLocal,
                                 AddressSpace));
}

LLVM37ValueRef LLVM37GetNamedGlobal(LLVM37ModuleRef M, const char *Name) {
  return wrap(unwrap(M)->getNamedGlobal(Name));
}

LLVM37ValueRef LLVM37GetFirstGlobal(LLVM37ModuleRef M) {
  Module *Mod = unwrap(M);
  Module::global_iterator I = Mod->global_begin();
  if (I == Mod->global_end())
    return nullptr;
  return wrap(I);
}

LLVM37ValueRef LLVM37GetLastGlobal(LLVM37ModuleRef M) {
  Module *Mod = unwrap(M);
  Module::global_iterator I = Mod->global_end();
  if (I == Mod->global_begin())
    return nullptr;
  return wrap(--I);
}

LLVM37ValueRef LLVM37GetNextGlobal(LLVM37ValueRef GlobalVar) {
  GlobalVariable *GV = unwrap<GlobalVariable>(GlobalVar);
  Module::global_iterator I = GV;
  if (++I == GV->getParent()->global_end())
    return nullptr;
  return wrap(I);
}

LLVM37ValueRef LLVM37GetPreviousGlobal(LLVM37ValueRef GlobalVar) {
  GlobalVariable *GV = unwrap<GlobalVariable>(GlobalVar);
  Module::global_iterator I = GV;
  if (I == GV->getParent()->global_begin())
    return nullptr;
  return wrap(--I);
}

void LLVM37DeleteGlobal(LLVM37ValueRef GlobalVar) {
  unwrap<GlobalVariable>(GlobalVar)->eraseFromParent();
}

LLVM37ValueRef LLVM37GetInitializer(LLVM37ValueRef GlobalVar) {
  GlobalVariable* GV = unwrap<GlobalVariable>(GlobalVar);
  if ( !GV->hasInitializer() )
    return nullptr;
  return wrap(GV->getInitializer());
}

void LLVM37SetInitializer(LLVM37ValueRef GlobalVar, LLVM37ValueRef ConstantVal) {
  unwrap<GlobalVariable>(GlobalVar)
    ->setInitializer(unwrap<Constant>(ConstantVal));
}

LLVM37Bool LLVM37IsThreadLocal(LLVM37ValueRef GlobalVar) {
  return unwrap<GlobalVariable>(GlobalVar)->isThreadLocal();
}

void LLVM37SetThreadLocal(LLVM37ValueRef GlobalVar, LLVM37Bool IsThreadLocal) {
  unwrap<GlobalVariable>(GlobalVar)->setThreadLocal(IsThreadLocal != 0);
}

LLVM37Bool LLVM37IsGlobalConstant(LLVM37ValueRef GlobalVar) {
  return unwrap<GlobalVariable>(GlobalVar)->isConstant();
}

void LLVM37SetGlobalConstant(LLVM37ValueRef GlobalVar, LLVM37Bool IsConstant) {
  unwrap<GlobalVariable>(GlobalVar)->setConstant(IsConstant != 0);
}

LLVM37ThreadLocalMode LLVM37GetThreadLocalMode(LLVM37ValueRef GlobalVar) {
  switch (unwrap<GlobalVariable>(GlobalVar)->getThreadLocalMode()) {
  case GlobalVariable::NotThreadLocal:
    return LLVM37NotThreadLocal;
  case GlobalVariable::GeneralDynamicTLSModel:
    return LLVM37GeneralDynamicTLSModel;
  case GlobalVariable::LocalDynamicTLSModel:
    return LLVM37LocalDynamicTLSModel;
  case GlobalVariable::InitialExecTLSModel:
    return LLVM37InitialExecTLSModel;
  case GlobalVariable::LocalExecTLSModel:
    return LLVM37LocalExecTLSModel;
  }

  llvm37_unreachable("Invalid GlobalVariable thread local mode");
}

void LLVM37SetThreadLocalMode(LLVM37ValueRef GlobalVar, LLVM37ThreadLocalMode Mode) {
  GlobalVariable *GV = unwrap<GlobalVariable>(GlobalVar);

  switch (Mode) {
  case LLVM37NotThreadLocal:
    GV->setThreadLocalMode(GlobalVariable::NotThreadLocal);
    break;
  case LLVM37GeneralDynamicTLSModel:
    GV->setThreadLocalMode(GlobalVariable::GeneralDynamicTLSModel);
    break;
  case LLVM37LocalDynamicTLSModel:
    GV->setThreadLocalMode(GlobalVariable::LocalDynamicTLSModel);
    break;
  case LLVM37InitialExecTLSModel:
    GV->setThreadLocalMode(GlobalVariable::InitialExecTLSModel);
    break;
  case LLVM37LocalExecTLSModel:
    GV->setThreadLocalMode(GlobalVariable::LocalExecTLSModel);
    break;
  }
}

LLVM37Bool LLVM37IsExternallyInitialized(LLVM37ValueRef GlobalVar) {
  return unwrap<GlobalVariable>(GlobalVar)->isExternallyInitialized();
}

void LLVM37SetExternallyInitialized(LLVM37ValueRef GlobalVar, LLVM37Bool IsExtInit) {
  unwrap<GlobalVariable>(GlobalVar)->setExternallyInitialized(IsExtInit);
}

/*--.. Operations on aliases ......................................--*/

LLVM37ValueRef LLVM37AddAlias(LLVM37ModuleRef M, LLVM37TypeRef Ty, LLVM37ValueRef Aliasee,
                          const char *Name) {
  auto *PTy = cast<PointerType>(unwrap(Ty));
  return wrap(GlobalAlias::create(PTy, GlobalValue::ExternalLinkage, Name,
                                  unwrap<Constant>(Aliasee), unwrap(M)));
}

/*--.. Operations on functions .............................................--*/

LLVM37ValueRef LLVM37AddFunction(LLVM37ModuleRef M, const char *Name,
                             LLVM37TypeRef FunctionTy) {
  return wrap(Function::Create(unwrap<FunctionType>(FunctionTy),
                               GlobalValue::ExternalLinkage, Name, unwrap(M)));
}

LLVM37ValueRef LLVM37GetNamedFunction(LLVM37ModuleRef M, const char *Name) {
  return wrap(unwrap(M)->getFunction(Name));
}

LLVM37ValueRef LLVM37GetFirstFunction(LLVM37ModuleRef M) {
  Module *Mod = unwrap(M);
  Module::iterator I = Mod->begin();
  if (I == Mod->end())
    return nullptr;
  return wrap(I);
}

LLVM37ValueRef LLVM37GetLastFunction(LLVM37ModuleRef M) {
  Module *Mod = unwrap(M);
  Module::iterator I = Mod->end();
  if (I == Mod->begin())
    return nullptr;
  return wrap(--I);
}

LLVM37ValueRef LLVM37GetNextFunction(LLVM37ValueRef Fn) {
  Function *Func = unwrap<Function>(Fn);
  Module::iterator I = Func;
  if (++I == Func->getParent()->end())
    return nullptr;
  return wrap(I);
}

LLVM37ValueRef LLVM37GetPreviousFunction(LLVM37ValueRef Fn) {
  Function *Func = unwrap<Function>(Fn);
  Module::iterator I = Func;
  if (I == Func->getParent()->begin())
    return nullptr;
  return wrap(--I);
}

void LLVM37DeleteFunction(LLVM37ValueRef Fn) {
  unwrap<Function>(Fn)->eraseFromParent();
}

LLVM37ValueRef LLVM37GetPersonalityFn(LLVM37ValueRef Fn) {
  return wrap(unwrap<Function>(Fn)->getPersonalityFn());
}

void LLVM37SetPersonalityFn(LLVM37ValueRef Fn, LLVM37ValueRef PersonalityFn) {
  unwrap<Function>(Fn)->setPersonalityFn(unwrap<Constant>(PersonalityFn));
}

unsigned LLVM37GetIntrinsicID(LLVM37ValueRef Fn) {
  if (Function *F = dyn_cast<Function>(unwrap(Fn)))
    return F->getIntrinsicID();
  return 0;
}

unsigned LLVM37GetFunctionCallConv(LLVM37ValueRef Fn) {
  return unwrap<Function>(Fn)->getCallingConv();
}

void LLVM37SetFunctionCallConv(LLVM37ValueRef Fn, unsigned CC) {
  return unwrap<Function>(Fn)->setCallingConv(
    static_cast<CallingConv::ID>(CC));
}

const char *LLVM37GetGC(LLVM37ValueRef Fn) {
  Function *F = unwrap<Function>(Fn);
  return F->hasGC()? F->getGC() : nullptr;
}

void LLVM37SetGC(LLVM37ValueRef Fn, const char *GC) {
  Function *F = unwrap<Function>(Fn);
  if (GC)
    F->setGC(GC);
  else
    F->clearGC();
}

void LLVM37AddFunctionAttr(LLVM37ValueRef Fn, LLVM37Attribute PA) {
  Function *Func = unwrap<Function>(Fn);
  const AttributeSet PAL = Func->getAttributes();
  AttrBuilder B(PA);
  const AttributeSet PALnew =
    PAL.addAttributes(Func->getContext(), AttributeSet::FunctionIndex,
                      AttributeSet::get(Func->getContext(),
                                        AttributeSet::FunctionIndex, B));
  Func->setAttributes(PALnew);
}

void LLVM37AddTargetDependentFunctionAttr(LLVM37ValueRef Fn, const char *A,
                                        const char *V) {
  Function *Func = unwrap<Function>(Fn);
  AttributeSet::AttrIndex Idx =
    AttributeSet::AttrIndex(AttributeSet::FunctionIndex);
  AttrBuilder B;

  B.addAttribute(A, V);
  AttributeSet Set = AttributeSet::get(Func->getContext(), Idx, B);
  Func->addAttributes(Idx, Set);
}

void LLVM37RemoveFunctionAttr(LLVM37ValueRef Fn, LLVM37Attribute PA) {
  Function *Func = unwrap<Function>(Fn);
  const AttributeSet PAL = Func->getAttributes();
  AttrBuilder B(PA);
  const AttributeSet PALnew =
    PAL.removeAttributes(Func->getContext(), AttributeSet::FunctionIndex,
                         AttributeSet::get(Func->getContext(),
                                           AttributeSet::FunctionIndex, B));
  Func->setAttributes(PALnew);
}

LLVM37Attribute LLVM37GetFunctionAttr(LLVM37ValueRef Fn) {
  Function *Func = unwrap<Function>(Fn);
  const AttributeSet PAL = Func->getAttributes();
  return (LLVM37Attribute)PAL.Raw(AttributeSet::FunctionIndex);
}

/*--.. Operations on parameters ............................................--*/

unsigned LLVM37CountParams(LLVM37ValueRef FnRef) {
  // This function is strictly redundant to
  //   LLVM37CountParamTypes(LLVM37GetElementType(LLVM37TypeOf(FnRef)))
  return unwrap<Function>(FnRef)->arg_size();
}

void LLVM37GetParams(LLVM37ValueRef FnRef, LLVM37ValueRef *ParamRefs) {
  Function *Fn = unwrap<Function>(FnRef);
  for (Function::arg_iterator I = Fn->arg_begin(),
                              E = Fn->arg_end(); I != E; I++)
    *ParamRefs++ = wrap(I);
}

LLVM37ValueRef LLVM37GetParam(LLVM37ValueRef FnRef, unsigned index) {
  Function::arg_iterator AI = unwrap<Function>(FnRef)->arg_begin();
  while (index --> 0)
    AI++;
  return wrap(AI);
}

LLVM37ValueRef LLVM37GetParamParent(LLVM37ValueRef V) {
  return wrap(unwrap<Argument>(V)->getParent());
}

LLVM37ValueRef LLVM37GetFirstParam(LLVM37ValueRef Fn) {
  Function *Func = unwrap<Function>(Fn);
  Function::arg_iterator I = Func->arg_begin();
  if (I == Func->arg_end())
    return nullptr;
  return wrap(I);
}

LLVM37ValueRef LLVM37GetLastParam(LLVM37ValueRef Fn) {
  Function *Func = unwrap<Function>(Fn);
  Function::arg_iterator I = Func->arg_end();
  if (I == Func->arg_begin())
    return nullptr;
  return wrap(--I);
}

LLVM37ValueRef LLVM37GetNextParam(LLVM37ValueRef Arg) {
  Argument *A = unwrap<Argument>(Arg);
  Function::arg_iterator I = A;
  if (++I == A->getParent()->arg_end())
    return nullptr;
  return wrap(I);
}

LLVM37ValueRef LLVM37GetPreviousParam(LLVM37ValueRef Arg) {
  Argument *A = unwrap<Argument>(Arg);
  Function::arg_iterator I = A;
  if (I == A->getParent()->arg_begin())
    return nullptr;
  return wrap(--I);
}

void LLVM37AddAttribute(LLVM37ValueRef Arg, LLVM37Attribute PA) {
  Argument *A = unwrap<Argument>(Arg);
  AttrBuilder B(PA);
  A->addAttr(AttributeSet::get(A->getContext(), A->getArgNo() + 1,  B));
}

void LLVM37RemoveAttribute(LLVM37ValueRef Arg, LLVM37Attribute PA) {
  Argument *A = unwrap<Argument>(Arg);
  AttrBuilder B(PA);
  A->removeAttr(AttributeSet::get(A->getContext(), A->getArgNo() + 1,  B));
}

LLVM37Attribute LLVM37GetAttribute(LLVM37ValueRef Arg) {
  Argument *A = unwrap<Argument>(Arg);
  return (LLVM37Attribute)A->getParent()->getAttributes().
    Raw(A->getArgNo()+1);
}


void LLVM37SetParamAlignment(LLVM37ValueRef Arg, unsigned align) {
  Argument *A = unwrap<Argument>(Arg);
  AttrBuilder B;
  B.addAlignmentAttr(align);
  A->addAttr(AttributeSet::get(A->getContext(),A->getArgNo() + 1, B));
}

/*--.. Operations on basic blocks ..........................................--*/

LLVM37ValueRef LLVM37BasicBlockAsValue(LLVM37BasicBlockRef BB) {
  return wrap(static_cast<Value*>(unwrap(BB)));
}

LLVM37Bool LLVM37ValueIsBasicBlock(LLVM37ValueRef Val) {
  return isa<BasicBlock>(unwrap(Val));
}

LLVM37BasicBlockRef LLVM37ValueAsBasicBlock(LLVM37ValueRef Val) {
  return wrap(unwrap<BasicBlock>(Val));
}

LLVM37ValueRef LLVM37GetBasicBlockParent(LLVM37BasicBlockRef BB) {
  return wrap(unwrap(BB)->getParent());
}

LLVM37ValueRef LLVM37GetBasicBlockTerminator(LLVM37BasicBlockRef BB) {
  return wrap(unwrap(BB)->getTerminator());
}

unsigned LLVM37CountBasicBlocks(LLVM37ValueRef FnRef) {
  return unwrap<Function>(FnRef)->size();
}

void LLVM37GetBasicBlocks(LLVM37ValueRef FnRef, LLVM37BasicBlockRef *BasicBlocksRefs){
  Function *Fn = unwrap<Function>(FnRef);
  for (Function::iterator I = Fn->begin(), E = Fn->end(); I != E; I++)
    *BasicBlocksRefs++ = wrap(I);
}

LLVM37BasicBlockRef LLVM37GetEntryBasicBlock(LLVM37ValueRef Fn) {
  return wrap(&unwrap<Function>(Fn)->getEntryBlock());
}

LLVM37BasicBlockRef LLVM37GetFirstBasicBlock(LLVM37ValueRef Fn) {
  Function *Func = unwrap<Function>(Fn);
  Function::iterator I = Func->begin();
  if (I == Func->end())
    return nullptr;
  return wrap(I);
}

LLVM37BasicBlockRef LLVM37GetLastBasicBlock(LLVM37ValueRef Fn) {
  Function *Func = unwrap<Function>(Fn);
  Function::iterator I = Func->end();
  if (I == Func->begin())
    return nullptr;
  return wrap(--I);
}

LLVM37BasicBlockRef LLVM37GetNextBasicBlock(LLVM37BasicBlockRef BB) {
  BasicBlock *Block = unwrap(BB);
  Function::iterator I = Block;
  if (++I == Block->getParent()->end())
    return nullptr;
  return wrap(I);
}

LLVM37BasicBlockRef LLVM37GetPreviousBasicBlock(LLVM37BasicBlockRef BB) {
  BasicBlock *Block = unwrap(BB);
  Function::iterator I = Block;
  if (I == Block->getParent()->begin())
    return nullptr;
  return wrap(--I);
}

LLVM37BasicBlockRef LLVM37AppendBasicBlockInContext(LLVM37ContextRef C,
                                                LLVM37ValueRef FnRef,
                                                const char *Name) {
  return wrap(BasicBlock::Create(*unwrap(C), Name, unwrap<Function>(FnRef)));
}

LLVM37BasicBlockRef LLVM37AppendBasicBlock(LLVM37ValueRef FnRef, const char *Name) {
  return LLVM37AppendBasicBlockInContext(LLVM37GetGlobalContext(), FnRef, Name);
}

LLVM37BasicBlockRef LLVM37InsertBasicBlockInContext(LLVM37ContextRef C,
                                                LLVM37BasicBlockRef BBRef,
                                                const char *Name) {
  BasicBlock *BB = unwrap(BBRef);
  return wrap(BasicBlock::Create(*unwrap(C), Name, BB->getParent(), BB));
}

LLVM37BasicBlockRef LLVM37InsertBasicBlock(LLVM37BasicBlockRef BBRef,
                                       const char *Name) {
  return LLVM37InsertBasicBlockInContext(LLVM37GetGlobalContext(), BBRef, Name);
}

void LLVM37DeleteBasicBlock(LLVM37BasicBlockRef BBRef) {
  unwrap(BBRef)->eraseFromParent();
}

void LLVM37RemoveBasicBlockFromParent(LLVM37BasicBlockRef BBRef) {
  unwrap(BBRef)->removeFromParent();
}

void LLVM37MoveBasicBlockBefore(LLVM37BasicBlockRef BB, LLVM37BasicBlockRef MovePos) {
  unwrap(BB)->moveBefore(unwrap(MovePos));
}

void LLVM37MoveBasicBlockAfter(LLVM37BasicBlockRef BB, LLVM37BasicBlockRef MovePos) {
  unwrap(BB)->moveAfter(unwrap(MovePos));
}

/*--.. Operations on instructions ..........................................--*/

LLVM37BasicBlockRef LLVM37GetInstructionParent(LLVM37ValueRef Inst) {
  return wrap(unwrap<Instruction>(Inst)->getParent());
}

LLVM37ValueRef LLVM37GetFirstInstruction(LLVM37BasicBlockRef BB) {
  BasicBlock *Block = unwrap(BB);
  BasicBlock::iterator I = Block->begin();
  if (I == Block->end())
    return nullptr;
  return wrap(I);
}

LLVM37ValueRef LLVM37GetLastInstruction(LLVM37BasicBlockRef BB) {
  BasicBlock *Block = unwrap(BB);
  BasicBlock::iterator I = Block->end();
  if (I == Block->begin())
    return nullptr;
  return wrap(--I);
}

LLVM37ValueRef LLVM37GetNextInstruction(LLVM37ValueRef Inst) {
  Instruction *Instr = unwrap<Instruction>(Inst);
  BasicBlock::iterator I = Instr;
  if (++I == Instr->getParent()->end())
    return nullptr;
  return wrap(I);
}

LLVM37ValueRef LLVM37GetPreviousInstruction(LLVM37ValueRef Inst) {
  Instruction *Instr = unwrap<Instruction>(Inst);
  BasicBlock::iterator I = Instr;
  if (I == Instr->getParent()->begin())
    return nullptr;
  return wrap(--I);
}

void LLVM37InstructionEraseFromParent(LLVM37ValueRef Inst) {
  unwrap<Instruction>(Inst)->eraseFromParent();
}

LLVM37IntPredicate LLVM37GetICmpPredicate(LLVM37ValueRef Inst) {
  if (ICmpInst *I = dyn_cast<ICmpInst>(unwrap(Inst)))
    return (LLVM37IntPredicate)I->getPredicate();
  if (ConstantExpr *CE = dyn_cast<ConstantExpr>(unwrap(Inst)))
    if (CE->getOpcode() == Instruction::ICmp)
      return (LLVM37IntPredicate)CE->getPredicate();
  return (LLVM37IntPredicate)0;
}

LLVM37RealPredicate LLVM37GetFCmpPredicate(LLVM37ValueRef Inst) {
  if (FCmpInst *I = dyn_cast<FCmpInst>(unwrap(Inst)))
    return (LLVM37RealPredicate)I->getPredicate();
  if (ConstantExpr *CE = dyn_cast<ConstantExpr>(unwrap(Inst)))
    if (CE->getOpcode() == Instruction::FCmp)
      return (LLVM37RealPredicate)CE->getPredicate();
  return (LLVM37RealPredicate)0;
}

LLVM37Opcode LLVM37GetInstructionOpcode(LLVM37ValueRef Inst) {
  if (Instruction *C = dyn_cast<Instruction>(unwrap(Inst)))
    return map_to_llvm37opcode(C->getOpcode());
  return (LLVM37Opcode)0;
}

LLVM37ValueRef LLVM37InstructionClone(LLVM37ValueRef Inst) {
  if (Instruction *C = dyn_cast<Instruction>(unwrap(Inst)))
    return wrap(C->clone());
  return nullptr;
}

/*--.. Call and invoke instructions ........................................--*/

unsigned LLVM37GetInstructionCallConv(LLVM37ValueRef Instr) {
  Value *V = unwrap(Instr);
  if (CallInst *CI = dyn_cast<CallInst>(V))
    return CI->getCallingConv();
  if (InvokeInst *II = dyn_cast<InvokeInst>(V))
    return II->getCallingConv();
  llvm37_unreachable("LLVM37GetInstructionCallConv applies only to call and invoke!");
}

void LLVM37SetInstructionCallConv(LLVM37ValueRef Instr, unsigned CC) {
  Value *V = unwrap(Instr);
  if (CallInst *CI = dyn_cast<CallInst>(V))
    return CI->setCallingConv(static_cast<CallingConv::ID>(CC));
  else if (InvokeInst *II = dyn_cast<InvokeInst>(V))
    return II->setCallingConv(static_cast<CallingConv::ID>(CC));
  llvm37_unreachable("LLVM37SetInstructionCallConv applies only to call and invoke!");
}

void LLVM37AddInstrAttribute(LLVM37ValueRef Instr, unsigned index,
                           LLVM37Attribute PA) {
  CallSite Call = CallSite(unwrap<Instruction>(Instr));
  AttrBuilder B(PA);
  Call.setAttributes(
    Call.getAttributes().addAttributes(Call->getContext(), index,
                                       AttributeSet::get(Call->getContext(),
                                                         index, B)));
}

void LLVM37RemoveInstrAttribute(LLVM37ValueRef Instr, unsigned index,
                              LLVM37Attribute PA) {
  CallSite Call = CallSite(unwrap<Instruction>(Instr));
  AttrBuilder B(PA);
  Call.setAttributes(Call.getAttributes()
                       .removeAttributes(Call->getContext(), index,
                                         AttributeSet::get(Call->getContext(),
                                                           index, B)));
}

void LLVM37SetInstrParamAlignment(LLVM37ValueRef Instr, unsigned index,
                                unsigned align) {
  CallSite Call = CallSite(unwrap<Instruction>(Instr));
  AttrBuilder B;
  B.addAlignmentAttr(align);
  Call.setAttributes(Call.getAttributes()
                       .addAttributes(Call->getContext(), index,
                                      AttributeSet::get(Call->getContext(),
                                                        index, B)));
}

/*--.. Operations on call instructions (only) ..............................--*/

LLVM37Bool LLVM37IsTailCall(LLVM37ValueRef Call) {
  return unwrap<CallInst>(Call)->isTailCall();
}

void LLVM37SetTailCall(LLVM37ValueRef Call, LLVM37Bool isTailCall) {
  unwrap<CallInst>(Call)->setTailCall(isTailCall);
}

/*--.. Operations on terminators ...........................................--*/

unsigned LLVM37GetNumSuccessors(LLVM37ValueRef Term) {
  return unwrap<TerminatorInst>(Term)->getNumSuccessors();
}

LLVM37BasicBlockRef LLVM37GetSuccessor(LLVM37ValueRef Term, unsigned i) {
  return wrap(unwrap<TerminatorInst>(Term)->getSuccessor(i));
}

void LLVM37SetSuccessor(LLVM37ValueRef Term, unsigned i, LLVM37BasicBlockRef block) {
  return unwrap<TerminatorInst>(Term)->setSuccessor(i,unwrap(block));
}

/*--.. Operations on branch instructions (only) ............................--*/

LLVM37Bool LLVM37IsConditional(LLVM37ValueRef Branch) {
  return unwrap<BranchInst>(Branch)->isConditional();
}

LLVM37ValueRef LLVM37GetCondition(LLVM37ValueRef Branch) {
  return wrap(unwrap<BranchInst>(Branch)->getCondition());
}

void LLVM37SetCondition(LLVM37ValueRef Branch, LLVM37ValueRef Cond) {
  return unwrap<BranchInst>(Branch)->setCondition(unwrap(Cond));
}

/*--.. Operations on switch instructions (only) ............................--*/

LLVM37BasicBlockRef LLVM37GetSwitchDefaultDest(LLVM37ValueRef Switch) {
  return wrap(unwrap<SwitchInst>(Switch)->getDefaultDest());
}

/*--.. Operations on phi nodes .............................................--*/

void LLVM37AddIncoming(LLVM37ValueRef PhiNode, LLVM37ValueRef *IncomingValues,
                     LLVM37BasicBlockRef *IncomingBlocks, unsigned Count) {
  PHINode *PhiVal = unwrap<PHINode>(PhiNode);
  for (unsigned I = 0; I != Count; ++I)
    PhiVal->addIncoming(unwrap(IncomingValues[I]), unwrap(IncomingBlocks[I]));
}

unsigned LLVM37CountIncoming(LLVM37ValueRef PhiNode) {
  return unwrap<PHINode>(PhiNode)->getNumIncomingValues();
}

LLVM37ValueRef LLVM37GetIncomingValue(LLVM37ValueRef PhiNode, unsigned Index) {
  return wrap(unwrap<PHINode>(PhiNode)->getIncomingValue(Index));
}

LLVM37BasicBlockRef LLVM37GetIncomingBlock(LLVM37ValueRef PhiNode, unsigned Index) {
  return wrap(unwrap<PHINode>(PhiNode)->getIncomingBlock(Index));
}


/*===-- Instruction builders ----------------------------------------------===*/

LLVMBuilderRef LLVM37CreateBuilderInContext(LLVM37ContextRef C) {
  return wrap(new IRBuilder<>(*unwrap(C)));
}

LLVMBuilderRef LLVM37CreateBuilder(void) {
  return LLVM37CreateBuilderInContext(LLVM37GetGlobalContext());
}

void LLVM37PositionBuilder(LLVMBuilderRef Builder, LLVM37BasicBlockRef Block,
                         LLVM37ValueRef Instr) {
  BasicBlock *BB = unwrap(Block);
  Instruction *I = Instr? unwrap<Instruction>(Instr) : (Instruction*) BB->end();
  unwrap(Builder)->SetInsertPoint(BB, I);
}

void LLVM37PositionBuilderBefore(LLVMBuilderRef Builder, LLVM37ValueRef Instr) {
  Instruction *I = unwrap<Instruction>(Instr);
  unwrap(Builder)->SetInsertPoint(I->getParent(), I);
}

void LLVM37PositionBuilderAtEnd(LLVMBuilderRef Builder, LLVM37BasicBlockRef Block) {
  BasicBlock *BB = unwrap(Block);
  unwrap(Builder)->SetInsertPoint(BB);
}

LLVM37BasicBlockRef LLVM37GetInsertBlock(LLVMBuilderRef Builder) {
   return wrap(unwrap(Builder)->GetInsertBlock());
}

void LLVM37ClearInsertionPosition(LLVMBuilderRef Builder) {
  unwrap(Builder)->ClearInsertionPoint();
}

void LLVM37InsertIntoBuilder(LLVMBuilderRef Builder, LLVM37ValueRef Instr) {
  unwrap(Builder)->Insert(unwrap<Instruction>(Instr));
}

void LLVM37InsertIntoBuilderWithName(LLVMBuilderRef Builder, LLVM37ValueRef Instr,
                                   const char *Name) {
  unwrap(Builder)->Insert(unwrap<Instruction>(Instr), Name);
}

void LLVM37DisposeBuilder(LLVMBuilderRef Builder) {
  delete unwrap(Builder);
}

/*--.. Metadata builders ...................................................--*/

void LLVM37SetCurrentDebugLocation(LLVMBuilderRef Builder, LLVM37ValueRef L) {
  MDNode *Loc =
      L ? cast<MDNode>(unwrap<MetadataAsValue>(L)->getMetadata()) : nullptr;
  unwrap(Builder)->SetCurrentDebugLocation(DebugLoc(Loc));
}

LLVM37ValueRef LLVM37GetCurrentDebugLocation(LLVMBuilderRef Builder) {
  LLVM37Context &Context = unwrap(Builder)->getContext();
  return wrap(MetadataAsValue::get(
      Context, unwrap(Builder)->getCurrentDebugLocation().getAsMDNode()));
}

void LLVM37SetInstDebugLocation(LLVMBuilderRef Builder, LLVM37ValueRef Inst) {
  unwrap(Builder)->SetInstDebugLocation(unwrap<Instruction>(Inst));
}


/*--.. Instruction builders ................................................--*/

LLVM37ValueRef LLVMBuildRetVoid(LLVMBuilderRef B) {
  return wrap(unwrap(B)->CreateRetVoid());
}

LLVM37ValueRef LLVMBuildRet(LLVMBuilderRef B, LLVM37ValueRef V) {
  return wrap(unwrap(B)->CreateRet(unwrap(V)));
}

LLVM37ValueRef LLVMBuildAggregateRet(LLVMBuilderRef B, LLVM37ValueRef *RetVals,
                                   unsigned N) {
  return wrap(unwrap(B)->CreateAggregateRet(unwrap(RetVals), N));
}

LLVM37ValueRef LLVMBuildBr(LLVMBuilderRef B, LLVM37BasicBlockRef Dest) {
  return wrap(unwrap(B)->CreateBr(unwrap(Dest)));
}

LLVM37ValueRef LLVMBuildCondBr(LLVMBuilderRef B, LLVM37ValueRef If,
                             LLVM37BasicBlockRef Then, LLVM37BasicBlockRef Else) {
  return wrap(unwrap(B)->CreateCondBr(unwrap(If), unwrap(Then), unwrap(Else)));
}

LLVM37ValueRef LLVMBuildSwitch(LLVMBuilderRef B, LLVM37ValueRef V,
                             LLVM37BasicBlockRef Else, unsigned NumCases) {
  return wrap(unwrap(B)->CreateSwitch(unwrap(V), unwrap(Else), NumCases));
}

LLVM37ValueRef LLVMBuildIndirectBr(LLVMBuilderRef B, LLVM37ValueRef Addr,
                                 unsigned NumDests) {
  return wrap(unwrap(B)->CreateIndirectBr(unwrap(Addr), NumDests));
}

LLVM37ValueRef LLVMBuildInvoke(LLVMBuilderRef B, LLVM37ValueRef Fn,
                             LLVM37ValueRef *Args, unsigned NumArgs,
                             LLVM37BasicBlockRef Then, LLVM37BasicBlockRef Catch,
                             const char *Name) {
  return wrap(unwrap(B)->CreateInvoke(unwrap(Fn), unwrap(Then), unwrap(Catch),
                                      makeArrayRef(unwrap(Args), NumArgs),
                                      Name));
}

LLVM37ValueRef LLVMBuildLandingPad(LLVMBuilderRef B, LLVM37TypeRef Ty,
                                 unsigned NumClauses, const char *Name) {
  return wrap(unwrap(B)->CreateLandingPad(unwrap(Ty), NumClauses, Name));
}

LLVM37ValueRef LLVMBuildResume(LLVMBuilderRef B, LLVM37ValueRef Exn) {
  return wrap(unwrap(B)->CreateResume(unwrap(Exn)));
}

LLVM37ValueRef LLVMBuildUnreachable(LLVMBuilderRef B) {
  return wrap(unwrap(B)->CreateUnreachable());
}

void LLVM37AddCase(LLVM37ValueRef Switch, LLVM37ValueRef OnVal,
                 LLVM37BasicBlockRef Dest) {
  unwrap<SwitchInst>(Switch)->addCase(unwrap<ConstantInt>(OnVal), unwrap(Dest));
}

void LLVM37AddDestination(LLVM37ValueRef IndirectBr, LLVM37BasicBlockRef Dest) {
  unwrap<IndirectBrInst>(IndirectBr)->addDestination(unwrap(Dest));
}

void LLVM37AddClause(LLVM37ValueRef LandingPad, LLVM37ValueRef ClauseVal) {
  unwrap<LandingPadInst>(LandingPad)->
    addClause(cast<Constant>(unwrap(ClauseVal)));
}

void LLVM37SetCleanup(LLVM37ValueRef LandingPad, LLVM37Bool Val) {
  unwrap<LandingPadInst>(LandingPad)->setCleanup(Val);
}

/*--.. Arithmetic ..........................................................--*/

LLVM37ValueRef LLVMBuildAdd(LLVMBuilderRef B, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                          const char *Name) {
  return wrap(unwrap(B)->CreateAdd(unwrap(LHS), unwrap(RHS), Name));
}

LLVM37ValueRef LLVMBuildNSWAdd(LLVMBuilderRef B, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                          const char *Name) {
  return wrap(unwrap(B)->CreateNSWAdd(unwrap(LHS), unwrap(RHS), Name));
}

LLVM37ValueRef LLVMBuildNUWAdd(LLVMBuilderRef B, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                          const char *Name) {
  return wrap(unwrap(B)->CreateNUWAdd(unwrap(LHS), unwrap(RHS), Name));
}

LLVM37ValueRef LLVMBuildFAdd(LLVMBuilderRef B, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                          const char *Name) {
  return wrap(unwrap(B)->CreateFAdd(unwrap(LHS), unwrap(RHS), Name));
}

LLVM37ValueRef LLVMBuildSub(LLVMBuilderRef B, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                          const char *Name) {
  return wrap(unwrap(B)->CreateSub(unwrap(LHS), unwrap(RHS), Name));
}

LLVM37ValueRef LLVMBuildNSWSub(LLVMBuilderRef B, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                          const char *Name) {
  return wrap(unwrap(B)->CreateNSWSub(unwrap(LHS), unwrap(RHS), Name));
}

LLVM37ValueRef LLVMBuildNUWSub(LLVMBuilderRef B, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                          const char *Name) {
  return wrap(unwrap(B)->CreateNUWSub(unwrap(LHS), unwrap(RHS), Name));
}

LLVM37ValueRef LLVMBuildFSub(LLVMBuilderRef B, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                          const char *Name) {
  return wrap(unwrap(B)->CreateFSub(unwrap(LHS), unwrap(RHS), Name));
}

LLVM37ValueRef LLVMBuildMul(LLVMBuilderRef B, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                          const char *Name) {
  return wrap(unwrap(B)->CreateMul(unwrap(LHS), unwrap(RHS), Name));
}

LLVM37ValueRef LLVMBuildNSWMul(LLVMBuilderRef B, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                          const char *Name) {
  return wrap(unwrap(B)->CreateNSWMul(unwrap(LHS), unwrap(RHS), Name));
}

LLVM37ValueRef LLVMBuildNUWMul(LLVMBuilderRef B, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                          const char *Name) {
  return wrap(unwrap(B)->CreateNUWMul(unwrap(LHS), unwrap(RHS), Name));
}

LLVM37ValueRef LLVMBuildFMul(LLVMBuilderRef B, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                          const char *Name) {
  return wrap(unwrap(B)->CreateFMul(unwrap(LHS), unwrap(RHS), Name));
}

LLVM37ValueRef LLVMBuildUDiv(LLVMBuilderRef B, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                           const char *Name) {
  return wrap(unwrap(B)->CreateUDiv(unwrap(LHS), unwrap(RHS), Name));
}

LLVM37ValueRef LLVMBuildSDiv(LLVMBuilderRef B, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                           const char *Name) {
  return wrap(unwrap(B)->CreateSDiv(unwrap(LHS), unwrap(RHS), Name));
}

LLVM37ValueRef LLVMBuildExactSDiv(LLVMBuilderRef B, LLVM37ValueRef LHS,
                                LLVM37ValueRef RHS, const char *Name) {
  return wrap(unwrap(B)->CreateExactSDiv(unwrap(LHS), unwrap(RHS), Name));
}

LLVM37ValueRef LLVMBuildFDiv(LLVMBuilderRef B, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                           const char *Name) {
  return wrap(unwrap(B)->CreateFDiv(unwrap(LHS), unwrap(RHS), Name));
}

LLVM37ValueRef LLVMBuildURem(LLVMBuilderRef B, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                           const char *Name) {
  return wrap(unwrap(B)->CreateURem(unwrap(LHS), unwrap(RHS), Name));
}

LLVM37ValueRef LLVMBuildSRem(LLVMBuilderRef B, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                           const char *Name) {
  return wrap(unwrap(B)->CreateSRem(unwrap(LHS), unwrap(RHS), Name));
}

LLVM37ValueRef LLVMBuildFRem(LLVMBuilderRef B, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                           const char *Name) {
  return wrap(unwrap(B)->CreateFRem(unwrap(LHS), unwrap(RHS), Name));
}

LLVM37ValueRef LLVMBuildShl(LLVMBuilderRef B, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                          const char *Name) {
  return wrap(unwrap(B)->CreateShl(unwrap(LHS), unwrap(RHS), Name));
}

LLVM37ValueRef LLVMBuildLShr(LLVMBuilderRef B, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                           const char *Name) {
  return wrap(unwrap(B)->CreateLShr(unwrap(LHS), unwrap(RHS), Name));
}

LLVM37ValueRef LLVMBuildAShr(LLVMBuilderRef B, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                           const char *Name) {
  return wrap(unwrap(B)->CreateAShr(unwrap(LHS), unwrap(RHS), Name));
}

LLVM37ValueRef LLVMBuildAnd(LLVMBuilderRef B, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                          const char *Name) {
  return wrap(unwrap(B)->CreateAnd(unwrap(LHS), unwrap(RHS), Name));
}

LLVM37ValueRef LLVMBuildOr(LLVMBuilderRef B, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                         const char *Name) {
  return wrap(unwrap(B)->CreateOr(unwrap(LHS), unwrap(RHS), Name));
}

LLVM37ValueRef LLVMBuildXor(LLVMBuilderRef B, LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                          const char *Name) {
  return wrap(unwrap(B)->CreateXor(unwrap(LHS), unwrap(RHS), Name));
}

LLVM37ValueRef LLVMBuildBinOp(LLVMBuilderRef B, LLVM37Opcode Op,
                            LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                            const char *Name) {
  return wrap(unwrap(B)->CreateBinOp(Instruction::BinaryOps(map_from_llvm37opcode(Op)), unwrap(LHS),
                                     unwrap(RHS), Name));
}

LLVM37ValueRef LLVMBuildNeg(LLVMBuilderRef B, LLVM37ValueRef V, const char *Name) {
  return wrap(unwrap(B)->CreateNeg(unwrap(V), Name));
}

LLVM37ValueRef LLVMBuildNSWNeg(LLVMBuilderRef B, LLVM37ValueRef V,
                             const char *Name) {
  return wrap(unwrap(B)->CreateNSWNeg(unwrap(V), Name));
}

LLVM37ValueRef LLVMBuildNUWNeg(LLVMBuilderRef B, LLVM37ValueRef V,
                             const char *Name) {
  return wrap(unwrap(B)->CreateNUWNeg(unwrap(V), Name));
}

LLVM37ValueRef LLVMBuildFNeg(LLVMBuilderRef B, LLVM37ValueRef V, const char *Name) {
  return wrap(unwrap(B)->CreateFNeg(unwrap(V), Name));
}

LLVM37ValueRef LLVMBuildNot(LLVMBuilderRef B, LLVM37ValueRef V, const char *Name) {
  return wrap(unwrap(B)->CreateNot(unwrap(V), Name));
}

/*--.. Memory ..............................................................--*/

LLVM37ValueRef LLVMBuildMalloc(LLVMBuilderRef B, LLVM37TypeRef Ty,
                             const char *Name) {
  Type* ITy = Type::getInt32Ty(unwrap(B)->GetInsertBlock()->getContext());
  Constant* AllocSize = ConstantExpr::getSizeOf(unwrap(Ty));
  AllocSize = ConstantExpr::getTruncOrBitCast(AllocSize, ITy);
  Instruction* Malloc = CallInst::CreateMalloc(unwrap(B)->GetInsertBlock(),
                                               ITy, unwrap(Ty), AllocSize,
                                               nullptr, nullptr, "");
  return wrap(unwrap(B)->Insert(Malloc, Twine(Name)));
}

LLVM37ValueRef LLVMBuildArrayMalloc(LLVMBuilderRef B, LLVM37TypeRef Ty,
                                  LLVM37ValueRef Val, const char *Name) {
  Type* ITy = Type::getInt32Ty(unwrap(B)->GetInsertBlock()->getContext());
  Constant* AllocSize = ConstantExpr::getSizeOf(unwrap(Ty));
  AllocSize = ConstantExpr::getTruncOrBitCast(AllocSize, ITy);
  Instruction* Malloc = CallInst::CreateMalloc(unwrap(B)->GetInsertBlock(),
                                               ITy, unwrap(Ty), AllocSize,
                                               unwrap(Val), nullptr, "");
  return wrap(unwrap(B)->Insert(Malloc, Twine(Name)));
}

LLVM37ValueRef LLVMBuildAlloca(LLVMBuilderRef B, LLVM37TypeRef Ty,
                             const char *Name) {
  return wrap(unwrap(B)->CreateAlloca(unwrap(Ty), nullptr, Name));
}

LLVM37ValueRef LLVMBuildArrayAlloca(LLVMBuilderRef B, LLVM37TypeRef Ty,
                                  LLVM37ValueRef Val, const char *Name) {
  return wrap(unwrap(B)->CreateAlloca(unwrap(Ty), unwrap(Val), Name));
}

LLVM37ValueRef LLVMBuildFree(LLVMBuilderRef B, LLVM37ValueRef PointerVal) {
  return wrap(unwrap(B)->Insert(
     CallInst::CreateFree(unwrap(PointerVal), unwrap(B)->GetInsertBlock())));
}


LLVM37ValueRef LLVMBuildLoad(LLVMBuilderRef B, LLVM37ValueRef PointerVal,
                           const char *Name) {
  return wrap(unwrap(B)->CreateLoad(unwrap(PointerVal), Name));
}

LLVM37ValueRef LLVMBuildStore(LLVMBuilderRef B, LLVM37ValueRef Val,
                            LLVM37ValueRef PointerVal) {
  return wrap(unwrap(B)->CreateStore(unwrap(Val), unwrap(PointerVal)));
}

static AtomicOrdering mapFromLLVM37Ordering(LLVM37AtomicOrdering Ordering) {
  switch (Ordering) {
    case LLVM37AtomicOrderingNotAtomic: return NotAtomic;
    case LLVM37AtomicOrderingUnordered: return Unordered;
    case LLVM37AtomicOrderingMonotonic: return Monotonic;
    case LLVM37AtomicOrderingAcquire: return Acquire;
    case LLVM37AtomicOrderingRelease: return Release;
    case LLVM37AtomicOrderingAcquireRelease: return AcquireRelease;
    case LLVM37AtomicOrderingSequentiallyConsistent:
      return SequentiallyConsistent;
  }

  llvm37_unreachable("Invalid LLVM37AtomicOrdering value!");
}

LLVM37ValueRef LLVMBuildFence(LLVMBuilderRef B, LLVM37AtomicOrdering Ordering,
                            LLVM37Bool isSingleThread, const char *Name) {
  return wrap(
    unwrap(B)->CreateFence(mapFromLLVM37Ordering(Ordering),
                           isSingleThread ? SingleThread : CrossThread,
                           Name));
}

LLVM37ValueRef LLVMBuildGEP(LLVMBuilderRef B, LLVM37ValueRef Pointer,
                          LLVM37ValueRef *Indices, unsigned NumIndices,
                          const char *Name) {
  ArrayRef<Value *> IdxList(unwrap(Indices), NumIndices);
  return wrap(unwrap(B)->CreateGEP(nullptr, unwrap(Pointer), IdxList, Name));
}

LLVM37ValueRef LLVMBuildInBoundsGEP(LLVMBuilderRef B, LLVM37ValueRef Pointer,
                                  LLVM37ValueRef *Indices, unsigned NumIndices,
                                  const char *Name) {
  ArrayRef<Value *> IdxList(unwrap(Indices), NumIndices);
  return wrap(
      unwrap(B)->CreateInBoundsGEP(nullptr, unwrap(Pointer), IdxList, Name));
}

LLVM37ValueRef LLVMBuildStructGEP(LLVMBuilderRef B, LLVM37ValueRef Pointer,
                                unsigned Idx, const char *Name) {
  return wrap(unwrap(B)->CreateStructGEP(nullptr, unwrap(Pointer), Idx, Name));
}

LLVM37ValueRef LLVMBuildGlobalString(LLVMBuilderRef B, const char *Str,
                                   const char *Name) {
  return wrap(unwrap(B)->CreateGlobalString(Str, Name));
}

LLVM37ValueRef LLVMBuildGlobalStringPtr(LLVMBuilderRef B, const char *Str,
                                      const char *Name) {
  return wrap(unwrap(B)->CreateGlobalStringPtr(Str, Name));
}

LLVM37Bool LLVM37GetVolatile(LLVM37ValueRef MemAccessInst) {
  Value *P = unwrap<Value>(MemAccessInst);
  if (LoadInst *LI = dyn_cast<LoadInst>(P))
    return LI->isVolatile();
  return cast<StoreInst>(P)->isVolatile();
}

void LLVM37SetVolatile(LLVM37ValueRef MemAccessInst, LLVM37Bool isVolatile) {
  Value *P = unwrap<Value>(MemAccessInst);
  if (LoadInst *LI = dyn_cast<LoadInst>(P))
    return LI->setVolatile(isVolatile);
  return cast<StoreInst>(P)->setVolatile(isVolatile);
}

/*--.. Casts ...............................................................--*/

LLVM37ValueRef LLVMBuildTrunc(LLVMBuilderRef B, LLVM37ValueRef Val,
                            LLVM37TypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreateTrunc(unwrap(Val), unwrap(DestTy), Name));
}

LLVM37ValueRef LLVMBuildZExt(LLVMBuilderRef B, LLVM37ValueRef Val,
                           LLVM37TypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreateZExt(unwrap(Val), unwrap(DestTy), Name));
}

LLVM37ValueRef LLVMBuildSExt(LLVMBuilderRef B, LLVM37ValueRef Val,
                           LLVM37TypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreateSExt(unwrap(Val), unwrap(DestTy), Name));
}

LLVM37ValueRef LLVMBuildFPToUI(LLVMBuilderRef B, LLVM37ValueRef Val,
                             LLVM37TypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreateFPToUI(unwrap(Val), unwrap(DestTy), Name));
}

LLVM37ValueRef LLVMBuildFPToSI(LLVMBuilderRef B, LLVM37ValueRef Val,
                             LLVM37TypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreateFPToSI(unwrap(Val), unwrap(DestTy), Name));
}

LLVM37ValueRef LLVMBuildUIToFP(LLVMBuilderRef B, LLVM37ValueRef Val,
                             LLVM37TypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreateUIToFP(unwrap(Val), unwrap(DestTy), Name));
}

LLVM37ValueRef LLVMBuildSIToFP(LLVMBuilderRef B, LLVM37ValueRef Val,
                             LLVM37TypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreateSIToFP(unwrap(Val), unwrap(DestTy), Name));
}

LLVM37ValueRef LLVMBuildFPTrunc(LLVMBuilderRef B, LLVM37ValueRef Val,
                              LLVM37TypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreateFPTrunc(unwrap(Val), unwrap(DestTy), Name));
}

LLVM37ValueRef LLVMBuildFPExt(LLVMBuilderRef B, LLVM37ValueRef Val,
                            LLVM37TypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreateFPExt(unwrap(Val), unwrap(DestTy), Name));
}

LLVM37ValueRef LLVMBuildPtrToInt(LLVMBuilderRef B, LLVM37ValueRef Val,
                               LLVM37TypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreatePtrToInt(unwrap(Val), unwrap(DestTy), Name));
}

LLVM37ValueRef LLVMBuildIntToPtr(LLVMBuilderRef B, LLVM37ValueRef Val,
                               LLVM37TypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreateIntToPtr(unwrap(Val), unwrap(DestTy), Name));
}

LLVM37ValueRef LLVMBuildBitCast(LLVMBuilderRef B, LLVM37ValueRef Val,
                              LLVM37TypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreateBitCast(unwrap(Val), unwrap(DestTy), Name));
}

LLVM37ValueRef LLVMBuildAddrSpaceCast(LLVMBuilderRef B, LLVM37ValueRef Val,
                                    LLVM37TypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreateAddrSpaceCast(unwrap(Val), unwrap(DestTy), Name));
}

LLVM37ValueRef LLVMBuildZExtOrBitCast(LLVMBuilderRef B, LLVM37ValueRef Val,
                                    LLVM37TypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreateZExtOrBitCast(unwrap(Val), unwrap(DestTy),
                                             Name));
}

LLVM37ValueRef LLVMBuildSExtOrBitCast(LLVMBuilderRef B, LLVM37ValueRef Val,
                                    LLVM37TypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreateSExtOrBitCast(unwrap(Val), unwrap(DestTy),
                                             Name));
}

LLVM37ValueRef LLVMBuildTruncOrBitCast(LLVMBuilderRef B, LLVM37ValueRef Val,
                                     LLVM37TypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreateTruncOrBitCast(unwrap(Val), unwrap(DestTy),
                                              Name));
}

LLVM37ValueRef LLVMBuildCast(LLVMBuilderRef B, LLVM37Opcode Op, LLVM37ValueRef Val,
                           LLVM37TypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreateCast(Instruction::CastOps(map_from_llvm37opcode(Op)), unwrap(Val),
                                    unwrap(DestTy), Name));
}

LLVM37ValueRef LLVMBuildPointerCast(LLVMBuilderRef B, LLVM37ValueRef Val,
                                  LLVM37TypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreatePointerCast(unwrap(Val), unwrap(DestTy), Name));
}

LLVM37ValueRef LLVMBuildIntCast(LLVMBuilderRef B, LLVM37ValueRef Val,
                              LLVM37TypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreateIntCast(unwrap(Val), unwrap(DestTy),
                                       /*isSigned*/true, Name));
}

LLVM37ValueRef LLVMBuildFPCast(LLVMBuilderRef B, LLVM37ValueRef Val,
                             LLVM37TypeRef DestTy, const char *Name) {
  return wrap(unwrap(B)->CreateFPCast(unwrap(Val), unwrap(DestTy), Name));
}

/*--.. Comparisons .........................................................--*/

LLVM37ValueRef LLVMBuildICmp(LLVMBuilderRef B, LLVM37IntPredicate Op,
                           LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                           const char *Name) {
  return wrap(unwrap(B)->CreateICmp(static_cast<ICmpInst::Predicate>(Op),
                                    unwrap(LHS), unwrap(RHS), Name));
}

LLVM37ValueRef LLVMBuildFCmp(LLVMBuilderRef B, LLVM37RealPredicate Op,
                           LLVM37ValueRef LHS, LLVM37ValueRef RHS,
                           const char *Name) {
  return wrap(unwrap(B)->CreateFCmp(static_cast<FCmpInst::Predicate>(Op),
                                    unwrap(LHS), unwrap(RHS), Name));
}

/*--.. Miscellaneous instructions ..........................................--*/

LLVM37ValueRef LLVMBuildPhi(LLVMBuilderRef B, LLVM37TypeRef Ty, const char *Name) {
  return wrap(unwrap(B)->CreatePHI(unwrap(Ty), 0, Name));
}

LLVM37ValueRef LLVMBuildCall(LLVMBuilderRef B, LLVM37ValueRef Fn,
                           LLVM37ValueRef *Args, unsigned NumArgs,
                           const char *Name) {
  return wrap(unwrap(B)->CreateCall(unwrap(Fn),
                                    makeArrayRef(unwrap(Args), NumArgs),
                                    Name));
}

LLVM37ValueRef LLVMBuildSelect(LLVMBuilderRef B, LLVM37ValueRef If,
                             LLVM37ValueRef Then, LLVM37ValueRef Else,
                             const char *Name) {
  return wrap(unwrap(B)->CreateSelect(unwrap(If), unwrap(Then), unwrap(Else),
                                      Name));
}

LLVM37ValueRef LLVMBuildVAArg(LLVMBuilderRef B, LLVM37ValueRef List,
                            LLVM37TypeRef Ty, const char *Name) {
  return wrap(unwrap(B)->CreateVAArg(unwrap(List), unwrap(Ty), Name));
}

LLVM37ValueRef LLVMBuildExtractElement(LLVMBuilderRef B, LLVM37ValueRef VecVal,
                                      LLVM37ValueRef Index, const char *Name) {
  return wrap(unwrap(B)->CreateExtractElement(unwrap(VecVal), unwrap(Index),
                                              Name));
}

LLVM37ValueRef LLVMBuildInsertElement(LLVMBuilderRef B, LLVM37ValueRef VecVal,
                                    LLVM37ValueRef EltVal, LLVM37ValueRef Index,
                                    const char *Name) {
  return wrap(unwrap(B)->CreateInsertElement(unwrap(VecVal), unwrap(EltVal),
                                             unwrap(Index), Name));
}

LLVM37ValueRef LLVMBuildShuffleVector(LLVMBuilderRef B, LLVM37ValueRef V1,
                                    LLVM37ValueRef V2, LLVM37ValueRef Mask,
                                    const char *Name) {
  return wrap(unwrap(B)->CreateShuffleVector(unwrap(V1), unwrap(V2),
                                             unwrap(Mask), Name));
}

LLVM37ValueRef LLVMBuildExtractValue(LLVMBuilderRef B, LLVM37ValueRef AggVal,
                                   unsigned Index, const char *Name) {
  return wrap(unwrap(B)->CreateExtractValue(unwrap(AggVal), Index, Name));
}

LLVM37ValueRef LLVMBuildInsertValue(LLVMBuilderRef B, LLVM37ValueRef AggVal,
                                  LLVM37ValueRef EltVal, unsigned Index,
                                  const char *Name) {
  return wrap(unwrap(B)->CreateInsertValue(unwrap(AggVal), unwrap(EltVal),
                                           Index, Name));
}

LLVM37ValueRef LLVMBuildIsNull(LLVMBuilderRef B, LLVM37ValueRef Val,
                             const char *Name) {
  return wrap(unwrap(B)->CreateIsNull(unwrap(Val), Name));
}

LLVM37ValueRef LLVMBuildIsNotNull(LLVMBuilderRef B, LLVM37ValueRef Val,
                                const char *Name) {
  return wrap(unwrap(B)->CreateIsNotNull(unwrap(Val), Name));
}

LLVM37ValueRef LLVMBuildPtrDiff(LLVMBuilderRef B, LLVM37ValueRef LHS,
                              LLVM37ValueRef RHS, const char *Name) {
  return wrap(unwrap(B)->CreatePtrDiff(unwrap(LHS), unwrap(RHS), Name));
}

LLVM37ValueRef LLVMBuildAtomicRMW(LLVMBuilderRef B,LLVM37AtomicRMWBinOp op,
                               LLVM37ValueRef PTR, LLVM37ValueRef Val,
                               LLVM37AtomicOrdering ordering,
                               LLVM37Bool singleThread) {
  AtomicRMWInst::BinOp intop;
  switch (op) {
    case LLVM37AtomicRMWBinOpXchg: intop = AtomicRMWInst::Xchg; break;
    case LLVM37AtomicRMWBinOpAdd: intop = AtomicRMWInst::Add; break;
    case LLVM37AtomicRMWBinOpSub: intop = AtomicRMWInst::Sub; break;
    case LLVM37AtomicRMWBinOpAnd: intop = AtomicRMWInst::And; break;
    case LLVM37AtomicRMWBinOpNand: intop = AtomicRMWInst::Nand; break;
    case LLVM37AtomicRMWBinOpOr: intop = AtomicRMWInst::Or; break;
    case LLVM37AtomicRMWBinOpXor: intop = AtomicRMWInst::Xor; break;
    case LLVM37AtomicRMWBinOpMax: intop = AtomicRMWInst::Max; break;
    case LLVM37AtomicRMWBinOpMin: intop = AtomicRMWInst::Min; break;
    case LLVM37AtomicRMWBinOpUMax: intop = AtomicRMWInst::UMax; break;
    case LLVM37AtomicRMWBinOpUMin: intop = AtomicRMWInst::UMin; break;
  }
  return wrap(unwrap(B)->CreateAtomicRMW(intop, unwrap(PTR), unwrap(Val),
    mapFromLLVM37Ordering(ordering), singleThread ? SingleThread : CrossThread));
}


/*===-- Module providers --------------------------------------------------===*/

LLVM37ModuleProviderRef
LLVM37CreateModuleProviderForExistingModule(LLVM37ModuleRef M) {
  return reinterpret_cast<LLVM37ModuleProviderRef>(M);
}

void LLVM37DisposeModuleProvider(LLVM37ModuleProviderRef MP) {
  delete unwrap(MP);
}


/*===-- Memory buffers ----------------------------------------------------===*/

LLVM37Bool LLVM37CreateMemoryBufferWithContentsOfFile(
    const char *Path,
    LLVM37MemoryBufferRef *OutMemBuf,
    char **OutMessage) {

  ErrorOr<std::unique_ptr<MemoryBuffer>> MBOrErr = MemoryBuffer::getFile(Path);
  if (std::error_code EC = MBOrErr.getError()) {
    *OutMessage = strdup(EC.message().c_str());
    return 1;
  }
  *OutMemBuf = wrap(MBOrErr.get().release());
  return 0;
}

LLVM37Bool LLVM37CreateMemoryBufferWithSTDIN(LLVM37MemoryBufferRef *OutMemBuf,
                                         char **OutMessage) {
  ErrorOr<std::unique_ptr<MemoryBuffer>> MBOrErr = MemoryBuffer::getSTDIN();
  if (std::error_code EC = MBOrErr.getError()) {
    *OutMessage = strdup(EC.message().c_str());
    return 1;
  }
  *OutMemBuf = wrap(MBOrErr.get().release());
  return 0;
}

LLVM37MemoryBufferRef LLVM37CreateMemoryBufferWithMemoryRange(
    const char *InputData,
    size_t InputDataLength,
    const char *BufferName,
    LLVM37Bool RequiresNullTerminator) {

  return wrap(MemoryBuffer::getMemBuffer(StringRef(InputData, InputDataLength),
                                         StringRef(BufferName),
                                         RequiresNullTerminator).release());
}

LLVM37MemoryBufferRef LLVM37CreateMemoryBufferWithMemoryRangeCopy(
    const char *InputData,
    size_t InputDataLength,
    const char *BufferName) {

  return wrap(
      MemoryBuffer::getMemBufferCopy(StringRef(InputData, InputDataLength),
                                     StringRef(BufferName)).release());
}

const char *LLVM37GetBufferStart(LLVM37MemoryBufferRef MemBuf) {
  return unwrap(MemBuf)->getBufferStart();
}

size_t LLVM37GetBufferSize(LLVM37MemoryBufferRef MemBuf) {
  return unwrap(MemBuf)->getBufferSize();
}

void LLVM37DisposeMemoryBuffer(LLVM37MemoryBufferRef MemBuf) {
  delete unwrap(MemBuf);
}

/*===-- Pass Registry -----------------------------------------------------===*/

LLVM37PassRegistryRef LLVM37GetGlobalPassRegistry(void) {
  return wrap(PassRegistry::getPassRegistry());
}

/*===-- Pass Manager ------------------------------------------------------===*/

LLVM37PassManagerRef LLVM37CreatePassManager() {
  return wrap(new legacy::PassManager());
}

LLVM37PassManagerRef LLVM37CreateFunctionPassManagerForModule(LLVM37ModuleRef M) {
  return wrap(new legacy::FunctionPassManager(unwrap(M)));
}

LLVM37PassManagerRef LLVM37CreateFunctionPassManager(LLVM37ModuleProviderRef P) {
  return LLVM37CreateFunctionPassManagerForModule(
                                            reinterpret_cast<LLVM37ModuleRef>(P));
}

LLVM37Bool LLVM37RunPassManager(LLVM37PassManagerRef PM, LLVM37ModuleRef M) {
  return unwrap<legacy::PassManager>(PM)->run(*unwrap(M));
}

LLVM37Bool LLVM37InitializeFunctionPassManager(LLVM37PassManagerRef FPM) {
  return unwrap<legacy::FunctionPassManager>(FPM)->doInitialization();
}

LLVM37Bool LLVM37RunFunctionPassManager(LLVM37PassManagerRef FPM, LLVM37ValueRef F) {
  return unwrap<legacy::FunctionPassManager>(FPM)->run(*unwrap<Function>(F));
}

LLVM37Bool LLVM37FinalizeFunctionPassManager(LLVM37PassManagerRef FPM) {
  return unwrap<legacy::FunctionPassManager>(FPM)->doFinalization();
}

void LLVM37DisposePassManager(LLVM37PassManagerRef PM) {
  delete unwrap(PM);
}

/*===-- Threading ------------------------------------------------------===*/

LLVM37Bool LLVM37StartMultithreaded() {
  return LLVM37IsMultithreaded();
}

void LLVM37StopMultithreaded() {
}

LLVM37Bool LLVM37IsMultithreaded() {
  return llvm37_is_multithreaded();
}
