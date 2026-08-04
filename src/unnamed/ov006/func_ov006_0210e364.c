extern void RenderOamBothScreens(int a, int b, int c, int d, int e, int f);
extern int data_ov006_02137a9c[];
void func_ov006_0210e364(int this) {
    RenderOamBothScreens(data_ov006_02137a9c[*(unsigned char*)(this + 0x31)],
                        *(int*)(this + 8) >> 12,
                        *(int*)(this + 0xc) >> 12,
                        -1, 1, 0);
    RenderOamBothScreens(data_ov006_02137a9c[2],
                        *(int*)(this + 8) >> 12,
                        (*(int*)(this + 0xc) >> 12) + 5,
                        -1, 2, 0);
}
