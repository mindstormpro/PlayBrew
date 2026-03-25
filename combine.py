with open("pdfw-b", "rb") as f:
    fw = f.read()

with open("build/loader-b.bin", "rb") as f:
    patch = f.read()

padding = (4 - (len(fw) % 4)) % 4
fw_padded = fw + b'\x00' * padding
combined = fw_padded + patch

with open("build/firmware_patched.bin", "wb") as f:
    f.write(combined)