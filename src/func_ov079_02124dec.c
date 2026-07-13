//cpp
struct Vector3 { int x, y, z; };
extern "C" void _ZN9Animation7AdvanceEv(void*);
extern "C" void _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(
    void* self, const Vector3* pos, unsigned n, int fix, short s);

extern "C" void func_ov079_02124dec(char* c)
{
    volatile Vector3 v1;
    Vector3 w1;
    _ZN9Animation7AdvanceEv(c + 0x31c);
    if (*(unsigned char*)(c + 0x40c) == 0) {
        if (*(unsigned char*)(c + 0x403) != 0) {
            if (*(unsigned char*)(c + 0x402) != 0) {
                int x = *(int*)(c + 0x5c);
                v1.x = x;
                int y = *(int*)(c + 0x60);
                v1.y = y;
                int z = *(int*)(c + 0x64);
                y = y + 0x78000;
                
                *(volatile int *)&w1.x = x;
                v1.z = z;
                w1.z = z;
                v1.y = y;
                w1.y = y;

                _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(c, &w1, 5, 0x8000, 0);
                *(int*)(c + 0x3b0) = 8;
            } else {
                unsigned char* p = (unsigned char*)(((int)c + 0x40c) & 0xFFFFFFFFFFFFFFFFLL);
                (*p)++;
            }
        } else {
            *(unsigned char*)(c + 0x405) = 0;
        }
    } else {
        if (*(unsigned char*)(c + 0x403) == 0)
            *(unsigned char*)(c + 0x40c) = 0;
    }
}
