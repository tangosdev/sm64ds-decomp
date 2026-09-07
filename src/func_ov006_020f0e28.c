extern int GetGameLanguage(void);
extern void DrawOamSprite(int p, int a, int b, int c);
extern void func_ov004_020b2220(int a, int b, int c, int d, int e, int f, int g);
extern int *data_ov006_0213ce70[];
void func_ov006_020f0e28(char *this) {
    int idx;
    if (*(int*)(this + 0x4000 + 0xf78) < 2)
        return;
    idx = GetGameLanguage();
    DrawOamSprite(*(int*)((char*)data_ov006_0213ce70[idx] + 0xc), 0x80, 0x10, 0);
    func_ov004_020b2220(0x80, 0x28, *(unsigned short*)(this + 0x5100 + 0x72), 1, -1, 0x800, 0);
}
