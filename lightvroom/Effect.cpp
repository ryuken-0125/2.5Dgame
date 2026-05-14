#include "Effect.h"
#include "PlayerManager.h"


const int   EFFECT_COLUMNS = 8;     // 横のコマ数
const int   EFFECT_ROWS = 4;     // 縦のコマ数
const int   EFFECT_MAX_FRAMES = 24;    // 総フレーム数
const float EFFECT_FRAME_TIME = 0.03f; // 1コマの表示時間（秒）
const float EFFECT_SCALE = 10.0f;  // エフェクトの大きさ

Effect::Effect(): 
    m_position(0.0f, 0.0f, 0.0f)
    , m_isActive(false)
    , m_currentFrame(0)
    , m_frameTimer(0.0f)
{
}

Effect::~Effect() { Release(); }

bool Effect::Initialize(ID3D11Device* device, const char* filePath)
{
    m_texture = std::make_unique<Texture>();
    return m_texture->Load(device, filePath);
}

void Effect::Release()
{
    m_texture.reset();
}

void Effect::Play(const DirectX::XMFLOAT3& position)
{
    m_position = position;
    m_position.y += 1.0f; // 地面に埋まらないように少し上に浮かせる
    m_isActive = true;
    m_currentFrame = 0;
    m_frameTimer = 0.0f;
}

void Effect::Update(float deltaTime)
{
    if (!m_isActive) return;

    m_frameTimer += deltaTime;
    if (m_frameTimer >= EFFECT_FRAME_TIME)
    {
        m_frameTimer -= EFFECT_FRAME_TIME; // タイマーのズレを防止
        m_currentFrame++;

        // 最後のコマまで再生したら非アクティブ化
        if (m_currentFrame >= EFFECT_MAX_FRAMES)
        {
            m_isActive = false;
        }
    }
}

void Effect::Draw(ID3D11DeviceContext* context, ShaderManager* shaderManager, Mesh* quadMesh)
{
    if (!m_isActive) return;

    using namespace DirectX;

    // 現在のコマの行と列を計算
    int currentColumn = m_currentFrame % EFFECT_COLUMNS;
    int currentRow = m_currentFrame / EFFECT_COLUMNS;

    // シェーダーに送る「切り取るサイズ」と「開始位置」
    XMFLOAT2 uvScale(1.0f / EFFECT_COLUMNS, 1.0f / EFFECT_ROWS);
    XMFLOAT2 uvOffset(currentColumn * uvScale.x, currentRow * uvScale.y);

    CBPerObject objectData;
    XMMATRIX scaleMatrix = XMMatrixScaling(EFFECT_SCALE, EFFECT_SCALE, EFFECT_SCALE);
    XMMATRIX rotationMatrix = XMMatrixRotationX(XMConvertToRadians(30.0f));
    XMMATRIX translationMatrix = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

    objectData.worldMatrix = XMMatrixTranspose(scaleMatrix * rotationMatrix * translationMatrix);
    objectData.uvOffset = uvOffset;
    objectData.uvScale = uvScale;
    shaderManager->UpdatePerObject(context, objectData);

    // エフェクトは自ら発光させる (Emissive = 1.0f)
    CBPerMaterial materialData = { XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0.0f, 1.0f, 1.0f };
    shaderManager->UpdatePerMaterial(context, materialData);

    ID3D11ShaderResourceView* shaderResourceView = m_texture->GetSRV();
    context->PSSetShaderResources(1, 1, &shaderResourceView);

    quadMesh->Draw(context);
}