#include "3.0.2.h"

__attribute__((section(".playbrew.entry"), used))
void playbrew_payload_entry(PlayBrewAPI *api) {
	api->printf("Hello from PlayBrew payload! ");
	api->gfx_clear(0);
	api->printf("Payload finished. ");
}
