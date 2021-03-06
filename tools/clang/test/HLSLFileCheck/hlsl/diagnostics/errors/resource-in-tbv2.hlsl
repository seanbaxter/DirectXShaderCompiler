// RUN: %dxc -E main -T ps_6_0 %s  | FileCheck %s

// CHECK: error: object types not supported in cbuffer/tbuffer view arrays.

SamplerState Samp;
struct Resource
{
  Texture2D Tex1;
  // Texture3D Tex2;
  // RWTexture2D<float4> RWTex1;
  // RWTexture3D<float4> RWTex2;
  // SamplerState Samp;
  float4 foo;
};

struct MyStruct
{
  Resource res;
  int4 bar;
};

TextureBuffer<MyStruct> TB[4];

float4 main(int4 a : A, float4 coord : TEXCOORD) : SV_TARGET
{
  return TB[2].res.foo;
}
