typedef unsigned char u8;
typedef unsigned short u16;

extern int _ZN5Sound15PlaySecretSoundEP5ActorPt(void* actor, void* pt);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int bank, void* pos);
extern void func_ov024_02111350(char* c);
extern void func_ov024_021112c0(char* c);
extern void func_ov024_021114c4(char* c);
extern void func_ov024_02111480(char* c);

int _ZN10PyramidTop8BehaviorEv(char* c)
{
    u8 state = *(u8*)(c + 0x3b7);
    switch (state) {
    case 0:
        if (*(u8*)(c + 0x3b6) == 4) {
            (*(u8*)(((long long)(int)(c + 0x3b7)) & 0xffffffffffffffffLL))++;
        }
        break;
    case 1:
        _ZN5Sound15PlaySecretSoundEP5ActorPt(c, (void*)(c + 0x3b4));
        if (*(u16*)(c + 0x3b2) == 0) {
            _ZN5Sound9PlayBank3EjRK7Vector3(0x4b, (void*)(c + 0x74));
        }
        func_ov024_02111350(c);
        break;
    case 2:
        if (_ZN5Sound15PlaySecretSoundEP5ActorPt(c, (void*)(c + 0x3b4))) {
            _ZN5Sound9PlayBank3EjRK7Vector3(0x4c, (void*)(c + 0x74));
            func_ov024_021112c0(c);
        } else {
            func_ov024_02111350(c);
        }
        break;
    }
    (*(u16*)(((long long)(int)(c + 0x3b2)) & 0xffffffffffffffffLL))++;
    if (state != *(u8*)(c + 0x3b7)) {
        *(u16*)(c + 0x3b2) = 0;
    }
    func_ov024_021114c4(c);
    func_ov024_02111480(c);
    return 1;
}
