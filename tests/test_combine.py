import hashlib
import importlib
import os
import tempfile
import unittest
from pathlib import Path


combine = importlib.import_module("combine")


def make_firmware(body):
	header = bytearray(32)
	header[8:12] = len(body).to_bytes(4, byteorder="little")
	header[24:32] = hashlib.md5(body).digest()[:8]
	return bytes(header) + body


class FirmwareValidationTests(unittest.TestCase):
	def test_read_firmware_image_returns_valid_header_and_body(self):
		body = b"firmware-body"
		with tempfile.NamedTemporaryFile(delete=False) as tmp:
			tmp.write(make_firmware(body))
			path = tmp.name
		try:
			header, read_body = combine.read_firmware_image(path)
		finally:
			os.unlink(path)

		self.assertEqual(len(header), 32)
		self.assertEqual(read_body, body)

	def test_read_firmware_image_rejects_bad_body_length(self):
		body = b"firmware-body"
		image = bytearray(make_firmware(body))
		image[8:12] = (len(body) + 1).to_bytes(4, byteorder="little")
		with tempfile.NamedTemporaryFile(delete=False) as tmp:
			tmp.write(image)
			path = tmp.name
		try:
			with self.assertRaisesRegex(ValueError, "length"):
				combine.read_firmware_image(path)
		finally:
			os.unlink(path)

	def test_read_firmware_image_rejects_bad_body_hash(self):
		body = b"firmware-body"
		image = bytearray(make_firmware(body))
		image[24:32] = b"\x00" * 8
		with tempfile.NamedTemporaryFile(delete=False) as tmp:
			tmp.write(image)
			path = tmp.name
		try:
			with self.assertRaisesRegex(ValueError, "hash"):
				combine.read_firmware_image(path)
		finally:
			os.unlink(path)

	def test_read_firmware_image_rejects_short_header(self):
		with tempfile.NamedTemporaryFile(delete=False) as tmp:
			tmp.write(b"short")
			path = tmp.name
		try:
			with self.assertRaisesRegex(ValueError, "32-byte header"):
				combine.read_firmware_image(path)
		finally:
			os.unlink(path)


class LoaderInputTests(unittest.TestCase):
	def test_read_loader_binary_returns_existing_readable_file(self):
		with tempfile.NamedTemporaryFile(delete=False) as tmp:
			tmp.write(b"loader")
			path = tmp.name
		try:
			self.assertEqual(combine.read_loader_binary(path), b"loader")
		finally:
			os.unlink(path)

	def test_read_loader_binary_rejects_missing_file(self):
		path = Path(tempfile.gettempdir()) / "missing-playbrew-loader.bin"
		with self.assertRaises(FileNotFoundError):
			combine.read_loader_binary(path)


class PatchBoundsTests(unittest.TestCase):
	def test_patch_rev_b_pointer_rejects_out_of_bounds_offset(self):
		with self.assertRaisesRegex(ValueError, "outside firmware body"):
			combine.patch_rev_b_pointer(bytearray(3), ptr_addr=0, update_func_addr=1)


if __name__ == "__main__":
	unittest.main()
