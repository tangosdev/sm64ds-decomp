// NONMATCHING: base materialization / addressing (div=7). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* sfp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* file, int a, int b);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* sfp);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* self, void* kcl, void* mtx, int fix, short s, void* clps);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* c);
extern void func_020393d4(void* p, void* v);
extern void func_020393c4(void* p, void* v);
extern void _ZN7PathPtr6FromIDEj(void* self, unsigned int id);
extern int _ZNK7PathPtr8NumNodesEv(void* self);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void* self, void* out, unsigned int idx);
extern int Vec3_Equal(void* a, void* b);

extern int data_ov091_021344fc[];
extern int data_ov091_021344f4[];
extern int data_ov091_02134e5c[];
extern char _ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_;
extern char func_ov091_02132380;

int _ZN22RotatingUpDownPlatform13InitResourcesEv(char* c)
{
    unsigned char idx;

    *(unsigned char*)(c + 0x352) = (unsigned char)(*(unsigned*)(c + 8) >> 8);
    idx = *(unsigned char*)(c + 0x352);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, _ZN5Model8LoadFileER13SharedFilePtr((void*)data_ov091_021344fc[idx]), 1, -1);

    idx = *(unsigned char*)(c + 0x352);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        c + 0x124,
        _ZN12MeshCollider8LoadFileER13SharedFilePtr((void*)data_ov091_021344f4[idx]),
        c + 0x2ec,
        0x199,
        *(short*)(c + 0x8e),
        (void*)data_ov091_02134e5c[idx]);

    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    func_020393d4(c + 0x124, &_ZN16MeshColliderBase21UpdatePosWithVelocityERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    func_020393c4(c + 0x124, &func_ov091_02132380);

    _ZN7PathPtr6FromIDEj(c + 0x344, *(unsigned*)(c + 8) & 0xf);
    *(int*)(c + 0x324) = _ZNK7PathPtr8NumNodesEv(c + 0x344);
    *(int*)(c + 0x328) = 0;
    *(int*)(c + 0x32c) = *(int*)(c + 0x5c);
    *(int*)(c + 0x330) = *(int*)(c + 0x60);
    *(int*)(c + 0x334) = *(int*)(c + 0x64);
    _ZNK7PathPtr7GetNodeER7Vector3j(c + 0x344, c + 0x338, *(unsigned*)(c + 0x328));

    if (Vec3_Equal(c + 0x338, c + 0x32c)) {
        int* p = (int*)(c + 0x328);
        *p = *p + 1;
        _ZNK7PathPtr7GetNodeER7Vector3j(c + 0x344, c + 0x338, *p);
    }

    *(short*)(c + 0x350) = *(short*)(c + 0x8e);
    *(int*)(c + 0x34c) = 0;
    *(unsigned char*)(c + 0x356) = 0;
    return 1;
}
