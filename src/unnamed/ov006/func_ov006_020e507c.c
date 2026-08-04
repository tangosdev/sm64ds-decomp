extern void RenderOamBothScreens(void* a0, int a1, int a2, int a3, int a4, void* a5);
extern int data_ov006_0213c44c;
extern int data_ov006_0213c3fc;
extern int data_ov006_0213c454;

void func_ov006_020e507c(char* a0) {
    int x, y;
    int i;
    for (i = 0; i < 0xb; i++) {
        void* d;
        if (*(unsigned char*)(a0 + 0x4689) == 0) goto next;
        if (*(unsigned char*)(a0 + 0x468a) == 0) goto next;
        x = *(int*)(a0 + 0x4660) >> 12;
        y = *(int*)(a0 + 0x4664) >> 12;
        if (*(unsigned char*)(a0 + 0x468d) != 0) d = &data_ov006_0213c44c;
        else d = &data_ov006_0213c3fc;
        RenderOamBothScreens(d, x, y, -1, 1, 0);
        RenderOamBothScreens(&data_ov006_0213c454, x, y + 8, -1, 2, 0);
    next:
        a0 += 0x30;
    }
}
