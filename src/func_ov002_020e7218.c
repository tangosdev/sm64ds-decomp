// @symbol func_ov002_020e7218
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern void _ZN5Actor11SpawnNumberERK7Vector3jbtPS_(void* self, struct Vector3* pos, unsigned int n, int flag, unsigned short t, void* src);
extern signed char data_0209f310[];

void func_ov002_020e7218(char* c, char* a, int gate) {
    if (gate == 0) {
        struct Vector3 pos[2];
        int b;
        int* v;
        v = (int*)((int)(((long long)(int)(a + 0x5c))));
        pos[0].x = v[0];
        pos[0].y = v[1];
        pos[0].z = v[2];
        b = (*(unsigned short*)(c + 0xc) != 0xb2);
        b = (b != 0);
        pos[0].y = pos[0].y + 0xc8000;
        pos[1].x = pos[0].x;
        pos[1].z = pos[0].z;
        pos[1].y = pos[0].y;
        _ZN5Actor11SpawnNumberERK7Vector3jbtPS_(c, &pos[1], data_0209f310[*(unsigned char*)(a + 0x6d8)], b, 0x15, a);
    }
    *(unsigned short*)((int)(((long long)(int)(c + 0x4a2)))) |= 0x40;
    func_ov002_020e7554(c);
}
