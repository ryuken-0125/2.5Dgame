#pragma once
/*------------------------------
 * tilemap.h
 * 2D TileMap class
 *------------------------------*/

#include <DirectXMath.h>
#include "ShaderManager.h"
#include "Mesh.h"

// Tile types
enum TileType
{
    TILE_EMPTY    = 0,  // Empty (nothing drawn)
    TILE_GROUND   = 1,  // Ground (solid floor)
    TILE_PLATFORM = 2,  // Platform (solid, floats)
    TILE_WALL     = 3,  // Wall
    TILE_EXIT     = 4,  // Exit / warp zone (drawn as beacon)
    TILE_DECO     = 5,  // Decorative tile
};

class TileMap
{
public:
    static const int MAP_WIDTH  = 20;
    static const int MAP_HEIGHT = 10;

    float tileSize = 2.0f;
    DirectX::XMFLOAT3 origin;

    int m_tiles[MAP_HEIGHT][MAP_WIDTH];

    TileMap();

    // Load from flat int array (MAP_HEIGHT * MAP_WIDTH elements)
    void LoadFromArray(const int* data);

    // Draw all tiles using cubeMesh
    void Draw(ID3D11DeviceContext* ctx, ShaderManager* sm, Mesh* cubeMesh);

    // Returns true if world-space (x,y) is inside a solid tile
    bool IsSolid(float worldX, float worldY) const;

    // Returns world-space position of the TILE_EXIT tile
    DirectX::XMFLOAT3 GetExitWorldPos() const;

    // Returns spawn position (top of first ground/platform tile from bottom)
    DirectX::XMFLOAT3 GetSpawnPosition() const;

private:
    DirectX::XMFLOAT4 GetTileColor(int tileType) const;
    float              GetTileEmissive(int tileType) const;
};
