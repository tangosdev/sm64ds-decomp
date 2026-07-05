typedef int Fix12i;
struct Vector3 { int x, y, z; };
extern unsigned char func_ov002_020ec640(char* p);
extern void _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(char* thiz, const struct Vector3* v, unsigned int n, Fix12i f, short s);
extern int func_ov002_020ec628(char* p);
extern void func_ov002_020ec80c(char* thiz, const struct Vector3* v, int a, Fix12i f, int s);

void func_ov002_020ec728(char* c)
{
    unsigned int idx = *(unsigned char*)(c + 0x420);
    unsigned int n;
    struct Vector3 vec;
    if (idx >= 5) return;
    if (*(unsigned char*)(c + idx + 0x421) != 0) return;
    n = func_ov002_020ec640(c);
    {
        int tx = *(int*)(c + 0x5c);
        int tz = *(int*)(c + 0x64);
        int ty = *(int*)(c + 0x60) + 0x28000;
        vec.x = tx;
        vec.y = ty;
        vec.z = tz;
    }
    if (n != 0) {
        struct Vector3 v2;
        v2.x = vec.x;
        v2.y = vec.y;
        v2.z = vec.z;
        _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(c, &v2, n, 0x2000, 0);
    }
    if (func_ov002_020ec628(c)) {
        struct Vector3 v3;
        v3.x = *(int*)&vec.x;
        v3.y = *(int*)&vec.y;
        v3.z = *(int*)&vec.z;
        func_ov002_020ec80c(c, &v3, 1, 0x2000, 0);
    }
    *(unsigned char*)(c + *(unsigned char*)(c + 0x420) + 0x421) = 1;
}
