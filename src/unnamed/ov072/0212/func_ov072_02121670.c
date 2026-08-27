typedef int Fix12i;
extern Fix12i Vec3_Dist(const void *a, const void *b);
extern short Vec3_HorzAngle(const void *v0, const void *v1);
extern void _Z14ApproachLinearRsss(short *cur, short tgt, short step);
extern void _ZN9Animation7AdvanceEv(void *self);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, void *clsn);
extern void func_ov072_02120fd4(void *self, void *p);
extern void func_ov072_02120f14(void *self);
extern void func_ov072_02120e50(void *self);
extern void _ZN5dCc_c5ClearEv(void *self);
extern void _ZN5dCc_c6UpdateEv(void *self);
extern void func_0201267c(int a, void *b);
int func_ov072_02121670(char *c) {
    int dist = Vec3_Dist(c + 0x5c, (char*)*(void**)(c + 0x364) + 0x5c);
    short tgt = *(short*)(c + 0x8e);
    if (dist > 0xdc000) {
        tgt = Vec3_HorzAngle(c + 0x5c, (char*)*(void**)(c + 0x364) + 0x5c);
    } else if (dist < 0xc8000) {
        tgt = (short)(Vec3_HorzAngle(c + 0x5c, (char*)*(void**)(c + 0x364) + 0x5c) + 0x8000);
    }
    _Z14ApproachLinearRsss((short*)(c + 0x8e), tgt, 0x2bc);
    *(short*)(c + 0x94) = *(short*)(c + 0x8e);
    _ZN9Animation7AdvanceEv(c + 0x124);
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, c + 0x160);
    func_ov072_02120fd4(c, c + 0x194);
    func_ov072_02120f14(c);
    func_ov072_02120e50(c);
    _ZN5dCc_c5ClearEv(c + 0x160);
    _ZN5dCc_c6UpdateEv(c + 0x160);
    {
        int s = (int)((unsigned)(*(int*)(c + 0x12c) << 4) >> 16);
        if (s == 9 || s == 0x15) {
            func_0201267c(0xf3, c + 0x74);
        }
    }
    return 1;
}
