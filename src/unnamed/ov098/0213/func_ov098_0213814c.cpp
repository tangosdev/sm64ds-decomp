//cpp
#include "dBgW.h"
extern "C" {
extern int func_ov098_02138ce0(void*);
extern int _ZN8dActor_c13DistToCPlayerEv(void*);
extern int Crate_SetState(void*, int);
extern int _ZN5dCc_c5ClearEv(void*);
extern int func_ov098_02139850(void*);
extern int func_ov098_021397c8(void*);
void func_ov098_0213814c(char* c){
    func_ov098_02138ce0(c);
    unsigned b = (unsigned)((*(int*)(c+0xb0) & 8) != 0);
    if(b != 0 && _ZN8dActor_c13DistToCPlayerEv(c) > 0x7d0000){
        Crate_SetState(c, 0);
        return;
    }
    _ZN5dCc_c5ClearEv(c+0x564);
    _ZN5dCc_c5ClearEv(c+0x5a4);
    func_ov098_02139850(c);
    func_ov098_021397c8(c);
    if(((dBgW *)(c+0x124))->IsEnabled())
        ((dBgW *)(c+0x124))->Disable();
}
}
