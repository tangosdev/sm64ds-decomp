typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef int Fix12;

extern int _ZN6Player6IsAnimEj(void *c, u32 anim);
extern int _ZNK6Player14GetBodyModelIDEjb(void *c, u32 id, int b);
extern int _ZNK9Animation12WillHitFrameEi(void *anim, int frame);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 a, void *v);
extern void func_ov002_020c2f64(void *c);
extern void _ZN6Player4HealEi(void *c, int amt);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void *c, u32 anim, int a, Fix12 b, u32 d);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, void *v);
extern void func_02012694(u32 id, void *v);
extern u8 data_0209f2fc;

void func_ov002_020c75f0(char *c)
{
    if (_ZN6Player6IsAnimEj(c, 0x56))
    {
        int model = _ZNK6Player14GetBodyModelIDEjb(c, (*(int *)(c + 8)) & 0xff, 0);
        if (_ZNK9Animation12WillHitFrameEi((char *)(*(int *)(c + model * 4 + 0xdc)) + 0x50, 0))
        {
            _ZN5Sound9PlayBank0EjRK7Vector3(6, c + 0x74);
        }
    }

    if (*(u8 *)(c + 0x6de) == 0)
    {
        func_ov002_020c2f64(c);
        *(u8 *)(c + 0x70c) = 0;
        *(u8 *)(c + 0x6e5) = 1;
        if (data_0209f2fc == 1 || data_0209f2fc == 2)
        {
            _ZN6Player4HealEi(c, 0x880);
        }
        switch (*(u8 *)(c + 0x6e3))
        {
        case 3:
        case 10:
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x51, 0x40000000, 0x1000, 0);
            break;
        case 4:
            *(int *)(c + 0x98) = 0;
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x55, 0x40000000, 0x1000, 0);
            break;
        case 6:
        case 7:
            *(u8 *)(c + 0x6de) = 1;
            *(u8 *)(c + 0x6df) = 0;
            *(int *)(((int)c + 0x98)) >>= 2;
            *(int *)(c + 0xa8) = -(*(int *)(c + 0xa8) >> 2);
            *(u8 *)(c + 0x70c) = 0;
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x13, 0x40000000, 0x1000, 0);
            break;
        case 5:
        case 8:
        case 9:
        case 11:
        case 12:
        case 15:
            *(int *)(c + 0x98) = 0;
            break;
        case 13:
        case 16:
        case 17:
        case 18:
            *(int *)(c + 0x98) = 0;
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x52, 0x40000000, 0x1000, 0);
            break;
        }
        if (data_0209f2fc == 2)
        {
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8 *)(c + 0x6d9), 0x24, c + 0x74);
        }
    }
    else
    {
        u8 t = *(u8 *)(c + 0x6e3);
        if (t == 0xb || t == 0xc || t == 0x12)
        {
            if (*(u16 *)(c + 0x6a6) == 1)
            {
                func_02012694(0x28, c + 0x74);
            }
        }
    }
}
