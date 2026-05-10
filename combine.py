from hashlib import md5
import struct

from firmware_addresses import DEFAULT_VERSION, FirmwareAddressError, get_firmware_addresses


FIRMWARE_HEADER_SIZE = 32
LENGTH_OFFSET = 8
HASH_OFFSET = 24
HASH_SIZE = 8


def read_firmware_image(path):
	with open(path, "rb") as f:
		fw_header = bytearray(f.read(FIRMWARE_HEADER_SIZE))
		fw = f.read()

	if len(fw_header) != FIRMWARE_HEADER_SIZE:
		raise ValueError(f"{path}: expected 32-byte header, got {len(fw_header)} bytes")

	expected_length = int.from_bytes(fw_header[LENGTH_OFFSET:LENGTH_OFFSET + 4], byteorder="little")
	if expected_length != len(fw):
		raise ValueError(f"{path}: header body length {expected_length} does not match actual body length {len(fw)}")

	expected_hash = bytes(fw_header[HASH_OFFSET:HASH_OFFSET + HASH_SIZE])
	actual_hash = md5(fw).digest()[:HASH_SIZE]
	if expected_hash != actual_hash:
		raise ValueError(f"{path}: header body hash does not match actual body hash")

	return fw_header, fw


def read_loader_binary(path):
	try:
		with open(path, "rb") as f:
			return f.read()
	except FileNotFoundError:
		raise FileNotFoundError(f"required loader input not found: {path}") from None
	except OSError as exc:
		raise OSError(f"required loader input is not readable: {path}") from exc


def patch_rev_b_pointer(fw, ptr_addr, update_func_addr):
	if ptr_addr < 0 or ptr_addr + 4 > len(fw):
		raise ValueError(f"Rev. B patch address 0x{ptr_addr:x} is outside firmware body length {len(fw)}")
	struct.pack_into("<I", fw, ptr_addr, update_func_addr)


def finalize_firmware_header(fw_header, fw):
	fw_header[LENGTH_OFFSET:LENGTH_OFFSET + 4] = len(fw).to_bytes(4, byteorder="little")
	fw_header[HASH_OFFSET:HASH_OFFSET + HASH_SIZE] = md5(fw).digest()[:HASH_SIZE]


def pad_firmware_body(fw):
	while bool(len(fw) & 3):
		fw += b"\x00"
	return fw


def patch_rev_b():
	addresses = get_firmware_addresses(version=DEFAULT_VERSION, revision="B")
	try:
		ptr_addr = addresses.require_patch_pointer_offset()
		loader_entry_address = addresses.require_loader_entry_address()
	except FirmwareAddressError as exc:
		raise SystemExit(f"error: {exc}") from None

	fw_header, fw = read_firmware_image("pdfw-b")
	fw = pad_firmware_body(fw)
	fw += read_loader_binary("build/loader-b.bin")

	fw = bytearray(fw)
	patch_rev_b_pointer(fw, ptr_addr, loader_entry_address)
	finalize_firmware_header(fw_header, fw)

	with open("build/pdfw-b-patched", "wb") as f:
		f.write(fw_header)
		f.write(fw)


def main():
	print("Enter target system: B for Rev. B")
	target = input("|)> ").upper()

	if "A" in target:
		raise SystemExit("Error: Rev A patching is not implemented yet.")

	if "B" in target:
		patch_rev_b()


if __name__ == "__main__":
	main()
