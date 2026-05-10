#!/usr/bin/env python3

import argparse
import struct
from pathlib import Path


MAX_PAYLOAD_SIZE = 8 * 1024 * 1024


def build_payload_bytes(raw_payload, entry_offset):
	raw_size = len(raw_payload)

	if raw_size == 0:
		raise ValueError("payload body is empty")

	if raw_size > MAX_PAYLOAD_SIZE:
		raise ValueError(f"payload body is larger than {MAX_PAYLOAD_SIZE} bytes")

	if entry_offset < 0:
		raise ValueError("entry offset cannot be negative")

	if entry_offset >= raw_size:
		raise ValueError("entry offset is outside the payload body")

	if (entry_offset & 1) != 0:
		raise ValueError("entry offset must be Thumb-aligned")

	if raw_size - entry_offset < 2:
		raise ValueError("entry offset must leave room for at least one Thumb instruction")

	return struct.pack("<II", raw_size, entry_offset) + raw_payload


def package_payload_file(input_path, output_path, entry_offset):
	input_path = Path(input_path)
	output_path = Path(output_path)

	payload = build_payload_bytes(input_path.read_bytes(), entry_offset)
	output_path.parent.mkdir(parents=True, exist_ok=True)
	output_path.write_bytes(payload)


def parse_entry_offset(value):
	try:
		entry_offset = int(value, 0)
	except ValueError as exc:
		raise argparse.ArgumentTypeError("entry offset must be an integer") from exc

	if entry_offset < 0:
		raise argparse.ArgumentTypeError("entry offset cannot be negative")

	return entry_offset


def parse_args(argv=None):
	parser = argparse.ArgumentParser(description="Package a raw PlayBrew payload as /payload.bin.")
	parser.add_argument("--input", required=True, type=Path, help="Raw payload binary to package")
	parser.add_argument("--output", required=True, type=Path, help="Destination payload.bin path")
	parser.add_argument(
		"--entry-offset",
		default=0,
		type=parse_entry_offset,
		help="Entry offset from the start of the raw payload, in decimal or 0x-prefixed hex",
	)
	return parser.parse_args(argv)


def main(argv=None):
	args = parse_args(argv)
	try:
		package_payload_file(args.input, args.output, args.entry_offset)
	except OSError as exc:
		raise SystemExit(f"error: {exc}") from exc
	except ValueError as exc:
		raise SystemExit(f"error: {exc}") from exc

	return 0


if __name__ == "__main__":
	raise SystemExit(main())
