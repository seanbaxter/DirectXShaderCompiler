// RUN: %fxc /T ps_5_0 %s /Fo %t.dxbc
// RUN: %dxbc2dxil %t.dxbc /emit-llvm37 /o %t.ll.converted
// RUN: fc %b.ref %t.ll.converted




int4 main(int4 a : A) : SV_TARGET
{
  return -a.yxxx;
}