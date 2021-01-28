; RUN: opt < %s -indvars -disable-output
; ModuleID = '2004-04-05-InvokeCastCrash.ll'
	%struct.__false_type = type { i8 }
	%"struct.__gnu_cxx::_Hashtable_node<const llvm37::Constant*>" = type { %"struct.__gnu_cxx::_Hashtable_node<const llvm37::Constant*>"*, %"struct.llvm37::Constant"* }
	%"struct.__gnu_cxx::_Hashtable_node<std::pair<const llvm37::Value* const, int> >" = type { %"struct.__gnu_cxx::_Hashtable_node<std::pair<const llvm37::Value* const, int> >"*, %"struct.std::pair<const llvm37::Value* const,int>" }
	%"struct.__gnu_cxx::hash_map<const llvm37::Value*,int,__gnu_cxx::hash<const llvm37::Value*>,std::equal_to<const llvm37::Value*>,std::allocator<int> >" = type { %"struct.__gnu_cxx::hashtable<std::pair<const llvm37::Value* const, int>,const llvm37::Value*,__gnu_cxx::hash<const llvm37::Value*>,std::_Select1st<std::pair<const llvm37::Value* const, int> >,std::equal_to<const llvm37::Value*>,std::allocator<int> >" }
	%"struct.__gnu_cxx::hash_set<const llvm37::Constant*,__gnu_cxx::hash<const llvm37::Constant*>,std::equal_to<const llvm37::Constant*>,std::allocator<const llvm37::Constant*> >" = type { %"struct.__gnu_cxx::hashtable<const llvm37::Constant*,const llvm37::Constant*,__gnu_cxx::hash<const llvm37::Constant*>,std::_Identity<const llvm37::Constant*>,std::equal_to<const llvm37::Constant*>,std::allocator<const llvm37::Constant*> >" }
	%"struct.__gnu_cxx::hashtable<const llvm37::Constant*,const llvm37::Constant*,__gnu_cxx::hash<const llvm37::Constant*>,std::_Identity<const llvm37::Constant*>,std::equal_to<const llvm37::Constant*>,std::allocator<const llvm37::Constant*> >" = type { %struct.__false_type, %struct.__false_type, %struct.__false_type, %struct.__false_type, %"struct.std::vector<__gnu_cxx::_Hashtable_node<const llvm37::Constant*>*,std::allocator<const llvm37::Constant*> >", i32 }
	%"struct.__gnu_cxx::hashtable<std::pair<const llvm37::Value* const, int>,const llvm37::Value*,__gnu_cxx::hash<const llvm37::Value*>,std::_Select1st<std::pair<const llvm37::Value* const, int> >,std::equal_to<const llvm37::Value*>,std::allocator<int> >" = type { %struct.__false_type, %struct.__false_type, %struct.__false_type, %struct.__false_type, %"struct.std::vector<__gnu_cxx::_Hashtable_node<std::pair<const llvm37::Value* const, int> >*,std::allocator<int> >", i32 }
	%"struct.llvm37::AbstractTypeUser" = type { i32 (...)** }
	%"struct.llvm37::Annotable" = type { i32 (...)**, %"struct.llvm37::Annotation"* }
	%"struct.llvm37::Annotation" = type { i32 (...)**, %"struct.llvm37::AnnotationID", %"struct.llvm37::Annotation"* }
	%"struct.llvm37::AnnotationID" = type { i32 }
	%"struct.llvm37::Argument" = type { %"struct.llvm37::Value", %"struct.llvm37::Function"*, %"struct.llvm37::Argument"*, %"struct.llvm37::Argument"* }
	%"struct.llvm37::BasicBlock" = type { %"struct.llvm37::Value", %"struct.llvm37::iplist<llvm37::Instruction,llvm37::ilist_traits<llvm37::Instruction> >", %"struct.llvm37::BasicBlock"*, %"struct.llvm37::BasicBlock"* }
	%"struct.llvm37::Constant" = type opaque
	%"struct.llvm37::DerivedType" = type { %"struct.llvm37::Type", %"struct.llvm37::AbstractTypeUser", %"struct.std::vector<llvm37::AbstractTypeUser*,std::allocator<llvm37::AbstractTypeUser*> >" }
	%"struct.llvm37::Function" = type { %"struct.llvm37::GlobalValue", %"struct.llvm37::Annotable", %"struct.llvm37::iplist<llvm37::BasicBlock,llvm37::ilist_traits<llvm37::BasicBlock> >", %"struct.llvm37::iplist<llvm37::Argument,llvm37::ilist_traits<llvm37::Argument> >", %"struct.llvm37::SymbolTable"*, %"struct.llvm37::Function"*, %"struct.llvm37::Function"* }
	%"struct.llvm37::FunctionPass" = type { %"struct.llvm37::Pass" }
	%"struct.llvm37::FunctionType" = type { %"struct.llvm37::DerivedType", i1 }
	%"struct.llvm37::GlobalValue" = type { %"struct.llvm37::User", i32, %"struct.llvm37::Module"* }
	%"struct.llvm37::Instruction" = type { %"struct.llvm37::User", %"struct.llvm37::Annotable", %"struct.llvm37::BasicBlock"*, %"struct.llvm37::Instruction"*, %"struct.llvm37::Instruction"*, i32 }
	%"struct.llvm37::IntrinsicLowering" = type opaque
	%"struct.llvm37::MachineBasicBlock" = type { %"struct.llvm37::ilist<llvm37::MachineInstr>", %"struct.llvm37::MachineBasicBlock"*, %"struct.llvm37::MachineBasicBlock"*, %"struct.llvm37::BasicBlock"* }
	%"struct.llvm37::MachineConstantPool" = type opaque
	%"struct.llvm37::MachineFrameInfo" = type opaque
	%"struct.llvm37::MachineFunction" = type { %"struct.llvm37::Annotation", %"struct.llvm37::Function"*, %"struct.llvm37::TargetMachine"*, %"struct.llvm37::iplist<llvm37::MachineBasicBlock,llvm37::ilist_traits<llvm37::MachineBasicBlock> >", %"struct.llvm37::SSARegMap"*, %"struct.llvm37::MachineFunctionInfo"*, %"struct.llvm37::MachineFrameInfo"*, %"struct.llvm37::MachineConstantPool"* }
	%"struct.llvm37::MachineFunctionInfo" = type { %"struct.__gnu_cxx::hash_set<const llvm37::Constant*,__gnu_cxx::hash<const llvm37::Constant*>,std::equal_to<const llvm37::Constant*>,std::allocator<const llvm37::Constant*> >", %"struct.__gnu_cxx::hash_map<const llvm37::Value*,int,__gnu_cxx::hash<const llvm37::Value*>,std::equal_to<const llvm37::Value*>,std::allocator<int> >", i32, i32, i32, i32, i32, i32, i32, i1, i1, i1, %"struct.llvm37::MachineFunction"* }
	%"struct.llvm37::MachineFunctionPass" = type { %"struct.llvm37::FunctionPass" }
	%"struct.llvm37::MachineInstr" = type { i16, i8, %"struct.std::vector<llvm37::MachineOperand,std::allocator<llvm37::MachineOperand> >", %"struct.llvm37::MachineInstr"*, %"struct.llvm37::MachineInstr"*, %"struct.llvm37::MachineBasicBlock"* }
	%"struct.llvm37::MachineInstrBuilder" = type { %"struct.llvm37::MachineInstr"* }
	%"struct.llvm37::MachineOperand" = type { %"union.llvm37::MachineOperand::._65", i32, i32 }
	%"struct.llvm37::Module" = type opaque
	%"struct.llvm37::PATypeHandle" = type { %"struct.llvm37::Type"*, %"struct.llvm37::AbstractTypeUser"* }
	%"struct.llvm37::PATypeHolder" = type { %"struct.llvm37::Type"* }
	%"struct.llvm37::Pass" = type { i32 (...)**, %"struct.llvm37::AbstractTypeUser"*, %"struct.llvm37::PassInfo"*, %"struct.std::vector<std::pair<const llvm37::PassInfo*, llvm37::Pass*>,std::allocator<std::pair<const llvm37::PassInfo*, llvm37::Pass*> > >" }
	%"struct.llvm37::PassInfo" = type { i8*, i8*, %"struct.std::type_info"*, i8, %"struct.std::vector<const llvm37::PassInfo*,std::allocator<const llvm37::PassInfo*> >", %"struct.llvm37::Pass"* ()*, %"struct.llvm37::Pass"* (%"struct.llvm37::TargetMachine"*)* }
	%"struct.llvm37::SSARegMap" = type opaque
	%"struct.llvm37::SymbolTable" = type opaque
	%"struct.llvm37::SymbolTableListTraits<llvm37::Argument,llvm37::Function,llvm37::Function,llvm37::ilist_traits<llvm37::Argument> >" = type { %"struct.llvm37::Function"*, %"struct.llvm37::Function"* }
	%"struct.llvm37::SymbolTableListTraits<llvm37::Instruction,llvm37::BasicBlock,llvm37::Function,llvm37::ilist_traits<llvm37::Instruction> >" = type { %"struct.llvm37::Function"*, %"struct.llvm37::BasicBlock"* }
	%"struct.llvm37::DataLayout" = type { %"struct.llvm37::FunctionPass", i1, i8, i8, i8, i8, i8, i8, i8, i8 }
	%"struct.llvm37::TargetFrameInfo" = type { i32 (...)**, i32, i32, i32 }
	%"struct.llvm37::TargetInstrDescriptor" = type { i8*, i32, i32, i32, i1, i32, i32, i32, i32, i32, i32*, i32* }
	%"struct.llvm37::TargetInstrInfo" = type { i32 (...)**, %"struct.llvm37::TargetInstrDescriptor"*, i32, i32 }
	%"struct.llvm37::TargetMachine" = type { i32 (...)**, %"struct.std::basic_string<char,std::char_traits<char>,std::allocator<char> >", %"struct.llvm37::DataLayout", %"struct.llvm37::IntrinsicLowering"* }
	%"struct.llvm37::TargetRegClassInfo" = type { i32 (...)**, i32, i32, i32 }
	%"struct.llvm37::TargetRegInfo" = type { i32 (...)**, %"struct.std::vector<const llvm37::TargetRegClassInfo*,std::allocator<const llvm37::TargetRegClassInfo*> >", %"struct.llvm37::TargetMachine"* }
	%"struct.llvm37::Type" = type { %"struct.llvm37::Value", i32, i32, i1, i32, %"struct.llvm37::Type"*, %"struct.std::vector<llvm37::PATypeHandle,std::allocator<llvm37::PATypeHandle> >" }
	%"struct.llvm37::Use" = type { %"struct.llvm37::Value"*, %"struct.llvm37::User"*, %"struct.llvm37::Use"*, %"struct.llvm37::Use"* }
	%"struct.llvm37::User" = type { %"struct.llvm37::Value", %"struct.std::vector<llvm37::Use,std::allocator<llvm37::Use> >" }
	%"struct.llvm37::Value" = type { i32 (...)**, %"struct.llvm37::iplist<llvm37::Use,llvm37::ilist_traits<llvm37::Use> >", %"struct.std::basic_string<char,std::char_traits<char>,std::allocator<char> >", %"struct.llvm37::PATypeHolder", i32 }
	%"struct.llvm37::_GLOBAL__N_::InsertPrologEpilogCode" = type { %"struct.llvm37::MachineFunctionPass" }
	%"struct.llvm37::ilist<llvm37::MachineInstr>" = type { %"struct.llvm37::iplist<llvm37::MachineInstr,llvm37::ilist_traits<llvm37::MachineInstr> >" }
	%"struct.llvm37::ilist_iterator<const llvm37::MachineBasicBlock>" = type { %"struct.llvm37::MachineBasicBlock"* }
	%"struct.llvm37::ilist_traits<llvm37::Argument>" = type { %"struct.llvm37::SymbolTableListTraits<llvm37::Argument,llvm37::Function,llvm37::Function,llvm37::ilist_traits<llvm37::Argument> >" }
	%"struct.llvm37::ilist_traits<llvm37::Instruction>" = type { %"struct.llvm37::SymbolTableListTraits<llvm37::Instruction,llvm37::BasicBlock,llvm37::Function,llvm37::ilist_traits<llvm37::Instruction> >" }
	%"struct.llvm37::iplist<llvm37::Argument,llvm37::ilist_traits<llvm37::Argument> >" = type { %"struct.llvm37::ilist_traits<llvm37::Argument>", %"struct.llvm37::Argument"*, %"struct.llvm37::Argument"* }
	%"struct.llvm37::iplist<llvm37::BasicBlock,llvm37::ilist_traits<llvm37::BasicBlock> >" = type { %"struct.llvm37::ilist_traits<llvm37::Argument>", %"struct.llvm37::BasicBlock"*, %"struct.llvm37::BasicBlock"* }
	%"struct.llvm37::iplist<llvm37::Instruction,llvm37::ilist_traits<llvm37::Instruction> >" = type { %"struct.llvm37::ilist_traits<llvm37::Instruction>", %"struct.llvm37::Instruction"*, %"struct.llvm37::Instruction"* }
	%"struct.llvm37::iplist<llvm37::MachineBasicBlock,llvm37::ilist_traits<llvm37::MachineBasicBlock> >" = type { %"struct.llvm37::MachineBasicBlock"*, %"struct.llvm37::MachineBasicBlock"* }
	%"struct.llvm37::iplist<llvm37::MachineInstr,llvm37::ilist_traits<llvm37::MachineInstr> >" = type { %"struct.llvm37::ilist_iterator<const llvm37::MachineBasicBlock>", %"struct.llvm37::MachineInstr"*, %"struct.llvm37::MachineInstr"* }
	%"struct.llvm37::iplist<llvm37::Use,llvm37::ilist_traits<llvm37::Use> >" = type { %"struct.llvm37::Use"*, %"struct.llvm37::Use"* }
	%"struct.std::_Vector_alloc_base<__gnu_cxx::_Hashtable_node<const llvm37::Constant*>*,std::allocator<const llvm37::Constant*>, true>" = type { %"struct.__gnu_cxx::_Hashtable_node<const llvm37::Constant*>"**, %"struct.__gnu_cxx::_Hashtable_node<const llvm37::Constant*>"**, %"struct.__gnu_cxx::_Hashtable_node<const llvm37::Constant*>"** }
	%"struct.std::_Vector_alloc_base<__gnu_cxx::_Hashtable_node<std::pair<const llvm37::Value* const, int> >*,std::allocator<int>, true>" = type { %"struct.__gnu_cxx::_Hashtable_node<std::pair<const llvm37::Value* const, int> >"**, %"struct.__gnu_cxx::_Hashtable_node<std::pair<const llvm37::Value* const, int> >"**, %"struct.__gnu_cxx::_Hashtable_node<std::pair<const llvm37::Value* const, int> >"** }
	%"struct.std::_Vector_alloc_base<const llvm37::PassInfo*,std::allocator<const llvm37::PassInfo*>, true>" = type { %"struct.llvm37::PassInfo"**, %"struct.llvm37::PassInfo"**, %"struct.llvm37::PassInfo"** }
	%"struct.std::_Vector_alloc_base<const llvm37::TargetRegClassInfo*,std::allocator<const llvm37::TargetRegClassInfo*>, true>" = type { %"struct.llvm37::TargetFrameInfo"**, %"struct.llvm37::TargetFrameInfo"**, %"struct.llvm37::TargetFrameInfo"** }
	%"struct.std::_Vector_alloc_base<llvm37::AbstractTypeUser*,std::allocator<llvm37::AbstractTypeUser*>, true>" = type { %"struct.llvm37::AbstractTypeUser"**, %"struct.llvm37::AbstractTypeUser"**, %"struct.llvm37::AbstractTypeUser"** }
	%"struct.std::_Vector_alloc_base<llvm37::MachineInstr*,std::allocator<llvm37::MachineInstr*>, true>" = type { %"struct.llvm37::MachineInstr"**, %"struct.llvm37::MachineInstr"**, %"struct.llvm37::MachineInstr"** }
	%"struct.std::_Vector_alloc_base<llvm37::MachineOperand,std::allocator<llvm37::MachineOperand>, true>" = type { %"struct.llvm37::MachineOperand"*, %"struct.llvm37::MachineOperand"*, %"struct.llvm37::MachineOperand"* }
	%"struct.std::_Vector_alloc_base<llvm37::PATypeHandle,std::allocator<llvm37::PATypeHandle>, true>" = type { %"struct.llvm37::PATypeHandle"*, %"struct.llvm37::PATypeHandle"*, %"struct.llvm37::PATypeHandle"* }
	%"struct.std::_Vector_alloc_base<llvm37::Use,std::allocator<llvm37::Use>, true>" = type { %"struct.llvm37::Use"*, %"struct.llvm37::Use"*, %"struct.llvm37::Use"* }
	%"struct.std::_Vector_alloc_base<std::pair<const llvm37::PassInfo*, llvm37::Pass*>,std::allocator<std::pair<const llvm37::PassInfo*, llvm37::Pass*> >, true>" = type { %"struct.std::pair<const llvm37::PassInfo*,llvm37::Pass*>"*, %"struct.std::pair<const llvm37::PassInfo*,llvm37::Pass*>"*, %"struct.std::pair<const llvm37::PassInfo*,llvm37::Pass*>"* }
	%"struct.std::_Vector_base<__gnu_cxx::_Hashtable_node<const llvm37::Constant*>*,std::allocator<const llvm37::Constant*> >" = type { %"struct.std::_Vector_alloc_base<__gnu_cxx::_Hashtable_node<const llvm37::Constant*>*,std::allocator<const llvm37::Constant*>, true>" }
	%"struct.std::_Vector_base<__gnu_cxx::_Hashtable_node<std::pair<const llvm37::Value* const, int> >*,std::allocator<int> >" = type { %"struct.std::_Vector_alloc_base<__gnu_cxx::_Hashtable_node<std::pair<const llvm37::Value* const, int> >*,std::allocator<int>, true>" }
	%"struct.std::_Vector_base<const llvm37::PassInfo*,std::allocator<const llvm37::PassInfo*> >" = type { %"struct.std::_Vector_alloc_base<const llvm37::PassInfo*,std::allocator<const llvm37::PassInfo*>, true>" }
	%"struct.std::_Vector_base<const llvm37::TargetRegClassInfo*,std::allocator<const llvm37::TargetRegClassInfo*> >" = type { %"struct.std::_Vector_alloc_base<const llvm37::TargetRegClassInfo*,std::allocator<const llvm37::TargetRegClassInfo*>, true>" }
	%"struct.std::_Vector_base<llvm37::AbstractTypeUser*,std::allocator<llvm37::AbstractTypeUser*> >" = type { %"struct.std::_Vector_alloc_base<llvm37::AbstractTypeUser*,std::allocator<llvm37::AbstractTypeUser*>, true>" }
	%"struct.std::_Vector_base<llvm37::MachineInstr*,std::allocator<llvm37::MachineInstr*> >" = type { %"struct.std::_Vector_alloc_base<llvm37::MachineInstr*,std::allocator<llvm37::MachineInstr*>, true>" }
	%"struct.std::_Vector_base<llvm37::MachineOperand,std::allocator<llvm37::MachineOperand> >" = type { %"struct.std::_Vector_alloc_base<llvm37::MachineOperand,std::allocator<llvm37::MachineOperand>, true>" }
	%"struct.std::_Vector_base<llvm37::PATypeHandle,std::allocator<llvm37::PATypeHandle> >" = type { %"struct.std::_Vector_alloc_base<llvm37::PATypeHandle,std::allocator<llvm37::PATypeHandle>, true>" }
	%"struct.std::_Vector_base<llvm37::Use,std::allocator<llvm37::Use> >" = type { %"struct.std::_Vector_alloc_base<llvm37::Use,std::allocator<llvm37::Use>, true>" }
	%"struct.std::_Vector_base<std::pair<const llvm37::PassInfo*, llvm37::Pass*>,std::allocator<std::pair<const llvm37::PassInfo*, llvm37::Pass*> > >" = type { %"struct.std::_Vector_alloc_base<std::pair<const llvm37::PassInfo*, llvm37::Pass*>,std::allocator<std::pair<const llvm37::PassInfo*, llvm37::Pass*> >, true>" }
	%"struct.std::basic_string<char,std::char_traits<char>,std::allocator<char> >" = type { %"struct.std::basic_string<char,std::char_traits<char>,std::allocator<char> >::_Alloc_hider" }
	%"struct.std::basic_string<char,std::char_traits<char>,std::allocator<char> >::_Alloc_hider" = type { i8* }
	%"struct.std::pair<const llvm37::PassInfo*,llvm37::Pass*>" = type { %"struct.llvm37::PassInfo"*, %"struct.llvm37::Pass"* }
	%"struct.std::pair<const llvm37::Value* const,int>" = type { %"struct.llvm37::Value"*, i32 }
	%"struct.std::type_info" = type { i32 (...)**, i8* }
	%"struct.std::vector<__gnu_cxx::_Hashtable_node<const llvm37::Constant*>*,std::allocator<const llvm37::Constant*> >" = type { %"struct.std::_Vector_base<__gnu_cxx::_Hashtable_node<const llvm37::Constant*>*,std::allocator<const llvm37::Constant*> >" }
	%"struct.std::vector<__gnu_cxx::_Hashtable_node<std::pair<const llvm37::Value* const, int> >*,std::allocator<int> >" = type { %"struct.std::_Vector_base<__gnu_cxx::_Hashtable_node<std::pair<const llvm37::Value* const, int> >*,std::allocator<int> >" }
	%"struct.std::vector<const llvm37::PassInfo*,std::allocator<const llvm37::PassInfo*> >" = type { %"struct.std::_Vector_base<const llvm37::PassInfo*,std::allocator<const llvm37::PassInfo*> >" }
	%"struct.std::vector<const llvm37::TargetRegClassInfo*,std::allocator<const llvm37::TargetRegClassInfo*> >" = type { %"struct.std::_Vector_base<const llvm37::TargetRegClassInfo*,std::allocator<const llvm37::TargetRegClassInfo*> >" }
	%"struct.std::vector<llvm37::AbstractTypeUser*,std::allocator<llvm37::AbstractTypeUser*> >" = type { %"struct.std::_Vector_base<llvm37::AbstractTypeUser*,std::allocator<llvm37::AbstractTypeUser*> >" }
	%"struct.std::vector<llvm37::MachineInstr*,std::allocator<llvm37::MachineInstr*> >" = type { %"struct.std::_Vector_base<llvm37::MachineInstr*,std::allocator<llvm37::MachineInstr*> >" }
	%"struct.std::vector<llvm37::MachineOperand,std::allocator<llvm37::MachineOperand> >" = type { %"struct.std::_Vector_base<llvm37::MachineOperand,std::allocator<llvm37::MachineOperand> >" }
	%"struct.std::vector<llvm37::PATypeHandle,std::allocator<llvm37::PATypeHandle> >" = type { %"struct.std::_Vector_base<llvm37::PATypeHandle,std::allocator<llvm37::PATypeHandle> >" }
	%"struct.std::vector<llvm37::Use,std::allocator<llvm37::Use> >" = type { %"struct.std::_Vector_base<llvm37::Use,std::allocator<llvm37::Use> >" }
	%"struct.std::vector<std::pair<const llvm37::PassInfo*, llvm37::Pass*>,std::allocator<std::pair<const llvm37::PassInfo*, llvm37::Pass*> > >" = type { %"struct.std::_Vector_base<std::pair<const llvm37::PassInfo*, llvm37::Pass*>,std::allocator<std::pair<const llvm37::PassInfo*, llvm37::Pass*> > >" }
	%"union.llvm37::MachineOperand::._65" = type { %"struct.std::basic_string<char,std::char_traits<char>,std::allocator<char> >"* }

