typedef unsigned int u32;
typedef unsigned short u16;

struct Vector3 { int x, y, z; };

extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void MulVec3Mat4x3(void* in, void* m, void* out);
extern void* _ZN5Actor10FindWithIDEj(u32 id);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* self, struct Vector3* v, u32 a, int fix, u32 b, u32 d, u32 e);

extern unsigned char data_0209f220;
extern int data_ov016_02114dbc;
extern int data_020a0e68[];

#define LA(p) ((void*)(unsigned long)(((long long)(int)(unsigned long)(p)) & 0xFFFFFFFFFFFFFFFFLL))

void func_ov016_02111284(char* c)
{
    struct Vector3 va;
    struct Vector3 vb;
    struct Vector3 out;
    struct Vector3 hv;
    int r4;
    int r6;
    void* p;
    int t;
    u32 id;
    int* ctr;
    int idx;
    char* base448;
    char* base44c;
    char* base450;
    int i;
    int* px;

    *(volatile int*)&va.x = *(int*)(c + 0x5c);
    *(volatile int*)&va.y = *(int*)(c + 0x60);
    *(volatile int*)&va.z = *(int*)(c + 0x64);

    vb.x = 0;
    vb.y = 0;
    vb.z = 0;
    out.x = 0;
    out.y = 0;
    out.z = 0;

    r4 = 0x52000;

    if (data_0209f220 == 1 && *(int*)(c + 0x34c) != (int)&data_ov016_02114dbc) {
        r6 = 0x128000;
        r6 = -r6;
        vb.y = r6;
        vb.z = 0x7c000;
        va.x = 0x15e0000;
        va.y = 0xfee90000;
        va.z = 0x65e000;
        Matrix4x3_FromRotationY(data_020a0e68, *(short*)(c + 0x8e));
        MulVec3Mat4x3(&vb, data_020a0e68, &out);
        va.x = va.x + out.x;
        va.y = va.y + out.y;
        va.z = va.z + out.z;
        *(int*)(c + 0x184) = va.x;
        {
            int c1 = 0xc8000;
            *(int*)(c + 0x188) = va.y;
            int c2 = 0xf0000;
            *(int*)(c + 0x18c) = va.z;
            *(int*)(c + 0x154) = c1;
            *(int*)(c + 0x158) = c2;
        }
    }

    ctr = (int*)LA(c + 0x3fc);
    *ctr = *ctr + 1;
    if (*(int*)(c + 0x3fc) > 6)
        *(int*)(c + 0x3fc) = 0;

    idx = *(int*)(c + 0x3fc);
    if (idx == 5)
        r4 = 0xb8000;
    vb.y = 0;
    vb.x = 0;
    vb.z = 0;
    out.x = 0;
    out.y = 0;
    out.z = 0;
    vb.y = -0x48000;
    if (*(int*)(c + 0x3fc) == 5)
        vb.z = 0x7c000;

    Matrix4x3_FromRotationY(data_020a0e68, *(short*)(c + 0x8e));
    MulVec3Mat4x3(&vb, data_020a0e68, &out);

    {
      /* V4 structure but force *0xc via char index on int base */
      int* p448 = (int*)(c + 0x448);
      int* p44c = (int*)(c + 0x44c);
      int* p450 = (int*)(c + 0x450);
      int idx2 = *(int*)(c + 0x3fc);
      ((int*)((char*)p448 + idx2 * 0xc))[0] = ((int*)((char*)p448 + idx2 * 0xc))[0] + out.x;
      idx2 = *(int*)(c + 0x3fc);
      ((int*)((char*)p44c + idx2 * 0xc))[0] = ((int*)((char*)p44c + idx2 * 0xc))[0] + out.y;
      idx2 = *(int*)(c + 0x3fc);
      ((int*)((char*)p450 + idx2 * 0xc))[0] = ((int*)((char*)p450 + idx2 * 0xc))[0] + out.z;
      idx2 = *(int*)(c + 0x3fc);
      px = (int*)((char*)p448 + idx2 * 0xc);
      *(int*)(c + 0x144) = px[0];
      *(int*)(c + 0x148) = px[1];
      *(int*)(c + 0x14c) = px[2];
    }

    *(int*)(c + 0x114) = r4;
    *(int*)(c + 0x118) = 0x70000;

    id = *(u32*)(c + 0x134);
    if (id == 0)
        return;

    p = _ZN5Actor10FindWithIDEj(id);
    t = (int)(*(u16*)((char*)p + 0xc) == 0xbf);
    if (t == 0)
        return;

    hv.x = *(int*)(c + 0x5c);
    hv.y = *(int*)(c + 0x60);
    hv.z = *(int*)(c + 0x64);
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(p, &hv, 3, 0xc000, 1, 0, 1);
}
