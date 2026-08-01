//cpp
typedef unsigned int u32;
typedef unsigned char u8;

struct Vector3 { int x, y, z; };
struct Matrix4x3 { int m[12]; };

extern "C" {

int _ZN9ActorBase14BeforeBehaviorEv(char* self);
unsigned char IsAreaShowing(int idx);
void MulVec3Mat4x3(Vector3* v, Matrix4x3* m, Vector3* dst);
int _ZN7Clipper13Func_020150E8ER7Vector35Fix12IiEPh(char* thisp, Vector3* v, int f, unsigned char* p);

u32 data_0209b458;
int data_0209fc68;
Matrix4x3 data_0209b3ec;
char data_0209f43c;
u8 data_0209f274;
u8 data_0209f2c4;
u8 data_0209f20c;
u8 data_0209f294;
u32 data_0209b464;

int _ZN5Actor14BeforeBehaviorEv(char* self)
{
    if (!_ZN9ActorBase14BeforeBehaviorEv(self))
        return 0;

    data_0209b458 = 0;

    signed char c = *(signed char*)(self + 0xcc);
    if (c >= 0 && !IsAreaShowing(c)) {
        *(u32*)((int)(self + 0xb0)) |= 0x38;
        *(int*)(self + 0x74) = 0x7fffffff;
        *(int*)(self + 0x78) = 0x7fffffff;
        *(int*)(self + 0x7c) = 0x7fffffff;
        if (data_0209fc68 == 0 || (*(u32*)(self + 0xb0) & 0x10000))
            return 0;
    } else {
        if (*(int*)(self + 0xb8) != 0) {
            Vector3 tmp;
            tmp.x = *(int*)(self + 0x5c) >> 3;
            tmp.y = (*(int*)(self + 0x60) + *(int*)(self + 0xb4)) >> 3;
            tmp.z = *(int*)(self + 0x64) >> 3;
            MulVec3Mat4x3(&tmp, &data_0209b3ec, (Vector3*)(self + 0x74));
        }
        if (*(u32*)(self + 0xb0) & 0x10003) {
            int r = _ZN7Clipper13Func_020150E8ER7Vector35Fix12IiEPh(
                        &data_0209f43c,
                        (Vector3*)(self + 0x74),
                        *(int*)(self + 0xb8),
                        (unsigned char*)(self + 0xc4));
            int thresh = *(int*)(self + 0xbc);
            if (data_0209f274)
                thresh <<= 1;
            if (r > thresh) {
                *(u32*)((int)(self + 0xb0)) |= 0x18;
                if (*(u32*)(self + 0xb0) & 0x10000)
                    return 0;
            } else {
                u32* p = (u32*)((int)(self + 0xb0));
                *p &= ~0x38;
                if (r > *(int*)(self + 0xc0))
                    *p |= 0x10;
            }
        } else {
            *(u32*)((int)(self + 0xb0)) &= ~0x38;
        }
    }

    if (((data_0209f2c4 | data_0209f20c | data_0209f294) & 0xff) == 0) {
        u32 f = *(u32*)(self + 0xb0);
        if ((f & 9) != 9 && (data_0209b464 == 0 || (f & data_0209b464) != 0))
            goto do_copy;
        if (data_0209fc68 != 0)
            goto do_copy;
    }
    return 0;

do_copy:
    *(int*)(self + 0x68) = *(int*)(self + 0x5c);
    *(int*)(self + 0x6c) = *(int*)(self + 0x60);
    *(int*)(self + 0x70) = *(int*)(self + 0x64);
    return 1;
}

}
