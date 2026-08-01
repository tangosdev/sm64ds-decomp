//cpp
// @symbol func_ov075_0211b458
/* recovered: shared common types */
#include "common.h"

struct Foo;
typedef void (Foo::*PMF)();
extern "C" {
extern void* _ZN6Memory13operator_new2Ej(unsigned int sz);
extern void* func_0201787c(void* sfp);
extern void func_ov075_0211aa94(void* a, void* b);
extern void _ZN13SharedFilePtr7ReleaseEv(void* sfp);
extern void func_ov075_0211aa00(char* r4);
extern char data_ov075_0211d980;
}
extern PMF data_ov075_0211d994;

extern "C" int func_ov075_0211b458(char* c, int* src, int a2) {
    *(unsigned char*)(c + 0xa6) = 0x14;
    *(unsigned char*)(c + 0xa7) = 0x14;
    *(short*)(c + 0xa4) = *(unsigned char*)(c + 0xa6) * *(unsigned char*)(c + 0xa7);
    *(void**)(c + 0x80) = _ZN6Memory13operator_new2Ej(*(unsigned short*)(c + 0xa4) * 0x18);
    func_ov075_0211aa94(c, func_0201787c(&data_ov075_0211d980));
    _ZN13SharedFilePtr7ReleaseEv(&data_ov075_0211d980);
    *(PMF**)(c + 0x84) = &data_ov075_0211d994;
    (((Foo*)c)->**(*(PMF**)(c + 0x84)))();
    *(int*)(c) = src[0];
    *(int*)(c + 4) = src[1];
    *(int*)(c + 8) = src[2];
    *(short*)(c + 0xc) = 0;
    *(short*)(c + 0xe) = a2;
    *(short*)(c + 0x10) = 0;
    func_ov075_0211aa00(c);
    return 1;
}
