typedef unsigned char u8;
typedef struct { int x, y, z; } Vector3;

extern unsigned int _ZN5Event6GetBitEj(unsigned int bit);
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int bankId, const Vector3 *pos);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *thiz);

#define LM(p) ((u8*)(int)(((long long)(int)(p)) & 0xFFFFFFFFFFFFFFFFLL))
#define LMS(p) ((short*)(int)(((long long)(int)(p)) & 0xFFFFFFFFFFFFFFFFLL))
#define LMI(p) ((int*)(int)(((long long)(int)(p)) & 0xFFFFFFFFFFFFFFFFLL))

int func_ov002_020bac18(char *c)
{
    u8 state = *(u8*)(c + 0x31f);

    switch (state) {
    case 0:
        if (_ZN5Event6GetBitEj(*(u8*)(c+0x321)) != 0) {
            if (DecIfAbove0_Byte((u8*)(c+0x320)) == 0) {
                *(u8*)(c+0x320) = 0x40;
                (*LM(c+0x31f))++;
                *LMI(c+0xb0) &= ~1;
                _ZN5Sound9PlayBank3EjRK7Vector3(0x3f, (const Vector3*)(c+0x74));
            }
        }
        break;
    case 1:
        if (DecIfAbove0_Byte((u8*)(c+0x320)) != 0) {
            if (*(u8*)(c+0x31e) != 0) {
                *LMS(c+0x8e) += 0x100;
            } else {
                *LMS(c+0x8e) -= 0x100;
            }
        } else {
            (*LM(c+0x31f))++;
            *LMI(c+0xb0) |= 1;
        }
        break;
    case 2:
        break;
    }

    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    return 1;
}
