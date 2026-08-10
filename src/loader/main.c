// not needed rn:
//#ifdef TARGET_DVT1
//#define PLAYDATE_RAM_END ((void *)0x61000000)
//#endif
//#ifdef TARGET_H7D1
//#define PLAYDATE_RAM_END ((void *)0x91000000)
//#endif

void playbrew_elevator_up(void) __attribute__((section(".up")));
void playbrew_elevator_down(void) __attribute__((section(".down")));


void playbrew_elevator_up(void) {
	__asm__ volatile ("svc #2");
}
void playbrew_elevator_down(void) {
	// I need to figure out how to de-elevate...
}
