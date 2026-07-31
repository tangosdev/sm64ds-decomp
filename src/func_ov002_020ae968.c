typedef unsigned short u16;

enum { false, true };

extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int id, void* v);
extern u16 data_ov002_020ff01c[];

void func_ov002_020ae968(char* c, char* arg)
{
    int b;
    *(int*)(c + 0xa8) = 0x14000;
    b = *(u16*)(arg + 0xc);
    b = b == 0xbf;
    if (b != false) {
        u16 t = data_ov002_020ff01c[(u16)*(int*)(arg + 8)];
        *(int*)(c + 0x98) = ((int)t << 0xf) / 50;
    } else {
        *(int*)(c + 0x98) = 0x14000;
    }
    *(int *)(((int)c + 0xb0)) &= ~1;
    _ZN5Sound9PlayBank0EjRK7Vector3(9, (void*)(c + 0x74));
}
