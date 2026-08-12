typedef unsigned char u8;
typedef unsigned short u16;
typedef int s32;
struct Vector3 { int x, y, z; };

extern u8 DecIfAbove0_Byte(u8* p);
extern void* _ZN5Actor10FindWithIDEj(unsigned int id);
extern void _ZN6Player4HealEi(char* p, int amt);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int a, const struct Vector3* v);
extern void Math_Function_0203b14c(int* p, int a, int b, int c, int d);
extern void _ZN9Animation7AdvanceEv(char* a);
extern void _ZN12CylinderClsn5ClearEv(char* c);
extern void _ZN12CylinderClsn6UpdateEv(char* c);

int _ZN7Seaweed8BehaviorEv(char* self)
{
    int spin = 0x1000;
    DecIfAbove0_Byte((u8*)(self + 0x170));

    if (*(int*)(self + 0x15c) != 0) {
        if (*(u8*)(self + 0x171) == 0) {
            *(u8*)(self + 0x171) = 1;
            *(u8*)(self + 0x170) = 0;
        }
        if (*(u8*)(self + 0x170) == 0) {
            char* other = (char*)_ZN5Actor10FindWithIDEj(*(int*)(self + 0x15c));
            if (other != 0) {
                int b = (int)(*(u16*)(other + 0xc) == 0xbf);
                if (b != 0) {
                    int hp = *(int*)(other + 0x98) / 0x1000;
                    int amt = 0x100;
                    if (hp >= 8) {
                        hp -= 8;
                        amt = (hp / 4 + 1) << 8;
                    }
                    _ZN6Player4HealEi(other, amt);
                    _ZN5Sound9PlayBank3EjRK7Vector3(0x61, (struct Vector3*)(self + 0x74));
                }
            }
            *(u8*)(self + 0x170) = 0x5a;
        }
    } else {
        *(u8*)(self + 0x171) = 0;
    }

    if (*(u8*)(self + 0x170) > 0x2d)
        spin = 0x8000;
    Math_Function_0203b14c((int*)(self + 0x16c), spin, 0x400, 0x2000, 0x80);
    *(int*)(self + 0x130) = *(int*)(self + 0x16c);
    _ZN9Animation7AdvanceEv(self + 0x124);
    _ZN12CylinderClsn5ClearEv(self + 0x138);
    _ZN12CylinderClsn6UpdateEv(self + 0x138);
    return 1;
}
