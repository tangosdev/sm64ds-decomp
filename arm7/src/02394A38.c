#pragma scheduling off
extern int dword_380FFF4;
extern int fn_02381C7C(int a1, int a2, int a3);
extern int fn_02394B64(int a1, int a2);
void fn_02394A38(int *a1) {
    int *glob;
    int *slot;
    int kind;
    int arg;
    int (*fp)(int);
    int result;
    unsigned short *p;
    glob = (int *)dword_380FFF4;
    slot = (int *)((char *)glob + 0x140);
    slot[0] = a1[0];
    slot[1] = a1[1];
    slot[2] = a1[2];
    kind = slot[0];
    switch (kind) {
    case 0:
        result = fn_02381C7C(slot[1], slot[2], 0x81);
        *(int *)((char *)dword_380FFF4 + 0x398) = result;
        break;
    case 1:
        arg = 0x81;
        fp = (int (*)(int))slot[1];
        result = fp(arg);
        *(int *)((char *)dword_380FFF4 + 0x398) = result;
        break;
    default:
        break;
    }
    p = (unsigned short *)((char *)dword_380FFF4 + 0x39C);
    *p = 0;
    p = (unsigned short *)((char *)dword_380FFF4 + 0x3A0);
    *p = 0;
    fn_02394B64((int)((char *)slot + 0x0C), 2);
    fn_02394B64((int)((char *)slot + 0x18), 3);
    fn_02394B64((int)((char *)slot + 0x24), 4);
    fn_02394B64((int)((char *)slot + 0x30), 5);
    fn_02394B64((int)((char *)slot + 0x3C), 6);
    fn_02394B64((int)((char *)slot + 0x48), 7);
    fn_02394B64((int)((char *)slot + 0x54), 8);
    fn_02394B64((int)((char *)slot + 0x60), 9);
    fn_02394B64((int)((char *)slot + 0x6C), 10);
    fn_02394B64((int)((char *)slot + 0x78), 11);
    fn_02394B64((int)((char *)slot + 0x84), 12);
}
