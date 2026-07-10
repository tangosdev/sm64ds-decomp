struct Vector3 { int x, y, z; };
struct Vector3_16 { short x, y, z; };
extern void _ZN6Camera9SetFlag_3Ev(void* cam);
extern void _Z14ApproachLinearRiii(int* p, int t, int s);
extern void _ZN7Message7EndTalkEv(void);
extern void _ZN5Sound22StopLoadedMusic_Layer3Ev(void);
extern void func_02011cfc(void);
extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned int a, int b);
extern void func_0200fa8c(void* t, int a);
extern short _ZN5Actor18HorzAngleToCPlayerEv(void* self);
extern void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int id, unsigned int p, void* pos, void* rot, int a, int b);
extern void* _ZN5Actor15FindWithActorIDEjPS_(unsigned int id, void* prev);
extern void func_02012694(int a, void* p);
extern void _ZN5Actor10PoofDustAtERK7Vector3(void* self, void* pos);
extern void _ZN9ActorBase18MarkForDestructionEv(void* self);
extern void func_ov073_0211f144(void* c);
extern void* data_0209f318;

int func_ov073_0211fa74(char* c) {
    void* cam;
    void* spawned;
    void* found;
    volatile struct Vector3_16 rot;
    struct Vector3 pos;
    cam = data_0209f318;
    _ZN6Camera9SetFlag_3Ev(cam);
    _Z14ApproachLinearRiii((int*)(c + 0x80), 0, 0x80);
    *(int*)(c + 0x88) = *(int*)(c + 0x80);
    *(int*)(c + 0x84) = *(int*)(c + 0x88);
    func_ov073_0211f144(c);
    if (*(int*)(c + 0x80) >= 0x100) goto end;
    pos.x = *(int*)(c + 0x3d8);
    pos.y = *(int*)(c + 0x3dc);
    pos.z = *(int*)(c + 0x3e0);
    _ZN7Message7EndTalkEv();
    _ZN5Sound22StopLoadedMusic_Layer3Ev();
    func_02011cfc();
    _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0x15666);
    pos.y = pos.y + 0x190000;
    func_0200fa8c(c, 1);

    {
        unsigned short a = *(unsigned short*)(c + 0x8c);
        unsigned short b = *(unsigned short*)(c + 0x8e);
        rot.x = b ? a : a;
        rot.y = b;
        rot.z = *(unsigned short*)(c + 0x90);
        rot.y = (unsigned short)_ZN5Actor18HorzAngleToCPlayerEv(c);
    }

    spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x11a, 4, &pos, 0, *(signed char*)(c + 0xcc), -1);
    found = _ZN5Actor15FindWithActorIDEjPS_(0x13d, 0);
    func_02012694(0xbb, c + 0x74);
    if (found != 0) {
        struct Vector3 fp;
        int pv = (int)(((int)found + 0x5c) & 0xFFFFFFFFFFFFFFFF);
        fp.x = *(int*)pv;
        fp.y = *(int*)(pv + 4);
        fp.z = *(int*)(pv + 8);
        _ZN5Actor10PoofDustAtERK7Vector3(c, &fp);
        _ZN9ActorBase18MarkForDestructionEv(found);
    }
    if (spawned != 0) {
        *(int*)(((int)cam + 0x154) & 0xFFFFFFFFFFFFFFFF) &= ~8;
        _ZN9ActorBase18MarkForDestructionEv(c);
    }
end:
    return 1;
}
