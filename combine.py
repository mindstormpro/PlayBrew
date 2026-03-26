from hashlib import md5

print("Enter target system: A for Rev. A, B for Rev. B, or AB for both")
target = input("|)> ")

if "A" in target.upper():
	with open("pdfw-a", "rb") as f:
		fw_header = bytearray(f.read(32))
		fw = f.read()
	
	while bool(len(fw) & 3):
		fw += b"\x00"
	
	with open("build/loader-a.bin", "rb") as f:
		fw += f.read()
	
	fw_header[8:12] = len(fw).to_bytes(4, byteorder="little")
	fw_header[24:] = md5(fw).digest()[:8]
	
	with open("build/pdfw-a-patched.bin", "wb") as f:
		f.write(fw_header)
		f.write(fw)

if "B" in target.upper():
	with open("pdfw-b", "rb") as f:
		fw_header = bytearray(f.read(32))
		fw = f.read()
	
	while bool(len(fw) & 3):
		fw += b"\x00"
	
	with open("build/loader-b.bin", "rb") as f:
		fw += f.read()
	
	fw_header[8:12] = len(fw).to_bytes(4, byteorder="little")
	fw_header[24:] = md5(fw).digest()[:8]
	
	with open("build/pdfw-b-patched.bin", "wb") as f:
		f.write(fw_header)
		f.write(fw)
