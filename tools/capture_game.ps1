Add-Type -AssemblyName System.Drawing
Add-Type -AssemblyName System.Windows.Forms

# Set working directory to build\Release
Set-Location "D:\Projects\Project1-DMET502-DivasInTech-2Dgame\build\Release"

# Start the game
$proc = Start-Process -FilePath ".\NinjagoGame.exe" -PassThru
Start-Sleep -Seconds 3

# Helper to capture window
function Capture-Window($filename) {
    [System.Windows.Forms.SendKeys]::SendWait("%") # bring to focus
    $bounds = [System.Windows.Forms.Screen]::PrimaryScreen.Bounds
    $bmp = New-Object System.Drawing.Bitmap $bounds.Width, $bounds.Height
    $gfx = [System.Drawing.Graphics]::FromImage($bmp)
    $gfx.CopyFromScreen($bounds.Location, [System.Drawing.Point]::Empty, $bounds.Size)
    $outPath = "D:\Projects\Project1-DMET502-DivasInTech-2Dgame\$filename"
    $bmp.Save($outPath, [System.Drawing.Imaging.ImageFormat]::Png)
    $gfx.Dispose()
    $bmp.Dispose()
    Write-Host "Captured $outPath"
}

# Capture Main Menu
Capture-Window "screenshot_main_menu.png"

# Start Cutscene: click or press Enter on Start
# Click center of screen button: button is at center (x=640, y=410 relative to 720)
# Or send Enter or click
[System.Windows.Forms.Cursor]::Position = New-Object System.Drawing.Point(640, 390)
Start-Sleep -Milliseconds 500
# Send Left Click via mouse_event
$signature = @"
[DllImport("user32.dll",CharSet=CharSet.Auto, CallingConvention=CallingConvention.StdCall)]
public static extern void mouse_event(long dwFlags, long dx, long dy, long cButtons, long dwExtraInfo);
"@
$mouse = Add-Type -memberDefinition $signature -name "Win32MouseEvent" -namespace Win32Functions -passThru
$mouse::mouse_event(0x0002, 0, 0, 0, 0) # left down
Start-Sleep -Milliseconds 100
$mouse::mouse_event(0x0004, 0, 0, 0, 0) # left up

Start-Sleep -Seconds 3
Capture-Window "screenshot_cutscene.png"

# Skip Cutscene to Gameplay
[System.Windows.Forms.SendKeys]::SendWait(" ")
Start-Sleep -Seconds 2
Capture-Window "screenshot_gameplay.png"

# Close the game
Stop-Process -Id $proc.Id -Force
Write-Host "Done capture"

