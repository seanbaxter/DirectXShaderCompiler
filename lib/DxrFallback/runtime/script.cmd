@setlocal
@set BINPATH=C:\Program Files\LLVM373.7\bin
@set CLANG="%BINPATH%\clang"
@set OPT="%BINPATH%\opt"


%CLANG% -S -emit-llvm37 -target nvptx runtime.c 
%OPT% -S -mem2reg  runtime.ll -o runtime.opt.ll
python rewriteRuntime.py
