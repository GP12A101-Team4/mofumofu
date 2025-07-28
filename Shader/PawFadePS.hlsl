
#include "common.hlsl"
//*****************************************************************************
// グローバル変数
//*****************************************************************************
Texture2D g_SceneTexture : register(t0); // 原畫面
Texture2D g_MaskTexture : register(t1); // 遮罩（透明背景 PNG）
SamplerState g_Sampler : register(s0);

//=============================================================================
// ピクセルシェーダ
//=============================================================================


float4 main(float4 svPos : SV_POSITION, float2 uv : TEXCOORD) : SV_TARGET
{
    float2 center = float2(0.5, 0.5);
    float2 delta = uv - center;
    float2 scaledUV = center + delta / PawFade.g_FadeAmount;
    scaledUV = clamp(scaledUV, 0.0, 1.0);

    float4 maskColor = g_MaskTexture.Sample(g_Sampler, scaledUV);


    float alpha = saturate(maskColor.a);

    float4 sceneColor = g_SceneTexture.Sample(g_Sampler, uv);
    return lerp(float4(0, 0, 0, 1), sceneColor, alpha);
}
