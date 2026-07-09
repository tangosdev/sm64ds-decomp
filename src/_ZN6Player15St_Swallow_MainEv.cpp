//cpp
extern "C" {
extern unsigned int _ZNK6Player14GetBodyModelIDEjb(void* c, unsigned int a, int b);
extern int _ZNK9Animation12WillHitFrameEi(void* anim, int frame);
extern void func_ov002_020d71a0(char* c);
extern int func_ov002_020d5ab4(void* p);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, void* v);
extern void func_ov002_020d6368(char* c);
extern int func_ov002_020ceaf4(char* c);
extern int _ZN6Player12FinishedAnimEv(char* c);
extern void _ZN6Player11ChangeStateERNS_5StateE(char* c, void* st);
extern void func_ov002_020bedd4(char* c);

extern int data_0209ee90[];
extern int data_ov002_0211067c;
extern int data_ov002_0211013c;

int _ZN6Player15St_Swallow_MainEv(char* c)
{
    void* p360;

    if (_ZNK9Animation12WillHitFrameEi((char*)(*(void**)(c + (_ZNK6Player14GetBodyModelIDEjb(c, *(unsigned int*)(c + 8) & 0xff, 0) << 2) + 0xdc)) + 0x50, 3)) {
        func_ov002_020d71a0(c);
        goto L65ec;
    }
    if (_ZNK9Animation12WillHitFrameEi((char*)(*(void**)(c + (_ZNK6Player14GetBodyModelIDEjb(c, *(unsigned int*)(c + 8) & 0xff, 0) << 2) + 0xdc)) + 0x50, 7) == 0) {
        goto L65ec;
    }

    p360 = *(void**)(c + 0x360);
    if (p360 == 0) goto L65e4;
    {
        int b = (*(unsigned short*)((char*)p360 + 0xc) == 0xbf);
        if (b == 0) goto L653c;
    }
    *(short*)((char*)p360 + 0x8c) = *(short*)(c + 0x8c);
    *(short*)((char*)p360 + 0x8e) = *(short*)(c + 0x8e);
    *(short*)((char*)p360 + 0x90) = *(short*)(c + 0x90);
    func_ov002_020d5ab4(*(void**)(c + 0x360));
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(0, 0x100, c + 0x74);
    goto L65e4;

L653c:
    (*(void (**)(void*, char*))(*(char**)p360 + 0x4c))(p360, c);
    {
        void* q = *(void**)(c + 0x360);
        if ((*(int (**)(void*))(*(char**)q + 0x48))(q) == 1) goto L65e4;
    }
    if ((unsigned short)(*(unsigned short*)(c + 0x6ce) & 0x1000)) goto L65e4;
    {
        unsigned char flag = 0;
        int b_c2 = (*(unsigned short*)((char*)*(void**)(c + 0x360) + 0xc) == 0xc2);
        if (b_c2 || *(int*)((char*)data_0209ee90 + 0x238) != 0) {
            flag |= 1;
        }
        *(unsigned char*)(((long long)(int)(c + 0x704)) & 0xFFFFFFFFFFFFFFFFLL) |= (flag + 1);
    }
    func_ov002_020d6368(c);
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(0, 0x100, c + 0x74);

L65e4:
    *(void**)(c + 0x360) = 0;
L65ec:
    if (*(unsigned char*)(c + 0x706) != 0) {
        *(int*)(((long long)(int)(c + 0xa8)) & 0xFFFFFFFFFFFFFFFFLL) += func_ov002_020ceaf4(c);
    }
    if (_ZN6Player12FinishedAnimEv(c)) {
        if (*(unsigned char*)(c + 0x706) != 0) {
            _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211067c);
        } else {
            _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211013c);
        }
    }
    func_ov002_020bedd4(c);
    return 1;
}
}
