#pragma once
/*------------------------------
 * warp_zone.h
 * Manages warp zones in field / sub-scenes
 *------------------------------*/

#include <DirectXMath.h>
#include <vector>
#include <string>
#include "collision.h"
#include "ShaderManager.h"
#include "Mesh.h"

// Data for one warp zone
struct WarpZone
{
    AABB                  bounds;       // Bounding area
    int                   targetIndex;  // Destination scene index (-1 = return to field)
    std::string           name;         // Display name (for confirmation dialog)
    DirectX::XMFLOAT4     color;        // Visualization color (RGBA)
    bool                  isPlayerInside = false; // For visual feedback
};

// Manages a collection of WarpZone entries
class WarpZoneManager
{
public:
    WarpZoneManager() = default;

    // Add a zone
    void AddZone(const AABB& bounds, int targetIndex,
                 const char* name, DirectX::XMFLOAT4 color);

    // Call each frame. Returns targetIndex when SPACE is pressed inside a zone,
    // or -2 if nothing happened.
    int Update(const DirectX::XMFLOAT3& playerPos);

    // Draw zones as flat cubes
    void Draw(ID3D11DeviceContext* ctx, ShaderManager* sm, Mesh* cubeMesh);

    const std::vector<WarpZone>& GetZones() const { return m_zones; }

private:
    std::vector<WarpZone> m_zones;
    bool m_spaceWasDown = false;
};
