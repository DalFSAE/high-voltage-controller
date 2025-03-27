# serial_monitor.ps1

$portName = "COM5"
$baudRate = 115200



$port = New-Object System.IO.Ports.SerialPort $portName, $baudRate, "None", 8, "One"

if ($port.IsOpen) {
    $port.Close()
}

$port.Open()

Write-Host "Connected to $portName at $baudRate baud."
Write-Host "Press Ctrl+C to exit."

try {
    while ($true) {
        if ($port.BytesToRead -gt 0) {
            $data = $port.ReadExisting()
            Write-Host -NoNewline $data
        }
        Start-Sleep -Milliseconds 100
    }
} finally {
    $port.Close()
    Write-Host "`nPort closed."
}
