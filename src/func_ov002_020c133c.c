typedef unsigned char u8;

typedef struct Vector3 { int x, y, z; } Vector3;

extern int func_ov002_020d82f0(void *c);
extern int _ZN6Player9GetHealthEv(void *self);
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int c, Vector3 const *v, unsigned int d);
extern int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *self, Vector3 const *v, unsigned int b, int c, unsigned int d, unsigned int e, unsigned int f);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, Vector3 const *v);
extern int func_ov002_020d91e0(char *thiz, int damage, int doPre);
extern int func_ov002_020c5dec(char *c, int r1);

void func_ov002_020c133c(char *a)
{
    if (*(u8 *)(a + 0x6f9) != 0) return;
    {
        int lim = *(int *)(a + 0x650);
        if (lim == (int)0x80000000) goto set0;
        if (*(int *)(a + 0x60) >= lim - 0x64000) goto set0;
    }
    if (!func_ov002_020d82f0(a)) return;
    if (*(u8 *)(a + 0x6de) != 0) return;
    if (_ZN6Player9GetHealthEv(a) <= 2)
        *(unsigned int *)(a + 0x624) = _ZN5Sound8PlayLongEjjjRK7Vector3j(*(unsigned int *)(a + 0x624), 2, 0x27, (Vector3 *)(a + 0x74), 0);
    if (*(u8 *)(a + 0x71d) != 0) {
        u8 *p = (u8 *)(((long long)(int)(a + 0x71d)) & 0xFFFFFFFFFFFFFFFFLL);
        *p = *p - 1;
        return;
    }
    {
        Vector3 v;
        v.x = *(int *)(a + 0x5c);
        v.y = *(int *)(a + 0x60);
        v.z = *(int *)(a + 0x64);
        if (!_ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, &v, 0, 0, 1, 0, 0)) return;
    }
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8 *)(a + 0x6d9), 6, (Vector3 *)(a + 0x74));
    if (func_ov002_020d91e0(a, 0x100, 0) != 0 && _ZN6Player9GetHealthEv(a) == 0)
        func_ov002_020c5dec(a, 5);
    *(u8 *)(a + 0x71d) = 0x40;
    return;
set0:
    *(u8 *)(a + 0x71d) = 0;
}
