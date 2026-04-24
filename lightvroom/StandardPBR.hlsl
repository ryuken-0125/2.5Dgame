#include "PBR_Math.hlsli"

// ==========================================
// 定数バッファ (C++構造体と完全に一致)
// ==========================================
cbuffer cbPerFrame : register(b0)
{
    matrix viewProjection;
    matrix lightViewProjection;
    
    float3 cameraPos;
    float pad1;

    float3 sunDir;
    float pad2;
    float3 sunColor;
    float pad3;

    float3 moonDir;
    float pad4;
    float3 moonColor;
    float pad5;

    float3 lightDir;
    float pad6;
    float3 lightColor;
    float pad7;

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
// 1. 頂点シェーダー
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
    
    // 影計算用
    output.LightSpacePos = mul(worldPos, lightViewProjection);
    
    return output;
}

// ==========================================
// 2. PBR計算関数
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
// 3. スポットライト計算関数
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
// 5. ピクセルシェーダー
// ==========================================
float4 PSMain(PS_INPUT input) : SV_TARGET
{
    float3 albedo = materialAlbedo.rgb;
    
    if (useTexture > 0.5f)
    {
        float4 texColor = txAlbedo.Sample(samLinear, input.TexCoord);
        clip(texColor.a - 0.1f);
        albedo *= texColor.rgb;
    }

    float roughness = materialRoughness;
    float metallic = materialMetallic;
    float emissive = materialEmissive;

    if (emissive > 0.0f)
    {
        return float4(albedo * emissive, 1.0f);
    }

    float3 N = normalize(input.Normal);
    float3 V = normalize(cameraPos - input.WorldPos);
    float3 F0 = lerp(float3(0.04f, 0.04f, 0.04f), albedo, metallic);

    // 1. 環境光
    float3 ambient = albedo * skyColor.rgb * 0.5f;

    // 2. 各ライトの計算
    float3 colorSun   = CalculatePBR(N, V, normalize(-sunDir), albedo, roughness, metallic, F0, sunColor);
    float3 colorMoon  = CalculatePBR(N, V, normalize(-moonDir), albedo, roughness, metallic, F0, moonColor);
    float3 colorLight = CalculatePBR(N, V, normalize(-lightDir), albedo, roughness, metallic, F0, lightColor);
    float3 colorSpot  = CalculateSpotLight(N, V, input.WorldPos, albedo, roughness, metallic, F0);

    // 3. 影の適用 (平行光源とスポットライト両方に影を落としたい場合は調整が必要ですが、ここではメイン平行光源とスポットライトに対して適用します)
    float3 L_main = normalize(-lightDir);
    float shadow = CalculateShadow(input.LightSpacePos, N, L_main);
    shadow = max(shadow, 0.1f);

    // 4. 合成
    float3 finalColor = ambient + (colorSun + colorMoon + colorLight + colorSpot) * shadow;
    
    // トーンマップ
    finalColor = finalColor / (finalColor + float3(1.0, 1.0, 1.0));
    finalColor = pow(finalColor, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));

    return float4(finalColor, 1.0f);
}
