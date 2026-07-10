extern void func_ov102_02149df0(void* c);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, void* clsn);
extern void func_ov102_02149ff0(char* c);
extern void func_ov102_02149ea4(void* c);
extern void func_020393a4(int* p, int v);
extern void func_02039394(int* p, int v);
extern int _ZN16MeshColliderBase9IsEnabledEv(void* m);
extern void _ZN16MeshColliderBase7DisableEv(void* m);
extern void _ZN9Animation7AdvanceEv(void* a);
extern void _ZN16MeshColliderBase6EnableEP5Actor(void* m, void* self);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void* self, int a, int b);
extern void func_ov102_02149e38(char* self);
extern int data_0209caa0[];
extern unsigned char data_0209f2d8;
extern signed char data_0209f2f8;
int _ZN13QuestionBlock8BehaviorEv(char* c) {
    func_ov102_02149df0(c);
    if (*(int*)(c+0x3e8) != 2) {
        _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
        if (*(int*)(c+0x60) <= *(int*)(c+0x3e0)) *(int*)(c+0x60) = *(int*)(c+0x3e0);
        func_ov102_02149ff0(c);
        func_ov102_02149ea4(c);
    }
    func_020393a4((int*)(c+0x124), 0x8c000);
    func_02039394((int*)(c+0x124), 0x46000);
    if (*(int*)(c+0x3e8) != 0) {
        if (_ZN16MeshColliderBase9IsEnabledEv(c+0x124) != 0) {
            _ZN16MeshColliderBase7DisableEv(c+0x124);
        }
        goto end;
    }
    if ((data_0209caa0[1] & 0x80000000) == 0) {
        int b = (int)(*(unsigned short*)(c+0xc) == 0x14);
        if (b != 0) {
            _ZN9Animation7AdvanceEv(c+0x370);
            if (_ZN16MeshColliderBase9IsEnabledEv(c+0x124) != 0) {
                _ZN16MeshColliderBase7DisableEv(c+0x124);
            }
            goto end;
        }
    }
    {
        int b = (int)(data_0209f2d8 == 1);
        if (b != 0) {
            if (_ZN16MeshColliderBase9IsEnabledEv(c+0x124) == 0) {
                _ZN16MeshColliderBase6EnableEP5Actor(c+0x124, c);
            }
            goto end;
        }
    }
    if (data_0209f2f8 == 0x1c) {
        if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0x460000, 0x46000) != 0) {
            func_ov102_02149e38(c);
        }
        goto end;
    }
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0x118000, 0x46000) != 0) {
        func_ov102_02149e38(c);
    }
end:
    return 1;
}
