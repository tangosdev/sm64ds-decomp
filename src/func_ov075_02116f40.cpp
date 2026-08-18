//cpp
struct OamAttr;
struct OAM {
    static void RenderSub(OamAttr*, int, int);
};

/* Render's 7th AND 8th parameters are Fix12<int> -- the S3_ in
   _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii back-references the Fix12<int> one place
   earlier, so the eighth is not the OamAttr* this file used to declare. It was being
   handed (OamAttr*)0x1000, which is the right WORD for the wrong reason. Declared by
   final name because Fix12<int> is an aggregate with no converting constructor from
   int, and both arguments here are int literals. */
extern "C" int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
    bool, OamAttr*, int, int, int, int, int, int, int, int);
extern "C" int func_0200f0bc(void);
extern short data_ov075_0211b5d4[];
extern "C" char data_ov075_0211c790[];
extern "C" char _ZN3OAM14BOUNCING_ARROWE[];

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
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
        false, (OamAttr*)_ZN3OAM14BOUNCING_ARROWE, 0x40, r5, -1, -1, 0x1000, 0x1000, 0, -1);
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
        false, (OamAttr*)_ZN3OAM14BOUNCING_ARROWE, 0x80, r5, -1, -1, 0x1000, 0x1000, 0, -1);
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
        false, (OamAttr*)_ZN3OAM14BOUNCING_ARROWE, 0xc0, r5, -1, -1, 0x1000, 0x1000, 0, -1);
}
