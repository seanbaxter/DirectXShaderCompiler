// RUN: %fxc /T ps_5_0 %s /Fo %t.dxbc
// RUN: %dxbc2dxil %t.dxbc /emit-llvm37 /o %t.ll.converted
// RUN: fc %b.ref %t.ll.converted




float main(min16float a : A) : SV_Target
{
    return a + (min16float)2.f;
}
