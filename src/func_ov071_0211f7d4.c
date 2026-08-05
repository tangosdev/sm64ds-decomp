//cpp
struct CylinderClsn;
struct Actor { void UpdatePos(CylinderClsn *c); };
struct WithMeshClsn {
    int JustHitGround() const;
    void *GetFloorResult() const;
    int IsOnGround() const;
    void ClearLimMovFlag();
};
extern "C" void WithMeshClsn_UpdateDiscreteNoLava_veneer(void *p);
extern "C" int func_02037e38(unsigned int *p);
extern "C" void func_ov071_0211f498(char *c);
extern "C" void Scuttlebug_SetState(char *c, int x);
extern "C" void func_ov071_0211f29c(char *c);
struct CylinderClsn2 { void Clear(); void Update(); };
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN12CylinderClsn5ClearEv(void *);
extern "C" void _ZN12CylinderClsn6UpdateEv(void *);


extern "C" int func_ov071_0211f7d4(Actor *self)
{
    char *s = (char*)self;
    WithMeshClsn_UpdateDiscreteNoLava_veneer(s + 0x194);
    *(short*)(s + 0x8c) = *(short*)(s + 0x8c) + 0x1000;
    if (((WithMeshClsn*)(s + 0x194))->JustHitGround()) {
        if (func_02037e38((unsigned int*)((char*)((WithMeshClsn*)(s + 0x194))->GetFloorResult() + 4)) == 4) {
            func_ov071_0211f498(s);
        } else {
            *(int*)(s + 0xa8) = (*(int*)(s + 0xa8) * -0x3c) / 0x64;
        }
    } else if (((WithMeshClsn*)(s + 0x194))->IsOnGround()) {
        WithMeshClsn *wm = (WithMeshClsn*)(s + 0x194);
        *(int*)(s + 0xa8) = 0;
        wm->ClearLimMovFlag();
        *(int *)(((long long)(int)(s + 0xb0))) |= 1;
        short z = 0;
        short ang = *(short*)(s + 0x94);
        *(short*)(s + 0x8c) = z;
        *(short*)(s + 0x8e) = ang;
        *(short*)(s + 0x90) = z;
        Scuttlebug_SetState(s, 2);
    }
    self->UpdatePos((CylinderClsn*)(s + 0x160));
    func_ov071_0211f29c(s);
    _ZN12CylinderClsn5ClearEv((CylinderClsn2*)(s + 0x160));
    _ZN12CylinderClsn6UpdateEv((CylinderClsn2*)(s + 0x160));
    return 1;
}
