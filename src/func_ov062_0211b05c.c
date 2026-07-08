extern char *_ZN5Actor10FindWithIDEj(unsigned int id);
extern void _ZN5Timer9StopTimerEv(void *t);
extern int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int c, int d, int e);
extern void _ZN9Animation7AdvanceEv(void *a);
extern void func_ov062_0211afbc(char *t);
extern void _ZN12CylinderClsn5ClearEv(void *c);
extern void _ZN12CylinderClsn6UpdateEv(void *c);
extern char TIME_TIMER[];

int func_ov062_0211b05c(char *c)
{
    unsigned int id;
    char *a;
    int b;

    if (*(unsigned char *)(c + 0x16E) == 0) {
        id = *(unsigned int *)(c + 0xF8);
        if (id != 0) {
            a = _ZN5Actor10FindWithIDEj(id);
            if (a != 0) {
                b = *(unsigned short *)(a + 0xC);
                b = b == 0xBF;
                if (b) {
                    *(unsigned char *)(c + 0x16E) = 1;
                    _ZN5Timer9StopTimerEv(TIME_TIMER);
                    *(short *)(c + 0x16C) = 1;
                    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x1F, 0x14, 0x7F, 0x6B000, 0);
                }
            }
        }
    }

    if (*(unsigned short *)(c + 0x16C) != 0) {
        *(unsigned short *)(((int)c + 0x16C) & 0xFFFFFFFFFFFFFFFF) += 1;
        if (*(unsigned short *)(c + 0x16C) >= 0x5A) {
            if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x1F, 0x7F, 0, 0x8777, 0) != 0) {
                *(unsigned short *)(c + 0x16C) = 0;
            }
        }
    }

    _ZN9Animation7AdvanceEv(c + 0x158);
    func_ov062_0211afbc(c);
    _ZN12CylinderClsn5ClearEv(c + 0xD4);
    _ZN12CylinderClsn6UpdateEv(c + 0xD4);
    return 1;
}
