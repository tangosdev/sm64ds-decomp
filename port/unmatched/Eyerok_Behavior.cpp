/* HOST COPY of src/_ZN6Eyerok8BehaviorEv.cpp -- EYEROK's (176, ov066) vtable
 * slot 6, and the TICK half of its nineteen-cell pointer-to-member state
 * machine. Run rel0215, lane cast-ov066.
 *
 * WHY A HOST COPY. Unlike the Ukiki and BabyPenguin families, Eyerok does not
 * put its tick dispatch in a separate one-line body -- it is INLINE inside
 * Behavior, in the matched source's own words:
 *
 *     struct C; typedef int (C::*PMF)();
 *     struct State { char pad[8]; PMF fn; };
 *     State *st = *(State **)(c + 0x48c);
 *     if (*(int *)((char *)st + 8) != 0)
 *         (((C *)c)->*(st->fn))();
 *
 * a real mwcc pointer-to-member on a deliberately INCOMPLETE class, so the
 * whole 0x4b0-byte body has to come across rather than a wrapper. The reading
 * is port/unmatched/Eyerok_StateEnter.cpp's: mwcc's PMF is the ROM's 8-byte
 * {function, delta} pair, MSVC's for an incomplete class is the "unknown
 * inheritance" form, and the two do not agree.
 *
 * `struct State { char pad[8]; PMF fn; }` says the cell layout out loud: the
 * ENTER pair is at +0 (what func_ov066_02119454 calls) and the TICK pair at
 * +8 (what this reads), 16 bytes a cell -- exactly the `S16 { S8 x, y; }`
 * unit src/__sinit_ov066_0211a418.c copies the thirty-eight .data source pairs
 * into, and exactly the 0x10 stride dsd infers for the nineteen .bss records
 * at 0x0211afcc..0x0211b0ec. Four independent reads of the same number.
 * The `!= 0` guard is on the pair's FUNCTION word, which is what mwcc compares.
 *
 * EVERYTHING ELSE IS THE MATCHED SOURCE STATEMENT FOR STATEMENT, including the
 * two `#pragma opt_*` lines' consequences (they are codegen directives for
 * mwcc and carry no host meaning, so they are dropped rather than translated),
 * the twenty-slot dust-particle ring at +0x4dc, the two Particle::System::New
 * ids 0x13a/0x13b, and the +0x400+0xd4 > 0x26 ring reset. The one deliberate
 * simplification is the `(long long)(int)` and `volatile` casts the recovered
 * source uses to pin mwcc's register choices; they change no value.
 *
 * The matched src stays in src/ as the byte proof and is dropped from
 * port/slice_ov066.txt.
 *
 * PORT_HOST_ABI: mwcc pointer-to-member dispatch on a deliberately incomplete
 * class; MSVC's PMF representation there does not reproduce the ROM's
 * {function,delta} pair.
 */

typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;

struct Vector3 { int x, y, z; };

extern "C" {

struct PortEyerokCell { unsigned enter_fn, enter_delta, tick_fn, tick_delta; };
typedef int (*PortEyerokFn)(void *);

u16 DecIfAbove0_Short(u16 *p);
int RandomIntInternal(int *seed);
void Matrix4x3_FromRotationY(void *m, int angle);
void MulVec3Mat4x3(void *src, void *m, void *dst);
u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 a, u32 b, int x, int y, int z, const void *v, void *cb);
void func_ov066_021194fc(char *c);
int _ZN16MeshColliderBase9IsEnabledEv(char *c);
void func_ov066_021194a4(char *c);
void _ZN5Actor9UpdatePosEP12CylinderClsn(char *c, void *clsn);
void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(char *c, const Vector3 *v);
void _ZN12CylinderClsn5ClearEv(char *c);
void _ZN12CylinderClsn6UpdateEv(char *c);
void _ZN14BlendModelAnim7AdvanceEv(char *c);
void _ZN9Animation7AdvanceEv(char *c);

extern int data_0209e650;
extern char data_ov066_0211b07c;
extern char data_020a0e68[];
extern int data_ov066_0211ad18[];

/* HOST COPY RETIRED, run link100 lane PMFB7 gate 2. src/_ZN6Eyerok8BehaviorEv.cpp
   dispatches its own field now: with /vmg /vmm (block R8) MSVC's pointer to
   member IS the ROM's eight-byte {code, adjust} pair, so the widening this
   banner was written for does not happen. The per-frame half of every state
   cell holds a zero-argument __fastcall face; the enter half does not change,
   because the helper that dispatches it tail-jumps. Measurements in
   port/slice_pmfb7.txt and runs/link100/out/PMFB7/. */

}  /* extern "C" */
