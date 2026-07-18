struct Vector3 { int x, y, z; };
struct Vector3_16 { short x, y, z; };

char* _ZN5Actor13ClosestPlayerEv(void* self);
short Vec3_HorzAngle(void* a, void* b);
void ApproachAngle(short* p, int target, int a, int b, int limit);
int _ZNK9Animation12WillHitFrameEi(void* a, int f);
short Vec3_VertAngle(void* a, void* b);
void* _ZN5Actor13SpawnFireballERK7Vector3PK10Vector3_165Fix12IiES7_j(void* self, void* pos, void* vel, int a, int b, unsigned int d);
void func_02012694(int a, void* p);
int _ZN9Animation8FinishedEv(void* a);
int func_ov070_02120020(void* c, void* p);
extern char data_ov070_0212359c[];

#define M(p) ((long long)(int)(p) & 0xffffffffffffffffLL)

int func_ov070_0211f48c(char* c) {
    char* pl;
    struct Vector3_16 vel;
    struct Vector3 posbuf;
    struct Vector3 fp;
    struct Vector3 tmp;

    pl = _ZN5Actor13ClosestPlayerEv(c);
    if ((unsigned)(*(int*)(c+0x358) << 4) >> 0x10 >= 0xd)
        goto hitframe;

    if (pl != 0) {
        posbuf = *(struct Vector3*)(pl+0x5c);
        tmp.x = posbuf.x;
        tmp.y = posbuf.y;
        tmp.z = posbuf.z;
        *(short*)(c + 0x3e6) = Vec3_HorzAngle(c+0x5c, &tmp);
    }
    ApproachAngle((short*)(c+0x94), *(short*)(c + 0x3e6), 0xa, 0x400, 0x200);

hitframe:
    if (_ZNK9Animation12WillHitFrameEi(c+0x350, 0xd) != 0) {
        vel = *(struct Vector3_16*)(c+0x8c);
        if (pl != 0) {
            int *base = (int *)(int)M(pl + 0x5c);
            fp.x = base[0];
            fp.y = base[1];
            fp.z = base[2];
            vel.x = Vec3_VertAngle(c+0x5c, &fp);
        }
        _ZN5Actor13SpawnFireballERK7Vector3PK10Vector3_165Fix12IiES7_j(c, c+0x5c, &vel, 0x1e000, 0xa000, 1);
        func_02012694(0x105, c+0x74);
    }
    if (_ZN9Animation8FinishedEv(c+0x350) != 0) {
        *(int*)(c+0x358) = 0;
        *(short*)(c + 0x3cc) = 0x5a;
        func_ov070_02120020(c, data_ov070_0212359c);
    }
    return 1;
}
