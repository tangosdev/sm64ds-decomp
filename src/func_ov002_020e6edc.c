typedef unsigned int u32;
typedef int s32;
typedef short s16;
typedef unsigned short u16;
struct Actor;
struct BCA_File;
struct Vector3 { s32 x, y, z; };
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char* self, struct BCA_File* f, int a, int fix, u32 b);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(char* self, struct Actor* a, const struct Vector3* pos, int r, u32 f1, u32 f2, u32 f3);
extern char data_ov002_02110934[];
extern struct Vector3 data_ov002_0210aa0c;

void func_ov002_020e6edc(char* c)
{
    struct Vector3 v;
    u16* p;
    *(int*)(c+0x43c) = 8;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x30c, *(struct BCA_File**)(data_ov002_02110934+4), 0x40000000, 0x1000, 0);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x370, *(struct BCA_File**)(data_ov002_02110934+4), 0x40000000, 0x1000, 0);
    v.x = data_ov002_0210aa0c.x;
    v.y = data_ov002_0210aa0c.y;
    v.z = data_ov002_0210aa0c.z;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(c+0x110, (struct Actor*)c, &v, 0x64000, 0x96000, 1, 0);
    *(int*)(c+0x440) = 0xc;
    p = (u16*)(((int)c + 0x4a2) & 0xFFFFFFFFFFFFFFFF);
    *p = (*p & ~1) | 1;
    *p |= 2;
}
