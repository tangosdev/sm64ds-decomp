struct Vector3 { int x, y, z; };

extern void _ZN6Camera9SetFlag_3Ev(void* cam);
extern void* _ZN5Actor13ClosestPlayerEv(void* self);
extern void _ZN6Player17SetNoControlStateEhih(void* self, unsigned char a, int b, unsigned char c);
extern void func_020092c4(void* cam, void* out, void* target);
extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned int a, int b);
extern void _ZN7Message11PrepareTalkEv(void);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void* self, void* actor, unsigned int msg, struct Vector3* v, unsigned int d, unsigned int e);
extern void func_02012694(int a, void* b);
extern int _ZN6Player12GetTalkStateEv(void* self);
extern void _ZN7Message7EndTalkEv(void);
extern void _ZN5Sound22LoadAndSetMusic_Layer3Ej(unsigned int a);
extern void func_02011d2c(void);
extern void func_ov066_02119454(void* c, void* p);
extern void _ZN5Sound22StopLoadedMusic_Layer3Ev(void);
extern void func_02011cfc(void);
extern void _ZN5Actor19UntrackAndSpawnStarERajRK7Vector3j(void* self, signed char* a, unsigned int b, struct Vector3* v, unsigned int d);
extern void _ZN9ActorBase18MarkForDestructionEv(void* self);
extern void Matrix4x3_FromRotationY(void* m, short ang);
extern void MulVec3Mat4x3(void* a, void* m, void* b);

extern void* data_0209f318;
extern int data_020a0e68[];
extern unsigned char data_ov066_0211abe0;
extern unsigned char data_ov066_0211ae0c;
extern void* data_ov066_0211b0cc;

int func_ov066_0211903c(char* self) {
    struct Vector3 v1, v2, in, out, star;
    void* cam;
    int msgid;

    if (*(unsigned short*)(self + 0x4d2)) return 1;

    cam = data_0209f318;
    if (*(int*)(self + 0x4a0) == 0) {
        _ZN6Camera9SetFlag_3Ev(cam);
        *(void**)(self + 0x490) = _ZN5Actor13ClosestPlayerEv(self);
        if (*(void**)(self + 0x490) != 0)
            _ZN6Player17SetNoControlStateEhih(*(void**)(self + 0x490), 5, -1, 0);
        *(int*)(self + 0x4a0) = 1;
    } else {
        v1.x = *(int*)(self + 0x5c);
        v1.y = *(int*)(self + 0x60);
        v1.z = *(int*)(self + 0x64);
        v2.x = *(int*)(self + 0x5c);
        v2.y = *(int*)(self + 0x60);
        v2.z = *(int*)(self + 0x64);
        v1.y += 0x100000;
        v2.x += 0x10000;
        v2.y += 0x100000;
        v2.z += 0x564000;
        func_020092c4(cam, (char*)cam + 0x80, &v1);
        func_020092c4(cam, (char*)cam + 0x8c, &v2);
    }

    if (data_ov066_0211abe0 == 3) {
        if (data_ov066_0211ae0c != 3) return 1;
    }

    if (*(int*)(self + 0x498) == 0) {
        if (*(void**)(self + 0x490) != 0) {
            in.x = 0; in.y = 0; in.z = 0;
            out.x = 0; out.y = 0; out.z = 0;
            in.y = 0x32000;
            in.z = -0x32000;

            Matrix4x3_FromRotationY(data_020a0e68, 0);
            MulVec3Mat4x3(&in, data_020a0e68, &out);

            out.x += *(int*)(self + 0x5c);
            out.y += *(int*)(self + 0x60);
            out.z += *(int*)(self + 0x64);

            msgid = 0xb8;
            if (data_ov066_0211abe0 == 0) {
                msgid = 0xb9;
                _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x14, 0x15666);
            }

            *(unsigned short*)(((int)*(void**)(self + 0x490) + 0x6ce) & 0xFFFFFFFFFFFFFFFF) |= 0x400;
            _ZN7Message11PrepareTalkEv();
            if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(*(void**)(self + 0x490), self, msgid, &out, 0, 0) == 1) {
                *(int*)(self + 0x498) = 1;
                func_02012694(0x145, self + 0x74);
            }
        }
    } else {
        if (*(void**)(self + 0x490) != 0) {
            if (_ZN6Player12GetTalkStateEv(*(void**)(self + 0x490)) < 0) {
                *(int*)(((int)cam + 0x154) & 0xFFFFFFFFFFFFFFFF) &= ~8;
                _ZN7Message7EndTalkEv();
                if (data_ov066_0211abe0 == 3) {
                    _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2d);
                    func_02011d2c();
                    func_ov066_02119454(self, &data_ov066_0211b0cc);
                } else {
                    _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0x15666);
                    _ZN5Sound22StopLoadedMusic_Layer3Ev();
                    func_02011cfc();
                    star.x = 0;
                    star.y = (int)0xffa24000;
                    star.z = (int)0xff1b4000;
                    _ZN5Actor19UntrackAndSpawnStarERajRK7Vector3j(self, (signed char*)(self + 0x673), *(unsigned char*)(self + 0x672), &star, 4);
                    _ZN9ActorBase18MarkForDestructionEv(self);
                }
            }
        }
    }
    return 1;
}
