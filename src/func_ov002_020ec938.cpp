//cpp
extern "C" {
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void* self, void* cyl);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void* self);
extern void func_ov002_020edca4(void* c);
extern void dBgCh_Actr_UpdateContinuous_Veneer(void* p);
extern void _ZN5dCc_c5ClearEv(void* self);

void func_ov002_020ec938(char* c){
    _ZN8dActor_c9UpdatePosEP5dCc_c(c, c + 0x110);
    if (_ZNK10dBgCh_Actr10IsOnGroundEv(c + 0x144))
        func_ov002_020edca4(c);
    dBgCh_Actr_UpdateContinuous_Veneer(c + 0x144);
    _ZN5dCc_c5ClearEv(c + 0x110);
}
}
