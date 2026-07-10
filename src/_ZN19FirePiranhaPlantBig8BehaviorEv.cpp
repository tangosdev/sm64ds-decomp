//cpp
extern "C" {
extern void _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(char *thiz, char *clsn);
extern int func_ov084_0212d564(char *);
extern void _ZN9Animation7AdvanceEv(char *thiz);
extern int func_ov084_0212e4e0(char *);
extern int func_ov084_0212e010(char *);
extern int func_ov084_0212ddbc(char *);
extern int func_ov084_0212dc30(char *);
extern int func_ov084_0212d86c(char *);
extern void _ZN12CylinderClsn5ClearEv(char *thiz);
extern void _ZN12CylinderClsn6UpdateEv(char *thiz);
extern void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(char *thiz, char *v);
}

extern "C" int _ZN19FirePiranhaPlantBig8BehaviorEv(char *c)
{
    _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(c, c + 0x174);
    int b = (*(int *)(c + 0xb0) & 0x60000) != 0;
    if (b != 0) {
        func_ov084_0212d564(c);
        return 1;
    }
    _ZN9Animation7AdvanceEv(c + 0x160);
    int s = *(int *)(c + 0x1ec);
    switch (s) {
    case 0:
        func_ov084_0212e4e0(c);
        break;
    case 1:
        func_ov084_0212e010(c);
        break;
    case 2:
        func_ov084_0212ddbc(c);
        break;
    case 3:
        func_ov084_0212dc30(c);
        break;
    case 4:
        break;
    }
    {
        char * p = c + 0x100;
        *(unsigned short *)p = *(unsigned short *)p + 1;
        if (s != *(int *)(c + 0x1ec))
            *(unsigned short *)p = 0;
    }
    func_ov084_0212d86c(c);
    func_ov084_0212d564(c);
    _ZN12CylinderClsn5ClearEv(c + 0x174);
    *(int *)(c + 0x178) = *(int *)(c + 0x204) * *(int *)(c + 0x208);
    *(int *)(c + 0x17c) = *(int *)(c + 0x204) * *(int *)(c + 0x20c);
    _ZN12CylinderClsn6UpdateEv(c + 0x174);
    _ZN12CylinderClsn5ClearEv(c + 0x1a8);
    int b2 = *(unsigned short *)(c + 0xc) == 0xfc;
    if (b2 == 0
        && (unsigned int)(*(int *)(c + 0x1ec) - 2) <= 1
        && *(int *)(c + 0x204) == *(int *)(c + 0x210)) {
        _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(c + 0x1a8, c + 0x1f8);
        _ZN12CylinderClsn6UpdateEv(c + 0x1a8);
    }
    return 1;
}
