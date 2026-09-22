/* HOST COPIES of the two ov020 book-closure TUs whose matched sources cannot
 * run on the host as written -- both correction classes already ruled on in
 * this lane (Boo_CrossNamedDispatch.c, Boo_Teleport.c):
 *
 * 1. func_ov020_02111418 (the book hit-response dispatcher): its source
 *    calls data_ov048_021115ac AS A FUNCTION -- ov048's name for 0x021115ac,
 *    the shared level-overlay window naming race. Inside ov020's own image
 *    the body at 0x021115ac is func_ov020_021115ac (matched, on the slice);
 *    the copy calls it by name. Mounting ov048 one day would otherwise hand
 *    this call host DATA to jump into.
 * 2. func_ov020_02111fc4 (the book aggro check): drops the ClosestPlayer
 *    receiver -- the exact TU the Actor_ClosestPlayer_OverlayReaders.cpp
 *    LATENT registry names for ov020 ("BEFORE hosting the overlay that
 *    carries one of these..."). The receiver rides the call here; nothing
 *    else changes (including the ROM's own unchecked dereference of the
 *    result -- single-player, the player always exists).
 */
#include "common.h"
typedef int Fix12i;
typedef short s16;

extern "C" {
int func_ov020_021115ac(char *c);      /* the 0x021115ac body, by its own name */
void func_ov020_02112110(char *c);
int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *self, void *v, unsigned a,
                                            int fix, unsigned b, unsigned d,
                                            unsigned e);
int _ZN6Player6BounceE5Fix12IiE(void *self, int fix);
char *_ZN8dActor_c13ClosestPlayerEv(void *self);
Fix12i Vec3_Dist(const Vector3 *a, const Vector3 *b);
s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
int _ZN8dActor_c14GetSubtractionEss(void *thiz, short a, short b);
void func_0201267c(int a, void *p);
}

/* PORT_HOST_ABI: shared-window naming race -- the matched TU spells the
   0x021115ac callee by ov048's data name; corrected by address here. */
extern "C" int func_ov020_02111418(char *c)
{
    int r = func_ov020_021115ac(c);
    if (r == 1) { func_ov020_02112110(c); return 1; }
    if (r == 2) {
        int v[3];
        v[0] = *(int *)(c + 0x5c); v[1] = *(int *)(c + 0x60); v[2] = *(int *)(c + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(*(void **)(c + 0x41c), v, 0, 0xc000, 1, 0, 1);
        func_ov020_02112110(c);
        return 1;
    }
    if (r == -1) {
        int eq = (*(unsigned short *)(c + 0xc) == 0x147);
        if (eq) {
            int v[3];
            v[0] = *(int *)(c + 0x5c); v[1] = *(int *)(c + 0x60); v[2] = *(int *)(c + 0x64);
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(*(void **)(c + 0x41c), v, 2, 0xc000, 1, 0, 1);
        } else {
            int v[3];
            v[0] = *(int *)(c + 0x5c); v[1] = *(int *)(c + 0x60); v[2] = *(int *)(c + 0x64);
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(*(void **)(c + 0x41c), v, 1, 0xc000, 1, 0, 1);
        }
        *(char *)(c + 0x108) = 0;
        func_ov020_02112110(c);
        return 1;
    }
    if (r != -2) return 0;
    _ZN6Player6BounceE5Fix12IiE(*(void **)(c + 0x41c), 0x28000);
    func_ov020_02112110(c);
    return 1;
}

/* PORT_HOST_ABI: r0-passthrough seam -- the matched TU drops the
   ClosestPlayer receiver; passed explicitly here (the registered ov020
   entry in the OverlayReaders latent list). */
extern "C" void func_ov020_02111fc4(char *thiz)
{
    char *c = thiz;
    Vector3 v;
    char *p = _ZN8dActor_c13ClosestPlayerEv(c);
    {
        int *s = (int *)(p + 0x5c);
        v.x = s[0];
        v.y = s[1];
        v.z = s[2];
    }
    if (Vec3_Dist((Vector3 *)(c + 0x5c), &v) >= 0x190000) return;
    {
        short ang = Vec3_HorzAngle((Vector3 *)(c + 0x5c), &v);
        if (_ZN8dActor_c14GetSubtractionEss(c, *(short *)(c + 0x94), ang) >= 0x3000) return;
    }
    *(int *)(c + 0x424) = 1;
    *(int *)(c + 0x98) = 0x5000;
    *(short *)(c + 0x100) = 0;
    {
        int *p234 = (int *)(c + 0x234);
        *p234 = *p234 & ~1;
    }
    func_0201267c(0x166, c + 0x74);
}
