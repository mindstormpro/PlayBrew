from hashlib import md5

with open("pdfw-b", "rb") as f:
	fw_header = bytearray(f.read(32))
	fw = f.read()

while bool(len(fw) & 3):
	fw += b"\x00"

with open("build/loader-b.bin", "rb") as f:
	fw += f.read()

fw_header[8:12] = len(fw).to_bytes(4, byteorder="little")
fw_header[16:] = md5(fw).digest()[:16]

with open("build/firmware_patched.bin", "wb") as f:
	f.write(fw_header)
	f.write(fw)