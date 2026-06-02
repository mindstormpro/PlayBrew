from dataclasses import dataclass, field
from typing import Mapping, Optional


DEFAULT_VERSION = "3.0.2"
DEFAULT_REVISION = "B"
FIRMWARE_BASE_ADDRESS = 0x24000000


class FirmwareAddressError(ValueError):
	pass


@dataclass(frozen=True)
class FirmwareAddresses:
	version: str
	revision: str
	continuation_address: Optional[int] = None
	patch_pointer_address: Optional[int] = None
	loader_entry_address: Optional[int] = None
	api_addresses: Mapping[str, int] = field(default_factory=dict)

	@property
	def target_label(self) -> str:
		return f"{self.version} Rev {self.revision}"

	def require_continuation_address(self) -> int:
		if self.continuation_address is None:
			raise FirmwareAddressError(f"No continuation address is known for {self.target_label}.")
		return self.continuation_address

	def require_patch_pointer_offset(self) -> int:
		if self.patch_pointer_address is None:
			raise FirmwareAddressError(f"No patch pointer address is known for {self.target_label}.")
		return self.patch_pointer_address - FIRMWARE_BASE_ADDRESS

	def require_loader_entry_address(self) -> int:
		if self.loader_entry_address is None:
			raise FirmwareAddressError(f"No loader entry address is known for {self.target_label}.")
		return self.loader_entry_address


KNOWN_FIRMWARE_ADDRESSES = {
	(DEFAULT_VERSION, DEFAULT_REVISION): FirmwareAddresses(
		version=DEFAULT_VERSION,
		revision=DEFAULT_REVISION,
		continuation_address=0x240C5DB3,
		patch_pointer_address=0x2400EE8C,
		loader_entry_address=0x240EC745,
		api_addresses={
			"sd_open": 0x24067595,
			"sd_close": 0x240676FD,
			"sd_read": 0x24067919,
			"printf": 0x240BAF51,
			"gfx_clear": 0x24055D5D,
			"gfx_drawLine": 0x24055BE5,
			"gfx_setPixel": 0x24055901,
		},
	),
}


def normalize_revision(revision: str) -> str:
	normalized = revision.strip().upper()
	if normalized.startswith("REV "):
		normalized = normalized[4:].strip()
	return normalized


def get_firmware_addresses(
	version: str = DEFAULT_VERSION,
	revision: str = DEFAULT_REVISION,
) -> FirmwareAddresses:
	target_version = version.strip()
	target_revision = normalize_revision(revision)
	key = (target_version, target_revision)
	if key in KNOWN_FIRMWARE_ADDRESSES:
		return KNOWN_FIRMWARE_ADDRESSES[key]
	return FirmwareAddresses(version=target_version, revision=target_revision)
