//cpp
typedef int Fix12;
struct OamAttr;
struct OAM {
    static int Render(bool, OamAttr*, int, int, int, int, Fix12, OamAttr*, int, int);
    static void RenderSub(OamAttr*, int, int);
};
extern "C" int func_0200f0bc(void);
extern short data_ov075_0211b5d4[];
extern "C" char data_ov075_0211c790[];
extern "C" char func_020abd88[];

extern "C" void func_ov075_02116f40(char* c)
{
    unsigned char idx = *(unsigned char*)(c + 0x281);
    int a1, a2;
    if (idx < 2) {
        a2 = (short)(idx * 0x30 + 0x38);
        a1 = 0xe;
    } else {
        int j = func_0200f0bc();
        a1 = data_ov075_0211b5d4[j];
        a2 = 0xa8;
    }
    OAM::RenderSub((OamAttr*)data_ov075_0211c790, a1, a2);

    *(int*)(c + 0x26c) += 1;
    int r5 = (*(int*)(c + 0x26c) & 8) ? 0xac : 0xae;
    OAM::Render(false, (OamAttr*)func_020abd88, 0x40, r5, -1, -1, 0x1000, (OamAttr*)0x1000, 0, -1);
    OAM::Render(false, (OamAttr*)func_020abd88, 0x80, r5, -1, -1, 0x1000, (OamAttr*)0x1000, 0, -1);
    OAM::Render(false, (OamAttr*)func_020abd88, 0xc0, r5, -1, -1, 0x1000, (OamAttr*)0x1000, 0, -1);
}
