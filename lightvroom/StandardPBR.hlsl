#include "PBR_Math.hlsli"

// ==========================================
// 定数バッファ (C++と完全に一致させる)
// ==========================================
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
    
    // スポットライト用変数
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

cbuffer cbPerMaterial : register(b2)
{
    float4 materialAlbedo;
    float materialRoughness;
    float materialMetallic;
    float materialEmissive;
    float useTexture;
}

Texture2D txShadowMap : register(t0);
Texture2D txAlbedo : register(t1);
SamplerState samLinear : register(s0);
SamplerState samClamp : register(s1);

// ==========================================
// 構造体
// ==========================================
struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
    float3 Tangent : TANGENT;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 WorldPos : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
    float4 LightSpacePos : TEXCOORD1;
};

// ==========================================
// 1. 頂点シェーダー (消えていたものを復活！)
// ==========================================
PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output;
    float4 worldPos = mul(float4(input.Pos, 1.0f), worldMatrix);
    output.WorldPos = worldPos.xyz;
    output.Pos = mul(worldPos, viewProjection);
    output.TexCoord = input.TexCoord;
    output.Normal = normalize(mul(input.Normal, (float3x3) worldMatrix));
    output.Tangent = normalize(mul(input.Tangent, (float3x3) worldMatrix));
    output.Binormal = cross(output.Normal, output.Tangent);
    
    // 影の計算用に、光源から見た座標も保存
    output.LightSpacePos = mul(worldPos, lightViewProjection);
    
    return output;
}

// ==========================================
// 2. PBRの光計算関数
// ==========================================
float3 CalculatePBR(float3 N, float3 V, float3 L, float3 albedo, float roughness, float metallic, float3 F0, float3 lightColor)
{
    float3 H = normalize(V + L);
    float NdotV = max(dot(N, V), 0.0001);
    float NdotL = max(dot(N, L), 0.0);
    float NdotH = max(dot(N, H), 0.0);

    float NDF = D_GGX(NdotH, roughness);
    float G = GeometrySmith(NdotV, NdotL, roughness);
    float3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    float3 numerator = NDF * G * F;
    float denominator = 4.0 * NdotV * NdotL + 0.0001;
    float3 specular = numerator / denominator;

    float3 kS = F;
    float3 kD = float3(1.0, 1.0, 1.0) - kS;
    kD *= 1.0 - metallic;

    return (kD * albedo / PI + specular) * lightColor * NdotL;
}

// ==========================================
// 3. スポットライト計算関数 (必ず CalculatePBR の下)
// ==========================================
float3 CalculateSpotLight(float3 N, float3 V, float3 worldPos, float3 albedo, float roughness, float metallic, float3 F0)
{
    float3 L = normalize(spotPos - worldPos);
    float dist = length(spotPos - worldPos);
    
    float attenuation = saturate(1.0 - dist / spotRange);
    
    float theta = dot(L, normalize(-spotDir));
    float epsilon = spotCosInner - spotCosOuter;
    float intensity = saturate((theta - spotCosOuter) / epsilon);
    
    float3 lighting = CalculatePBR(N, V, L, albedo, roughness, metallic, F0, spotColor);
    
    return lighting * attenuation * intensity;
}

// ==========================================
// 4. 影の計算関数
// ==========================================
float CalculateShadow(float4 lightSpacePos, float3 N, float3 L)
{
    float3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    projCoords.x = projCoords.x * 0.5f + 0.5f;
    projCoords.y = -projCoords.y * 0.5f + 0.5f;

    float shadow = 1.0f;
    
    if (projCoords.x >= 0.0f && projCoords.x <= 1.0f &&
        projCoords.y >= 0.0f && projCoords.y <= 1.0f &&
        projCoords.z >= 0.0f && projCoords.z <= 1.0f)
    {
        shadow = 0.0f;
        float bias = max(0.005f * (1.0f - dot(N, L)), 0.0005f);
        float2 texelSize = 1.0f / 2048.0f;
        
        [unroll]
        for (int x = -1; x <= 1; ++x)
        {
            for (int y = -1; y <= 1; ++y)
            {
                float pcfDepth = txShadowMap.Sample(samClamp, projCoords.xy + float2(x, y) * texelSize).r;
                shadow += (projCoords.z - bias > pcfDepth) ? 0.0f : 1.0f;
            }
        }
        shadow /= 9.0f;
    }
    return shadow;
}

// ==========================================
// 5. ピクセルシェーダー (一番下)
// ==========================================
float4 PSMain(PS_INPUT input) : SV_TARGET
{
    float3 albedo = materialAlbedo.rgb;
    
    // 画像の透明部分の切り抜き
    if (useTexture > 0.5f)
    {
        float4 texColor = txAlbedo.Sample(samLinear, input.TexCoord);
        clip(texColor.a - 0.1f);
        albedo *= texColor.rgb;
    }

    float roughness = materialRoughness;
    float metallic = materialMetallic;
    float emissive = materialEmissive;

    // 自ら光る物体（上位存在の目や月）はそのまま明るく描画
    if (emissive > 0.0f)
    {
        return float4(albedo * emissive, 1.0f);
    }

    float3 N = normalize(input.Normal);
    float3 V = normalize(cameraPos - input.WorldPos);
    float3 F0 = lerp(float3(0.04f, 0.04f, 0.04f), albedo, metallic);

    // 1. 環境光 (光が当たっていない部分の基本の明るさ)
    float3 ambient = albedo * skyColor.rgb * 0.5f;

    // 2. スポットライトの光の強さ
    float3 spotLightColor = CalculateSpotLight(N, V, input.WorldPos, albedo, roughness, metallic, F0);

    // 3. 影の濃さ (CalculateShadowを使って綺麗にぼかす)
    float3 L_spot = normalize(spotPos - input.WorldPos);
    float shadow = CalculateShadow(input.LightSpacePos, N, L_spot);
    
    // 完全に真っ黒になるのを防ぐ (0.1f)
    shadow = max(shadow, 0.1f);

    // 4. 最終的な色の合成 (環境光 ＋ スポットライト × 影)
    float3 finalColor = ambient + (spotLightColor * shadow);
    
    // トーンマップとガンマ補正 (画面を綺麗に発色させる処理)
    finalColor = finalColor / (finalColor + float3(1.0, 1.0, 1.0));
    finalColor = pow(finalColor, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));

    return float4(finalColor, 1.0f);
}