declare void @_Znwj()

declare void @_ZN4llvm3712MachineInstrC1Esjbb()

declare void @_ZNSt6vectorIPN4llvm3712MachineInstrESaIS2_EE9push_backERKS2_()

declare void @_ZNK4llvm378Function15getFunctionTypeEv()

declare void @_ZNK4llvm3719MachineInstrBuilder7addMRegEiNS_14MachineOperand7UseTypeE()

declare void @_ZNK4llvm3719MachineInstrBuilder7addSImmEi()

declare i32 @__gxx_personality_v0(...)

define void @_ZN4llvm3711_GLOBAL__N_22InsertPrologEpilogCode20runOnMachineFunctionERNS_15MachineFunctionE(%"struct.llvm37::MachineFunction"* %F) personality i32 (...)* @__gxx_personality_v0 {
entry:
	%tmp.8.i = invoke %"struct.llvm37::TargetFrameInfo"* null( %"struct.llvm37::TargetMachine"* null )
			to label %invoke_cont.0.i unwind label %invoke_catch.0.i		; <%"struct.llvm37::TargetFrameInfo"*> [#uses=0]

invoke_catch.0.i:		; preds = %invoke_cont.49.i, %invoke_cont.48.i, %invoke_cont.47.i, %invoke_cont.i53.i, %no_exit.i, %invoke_cont.44.i, %invoke_cont.43.i, %invoke_cont.42.i, %invoke_cont.41.i, %invoke_cont.40.i, %invoke_cont.39.i, %invoke_cont.38.i, %invoke_cont.37.i, %then.2.i, %invoke_cont.35.i, %invoke_cont.34.i, %then.1.i, %endif.0.i, %invoke_cont.9.i, %invoke_cont.8.i, %invoke_cont.7.i, %invoke_cont.i.i, %then.0.i, %invoke_cont.4.i, %invoke_cont.3.i, %invoke_cont.2.i, %invoke_cont.1.i, %endif.0.i.i, %tmp.7.i.noexc.i, %invoke_cont.0.i, %entry
        %exn0.i = landingpad {i8*, i32}
                 cleanup
	ret void

invoke_cont.0.i:		; preds = %entry
	%tmp.7.i1.i = invoke %"struct.llvm37::TargetFrameInfo"* null( %"struct.llvm37::TargetMachine"* null )
			to label %tmp.7.i.noexc.i unwind label %invoke_catch.0.i		; <%"struct.llvm37::TargetFrameInfo"*> [#uses=2]

tmp.7.i.noexc.i:		; preds = %invoke_cont.0.i
	%tmp.17.i2.i = invoke i32 null( %"struct.llvm37::TargetFrameInfo"* %tmp.7.i1.i )
			to label %endif.0.i.i unwind label %invoke_catch.0.i		; <i32> [#uses=0]

endif.0.i.i:		; preds = %tmp.7.i.noexc.i
	%tmp.38.i4.i = invoke i32 null( %"struct.llvm37::TargetFrameInfo"* %tmp.7.i1.i )
			to label %tmp.38.i.noexc.i unwind label %invoke_catch.0.i		; <i32> [#uses=0]

tmp.38.i.noexc.i:		; preds = %endif.0.i.i
	br i1 false, label %invoke_cont.1.i, label %then.1.i.i

then.1.i.i:		; preds = %tmp.38.i.noexc.i
	ret void

invoke_cont.1.i:		; preds = %tmp.38.i.noexc.i
	%tmp.21.i = invoke %"struct.llvm37::TargetRegInfo"* null( %"struct.llvm37::TargetMachine"* null )
			to label %invoke_cont.2.i unwind label %invoke_catch.0.i		; <%"struct.llvm37::TargetRegInfo"*> [#uses=1]

invoke_cont.2.i:		; preds = %invoke_cont.1.i
	%tmp.28.i = invoke i32 null( %"struct.llvm37::TargetRegInfo"* %tmp.21.i )
			to label %invoke_cont.3.i unwind label %invoke_catch.0.i		; <i32> [#uses=0]

invoke_cont.3.i:		; preds = %invoke_cont.2.i
	%tmp.36.i = invoke %"struct.llvm37::TargetInstrInfo"* null( %"struct.llvm37::TargetMachine"* null )
			to label %invoke_cont.4.i unwind label %invoke_catch.0.i		; <%"struct.llvm37::TargetInstrInfo"*> [#uses=1]

invoke_cont.4.i:		; preds = %invoke_cont.3.i
	%tmp.43.i = invoke i1 null( %"struct.llvm37::TargetInstrInfo"* %tmp.36.i, i16 383, i64 0 )
			to label %invoke_cont.5.i unwind label %invoke_catch.0.i		; <i1> [#uses=1]

invoke_cont.5.i:		; preds = %invoke_cont.4.i
	br i1 %tmp.43.i, label %then.0.i, label %else.i

then.0.i:		; preds = %invoke_cont.5.i
	invoke void @_Znwj( )
			to label %tmp.0.i.noexc.i unwind label %invoke_catch.0.i

tmp.0.i.noexc.i:		; preds = %then.0.i
	invoke void @_ZN4llvm3712MachineInstrC1Esjbb( )
			to label %invoke_cont.i.i unwind label %cond_true.i.i

cond_true.i.i:		; preds = %tmp.0.i.noexc.i
        %exn.i.i = landingpad {i8*, i32}
                 cleanup
	ret void

invoke_cont.i.i:		; preds = %tmp.0.i.noexc.i
	invoke void @_ZNK4llvm3719MachineInstrBuilder7addMRegEiNS_14MachineOperand7UseTypeE( )
			to label %invoke_cont.7.i unwind label %invoke_catch.0.i

invoke_cont.7.i:		; preds = %invoke_cont.i.i
	invoke void @_ZNK4llvm3719MachineInstrBuilder7addSImmEi( )
			to label %invoke_cont.8.i unwind label %invoke_catch.0.i

invoke_cont.8.i:		; preds = %invoke_cont.7.i
	invoke void @_ZNK4llvm3719MachineInstrBuilder7addMRegEiNS_14MachineOperand7UseTypeE( )
			to label %invoke_cont.9.i unwind label %invoke_catch.0.i

invoke_cont.9.i:		; preds = %invoke_cont.8.i
	invoke void @_ZNSt6vectorIPN4llvm3712MachineInstrESaIS2_EE9push_backERKS2_( )
			to label %endif.0.i unwind label %invoke_catch.0.i

else.i:		; preds = %invoke_cont.5.i
	ret void

endif.0.i:		; preds = %invoke_cont.9.i
	invoke void @_ZNK4llvm378Function15getFunctionTypeEv( )
			to label %invoke_cont.33.i unwind label %invoke_catch.0.i

invoke_cont.33.i:		; preds = %endif.0.i
	br i1 false, label %then.1.i, label %endif.1.i

then.1.i:		; preds = %invoke_cont.33.i
	invoke void @_ZNK4llvm378Function15getFunctionTypeEv( )
			to label %invoke_cont.34.i unwind label %invoke_catch.0.i

invoke_cont.34.i:		; preds = %then.1.i
	%tmp.121.i = invoke %"struct.llvm37::TargetRegInfo"* null( %"struct.llvm37::TargetMachine"* null )
			to label %invoke_cont.35.i unwind label %invoke_catch.0.i		; <%"struct.llvm37::TargetRegInfo"*> [#uses=1]

invoke_cont.35.i:		; preds = %invoke_cont.34.i
	%tmp.128.i = invoke i32 null( %"struct.llvm37::TargetRegInfo"* %tmp.121.i )
			to label %invoke_cont.36.i unwind label %invoke_catch.0.i		; <i32> [#uses=0]

invoke_cont.36.i:		; preds = %invoke_cont.35.i
	br i1 false, label %then.2.i, label %endif.1.i

then.2.i:		; preds = %invoke_cont.36.i
	%tmp.140.i = invoke %"struct.llvm37::TargetRegInfo"* null( %"struct.llvm37::TargetMachine"* null )
			to label %invoke_cont.37.i unwind label %invoke_catch.0.i		; <%"struct.llvm37::TargetRegInfo"*> [#uses=0]

invoke_cont.37.i:		; preds = %then.2.i
	%tmp.148.i = invoke %"struct.llvm37::TargetRegInfo"* null( %"struct.llvm37::TargetMachine"* null )
			to label %invoke_cont.38.i unwind label %invoke_catch.0.i		; <%"struct.llvm37::TargetRegInfo"*> [#uses=1]

invoke_cont.38.i:		; preds = %invoke_cont.37.i
	%tmp.155.i = invoke i32 null( %"struct.llvm37::TargetRegInfo"* %tmp.148.i, %"struct.llvm37::Type"* null, i1 false )
			to label %invoke_cont.39.i unwind label %invoke_catch.0.i		; <i32> [#uses=0]

invoke_cont.39.i:		; preds = %invoke_cont.38.i
	%tmp.163.i = invoke %"struct.llvm37::TargetFrameInfo"* null( %"struct.llvm37::TargetMachine"* null )
			to label %invoke_cont.40.i unwind label %invoke_catch.0.i		; <%"struct.llvm37::TargetFrameInfo"*> [#uses=1]

invoke_cont.40.i:		; preds = %invoke_cont.39.i
	%tmp.170.i = invoke i32 null( %"struct.llvm37::TargetFrameInfo"* %tmp.163.i )
			to label %invoke_cont.41.i unwind label %invoke_catch.0.i		; <i32> [#uses=0]

invoke_cont.41.i:		; preds = %invoke_cont.40.i
	%tmp.177.i = invoke %"struct.llvm37::TargetFrameInfo"* null( %"struct.llvm37::TargetMachine"* null )
			to label %invoke_cont.42.i unwind label %invoke_catch.0.i		; <%"struct.llvm37::TargetFrameInfo"*> [#uses=1]

invoke_cont.42.i:		; preds = %invoke_cont.41.i
	%tmp.184.i = invoke i32 null( %"struct.llvm37::TargetFrameInfo"* %tmp.177.i )
			to label %invoke_cont.43.i unwind label %invoke_catch.0.i		; <i32> [#uses=1]

invoke_cont.43.i:		; preds = %invoke_cont.42.i
	%tmp.191.i = invoke %"struct.llvm37::TargetFrameInfo"* null( %"struct.llvm37::TargetMachine"* null )
			to label %invoke_cont.44.i unwind label %invoke_catch.0.i		; <%"struct.llvm37::TargetFrameInfo"*> [#uses=1]

invoke_cont.44.i:		; preds = %invoke_cont.43.i
	%tmp.198.i = invoke i32 null( %"struct.llvm37::TargetFrameInfo"* %tmp.191.i, %"struct.llvm37::MachineFunction"* %F, i1* null )
			to label %invoke_cont.45.i unwind label %invoke_catch.0.i		; <i32> [#uses=0]

invoke_cont.45.i:		; preds = %invoke_cont.44.i
	br i1 false, label %no_exit.i, label %endif.1.i

no_exit.i:		; preds = %invoke_cont.50.i, %invoke_cont.45.i
	%nextArgOffset.0.i.1 = phi i32 [ %tmp.221.i, %invoke_cont.50.i ], [ 0, %invoke_cont.45.i ]		; <i32> [#uses=1]
	invoke void @_Znwj( )
			to label %tmp.0.i.noexc55.i unwind label %invoke_catch.0.i

tmp.0.i.noexc55.i:		; preds = %no_exit.i
	invoke void @_ZN4llvm3712MachineInstrC1Esjbb( )
			to label %invoke_cont.i53.i unwind label %cond_true.i52.i

cond_true.i52.i:		; preds = %tmp.0.i.noexc55.i
        %exn.i52.i = landingpad {i8*, i32}
                 cleanup
	ret void

invoke_cont.i53.i:		; preds = %tmp.0.i.noexc55.i
	invoke void @_ZNK4llvm3719MachineInstrBuilder7addMRegEiNS_14MachineOperand7UseTypeE( )
			to label %invoke_cont.47.i unwind label %invoke_catch.0.i

invoke_cont.47.i:		; preds = %invoke_cont.i53.i
	invoke void @_ZNK4llvm3719MachineInstrBuilder7addMRegEiNS_14MachineOperand7UseTypeE( )
			to label %invoke_cont.48.i unwind label %invoke_catch.0.i

invoke_cont.48.i:		; preds = %invoke_cont.47.i
	invoke void @_ZNK4llvm3719MachineInstrBuilder7addSImmEi( )
			to label %invoke_cont.49.i unwind label %invoke_catch.0.i

invoke_cont.49.i:		; preds = %invoke_cont.48.i
	invoke void @_ZNSt6vectorIPN4llvm3712MachineInstrESaIS2_EE9push_backERKS2_( )
			to label %invoke_cont.50.i unwind label %invoke_catch.0.i

invoke_cont.50.i:		; preds = %invoke_cont.49.i
	%tmp.221.i = add i32 %nextArgOffset.0.i.1, %tmp.184.i		; <i32> [#uses=1]
	br i1 false, label %no_exit.i, label %endif.1.i

endif.1.i:		; preds = %invoke_cont.50.i, %invoke_cont.45.i, %invoke_cont.36.i, %invoke_cont.33.i
	ret void
}
