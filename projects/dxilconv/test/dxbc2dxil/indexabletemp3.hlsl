// RUN: %fxc /T ps_5_0 %s /Fo %t.dxbc
// RUN: %dxbc2dxil %t.dxbc /emit-llvm37 /o %t.ll.converted
// RUN: fc %b.ref %t.ll.converted




float g1[4], g2[8];

float main(float4 a : A, int b : B, int c : C) : SV_TARGET
{
  float x1[4];
  x1[0] = g1[b];
  x1[1] = g2[b];

  return x1[c];
}
