struct Vector3 { int x, y, z; };
struct ActorBase;

extern void _ZN5Sound22StopLoadedMusic_Layer3Ev(void);
extern void func_02011cfc(void);
extern void ApproachLinear(short* a, short b, short c);
extern int func_ov078_02123804(char* c);
extern void* _ZN5Actor13ClosestPlayerEv(void* actor);
extern int Vec3_Dist(struct Vector3* a, struct Vector3* b);
extern int _ZN6Player9StartTalkER9ActorBaseb(void* player, void* actor, int b);
extern void _ZN7Message11PrepareTalkEv(void);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void* player, void* actor, unsigned int msgid, struct Vector3* v, unsigned int arg4, unsigned int arg5);
extern void func_02012694(int a, void* b);
extern int func_ov078_02125c48(char* c, void* p);

extern unsigned char data_0209f220;
extern void* data_ov078_0212700c;
extern void* data_ov078_0212701c;

int func_ov078_021259ec(char* c)
{
    int dist;
    struct Vector3 ppos;
    struct Vector3 v;
    void* player;
    unsigned int b;

    if (*(unsigned char*)(c + 0x506) == 1) {
        unsigned char *p = (unsigned char*)(((long long)(int)(c + 0x50a)) & 0xFFFFFFFFFFFFFFFFLL);
        *p += 1;
        if (*(unsigned char*)(c + 0x50a) > 0xc8) {
            *(int*)(c + 0xb0) = 0x10000003;
            *(unsigned char*)(c + 0x506) = 0;
            *(unsigned char*)(c + 0x50a) = 0;
            *(int*)(c + 0x500) = 3;
            _ZN5Sound22StopLoadedMusic_Layer3Ev();
            func_02011cfc();
            *(unsigned char*)(c + 0x508) = 0;
        }
    }

    ApproachLinear((short*)(c + 0x8e), *(short*)(c + 0x4f8), 0x800);
    *(short*)(c + 0x94) = *(short*)(c + 0x8e);

    if (func_ov078_02123804(c) == 1) {
        return 1;
    }

    player = _ZN5Actor13ClosestPlayerEv(c);
    if (player != 0) {
        ppos = *(struct Vector3*)((char*)player + 0x5c);

        if (*(unsigned char*)(c + 0x508) == 0) {
            dist = Vec3_Dist((struct Vector3*)(c + 0x5c), &ppos);
            if (dist < 0x12c000) {
                *(void**)(c + 0x430) = player;
                v.x = *(int*)(c + 0x5c);
                v.y = *(int*)(c + 0x60);
                v.z = *(int*)(c + 0x64);
                v.y = v.y + 0xc8000;

                if (_ZN6Player9StartTalkER9ActorBaseb(*(void**)(c + 0x430), c, 1)) {
                    _ZN7Message11PrepareTalkEv();
                    b = (data_0209f220 != 1) ? 0x93 : (unsigned int)(short)(*(int*)((char*)player + 8) + 0x96);
                    if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(player, c, b, &v, 0, 0)) {
                        func_02012694(0x12a, c + 0x74);
                        *(unsigned char*)(c + 0x508) = 1;
                        func_ov078_02125c48(c, &data_ov078_0212700c);
                    }
                }
            }
        } else {
            dist = Vec3_Dist((struct Vector3*)(c + 0x5c), &ppos);
            if (dist < 0x258000) {
                if (*(int*)(c + 0x4d8) - 0xa000 < ppos.y) {
                    func_ov078_02125c48(c, &data_ov078_0212701c);
                }
            }
        }
    }

    return 1;
}
