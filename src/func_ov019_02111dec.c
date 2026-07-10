extern int _ZN6Player12Unk_020c4f40Et(void* p, unsigned short a);
extern void _Z11UpdateAngleRssis(short* p, short a, int b, short c);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, void* clsn);
extern int func_ov019_0211140c(int* self, void* clsn);
extern int _ZNK12WithMeshClsn13JustHitGroundEv(void* m);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* m, void* f, int a, int b, unsigned int d);
extern int _ZN9Animation8FinishedEv(void* a);
extern int _ZN6Player12GetTalkStateEv(void* p);
extern void _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int cc, int d, int e);
extern void func_0201267c(int a, void* p);
extern int func_ov019_021122dc(void* c, int s);
extern int data_ov019_02113460[];
extern int data_ov019_02113470[];

#define LB(off) (*(unsigned char *)(((int)c + (off)) & 0xFFFFFFFFFFFFFFFF))

int func_ov019_02111dec(char* c) {
    switch (*(unsigned char*)(c+0x38f)) {
    case 0:
        if (_ZN6Player12Unk_020c4f40Et(*(void**)(c+0x378), 0x5a) != 0) {
            LB(0x38f) = LB(0x38f) + 1;
        }
        break;
    case 1:
        _Z11UpdateAngleRssis(
            (short*)(c+0x94),
            *(short *)((char *)(((int)c + 0x300) & 0xFFFFFFFFFFFFFFFF) + 0x8c),
            2, 0x800);
        *(short*)(c+0x8e) = *(short*)(c+0x94);
        _ZN5Actor9UpdatePosEP12CylinderClsn(c, c+0x174);
        func_ov019_0211140c((int*)c, c+0x1a8);
        if (_ZNK12WithMeshClsn13JustHitGroundEv(c+0x1a8) != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xd4, (void*)data_ov019_02113460[1], 0x40000000, 0x1000, 0);
            LB(0x38f) = LB(0x38f) + 1;
        }
        break;
    case 2:
        if (_ZN9Animation8FinishedEv(c+0x124) != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xd4, (void*)data_ov019_02113470[1], 0, 0x1000, 0);
        }
        if (_ZN6Player12GetTalkStateEv(*(void**)(c+0x378)) == -1) {
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x1f, 0x7f, 0, 0x15666, 0);
            func_0201267c(0x4d, c+0x74);
            func_ov019_021122dc(c, 3);
        }
        break;
    }
    return 1;
}
