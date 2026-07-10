//cpp
typedef short s16;
struct V3 { int x, y, z; };
extern "C" {
int _ZN16MeshColliderBase9IsEnabledEv(void* self);
void _ZN16MeshColliderBase7DisableEv(void* self);
int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int c, void* v, unsigned int d);
void func_ov095_021358cc(void* c, void* a, void* b, int d, int e, int f, int g);
void func_ov095_0213597c(char *t);
int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void* self, int a, int b);
void _ZN8Platform19UpdateClsnPosAndRotEv(void* self);

int _ZN9SeesawBob8BehaviorEv(char* c){
    int b = (int)((*(int*)(c + 0xb0) & 8) != 0);
    if (b != 0) {
        if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x124)) {
            _ZN16MeshColliderBase7DisableEv(c + 0x124);
        }
        return 1;
    }
    if (*(unsigned char*)(c + 0x326) == 0) {
        func_ov095_021358cc(c, c + 0x8c, c + 0x324, 0, 6, 3, 3);
    }
    {
        int s = *(short*)(c + 0x324);
        if (s < 0) s = (short)-s;
        if (s > 0xa) {
            *(int*)(c + 0x320) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
                *(unsigned int*)(c + 0x320), 3, 0x8b, c + 0x74, 0);
        }
    }
    if (*(short*)(c + 0x8c) > 0x2000) *(short*)(c + 0x8c) = 0x2000;
    if (*(short*)(c + 0x8c) < -0x2000) *(short*)(c + 0x8c) = -0x2000;
    func_ov095_0213597c(c);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0)) {
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    }
    *(unsigned char*)(c + 0x326) = 0;
    return 1;
}
}
