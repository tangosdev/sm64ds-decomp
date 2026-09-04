//cpp
/* Compiled as C++: as C this same body assembles to 0xac bytes, and the
 * cartridge's function is 0xd8. The language mode is the whole difference --
 * see tools/match.py --c on both spellings. C linkage keeps the ROM symbol. */
typedef unsigned int u32;
typedef int s32;
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
struct Vector3 { s32 x, y, z; };
struct dActor_c;
/* Both are ROM symbols spelled by their final names, so they need C linkage:
 * seen as C++ the compiler would mangle them a second time and the link would
 * fail on the doubly-mangled spelling. */
extern "C" struct dActor_c* _ZN8dActor_c4NextEPKS_(const struct dActor_c* a);
extern "C" int Vec3_Dist(const struct Vector3* a, const struct Vector3* b);

extern "C" void func_ov002_020f051c(char* c)
{
    struct dActor_c* a;
    u32 t;
    int b;
    if (*(u8*)(c+0x112) != 0) return;
    a = _ZN8dActor_c4NextEPKS_(0);
    if (a == 0) goto done;
    do {
        t = *(u16*)((char*)a + 0xc);
        b = (t == 0x14);
        if (b == 0) {
            b = (t == 0x15);
            if (b == 0) goto chk2;
        }
        if (Vec3_Dist((struct Vector3*)(c+0x5c), (struct Vector3*)((char*)a + 0x5c)) < 0xc8000) {
            *(u8*)(c+0x111) = 1;
            *(int*)((char*)a + 0x3f4) = (int)c;
            goto done;
        }
        goto next;
      chk2:
        b = (t == 0xc0);
        if (b == 0) goto next;
        if (Vec3_Dist((struct Vector3*)(c+0x5c), (struct Vector3*)((char*)a + 0x5c)) < 0xc8000) {
            *(u8*)(c+0x111) = 1;
            *(int*)((char*)a + 0x4f0) = (int)c;
            goto done;
        }
      next:
        a = _ZN8dActor_c4NextEPKS_(a);
    } while (a != 0);
  done:
    *(u8*)(c+0x112) = 1;
}
