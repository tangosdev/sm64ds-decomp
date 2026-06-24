//cpp
struct CylinderClsn { void Clear(); void Update(); };
struct Actor { void UpdatePos(CylinderClsn *cc); };

void ApproachLinear2(short &v, short t, short step);
extern "C" unsigned char DecIfAbove0_Byte(unsigned char *p);
extern "C" void func_ov077_02123c6c(char *c, void *p);
extern "C" void func_ov077_02123a74(char *c);
extern "C" void func_ov077_0212478c(char *c, int x);

extern "C" int func_ov077_02124118(char *c)
{
    *(short *)(c + 0x8e) = (short)(*(short *)(c + 0x8e) + *(short *)(c + 0x41a));
    ApproachLinear2(*(short *)(c + 0x41a), 0, 0x300);
    ((Actor *)c)->UpdatePos((CylinderClsn *)(c + 0x1c4));
    func_ov077_02123c6c(c, (void *)(c + 0x204));
    func_ov077_02123a74(c);
    ((CylinderClsn *)(c + 0x1c4))->Clear();
    ((CylinderClsn *)(c + 0x1c4))->Update();
    if (!DecIfAbove0_Byte((unsigned char *)(c + 0x41c)))
        func_ov077_0212478c(c, 0);
    return 1;
}
