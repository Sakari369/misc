/* Emulator debug output */
void dprint_asm(char *s)
{
	asm volatile(
	"mov r0, %0;"
	"swi 0xff;"
	:
	: "r" (s)
	: "r0");
}
