extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void*);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void*);
extern void func_ov102_0214b384(void*, int);

int func_ov102_0214bbd8(void* c) {
    if (_ZNK10dBgCh_Actr10IsOnGroundEv((char*)c + 0x144) == 0) {
        int r = _ZNK10dBgCh_Actr8IsOnWallEv((char*)c + 0x144);
        if (r == 0) return r;
    }
    func_ov102_0214b384(c, 4);
    *(int*)((char*)c + 0x98) = 0;
    return 0;
}
