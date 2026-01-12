# PowerShell スクリプト：main.cpp → .ino に変換して export フォルダを開く

$workspace = $PSScriptRoot
$src = Join-Path $workspace "src\main.cpp"
$outDir = Join-Path $workspace "export"
$dst = Join-Path $outDir "06_LED_Blink_SW.ino"

# export フォルダがなければ作成
New-Item -ItemType Directory -Force -Path $outDir | Out-Null

# .ino ファイルとしてコピー
Copy-Item -LiteralPath $src -Destination $dst -Force

# エクスプローラで export フォルダを開く
Start-Process explorer.exe -ArgumentList $outDir
