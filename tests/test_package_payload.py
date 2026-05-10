import tempfile
import unittest
from pathlib import Path

from tools import package_payload


class PayloadBytesTests(unittest.TestCase):
	def test_build_payload_bytes_writes_little_endian_header_and_body(self):
		raw = b"\x00\xb5\x70\x47"
		payload = package_payload.build_payload_bytes(raw, entry_offset=0)

		self.assertEqual(payload[:4], len(raw).to_bytes(4, byteorder="little"))
		self.assertEqual(payload[4:8], (0).to_bytes(4, byteorder="little"))
		self.assertEqual(payload[8:], raw)

	def test_build_payload_bytes_preserves_nonzero_entry_offset(self):
		raw = b"\x00\xb5\x00\xbf\x70\x47"
		payload = package_payload.build_payload_bytes(raw, entry_offset=2)

		self.assertEqual(payload[4:8], (2).to_bytes(4, byteorder="little"))

	def test_build_payload_bytes_rejects_empty_payload(self):
		with self.assertRaisesRegex(ValueError, "empty"):
			package_payload.build_payload_bytes(b"", entry_offset=0)

	def test_build_payload_bytes_rejects_entry_offset_outside_payload(self):
		with self.assertRaisesRegex(ValueError, "outside"):
			package_payload.build_payload_bytes(b"\x00\xb5", entry_offset=2)

	def test_build_payload_bytes_rejects_odd_entry_offset(self):
		with self.assertRaisesRegex(ValueError, "Thumb-aligned"):
			package_payload.build_payload_bytes(b"\x00\xb5\x70\x47", entry_offset=1)

	def test_build_payload_bytes_rejects_entry_without_one_instruction(self):
		with self.assertRaisesRegex(ValueError, "instruction"):
			package_payload.build_payload_bytes(b"\x00\xb5\x70", entry_offset=2)


class PayloadFileTests(unittest.TestCase):
	def test_package_payload_file_reads_raw_and_writes_payload_bin(self):
		with tempfile.TemporaryDirectory() as tmpdir:
			raw_path = Path(tmpdir) / "hello.raw"
			output_path = Path(tmpdir) / "payload.bin"
			raw_path.write_bytes(b"\x00\xb5\x70\x47")

			package_payload.package_payload_file(raw_path, output_path, entry_offset=0)

			self.assertEqual(
				output_path.read_bytes(),
				(4).to_bytes(4, byteorder="little") + (0).to_bytes(4, byteorder="little") + b"\x00\xb5\x70\x47",
			)


if __name__ == "__main__":
	unittest.main()
