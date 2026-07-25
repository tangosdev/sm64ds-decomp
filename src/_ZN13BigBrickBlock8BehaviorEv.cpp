//cpp
typedef unsigned char u8;
typedef unsigned short u16;

extern "C" int _ZN5Event6GetBitEj(unsigned int a);
extern "C" void *_ZN5Actor15FindWithActorIDEjPS_(unsigned int id, void *after);
extern "C" int _ZN16MeshColliderBase9IsEnabledEv(void *p);
extern "C" void _ZN16MeshColliderBase7DisableEv(void *p);
extern "C" void func_020393a4(int *p, int v);
extern "C" void func_02039394(int *p, int v);
extern "C" void _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(void *self, int a, int b);

extern "C" int _ZN13BigBrickBlock8BehaviorEv(char *c)
{
    int is13 = (int)(*(u16 *)(c + 0xc) == 0x13);
    if (is13 != 0) {
        if (*(u8 *)(c + 0x31f) != _ZN5Event6GetBitEj(*(u8 *)(c + 0x320)))
            *(u8 *)(c + 0x31e) = 0;

        if (*(void **)(c + 0x324) == 0) {
            unsigned int id = 0xb;
            char *p;
            do {
                *(void **)(c + 0x324) = _ZN5Actor15FindWithActorIDEjPS_(id, *(void **)(c + 0x324));
                p = *(char **)(c + 0x324);
            } while (p == 0 || *(u8 *)(c + 0x320) != *(u8 *)(p + 0x34e));
        }

        if (_ZN5Event6GetBitEj(*(u8 *)(c + 0x320)) == 0 || *(u8 *)(c + 0x31e) != 0) {
            if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x124) != 0)
                _ZN16MeshColliderBase7DisableEv(c + 0x124);
        } else {
            func_020393a4((int *)(c + 0x124), 0x15e000);
            func_02039394((int *)(c + 0x124), 0x64000);
            _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(c, 0x150000, 0);
        }

        *(u8 *)(c + 0x31f) = _ZN5Event6GetBitEj(*(u8 *)(c + 0x320));
    } else {
        int v1 = 0x15e000;
        int v5 = 0x64000;
        int t;
        if ((t = (int)(*(u16 *)(c + 0xc) == 0x10)) != 0 ||
            (t = (int)(*(u16 *)(c + 0xc) == 0x11)) != 0 ||
            (t = (int)(*(u16 *)(c + 0xc) == 0x2e)) != 0) {
            v1 = 0x1c2000;
            v5 = 0x96000;
        }
        func_020393a4((int *)(c + 0x124), v1);
        func_02039394((int *)(c + 0x124), v5);
        _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(c, 0x600000, 0);
    }
    return 1;
}
