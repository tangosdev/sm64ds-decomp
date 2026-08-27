//cpp
extern "C" int func_ov002_020c18b0(char* self, int x);
extern "C" void _Z15ApproachLinear2Rsss(short&, short, short);
extern unsigned char data_020a0e40;
extern short data_0209f4a4;

extern "C" void func_ov002_020cd448(char* self);
void func_ov002_020cd448(char* self) {
    unsigned char fl;
    if (*(unsigned char*)(self + 0x6e3) < 2) goto tail;
    fl = *(unsigned char*)(self + 0x6e9);
    if (fl & 1) {
        short before;
        *(short*)(self + 0x8c) = *(short*)(self + 0x92);
        before = *(short*)(self + 0x8c);
        func_ov002_020c18b0(self, 0);
        if (before < *(short*)(self + 0x8c)) goto tail;
        _Z15ApproachLinear2Rsss(*(short*)(self + 0x92), *(short*)(self + 0x8c), 0x200);
        goto tail;
    }
    if (fl & 2) {
        if (*(short*)((char*)&data_0209f4a4 + data_020a0e40 * 0x18) != 0) goto tail;
        if (*(short*)(self + 0x92) > 0) {
            short* q = (short*)(((int)self + 0x92));
            *q += 0x200;
            if (*(short*)(self + 0x92) > 0x3f00) *(short*)(self + 0x92) = 0x3f00;
        } else {
            short* q = (short*)(((int)self + 0x92));
            *q -= 0x200;
            if (*(short*)(self + 0x92) < -0x3f00) *(short*)(self + 0x92) = -0x3f00;
        }
    }
tail:
    _Z15ApproachLinear2Rsss(*(short*)(self + 0x69e), *(short*)(self + 0x90), 0x200);
    *(short*)(self + 0x90) = *(short*)(self + 0x69e);
}
