/*------------------------------
 * warp_zone.cpp 
 *------------------------------*/

#include "warp_zone.h"
#include <Windows.h>

using namespace DirectX;

void WarpZoneManager::AddZone(const AABB& bounds, int targetIndex,
                               const char* name, XMFLOAT4 color)
{
    WarpZone zone;
    zone.bounds      = bounds;
    zone.targetIndex = targetIndex;
    zone.name        = name;
    zone.color       = color;
    m_zones.push_back(zone);
}

//--------------------------------------
// Update: Press SPACE inside a zone to warp
//--------------------------------------
int WarpZoneManager::Update(const XMFLOAT3& playerPos)
{
    bool spaceNow = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;
    bool spaceJustPressed = spaceNow && !m_spaceWasDown;
    m_spaceWasDown = spaceNow;

    int result = -2;

    for (auto& zone : m_zones)
    {
        // 2D overlap check (ignore Y completely to prevent AABB strictness issues)
        bool inX = (playerPos.x >= zone.bounds.GetMin().x && playerPos.x <= zone.bounds.GetMax().x);
        bool inZ = (playerPos.z >= zone.bounds.GetMin().z && playerPos.z <= zone.bounds.GetMax().z);
        
        zone.isPlayerInside = (inX && inZ);

        if (zone.isPlayerInside && spaceJustPressed)
        {
            result = zone.targetIndex;
        }
    }

    return result;
}

//--------------------------------------
// Draw: Visualize warp zones as flat cubes
//--------------------------------------
void WarpZoneManager::Draw(ID3D11DeviceContext* ctx, ShaderManager* sm, Mesh* cubeMesh)
{
    for (const auto& zone : m_zones)
    {
        const XMFLOAT3& mn = zone.bounds.GetMin();
        const XMFLOAT3& mx = zone.bounds.GetMax();

        float cx = (mn.x + mx.x) * 0.5f;
        float cy = (mn.y + mx.y) * 0.5f;  // use center Y, not min Y
        float cz = (mn.z + mx.z) * 0.5f;
        float sx = mx.x - mn.x;
        float sz = mx.z - mn.z;

        XMMATRIX world = XMMatrixTranspose(
            XMMatrixScaling(sx, 0.4f, sz) *
            XMMatrixTranslation(cx, cy, cz)
        );

        CBPerObject obj;
        obj.worldMatrix = world;
        sm->UpdatePerObject(ctx, obj);

        CBPerMaterial mat;
        mat.albedo     = zone.color;
        mat.roughness  = 1.0f;
        mat.metallic   = 0.0f;
        mat.emissive   = zone.isPlayerInside ? 5.0f : 1.5f;  // Glow brightly when player is inside!
        mat.useTexture = 0.0f;
        sm->UpdatePerMaterial(ctx, mat);

        cubeMesh->Draw(ctx);
    }
}
