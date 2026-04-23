cbuffer cbPerFrame : register(b0)
{
    matrix viewProjection;
    matrix lightViewProjection;
    float3 cameraPos;
    float pad1;
    
    // 使わなくても、サイズ合わせのために定義だけは必須
    float3 spotPos;
    float spotRange;
    float3 spotDir;
    float spotCosInner;
    float3 spotColor;
    float spotCosOuter;
    
    float4 skyColor;
}

cbuffer cbPerObject : register(b1)
{
    matrix worldMatrix;
}

// ：影のパスでも材質情報と画像を受け取る
cbuffer cbPerMaterial : register(b2)
{
    float4 materialAlbedo;
    float materialRoughness;
    float materialMetallic;
    float materialEmissive;
    float useTexture;
}

Texture2D txAlbedo : register(t0);
SamplerState samLinear : register(s0);

struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
    float3 Tangent : TANGENT;
};

//UV座標もPSに送るようにする
struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output;
    float4 worldPos = mul(float4(input.Pos, 1.0f), worldMatrix);
    output.Pos = mul(worldPos, lightViewProjection);
    output.TexCoord = input.TexCoord;
    return output;
}

//影の切り抜き用ピクセルシェーダー
void PSMain(PS_INPUT input)
{
    if (useTexture > 0.5f)
    {
        float4 texColor = txAlbedo.Sample(samLinear, input.TexCoord);
        clip(texColor.a - 0.1f); // 透明な部分は影も落とさない！
    }
}