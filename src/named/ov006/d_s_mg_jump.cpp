//cpp
/* STILL MACHINE-SHAPED (audit 2026-09-18) -- byte-exact; what blocks each part:
 *  2 func_ov006_*                unnamed in config symbols.txt; each needs a
 *                                coined, behaviour-justified name.
 *  6 ctor/dtor/op-new call(s)    C1/C2/D0/D1/D2 is not expressible
 *                                in C++ source; only a real ctor emits it.
 *  2 _ZTV vptr store(s)          stands in for the ctor that would emit it.
 *  ~1 *(T *)(p + 0x..)           class layout does not name these offsets.
 */

struct dMgJump3DMario_c;

/* The array runtime passes each element address and ignores callback results.
 * The casts below adapt the existing lifecycle entries at that ABI boundary. */
extern "C" {
extern void *_ZN7fBase_cnwEj(unsigned int sz);
extern void *_ZN11dScMgBase_cC2Ev(void *p);
extern void _ZN8Particle10SysTrackerC1Ev(void *);
extern void *_ZN5ModelC1Ev(void *);
extern void __cxa_vec_ctor(void *base, unsigned int count, unsigned int stride,
                    void (*ctor)(void *), void (*dtor)(void *));
extern int _ZTV14dScMgD3DBase_c[];
extern int _ZTV11dScMgJump_c[];
extern dMgJump3DMario_c *_ZN16dMgJump3DMario_cD1Ev(dMgJump3DMario_c *object);
extern dMgJump3DMario_c *_ZN16dMgJump3DMario_cC1Ev(dMgJump3DMario_c *object);
extern int func_ov006_020c6f70(char *object);
extern int func_ov006_020c6f3c(int *object);
}

/* Reconstructed source-style name: SM64DS proves dScMgJump_c through RTTI,
 * allocation size, vtable identity, and the MG_JUMP registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: MgBounceAndPounce_Spawn. */
extern "C" void *dScMgJump_c_classInit(void)
{
    char *p = (char *)_ZN7fBase_cnwEj(0x5834);
    if (p) {
        char *it;
        _ZN11dScMgBase_cC2Ev(p);
        *(int *)p = (int)_ZTV14dScMgD3DBase_c;
        *(short *)(p + 0x4664) = 0;
        it = p + 0x466c;
        {
            char *end = p;
            end += 0x47e4;
            do {
                it += 0xbc;
            } while (it != end);
        }
        _ZN8Particle10SysTrackerC1Ev(p + 0x47e4);
        *(int *)p = (int)_ZTV11dScMgJump_c;
        _ZN5ModelC1Ev(p + 0x501c);
        __cxa_vec_ctor(p + 0x506c, 3, 0xb8,
                      (void (*)(void *))_ZN16dMgJump3DMario_cC1Ev,
                      (void (*)(void *))_ZN16dMgJump3DMario_cD1Ev);
        __cxa_vec_ctor(p + 0x5294, 6, 0xf0,
                      (void (*)(void *))func_ov006_020c6f70,
                      (void (*)(void *))func_ov006_020c6f3c);
    }
    return p;
}
