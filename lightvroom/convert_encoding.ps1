$dir = Split-Path -Parent $MyInvocation.MyCommand.Path
$files = @(
    'Graphics.h', 'ShaderManager.h', 'ConstantBuffer.h', 'Mesh.h',
    'Graphics.cpp', 'ShaderManager.cpp', 'Mesh.cpp',
    'Camera.h', 'Camera.cpp',
    'Application.h', 'Application.cpp',
    'ShadowMap.h', 'ShadowMap.cpp',
    'Texture.h', 'Texture.cpp',
    'Move.h', 'Move.cpp',
    'collision.h', 'collision.cpp',
    'field_scene.h', 'field_scene.cpp',
    'game_context.h', 'scene.h',
    'scene_manager.h', 'scene_manager.cpp',
    'sub_scene.h', 'sub_scene.cpp',
    'tilemap.h', 'tilemap.cpp',
    'warp_zone.h', 'warp_zone.cpp',
    'main.cpp'
)

$sjis   = [System.Text.Encoding]::GetEncoding(932)
$utf8bom = New-Object System.Text.UTF8Encoding($true)

foreach ($file in $files) {
    $path = Join-Path $dir $file
    if (Test-Path $path) {
        try {
            $bytes   = [System.IO.File]::ReadAllBytes($path)
            $content = $sjis.GetString($bytes)
            [System.IO.File]::WriteAllText($path, $content, $utf8bom)
            Write-Host "Converted: $file"
        } catch {
            Write-Host "Error on $file : $_"
        }
    } else {
        Write-Host "Not found: $file"
    }
}
Write-Host "All done."
