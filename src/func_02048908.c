/* func_02048908 at 0x02048908
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
typedef unsigned char u8;
typedef unsigned short u16;

extern int data_02099fa8;
extern int data_02099fac;
extern u16 data_02099fa4;
void func_0204f924(void* obj, int v);
void func_0204f7cc(void* obj, int a, int b);
extern int __aeabi_idiv(int a, int b);

void func_02048908(void* obj, int* p)
{
    int num = data_02099fac;
    int den = data_02099fa8;
    int a = 0x7f;
    int b = 0;
    int* s;
    int typ, val;
    int cur, arg;
    int half;

    if (obj == 0) return;
    s = *(int**)obj;
    if (s == 0) return;

    typ = *(u16*)((char*)s + 0x38);
    val = *(u16*)((char*)s + 0x3a);

    if (typ == 3) {
        if (val >= 0x4b && val <= 0x4c) {
            num = 0x384;
            den = 0xc8;
            b = 0x28;
        } else if (val == 0x28) {
            b = 0x78;
            a = 0x7f;
        }
    } else if (typ == 1) {
        if (val >= 0x2a && val <= 0x2b) {
            num = 0x44c;
            den = 0xc8;
        }
    }

    cur = ((int*)obj)[1];
    if ((unsigned)cur < (unsigned)den)
        arg = a;
    else
        arg = b + __aeabi_idiv((a - b) * (num - cur), num - den);
    func_0204f924(obj, arg);

    half = *p >> 12;
    half = half / 2;
    if (half > 0x3f) half = 0x3f;
    else if (half < -0x40) half = -0x40;
    func_0204f7cc(obj, data_02099fa4, half);
}