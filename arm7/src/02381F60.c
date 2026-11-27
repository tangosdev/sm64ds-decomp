/* storage 0x02381F60 runtime 0x037F9DF8 size 0x74 wram ARM.
 * Tick service: bump the 64-bit tick, ack a pending timer stop, then
 * tail-call the worker with (0, self, 0). Five pool words. */

extern unsigned long long qword_3806FD8;
extern int dword_3806FD4;
extern int fn_0238092C(int a1, int a2, int a3);

int fn_02381F60(void) {
    ++qword_3806FD8;
    if (dword_3806FD4 != 0) {
        *(volatile unsigned short *)0x4000102 = 0;
        *(volatile unsigned short *)0x4000100 = 0;
        *(volatile unsigned short *)0x4000102 = 0xC1;
        dword_3806FD4 = 0;
    }
    return fn_0238092C(0, (int)fn_02381F60, 0);
}
