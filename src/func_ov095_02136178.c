extern int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int c, void* v, unsigned int e);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int a, void* v);
extern void _Z14ApproachLinearRiii(int* p, int a, int b);

void func_ov095_02136178(char* c)
{
    *(int*)(c + 0x340) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
        *(unsigned int*)(c + 0x340), 3, 0x82, c + 0x74, 0);
    if (*(unsigned short*)(c + 0x344) == 0) {
        int b = (int)(*(unsigned short*)(c + 0xc) == 0x21);
        if (b != 0)
            _ZN5Sound9PlayBank3EjRK7Vector3(0x40, c + 0x74);
    }

    _Z14ApproachLinearRiii((int*)(c + 0xa8), -0xa000, -0x2000);
    {
        int* p = (int*)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFF);
        int v = *p + *(int*)(c + 0xa8);
        *p = v;
    }

    if (*(int*)(c + 0x60) > *(int*)(c + 0x338)) return;
    *(int*)(c + 0x60) = *(int*)(c + 0x338);
    *(unsigned char*)(c + 0x346) = 1;

    {
        int s = *(unsigned char*)(c + 0x349);
        if (s == 1) { *(int*)(c + 0x32c) = 4; return; }
        if (s == 2) { *(int*)(c + 0x32c) = 3; return; }
    }
    if (*(void**)(c + 0x324) != 0) {
        if (*(int*)(*(char**)(c + 0x324) + 0x60) >= *(int*)(c + 0x33c)) {
            *(int*)(c + 0x32c) = 1;
            return;
        }
    }
    *(int*)(c + 0x32c) = 3;
}
