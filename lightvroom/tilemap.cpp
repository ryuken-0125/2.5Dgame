/*------------------------------
 * tilemap.cpp
 *------------------------------*/
#include "tilemap.h"
#include <cstring>
#include <cmath>
using namespace DirectX;

TileMap::TileMap()
{
    origin = XMFLOAT3(-20.0f, 0.0f, 0.0f);
    memset(m_tiles, 0, sizeof(m_tiles));
}

void TileMap::LoadFromArray(const int* data)
{
    for (int r = 0; r < MAP_HEIGHT; ++r)
        for (int c = 0; c < MAP_WIDTH; ++c)
            m_tiles[r][c] = data[r * MAP_WIDTH + c];
}

void TileMap::Draw(ID3D11DeviceContext* ctx, ShaderManager* sm, Mesh* cubeMesh)
{
    for (int row = 0; row < MAP_HEIGHT; ++row)
    {
        for (int col = 0; col < MAP_WIDTH; ++col)
        {
            int t = m_tiles[row][col];
            if (t == TILE_EMPTY) continue;

            float wx = origin.x + col * tileSize + tileSize * 0.5f;
            float wy = origin.y + (MAP_HEIGHT - 1 - row) * tileSize;
            float wz = 0.0f;

            CBPerObject obj;
            obj.worldMatrix = XMMatrixTranspose(
                XMMatrixScaling(tileSize, tileSize, tileSize) *
                XMMatrixTranslation(wx, wy, wz)
            );
            sm->UpdatePerObject(ctx, obj);

            CBPerMaterial mat;
            mat.albedo = GetTileColor(t);
            mat.roughness = (t == TILE_PLATFORM) ? 0.2f : 0.8f;
            mat.metallic = 0.0f;
            mat.emissive = GetTileEmissive(t);
            mat.useTexture = 0.0f;
            sm->UpdatePerMaterial(ctx, mat);

            cubeMesh->Draw(ctx);
        }
    }
}

bool TileMap::IsSolid(float worldX, float worldY) const
{
    float localX = worldX - origin.x;
    float localY = worldY - origin.y;

    int col = (int)floorf(localX / tileSize);
    int row = MAP_HEIGHT - 1 - (int)floorf(localY / tileSize);

    if (col < 0 || col >= MAP_WIDTH || row < 0 || row >= MAP_HEIGHT)
        return false;

    int t = m_tiles[row][col];
    return (t == TILE_GROUND || t == TILE_PLATFORM || t == TILE_WALL);
}

XMFLOAT3 TileMap::GetExitWorldPos() const
{
    for (int row = 0; row < MAP_HEIGHT; ++row)
        for (int col = 0; col < MAP_WIDTH; ++col)
            if (m_tiles[row][col] == TILE_EXIT)
            {
                float wx = origin.x + col * tileSize + tileSize * 0.5f;
                float wy = origin.y + (MAP_HEIGHT - 1 - row) * tileSize;
                return XMFLOAT3(wx, wy, 0.0f);
            }
    return origin;
}

XMFLOAT3 TileMap::GetSpawnPosition() const
{
    for (int row = MAP_HEIGHT - 1; row >= 0; --row)
        for (int col = 0; col < MAP_WIDTH; ++col)
        {
            int t = m_tiles[row][col];
            if (t == TILE_GROUND || t == TILE_PLATFORM)
            {
                float wx = origin.x + col * tileSize + tileSize * 0.5f;
                float wy = origin.y + (MAP_HEIGHT - 1 - row) * tileSize + tileSize;
                return XMFLOAT3(wx, wy, 0.0f);
            }
        }
    return XMFLOAT3(0.0f, 4.0f, 0.0f);
}

XMFLOAT4 TileMap::GetTileColor(int t) const
{
    switch (t)
    {
    case TILE_GROUND:   return XMFLOAT4(0.25f, 0.60f, 0.20f, 1.0f);
    case TILE_PLATFORM: return XMFLOAT4(0.55f, 0.55f, 0.60f, 1.0f);
    case TILE_WALL:     return XMFLOAT4(0.30f, 0.25f, 0.20f, 1.0f);
    case TILE_EXIT:     return XMFLOAT4(0.10f, 0.90f, 0.90f, 1.0f);
    case TILE_DECO:     return XMFLOAT4(0.80f, 0.70f, 0.20f, 1.0f);
    default:            return XMFLOAT4(0.50f, 0.50f, 0.50f, 1.0f);
    }
}

float TileMap::GetTileEmissive(int t) const
{
    return (t == TILE_EXIT) ? 1.5f : 0.0f;
}