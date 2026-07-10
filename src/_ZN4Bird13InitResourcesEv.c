//cpp
// NONMATCHING: register allocation (div=15). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" {
struct SharedFilePtr; struct BMD_File; struct BCA_File;
void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
void* _ZN9Animation8LoadFileER13SharedFilePtr(void*);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
void _ZN11ShadowModel12InitCylinderEv(void*);
extern char data_ov009_02113c20[];
extern char data_ov009_02113c28[];

void _ZN4Bird13InitResourcesEv(char* self){
    void* m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov009_02113c20);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(self+0xd4, m, 1, 1);
    void* a = _ZN9Animation8LoadFileER13SharedFilePtr(data_ov009_02113c28);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self+0xd4, a, 0, 0x1000, 0);
    _ZN11ShadowModel12InitCylinderEv(self+0x138);
    *(int*)(self+0x60) += 0xa000;
    *(int*)(self+0x9c) = 0;
    *(int*)(self+0xa0) = -0x32000;
    *(unsigned char*)(self+0x180) = 1;
    *(int*)(self+0x178) = *(int*)(self+0x4);
    *(int*)(self+0x160) = *(int*)(self+0x5c);
    *(int*)(self+0x164) = *(int*)(self+0x60);
    *(int*)(self+0x168) = *(int*)(self+0x64);
    *(int*)(self+0x17c) = 0;
}
}
