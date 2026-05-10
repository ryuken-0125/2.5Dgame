$dir = 'C:\Users\honoka.T\Desktop\GameJam\2.5Dgame\lightvroom'
$files = @('scene_manager.cpp','collision.cpp','Move.cpp','Camera.cpp',
           'Texture.cpp','ShadowMap.cpp','Mesh.cpp','Graphics.cpp',
           'ShaderManager.cpp','main.cpp')
foreach ($f in $files) {
    $path = Join-Path $dir $f
    if (!(Test-Path $path)) { continue }
    $lines = [System.IO.File]::ReadAllLines($path, [System.Text.Encoding]::UTF8)
    $i = 0
    foreach ($line in $lines) {
        $i++
        $hasBadComment = ($line -match '繧') -or ($line -match '縺') -or ($line -match '繝')
        $hasCode = ($line -match 'XMVECTOR|XMMATRIX|XMFLOAT|float |int |auto |CBPer|bool ')
        if ($hasBadComment -and $hasCode) {
            $preview = $line.Substring(0, [Math]::Min(100, $line.Length))
            Write-Host ("SUSPECT {0}:{1}: {2}" -f $f, $i, $preview)
        }
    }
}
Write-Host "Scan complete."
