extern void RenderOamBothScreens(void* a0, int a1, int a2, int a3, int a4, void* a5);
extern unsigned short data_ov006_0212e314[];
extern void* data_ov006_0213bf0c[];

typedef struct {
    int f0;
    int f4;
    char pad8[0xd];
    unsigned char b15;
    unsigned char b16;
    char pad17;
    unsigned char b18;
    char pad19[3];
} E;

typedef struct {
    char pad0[0x4660];
    E e[40];
    char pad1[0x708];
    int f51c8;
    int f51cc;
} Self;

void func_ov006_020dd594(Self* self) {
    int i;
    if (self->f51c8 == 5 && self->f51cc == 0) return;
    for (i = 0; i < 0x28; i++) {
        if (self->e[i].b16 == 0) continue;
        {
            unsigned int flag = self->e[i].b15;
            int v0 = self->e[i].f0;
            int v4 = self->e[i].f4;
            int a1, a2, a4;
            unsigned short idx;
            a1 = v0 >> 12;
            a2 = v4 >> 12;
            a4 = (flag >= 3) ? 1 : 0;
            idx = data_ov006_0212e314[self->e[i].b18];
            RenderOamBothScreens(data_ov006_0213bf0c[idx], a1, a2, -1, a4, 0);
        }
    }
}
