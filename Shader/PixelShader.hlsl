
#include "common.hlsl"


//*****************************************************************************
// グローバル変数
//*****************************************************************************
Texture2D g_Texture : register(t1);
SamplerState g_SamplerState : register(s1);


//=============================================================================
// ピクセルシェーダ
//=============================================================================
void main(  in float4 inPosition : SV_POSITION,
		    in float4 inNormal : NORMAL0,
		    in float2 inTexCoord : TEXCOORD0,
		    in float4 inDiffuse : COLOR0,
		    in float4 inWorldPos : POSITION0,

		    out float4 outDiffuse : SV_Target)
{
    float4 color;

    if (Material.noTexSampling == 0)
    {
        color = g_Texture.Sample(g_SamplerState, inTexCoord);

        color *= inDiffuse;
    }
    else
    {
        color = inDiffuse;
    }

    if (Light.Enable == 0)
    {
        color = color * Material.Diffuse;
    }
    else
    {
        float4 tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 outColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

        for (int i = 0; i < 5; i++)
        {
            float3 lightDir;
            float light;

            if (Light.Flags[i].y == 1)
            {
                if (Light.Flags[i].x == 1)
                {
                    lightDir = normalize(Light.Direction[i].xyz);
                    light = dot(lightDir, inNormal.xyz);

                    light = 0.5 - 0.5 * light;
                    tempColor = color * Material.Diffuse * light * Light.Diffuse[i];
                }
                else if (Light.Flags[i].x == 2)
                {
                    lightDir = normalize(Light.Position[i].xyz - inWorldPos.xyz);
                    light = dot(lightDir, inNormal.xyz);

                    tempColor = color * Material.Diffuse * light * Light.Diffuse[i];

                    float distance = length(inWorldPos - Light.Position[i]);

                    float att = saturate((Light.Attenuation[i].x - distance) / Light.Attenuation[i].x);
                    tempColor *= att;
                }
                else
                {
                    tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
                }

                outColor += tempColor;
            }
        }

        color = outColor;
        color.a = inDiffuse.a * Material.Diffuse.a;
    }

	//フォグ
    if (Fog.Enable == 1)
    {
        float z = inPosition.z * inPosition.w;
        float f = (Fog.Distance.y - z) / (Fog.Distance.y - Fog.Distance.x);
        f = saturate(f);
        outDiffuse = f * color + (1 - f) * Fog.FogColor;
        outDiffuse.a = color.a;
    }
    else
    {
        outDiffuse = color;
    }

	//縁取り
    if (fuchi == 1)
    {
        float angle = dot(normalize(inWorldPos.xyz - Camera.xyz), normalize(inNormal));
		//if ((angle < 0.5f)&&(angle > -0.5f))
        if (angle > -0.3f)
        {
            outDiffuse.rb = 1.0f;
            outDiffuse.g = 0.0f;
        }
    }
}