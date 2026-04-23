#pragma once
/*------------------------------
 * game_context.h
 * Shared DirectX resources passed between scenes
 *------------------------------*/

#include "Graphics.h"
#include "ShaderManager.h"
#include "Mesh.h"
#include "ShadowMap.h"
#include "Texture.h"

struct GameContext
{
    Graphics*      graphics      = nullptr;
    ShaderManager* shaderManager = nullptr;

    Mesh* quadMesh    = nullptr;  // 2D sprite quad
    Mesh* cubeMesh    = nullptr;  // Block / warp zone visualization
    Mesh* floorMesh   = nullptr;  // Floor (scaled cube)
    Mesh* sphereMesh  = nullptr;  // Sphere

    ShadowMap* shadowMap      = nullptr;
    Texture*   playerTexture  = nullptr;
    Texture*   tilesetTexture = nullptr;
};
