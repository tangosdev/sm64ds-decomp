/* func_ov002_020cec2c at 0x020cec2c (ov002), size 0x184
 * Matched byte-for-byte with mwccarm 1.2/sp2p3.
 * flags: -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
typedef unsigned char u8;
typedef unsigned short u16;
typedef signed char s8;

struct Vector3 { int x, y, z; };

extern int func_ov002_020ceb54(char *p);
extern void _ZN6Player4HealEi(char *self, int a);
extern int _ZN6Player9GetHealthEv(char *self);
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int c, const struct Vector3 *v, unsigned int d);
extern int func_ov002_020d91e0(char *thiz, int damage, int doPre);
extern void func_ov002_020da9d4(char *c);
extern int func_ov002_020c5dec(char *c, int r1);

extern s8 LEVEL_ID;

int func_ov002_020cec2c(char *self)
{
    int flag;
    int thresh;

    if (*(u8 *)(self + 0x709) != 0 ||
        *(u8 *)(self + 0x6f9) != 0 ||
        *(u8 *)(self + 0x703) != 0 ||
        *(u8 *)(self + 0x708) != 0)
        return 0;

    flag = 0;
    if (LEVEL_ID == 0x13 || LEVEL_ID == 0x14 || LEVEL_ID == 0x30)
        flag = 1;

    if (func_ov002_020ceb54(self) == 0 && flag == 0) {
        *(u16 *)(self + 0x6ca) = 0;
        if (*(u16 *)(self + 0x6b8) == 0) {
            _ZN6Player4HealEi(self, 0x100);
            *(u16 *)(self + 0x6b8) = 0xf;
        }
        return 0;
    }

    thresh = 0xf0;
    if (flag != 0) thresh = 0x50;

    if (_ZN6Player9GetHealthEv(self) <= 2) {
        *(unsigned int *)(self + 0x624) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
            *(unsigned int *)(self + 0x624), 2, 0x27, (struct Vector3 *)(self + 0x74), 0);
    }

    {
        u16 *p = (u16 *)(((long long)(int)(self + 0x6ca)) & 0xFFFFFFFFFFFFFFFFLL);
        *p = *p + 1;
    }

    if (*(u16 *)(self + 0x6ca) < thresh) goto ret0;

    *(u16 *)(self + 0x6ca) = 0;
    if (func_ov002_020d91e0(self, 0x100, 0) == 0) goto ret0;

    func_ov002_020da9d4(self);
    func_ov002_020c5dec(self, 7);
    return 1;

ret0:
    return 0;
}
