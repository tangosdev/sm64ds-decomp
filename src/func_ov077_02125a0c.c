extern short Vec3_HorzAngle();
extern void _ZN10dBgCh_Actr13SetLimMovFlagEv();
int func_ov077_02125a0c(char *c) {
    *(int*)(c+0x98) = 0x5000;
    *(int*)(c+0xa8) = 0xd000;
    short ang = (short)Vec3_HorzAngle((char*)*(int*)(c+0x3d4)+0x5c, c+0x5c);
    *(short*)(c+0x94) = ang;
    _ZN10dBgCh_Actr13SetLimMovFlagEv(c+0x1e4);
    *(int*)(c+0x3d8) = 2;
    return 1;
}
