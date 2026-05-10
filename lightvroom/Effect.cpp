#include "Effect.h"

const int EFFECT_COLUMNS = 8;        // 横のコマ数
const int EFFECT_ROWS = 3;           // 縦のコマ数
const int EFFECT_MAX_FRAMES = 24;    // 総フレーム数
const float EFFECT_FRAME_TIME = 0.03f; // 1コマの表示時間（秒）

Effect::Effect()
    : m_position(0.0f, 0.0f, 0.0f)
    , m_isActive(false)
    , m_currentFrame(0)
    , m_frameTimer(0.0f)
{
}

Effect::~Effect() { Release(); }

bool Effect::Initialize(ID3D11Device* device, const char* filePath)
{
    m_quadMesh = std::make_unique<Mesh>();
    if (!m_quadMesh->CreateQuad(device)) return false;

    m_texture = std::make_unique<Texture>();
    if (!m_texture->Load(device, filePath)) return false;

    return true;
}

void Effect::Release()
{
    m_quadMesh.reset();
    m_texture.reset();
}

void Effect::Play(const DirectX::XMFLOAT3& position)
{
    m_position = position;
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
        m_frameTimer = 0.0f;
        m_currentFrame++;

        // 最後のコマまで再生したら終了
        if (m_currentFrame >= EFFECT_MAX_FRAMES)
        {
            m_isActive = false;
        }
    }
}

void Effect::Draw(ID3D11DeviceContext* context, ShaderManager* shaderManager)
{
    if (!m_isActive) return;

    using namespace DirectX;

    // 現在のコマの行と列を計算
    int currentColumn = m_currentFrame % EFFECT_COLUMNS;
    int currentRow = m_currentFrame / EFFECT_COLUMNS;

    // 切り取るサイズと位置の計算
    XMFLOAT2 uvScale(1.0f / EFFECT_COLUMNS, 1.0f / EFFECT_ROWS);
    XMFLOAT2 uvOffset(currentColumn * uvScale.x, currentRow * uvScale.y);

    CBPerObject objectData;
    XMMATRIX scaleMatrix = XMMatrixScaling(2.5f, 2.5f, 2.5f);
    XMMATRIX rotationMatrix = XMMatrixRotationX(XMConvertToRadians(30.0f));
    XMMATRIX translationMatrix = XMMatrixTranslation(m_position.x, m_position.y + 1.0f, m_position.z);

    objectData.worldMatrix = XMMatrixTranspose(scaleMatrix * rotationMatrix * translationMatrix);
    objectData.uvOffset = uvOffset;
    objectData.uvScale = uvScale;
    shaderManager->UpdatePerObject(context, objectData);

    // エフェクトは自ら光る（Emissive = 1.0f）ように設定して目立たせる
    CBPerMaterial materialData = { XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 0.0f, 1.0f, 1.0f };
    shaderManager->UpdatePerMaterial(context, materialData);

    ID3D11ShaderResourceView* shaderResourceView = m_texture->GetSRV();
    context->PSSetShaderResources(1, 1, &shaderResourceView);

    m_quadMesh->Draw(context);
}