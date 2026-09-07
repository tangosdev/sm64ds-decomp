typedef struct Vector3 { int x, y, z; } Vector3;
#define AT(p,off) ((void*)(int)((char*)(p)+(off)))
extern short data_02082214[];
#define SINE_TABLE data_02082214
extern char* _ZN8dActor_c13ClosestPlayerEv(void*);
extern int func_ov002_020d0d2c(void*);
extern int Vec3_HorzLen(const Vector3*);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int, int);
extern short Vec3_HorzAngle(const Vector3*, const Vector3*);
extern void _Z14ApproachLinearRsss(short*, int, int);
extern void func_ov002_020af3a8(void*);

void func_ov002_020af0c0(char* c){
    char* p = _ZN8dActor_c13ClosestPlayerEv(c);
    if(p != 0){
        Vector3 diff;
        Vector3 ppos;
        int* s = (int*)AT(p, 0x5c);
        ppos.x = s[0];
        ppos.y = s[1];
        ppos.z = s[2];
        diff.x = ppos.x - *(int*)(c+0x5c);
        if(func_ov002_020d0d2c(p) != 0)
            diff.y = ppos.y - *(int*)(c+0x60) - 0x50000;
        else
            diff.y = ppos.y - *(int*)(c+0x60) + 0x78000;
        diff.z = ppos.z - *(int*)(c+0x64);
        int len = Vec3_HorzLen(&diff);
        int pitch = _ZN4cstd5atan2E5Fix12IiES1_(len, diff.y);
        short yaw = Vec3_HorzAngle((Vector3*)(c+0x5c), &ppos);
        _Z14ApproachLinearRsss((short*)(c+0x94), yaw, 0x1000);
        _Z14ApproachLinearRsss((short*)(c+0x92), pitch, 0x1000);
        *(int*)(c+0xa8) = (short)SINE_TABLE[(*(unsigned short*)(c+0x92) >> 4)*2+1] * (short)0x1e;
        *(int*)(c+0x98) = (short)SINE_TABLE[(*(unsigned short*)(c+0x92) >> 4)*2] * (short)0x1e;
    }
    func_ov002_020af3a8(c);
}
