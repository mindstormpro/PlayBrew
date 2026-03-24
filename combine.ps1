$fwSize = Get-Item "pdfw-b"
$size = $fwSize.length
$padding = 4 - ($size % 4)

$fw = [System.IO.File]::ReadAllBytes("pdfw-b")
$patch = [System.IO.File]::ReadAllBytes("loader-b.bin")
$combined = New-Object byte[] ($fw.Length + $patch.Length + $padding)

if ($padding -gt 0) {
    $bytes = [byte[]]::new($padding)
    $bytes.CopyTo($combined, $fw.Length)
}

$patch.CopyTo($combined, $fw.Length + $padding)
[System.IO.File]::WriteAllBytes("firmware_patched.bin", $combined)