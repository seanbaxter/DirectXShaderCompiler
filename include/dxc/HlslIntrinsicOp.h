///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// HlslIntrinsicOp.h                                                         //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// Enumeration for HLSL intrinsics operations.                               //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#pragma once
namespace hlsl
{

enum class IntrinsicOp
{
  IOP_AcceptHitAndEndSearch                          =   0,
  IOP_AddUint64                                      =   1,
  IOP_AllMemoryBarrier                               =   2,
  IOP_AllMemoryBarrierWithGroupSync                  =   3,
  IOP_AllocateRayQuery                               =   4,
  IOP_CallShader                                     =   5,
  IOP_CheckAccessFullyMapped                         =   6,
  IOP_CreateResourceFromHeap                         =   7,
  IOP_D3DCOLORtoUBYTE4                               =   8,
  IOP_DeviceMemoryBarrier                            =   9,
  IOP_DeviceMemoryBarrierWithGroupSync               =  10,
  IOP_DispatchMesh                                   =  11,
  IOP_DispatchRaysDimensions                         =  12,
  IOP_DispatchRaysIndex                              =  13,
  IOP_EvaluateAttributeAtSample                      =  14,
  IOP_EvaluateAttributeCentroid                      =  15,
  IOP_EvaluateAttributeSnapped                       =  16,
  IOP_GeometryIndex                                  =  17,
  IOP_GetAttributeAtVertex                           =  18,
  IOP_GetRenderTargetSampleCount                     =  19,
  IOP_GetRenderTargetSamplePosition                  =  20,
  IOP_GroupMemoryBarrier                             =  21,
  IOP_GroupMemoryBarrierWithGroupSync                =  22,
  IOP_HitKind                                        =  23,
  IOP_IgnoreHit                                      =  24,
  IOP_InstanceID                                     =  25,
  IOP_InstanceIndex                                  =  26,
  IOP_InterlockedAdd                                 =  27,
  IOP_InterlockedAnd                                 =  28,
  IOP_InterlockedCompareExchange                     =  29,
  IOP_InterlockedCompareExchangeFloatBitwise         =  30,
  IOP_InterlockedCompareStore                        =  31,
  IOP_InterlockedCompareStoreFloatBitwise            =  32,
  IOP_InterlockedExchange                            =  33,
  IOP_InterlockedMax                                 =  34,
  IOP_InterlockedMin                                 =  35,
  IOP_InterlockedOr                                  =  36,
  IOP_InterlockedXor                                 =  37,
  IOP_NonUniformResourceIndex                        =  38,
  IOP_ObjectRayDirection                             =  39,
  IOP_ObjectRayOrigin                                =  40,
  IOP_ObjectToWorld                                  =  41,
  IOP_ObjectToWorld3x4                               =  42,
  IOP_ObjectToWorld4x3                               =  43,
  IOP_PrimitiveIndex                                 =  44,
  IOP_Process2DQuadTessFactorsAvg                    =  45,
  IOP_Process2DQuadTessFactorsMax                    =  46,
  IOP_Process2DQuadTessFactorsMin                    =  47,
  IOP_ProcessIsolineTessFactors                      =  48,
  IOP_ProcessQuadTessFactorsAvg                      =  49,
  IOP_ProcessQuadTessFactorsMax                      =  50,
  IOP_ProcessQuadTessFactorsMin                      =  51,
  IOP_ProcessTriTessFactorsAvg                       =  52,
  IOP_ProcessTriTessFactorsMax                       =  53,
  IOP_ProcessTriTessFactorsMin                       =  54,
  IOP_QuadReadAcrossDiagonal                         =  55,
  IOP_QuadReadAcrossX                                =  56,
  IOP_QuadReadAcrossY                                =  57,
  IOP_QuadReadLaneAt                                 =  58,
  IOP_RayFlags                                       =  59,
  IOP_RayTCurrent                                    =  60,
  IOP_RayTMin                                        =  61,
  IOP_ReportHit                                      =  62,
  IOP_SetMeshOutputCounts                            =  63,
  IOP_TraceRay                                       =  64,
  IOP_WaveActiveAllEqual                             =  65,
  IOP_WaveActiveAllTrue                              =  66,
  IOP_WaveActiveAnyTrue                              =  67,
  IOP_WaveActiveBallot                               =  68,
  IOP_WaveActiveBitAnd                               =  69,
  IOP_WaveActiveBitOr                                =  70,
  IOP_WaveActiveBitXor                               =  71,
  IOP_WaveActiveCountBits                            =  72,
  IOP_WaveActiveMax                                  =  73,
  IOP_WaveActiveMin                                  =  74,
  IOP_WaveActiveProduct                              =  75,
  IOP_WaveActiveSum                                  =  76,
  IOP_WaveGetLaneCount                               =  77,
  IOP_WaveGetLaneIndex                               =  78,
  IOP_WaveIsFirstLane                                =  79,
  IOP_WaveMatch                                      =  80,
  IOP_WaveMultiPrefixBitAnd                          =  81,
  IOP_WaveMultiPrefixBitOr                           =  82,
  IOP_WaveMultiPrefixBitXor                          =  83,
  IOP_WaveMultiPrefixCountBits                       =  84,
  IOP_WaveMultiPrefixProduct                         =  85,
  IOP_WaveMultiPrefixSum                             =  86,
  IOP_WavePrefixCountBits                            =  87,
  IOP_WavePrefixProduct                              =  88,
  IOP_WavePrefixSum                                  =  89,
  IOP_WaveReadLaneAt                                 =  90,
  IOP_WaveReadLaneFirst                              =  91,
  IOP_WorldRayDirection                              =  92,
  IOP_WorldRayOrigin                                 =  93,
  IOP_WorldToObject                                  =  94,
  IOP_WorldToObject3x4                               =  95,
  IOP_WorldToObject4x3                               =  96,
  IOP_abort                                          =  97,
  IOP_abs                                            =  98,
  IOP_acos                                           =  99,
  IOP_all                                            = 100,
  IOP_any                                            = 101,
  IOP_asdouble                                       = 102,
  IOP_asfloat                                        = 103,
  IOP_asfloat16                                      = 104,
  IOP_asin                                           = 105,
  IOP_asint                                          = 106,
  IOP_asint16                                        = 107,
  IOP_asuint                                         = 108,
  IOP_asuint16                                       = 109,
  IOP_atan                                           = 110,
  IOP_atan2                                          = 111,
  IOP_ceil                                           = 112,
  IOP_clamp                                          = 113,
  IOP_clip                                           = 114,
  IOP_cos                                            = 115,
  IOP_cosh                                           = 116,
  IOP_countbits                                      = 117,
  IOP_cross                                          = 118,
  IOP_ddx                                            = 119,
  IOP_ddx_coarse                                     = 120,
  IOP_ddx_fine                                       = 121,
  IOP_ddy                                            = 122,
  IOP_ddy_coarse                                     = 123,
  IOP_ddy_fine                                       = 124,
  IOP_degrees                                        = 125,
  IOP_determinant                                    = 126,
  IOP_distance                                       = 127,
  IOP_dot                                            = 128,
  IOP_dot2add                                        = 129,
  IOP_dot4add_i8packed                               = 130,
  IOP_dot4add_u8packed                               = 131,
  IOP_dst                                            = 132,
  IOP_exp                                            = 133,
  IOP_exp2                                           = 134,
  IOP_f16tof32                                       = 135,
  IOP_f32tof16                                       = 136,
  IOP_faceforward                                    = 137,
  IOP_firstbithigh                                   = 138,
  IOP_firstbitlow                                    = 139,
  IOP_floor                                          = 140,
  IOP_fma                                            = 141,
  IOP_fmod                                           = 142,
  IOP_frac                                           = 143,
  IOP_frexp                                          = 144,
  IOP_fwidth                                         = 145,
  IOP_isfinite                                       = 146,
  IOP_isinf                                          = 147,
  IOP_isnan                                          = 148,
  IOP_ldexp                                          = 149,
  IOP_length                                         = 150,
  IOP_lerp                                           = 151,
  IOP_lit                                            = 152,
  IOP_log                                            = 153,
  IOP_log10                                          = 154,
  IOP_log2                                           = 155,
  IOP_mad                                            = 156,
  IOP_max                                            = 157,
  IOP_min                                            = 158,
  IOP_modf                                           = 159,
  IOP_msad4                                          = 160,
  IOP_mul                                            = 161,
  IOP_normalize                                      = 162,
  IOP_pack_clamp_s8                                  = 163,
  IOP_pack_clamp_u8                                  = 164,
  IOP_pack_s8                                        = 165,
  IOP_pack_u8                                        = 166,
  IOP_pow                                            = 167,
  IOP_printf                                         = 168,
  IOP_radians                                        = 169,
  IOP_rcp                                            = 170,
  IOP_reflect                                        = 171,
  IOP_refract                                        = 172,
  IOP_reversebits                                    = 173,
  IOP_round                                          = 174,
  IOP_rsqrt                                          = 175,
  IOP_saturate                                       = 176,
  IOP_sign                                           = 177,
  IOP_sin                                            = 178,
  IOP_sincos                                         = 179,
  IOP_sinh                                           = 180,
  IOP_smoothstep                                     = 181,
  IOP_source_mark                                    = 182,
  IOP_sqrt                                           = 183,
  IOP_step                                           = 184,
  IOP_tan                                            = 185,
  IOP_tanh                                           = 186,
  IOP_tex1D                                          = 187,
  IOP_tex1Dbias                                      = 188,
  IOP_tex1Dgrad                                      = 189,
  IOP_tex1Dlod                                       = 190,
  IOP_tex1Dproj                                      = 191,
  IOP_tex2D                                          = 192,
  IOP_tex2Dbias                                      = 193,
  IOP_tex2Dgrad                                      = 194,
  IOP_tex2Dlod                                       = 195,
  IOP_tex2Dproj                                      = 196,
  IOP_tex3D                                          = 197,
  IOP_tex3Dbias                                      = 198,
  IOP_tex3Dgrad                                      = 199,
  IOP_tex3Dlod                                       = 200,
  IOP_tex3Dproj                                      = 201,
  IOP_texCUBE                                        = 202,
  IOP_texCUBEbias                                    = 203,
  IOP_texCUBEgrad                                    = 204,
  IOP_texCUBElod                                     = 205,
  IOP_texCUBEproj                                    = 206,
  IOP_transpose                                      = 207,
  IOP_trunc                                          = 208,
  IOP_unpack_s8s16                                   = 209,
  IOP_unpack_s8s32                                   = 210,
  IOP_unpack_u8u16                                   = 211,
  IOP_unpack_u8u32                                   = 212,
  IOP_VkReadClock                                    = 213,
  MOP_Append                                         = 214,
  MOP_RestartStrip                                   = 215,
  MOP_CalculateLevelOfDetail                         = 216,
  MOP_CalculateLevelOfDetailUnclamped                = 217,
  MOP_GetDimensions                                  = 218,
  MOP_Load                                           = 219,
  MOP_Sample                                         = 220,
  MOP_SampleBias                                     = 221,
  MOP_SampleCmp                                      = 222,
  MOP_SampleCmpLevelZero                             = 223,
  MOP_SampleGrad                                     = 224,
  MOP_SampleLevel                                    = 225,
  MOP_Gather                                         = 226,
  MOP_GatherAlpha                                    = 227,
  MOP_GatherBlue                                     = 228,
  MOP_GatherCmp                                      = 229,
  MOP_GatherCmpAlpha                                 = 230,
  MOP_GatherCmpBlue                                  = 231,
  MOP_GatherCmpGreen                                 = 232,
  MOP_GatherCmpRed                                   = 233,
  MOP_GatherGreen                                    = 234,
  MOP_GatherRed                                      = 235,
  MOP_GetSamplePosition                              = 236,
  MOP_Load2                                          = 237,
  MOP_Load3                                          = 238,
  MOP_Load4                                          = 239,
  MOP_InterlockedAdd                                 = 240,
  MOP_InterlockedAdd64                               = 241,
  MOP_InterlockedAnd                                 = 242,
  MOP_InterlockedAnd64                               = 243,
  MOP_InterlockedCompareExchange                     = 244,
  MOP_InterlockedCompareExchange64                   = 245,
  MOP_InterlockedCompareExchangeFloatBitwise         = 246,
  MOP_InterlockedCompareStore                        = 247,
  MOP_InterlockedCompareStore64                      = 248,
  MOP_InterlockedCompareStoreFloatBitwise            = 249,
  MOP_InterlockedExchange                            = 250,
  MOP_InterlockedExchange64                          = 251,
  MOP_InterlockedExchangeFloat                       = 252,
  MOP_InterlockedMax                                 = 253,
  MOP_InterlockedMax64                               = 254,
  MOP_InterlockedMin                                 = 255,
  MOP_InterlockedMin64                               = 256,
  MOP_InterlockedOr                                  = 257,
  MOP_InterlockedOr64                                = 258,
  MOP_InterlockedXor                                 = 259,
  MOP_InterlockedXor64                               = 260,
  MOP_Store                                          = 261,
  MOP_Store2                                         = 262,
  MOP_Store3                                         = 263,
  MOP_Store4                                         = 264,
  MOP_DecrementCounter                               = 265,
  MOP_IncrementCounter                               = 266,
  MOP_Consume                                        = 267,
  MOP_WriteSamplerFeedback                           = 268,
  MOP_WriteSamplerFeedbackBias                       = 269,
  MOP_WriteSamplerFeedbackGrad                       = 270,
  MOP_WriteSamplerFeedbackLevel                      = 271,
  MOP_Abort                                          = 272,
  MOP_CandidateGeometryIndex                         = 273,
  MOP_CandidateInstanceContributionToHitGroupIndex   = 274,
  MOP_CandidateInstanceID                            = 275,
  MOP_CandidateInstanceIndex                         = 276,
  MOP_CandidateObjectRayDirection                    = 277,
  MOP_CandidateObjectRayOrigin                       = 278,
  MOP_CandidateObjectToWorld3x4                      = 279,
  MOP_CandidateObjectToWorld4x3                      = 280,
  MOP_CandidatePrimitiveIndex                        = 281,
  MOP_CandidateProceduralPrimitiveNonOpaque          = 282,
  MOP_CandidateTriangleBarycentrics                  = 283,
  MOP_CandidateTriangleFrontFace                     = 284,
  MOP_CandidateTriangleRayT                          = 285,
  MOP_CandidateType                                  = 286,
  MOP_CandidateWorldToObject3x4                      = 287,
  MOP_CandidateWorldToObject4x3                      = 288,
  MOP_CommitNonOpaqueTriangleHit                     = 289,
  MOP_CommitProceduralPrimitiveHit                   = 290,
  MOP_CommittedGeometryIndex                         = 291,
  MOP_CommittedInstanceContributionToHitGroupIndex   = 292,
  MOP_CommittedInstanceID                            = 293,
  MOP_CommittedInstanceIndex                         = 294,
  MOP_CommittedObjectRayDirection                    = 295,
  MOP_CommittedObjectRayOrigin                       = 296,
  MOP_CommittedObjectToWorld3x4                      = 297,
  MOP_CommittedObjectToWorld4x3                      = 298,
  MOP_CommittedPrimitiveIndex                        = 299,
  MOP_CommittedRayT                                  = 300,
  MOP_CommittedStatus                                = 301,
  MOP_CommittedTriangleBarycentrics                  = 302,
  MOP_CommittedTriangleFrontFace                     = 303,
  MOP_CommittedWorldToObject3x4                      = 304,
  MOP_CommittedWorldToObject4x3                      = 305,
  MOP_Proceed                                        = 306,
  MOP_RayFlags                                       = 307,
  MOP_RayTMin                                        = 308,
  MOP_TraceRayInline                                 = 309,
  MOP_WorldRayDirection                              = 310,
  MOP_WorldRayOrigin                                 = 311,
  MOP_SubpassLoad                                    = 312,
  IOP_InterlockedUMax                                = 313,
  IOP_InterlockedUMin                                = 314,
  IOP_WaveActiveUMax                                 = 315,
  IOP_WaveActiveUMin                                 = 316,
  IOP_WaveActiveUProduct                             = 317,
  IOP_WaveActiveUSum                                 = 318,
  IOP_WaveMultiPrefixUProduct                        = 319,
  IOP_WaveMultiPrefixUSum                            = 320,
  IOP_WavePrefixUProduct                             = 321,
  IOP_WavePrefixUSum                                 = 322,
  IOP_uabs                                           = 323,
  IOP_uclamp                                         = 324,
  IOP_ufirstbithigh                                  = 325,
  IOP_umad                                           = 326,
  IOP_umax                                           = 327,
  IOP_umin                                           = 328,
  IOP_umul                                           = 329,
  IOP_usign                                          = 330,
  MOP_InterlockedUMax                                = 331,
  MOP_InterlockedUMin                                = 332,
  Num_Intrinsics
};

inline bool HasUnsignedIntrinsicOpcode(IntrinsicOp opcode) {
  switch (opcode) {
/* <py>
import hctdb_instrhelp
</py> */

/* <py::lines('HLSL-HAS-UNSIGNED-INTRINSICS')>hctdb_instrhelp.has_unsigned_hlsl_intrinsics()</py>*/
// HLSL-HAS-UNSIGNED-INTRINSICS:BEGIN
  case IntrinsicOp::IOP_InterlockedMax:
  case IntrinsicOp::IOP_InterlockedMin:
  case IntrinsicOp::IOP_WaveActiveMax:
  case IntrinsicOp::IOP_WaveActiveMin:
  case IntrinsicOp::IOP_WaveActiveProduct:
  case IntrinsicOp::IOP_WaveActiveSum:
  case IntrinsicOp::IOP_WaveMultiPrefixProduct:
  case IntrinsicOp::IOP_WaveMultiPrefixSum:
  case IntrinsicOp::IOP_WavePrefixProduct:
  case IntrinsicOp::IOP_WavePrefixSum:
  case IntrinsicOp::IOP_abs:
  case IntrinsicOp::IOP_clamp:
  case IntrinsicOp::IOP_firstbithigh:
  case IntrinsicOp::IOP_mad:
  case IntrinsicOp::IOP_max:
  case IntrinsicOp::IOP_min:
  case IntrinsicOp::IOP_mul:
  case IntrinsicOp::IOP_sign:
  case IntrinsicOp::MOP_InterlockedMax:
  case IntrinsicOp::MOP_InterlockedMax64:
  case IntrinsicOp::MOP_InterlockedMin:
  case IntrinsicOp::MOP_InterlockedMin64:
// HLSL-HAS-UNSIGNED-INTRINSICS:END
    return true;
  default:
    return false;
  }
}

inline unsigned GetUnsignedIntrinsicOpcode(IntrinsicOp opcode) {
  switch (opcode) {
/* <py>
import hctdb_instrhelp
</py> */

/* <py::lines('HLSL-GET-UNSIGNED-INTRINSICS')>hctdb_instrhelp.get_unsigned_hlsl_intrinsics()</py>*/
// HLSL-GET-UNSIGNED-INTRINSICS:BEGIN
  case IntrinsicOp::IOP_InterlockedMax:
    return static_cast<unsigned>(IntrinsicOp::IOP_InterlockedUMax);
  case IntrinsicOp::IOP_InterlockedMin:
    return static_cast<unsigned>(IntrinsicOp::IOP_InterlockedUMin);
  case IntrinsicOp::IOP_WaveActiveMax:
    return static_cast<unsigned>(IntrinsicOp::IOP_WaveActiveUMax);
  case IntrinsicOp::IOP_WaveActiveMin:
    return static_cast<unsigned>(IntrinsicOp::IOP_WaveActiveUMin);
  case IntrinsicOp::IOP_WaveActiveProduct:
    return static_cast<unsigned>(IntrinsicOp::IOP_WaveActiveUProduct);
  case IntrinsicOp::IOP_WaveActiveSum:
    return static_cast<unsigned>(IntrinsicOp::IOP_WaveActiveUSum);
  case IntrinsicOp::IOP_WaveMultiPrefixProduct:
    return static_cast<unsigned>(IntrinsicOp::IOP_WaveMultiPrefixUProduct);
  case IntrinsicOp::IOP_WaveMultiPrefixSum:
    return static_cast<unsigned>(IntrinsicOp::IOP_WaveMultiPrefixUSum);
  case IntrinsicOp::IOP_WavePrefixProduct:
    return static_cast<unsigned>(IntrinsicOp::IOP_WavePrefixUProduct);
  case IntrinsicOp::IOP_WavePrefixSum:
    return static_cast<unsigned>(IntrinsicOp::IOP_WavePrefixUSum);
  case IntrinsicOp::IOP_abs:
    return static_cast<unsigned>(IntrinsicOp::IOP_uabs);
  case IntrinsicOp::IOP_clamp:
    return static_cast<unsigned>(IntrinsicOp::IOP_uclamp);
  case IntrinsicOp::IOP_firstbithigh:
    return static_cast<unsigned>(IntrinsicOp::IOP_ufirstbithigh);
  case IntrinsicOp::IOP_mad:
    return static_cast<unsigned>(IntrinsicOp::IOP_umad);
  case IntrinsicOp::IOP_max:
    return static_cast<unsigned>(IntrinsicOp::IOP_umax);
  case IntrinsicOp::IOP_min:
    return static_cast<unsigned>(IntrinsicOp::IOP_umin);
  case IntrinsicOp::IOP_mul:
    return static_cast<unsigned>(IntrinsicOp::IOP_umul);
  case IntrinsicOp::IOP_sign:
    return static_cast<unsigned>(IntrinsicOp::IOP_usign);
  case IntrinsicOp::MOP_InterlockedMax:
    return static_cast<unsigned>(IntrinsicOp::MOP_InterlockedUMax);
  case IntrinsicOp::MOP_InterlockedMax64:
    return static_cast<unsigned>(IntrinsicOp::MOP_InterlockedUMax);
  case IntrinsicOp::MOP_InterlockedMin:
    return static_cast<unsigned>(IntrinsicOp::MOP_InterlockedUMin);
  case IntrinsicOp::MOP_InterlockedMin64:
    return static_cast<unsigned>(IntrinsicOp::MOP_InterlockedUMin);
// HLSL-GET-UNSIGNED-INTRINSICS:END
  default:
    return static_cast<unsigned>(opcode);
  }
}

}
