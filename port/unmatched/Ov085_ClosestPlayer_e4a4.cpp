/* HOST COPY of src/func_ov085_0212e4a4.c -- one of the LakituBro states.
 *
 * THE r0-PASSTHROUGH SEAM (the same class as Actor_ClosestPlayerWrappers.cpp):
 *
 * func_ov085_0212e4a4 is a LakituBro state body; it receives the actor's `this`
 * as its first parameter (`self`, the ROM's r0). It calls Actor::ClosestPlayer()
 * with NO argument -- byte-identical on ARM because ClosestPlayer is a nonstatic
 * member that reads `this` from r0, and the caller's own r0 (self) is still live
 * across the `bl`.
 *
 * The matched src spells the call `_ZN5Actor13ClosestPlayerEv()` (the extern is
 * declared `(void)`), so on the host the zero-argument call pushes nothing and
 * the ClosestPlayer bridge (hal/reverse_bridges.cpp, `(void *self)` cdecl) reads
 * stack garbage as `this` -> Vec3_Dist faults (the frame-0 castle-grounds crash:
 * Vec3_Dist <- Actor::ClosestPlayer <- func_ov085_0212e4a4 <- LakituBro::Behavior).
 *
 * THE FIX passes `self` -- exactly the value the ROM leaves in r0 -- to
 * ClosestPlayer. The byte-locked src is unchanged; its slice line (gate 18) is
 * commented out in favour of this copy.
 */

typedef struct V3 {
    int x, y, z;
} V3;

extern "C" {

void *_ZN5Actor13ClosestPlayerEv(void *self);   /* real one-arg (this) shape */
int _ZN6Player17SetNoControlStateEhih(int self, unsigned char a, int b, unsigned char c);
void func_ov085_0212e728(int c, int a, int b);

extern int data_0209caa0[];
extern int data_ov085_02130800[];

int func_ov085_0212e4a4(unsigned int self)
{
    int p = (int)(long)_ZN5Actor13ClosestPlayerEv((void *)(long)self);  /* <- this rode r0 */
    if (p != 0) {
        V3 v = *(V3 *)(p + 0x5c);
        if ((data_0209caa0[2] & 0x10000) != 0 &&
            v.z > -0x28000 &&
            _ZN6Player17SetNoControlStateEhih(p, 0x12, -1, 0) != 0) {
            *(int *)(p + 0x744) = *(int *)(self + 0x5c);
            *(int *)(p + 0x748) = *(int *)(self + 0x60);
            *(int *)(p + 0x74c) = *(int *)(self + 0x64);
            *(int *)(self + 0x5c) = v.x;
            *(int *)(self + 0x60) = v.y;
            *(int *)(self + 0x64) = v.z;
            *(int *)(((long long)((int)(self + 0x5c)))) -= 0x3e8000;
            *(int *)(self + 0x60) = *(int *)(p + 0x644) + 0x3e8000;
            *(int *)(self + 0x2a4) = *(int *)(self + 0x5c);
            *(int *)(self + 0x2a8) = *(int *)(self + 0x60);
            *(int *)(self + 0x2ac) = *(int *)(self + 0x64);
            *(signed char *)(self + 0x2dc) = 0;
            func_ov085_0212e728(self, (int)data_ov085_02130800, 0);
        }
    }
    return 1;
}

}  /* extern "C" */
