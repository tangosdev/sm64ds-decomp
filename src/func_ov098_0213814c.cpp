//cpp
extern "C" {
extern int func_ov098_02138ce0(void*);
extern int Actor_DistToCPlayer(void*);
extern int Crate_SetState(void*, int);
extern int CylinderClsn_Clear(void*);
extern int func_ov098_02139850(void*);
extern int func_ov098_021397c8(void*);
extern int MeshColliderBase_IsEnabled(void*);
extern int MeshColliderBase_Disable(void*);
void func_ov098_0213814c(char* c){
    func_ov098_02138ce0(c);
    unsigned b = (unsigned)((*(int*)(c+0xb0) & 8) != 0);
    if(b != 0 && Actor_DistToCPlayer(c) > 0x7d0000){
        Crate_SetState(c, 0);
        return;
    }
    CylinderClsn_Clear(c+0x564);
    CylinderClsn_Clear(c+0x5a4);
    func_ov098_02139850(c);
    func_ov098_021397c8(c);
    if(MeshColliderBase_IsEnabled(c+0x124))
        MeshColliderBase_Disable(c+0x124);
}
}
