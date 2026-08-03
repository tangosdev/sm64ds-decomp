//cpp
#include "MeshColliderBase.h"
extern "C" {
extern int func_ov098_02138ce0(void*);
extern int _ZN5Actor13DistToCPlayerEv(void*);
extern int Crate_SetState(void*, int);
extern int _ZN12CylinderClsn5ClearEv(void*);
extern int func_ov098_02139850(void*);
extern int func_ov098_021397c8(void*);
void func_ov098_0213814c(char* c){
    func_ov098_02138ce0(c);
    unsigned b = (unsigned)((*(int*)(c+0xb0) & 8) != 0);
    if(b != 0 && _ZN5Actor13DistToCPlayerEv(c) > 0x7d0000){
        Crate_SetState(c, 0);
        return;
    }
    _ZN12CylinderClsn5ClearEv(c+0x564);
    _ZN12CylinderClsn5ClearEv(c+0x5a4);
    func_ov098_02139850(c);
    func_ov098_021397c8(c);
    if(((MeshColliderBase *)(c+0x124))->IsEnabled())
        ((MeshColliderBase *)(c+0x124))->Disable();
}
}
