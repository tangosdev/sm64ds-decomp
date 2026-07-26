typedef short s16;
typedef unsigned short u16;
struct Vector3 { int x, y, z; };

extern s16 _ZN5Actor18HorzAngleToCPlayerEv(void* c);
extern void _Z14ApproachLinearRsss(s16* p, s16 a, s16 b);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void* self, void* ab, unsigned int id, const struct Vector3* v, unsigned int a, unsigned int b);
extern void func_02012790(int x);
extern int _ZN6Player12GetTalkStateEv(void* self);
extern int func_ov085_0212e728(void* c, void* p);

extern unsigned char data_0209d6bc;
extern unsigned char data_0209f284;
extern int data_ov085_02130830[];

#define AT(p, off) ((void*)(int)(((long long)(int)((char*)(p) + (off))) & 0xFFFFFFFFFFFFFFFFLL))

int func_ov085_0212e19c(char* c)
{
    _Z14ApproachLinearRsss((s16*)(c + 0x8e), _ZN5Actor18HorzAngleToCPlayerEv(c), 0x800);
    switch (*(int*)(c + 0x2d8)) {
    case 0:
        {
            u16* m = (u16*)AT(*(char**)(c + 0x2a0), 0x6ce);
            *m = *m | 0x400;
        }
        if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(
                *(void**)(c + 0x2a0), c, 0x182, (struct Vector3*)(c + 0x5c), 1, 0) == 1) {
            *(int*)AT(c, 0x2d8) += 1;
        }
        break;
    case 1:
        if (data_0209d6bc == 7) { *(int*)AT(c, 0x2d8) += 1; }
        break;
    case 2:
        if (data_0209d6bc == 7)
            break;
        func_02012790(0x24);
        data_0209f284 = 1;
        { *(int*)AT(c, 0x2d8) += 1; }
        break;
    case 3:
        if (data_0209d6bc == 9) { *(int*)AT(c, 0x2d8) += 1; }
        break;
    case 4:
        if (_ZN6Player12GetTalkStateEv(*(void**)(c + 0x2a0)) == 2)
            func_ov085_0212e728(c, data_ov085_02130830);
        break;
    }
    return 1;
}
