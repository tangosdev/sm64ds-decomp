//cpp
struct Sound { static int PlaySub(unsigned int, unsigned int, unsigned int, int, bool); };
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int, unsigned int, unsigned int, int, bool);

extern signed char data_0209f2f8;
extern int data_0209b49c;

extern "C" void func_ov002_020c1234(char* self);
void func_ov002_020c1234(char* self) {
    unsigned int a1, a2;
    int fx;
    if (data_0209f2f8 != 0xc && data_0209f2f8 != 0x2e) return;
    switch (*(int*)(self + 0x66c)) {
    case 0xa:
        a1 = 0x2d;
        a2 = 0x28;
        fx = (data_0209b49c >> 12 > 0x28) ? 0x8b33 : 0xd1e;
        *(unsigned char*)(self + 0x71c) = 1;
        break;
    case 0xb:
        *(unsigned char*)(self + 0x71c) = 1;
        a1 = 0;
        a2 = 0x7f;
        fx = 0x8b33;
        break;
    default:
        fx = 0x1451;
        a1 = 0x7f;
        a2 = 0;
        break;
    }
    if (*(unsigned char*)(self + 0x71c) == 0) return;
    if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x35, a1, a2, fx, 0) == 0) return;
    if (*(int*)(self + 0x66c) == 0xa) return;
    if (*(int*)(self + 0x66c) != 0xb) *(unsigned char*)(self + 0x71c) = 0;
}
