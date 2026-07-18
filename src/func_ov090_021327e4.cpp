//cpp
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned int u32;

struct Vector3 { int x, y, z; };
struct Vector3_16 { s16 x, y, z; };

extern "C" {
void _ZN9Animation7AdvanceEv(void* a);
void func_ov090_02132730(char* thiz);
void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 id, u32 a, const Vector3& v, const Vector3_16* r, int b, int c);
void func_02012790(u32 a);
void* _ZN5Actor11SpawnNumberERK7Vector3jbtPS_(void* self, const Vector3& v, u32 n, int b, u16 t, void* p);
}

extern "C" int func_ov090_021327e4(char* c)
{
    void* o;
    Vector3 num1;
    Vector3 num2;

    _ZN9Animation7AdvanceEv(c + 0x35c);
    func_ov090_02132730(c);

    if (*(int*)(c + 0x378) >= 5)
        goto Ldecay;

    if (*(u16*)(c + 0x100) == 0) {
        o = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                0xf4, 1, *(Vector3*)(c + 0x39c), (Vector3_16*)(c + 0x8c),
                *(signed char*)(c + 0xcc), -1);
        if (o != 0) {
            int idx = *(int*)(c + 0x3fc);
            int* pf = (int*)((((long long)(int)(c + 0x3fc)) & 0xFFFFFFFFFFFFFFFFLL));
            *(int*)(c + idx * 4 + 0x3ac) = *(int*)((char*)o + 4);
            *pf += 1;
            if (*(int*)(c + 0x3fc) >= 0x14)
                *(int*)(c + 0x3fc) = 0;
            *(int*)((char*)o + 0x38c) = (int)c;
        }
        *(u16*)(c + 0x100) = 0x27;
    }

    if (*(int**)(c + 0x3a8) == 0)
        goto Ldecay;

    if (*(int*)(c + 0x378) == 0) {
        int i;
        int key = (*(int**)(c + 0x3a8))[1];
        for (i = 0; i < 0x14; i++) {
            int slot = ((int*)(c + 0x3ac))[i];
            if ((0, slot) == key) {
                *(int*)(c + 0x400) = i;
                *(int*)((((long long)(int)(c + 0x378)) & 0xFFFFFFFFFFFFFFFFLL)) += 1;
                func_02012790(0x25);
                num1 = *(Vector3*)((((long long)(int)((char*)*(void**)(c + 0x3a8) + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL));
                _ZN5Actor11SpawnNumberERK7Vector3jbtPS_(c, num1, *(int*)(c + 0x378), 0, 0, 0);
                *(int*)(c + 0x3a8) = 0;
                return 1;
            }
        }
        goto Lreset;
    } else {
        *(int*)((((long long)(int)(c + 0x400)) & 0xFFFFFFFFFFFFFFFFLL)) += 1;
        if (*(int*)(c + 0x400) >= 0x14)
            *(int*)(c + 0x400) = 0;
        {
            int r0 = *(int*)(c + 0x400);
            int r1 = (int)*(int**)(c + 0x3a8);
            r0 = (int)(c + (r0 << 2));
            r1 = *(int*)(r1 + 4);
            r0 = *(int*)(r0 + 0x3ac);
            if (r0 != r1)
                goto Lreset;
        }
        *(int*)((((long long)(int)(c + 0x378)) & 0xFFFFFFFFFFFFFFFFLL)) += 1;
        func_02012790(0x25);
        num2 = *(Vector3*)((((long long)(int)((char*)*(void**)(c + 0x3a8) + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL));
        _ZN5Actor11SpawnNumberERK7Vector3jbtPS_(c, num2, *(int*)(c + 0x378), 0, 0, 0);
        *(int*)(c + 0x3a8) = 0;
        return 1;
    }

Lreset:
    *(int*)(c + 0x378) = 0;
    *(int*)(c + 0x400) = 0;
    *(int*)(c + 0x3a8) = 0;
Ldecay:
    if (*(int*)(c + 0x378) == 5) {
        *(int*)((((long long)(int)(c + 0x38c)) & 0xFFFFFFFFFFFFFFFFLL)) += 1;
        if (*(int*)(c + 0x38c) > 0x1e) {
            _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                0xb2, *(int*)(c + 0x388) | 0x40, *(Vector3*)(c + 0x5c),
                (Vector3_16*)(c + 0x8c), *(signed char*)(c + 0xcc), -1);
            *(int*)(c + 0x378) = 0xa;
        }
    }
    return 1;
}
