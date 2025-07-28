//*****************************************************************************
// 定数バッファ
//*****************************************************************************
// マテリアルバッファ
struct MATERIAL
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emission;
    float Shininess;
    int noTexSampling;
    float Dummy[2]; //16byte境界用
};

struct FOG
{
    float4 Distance;
    float4 FogColor;
    int Enable;
    float Dummy[3]; //16byte境界用
};

// ライト用バッファ
struct LIGHT
{
    float4 Direction[5];
    float4 Position[5];
    float4 Diffuse[5];
    float4 Ambient[5];
    float4 Attenuation[5];
    int4 Flags[5];
    int Enable;
    int Dummy[3]; //16byte境界用
};

struct PAWFADE
{
    float g_FadeAmount; // 控制遮罩大小，例如 0.0（小）～1.5（大）
    float Dummy[3];
};


cbuffer WorldBuffer : register(b0)
{
    matrix World;
}

cbuffer ViewBuffer : register(b1)
{
    matrix View;
}

cbuffer ProjectionBuffer : register(b2)
{
    matrix Projection;
}

cbuffer MaterialBuffer : register(b3)
{
    MATERIAL Material;
}

cbuffer LightBuffer : register(b4)
{
    LIGHT Light;
}


// フォグ用バッファ
cbuffer FogBuffer : register(b5)
{
    FOG Fog;
};

// 縁取り用バッファ
cbuffer Fuchi : register(b6)
{
    int fuchi;
    int fill[3];
};


cbuffer CameraBuffer : register(b7)
{
    float4 Camera;
}

cbuffer FadeBuffer : register(b8)
{
    int         Enable;
    float2      center;
    float       radius;
    float       softness;
    float       padding[3]; // ← 額外補滿 16 bytes，選擇性加
};

cbuffer PawFadeBuffer : register(b9)
{
    PAWFADE PawFade;
};



