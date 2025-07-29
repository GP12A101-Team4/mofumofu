
#include "common.hlsl"


//*****************************************************************************
// グローバル変数
//*****************************************************************************
//Texture2D		g_GameScene : register(t1); //テクスチャ変数
//SamplerState	g_Sampler	: register(s1); //サンプラー変数

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

//=============================================================================
// ピクセルシェーダ
//=============================================================================
float4 main(in float4 pos : SV_POSITION, 
            in float2 tex : TEXCOORD0) : SV_TARGET
{
    // 計算目前像素和中心點的距離（螢幕空間）
    float2 uv = tex;
    float dist = distance(uv, center);

    // 計算透明度（alpha）
    float alpha = 1.0;

    // 如果在半徑範圍內就透明	
    if (dist < radius)
    {
        alpha = 0.0;
    }
    else if (dist < radius + softness)
    {
        // 邊緣進行平滑過渡（0~1）
        alpha = (dist - radius) / softness;
    }

    float4 color = g_Texture.Sample(g_SamplerState, uv);
    color.a *= alpha;
    return color;
    
//    return float4(radius, 0, 0, 1);
}
