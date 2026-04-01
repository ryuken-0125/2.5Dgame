cbuffer cbPerFrame : register(b0)
{
    matrix viewProjection;
    matrix lightViewProjection;
    float3 cameraPos;
    float pad1;
    float3 lightDir;
    float pad2;
    float3 lightColor;
    float pad3;
    float3 sunDir;
    float pad4;
    float3 sunColor;
    float pad5;
    float3 moonDir;
    float pad6;
    float3 moonColor;
    float pad7;
    float4 skyColor;
}
cbuffer cbPerObject : register(b1)
{
    matrix worldMatrix;
}

// ★追加：影のパスでも材質情報と画像を受け取る
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

// ★変更：UV座標もPSに送るようにする
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