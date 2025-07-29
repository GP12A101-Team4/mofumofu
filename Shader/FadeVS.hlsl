#include "common.hlsl"
//=============================================================================
// 頂点シェーダ
//=============================================================================
void main(
    in float4 inPosition : POSITION0,
    in float2 inTexCoord : TEXCOORD0,

    out float4 outPosition : SV_POSITION,
    out float2 outTexCoord : TEXCOORD0
)
{
    // 模型 -> 世界 -> 相機 -> 投影
    outPosition = mul(mul(mul(inPosition, World), View), Projection);
    outTexCoord = inTexCoord;
}