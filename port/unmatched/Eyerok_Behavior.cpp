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

// PORT_HOST_ABI: mwcc pointer-to-member dispatch on a deliberately incomplete class; MSVC's PMF representation does not reproduce the ROM's {function,delta} pair.
int _ZN6Eyerok8BehaviorEv(char *c)
{
    DecIfAbove0_Short((u16 *)(c + 0x4d0));
    DecIfAbove0_Short((u16 *)(c + 0x4d2));

    {
        /* the ROM's `(((C *)c)->*(st->fn))()`: the TICK pair at cell +8 */
        PortEyerokCell *st = *(PortEyerokCell **)(c + 0x48c);
        if (st->tick_fn != 0)
            ((PortEyerokFn)(size_t)st->tick_fn)(c);
    }

    if (*(u16 *)(c + 0x4d4) != 0) {
        if ((*(u16 *)(c + 0x4d4) & 1) == 0) {
            int rnd = RandomIntInternal(&data_0209e650);
            int off = (*(u16 *)(c + 0x4d4) >> 1) * 0xc;
            char *bx = c + 0x4dc;
            char *bz = c + 0x4e4;
            char *by = c + 0x4e0;
            int *px;
            int *pz;
            int *py;
            Vector3 vin;
            Vector3 vout;
            *(int *)(bx + off) = *(int *)(c + 0x5c);
            *(int *)(by + off) = *(int *)(c + 0x60);
            *(int *)(bz + off) = *(int *)(c + 0x64);
            px = (int *)(bx + off);
            py = (int *)(by + off);
            pz = (int *)(bz + off);
            vin.x = 0;
            vin.y = 0;
            vin.z = 0;
            vout.x = 0;
            vout.y = 0;
            vout.z = 0;
            if (*(void **)(c + 0x48c) != (void *)&data_ov066_0211b07c) {
                if (*(s16 *)(c + 0x8e) != 0) {
                    vin.z = (0x7e - (((rnd >> 8) & 0x3f) << 2)) << 12;
                    Matrix4x3_FromRotationY(data_020a0e68, (s16)(*(s16 *)(c + 0x8e) - 0x4000));
                    MulVec3Mat4x3(&vin, data_020a0e68, &vout);
                    *px += vout.x;
                    *pz += vout.z;
                } else {
                    if (((rnd >> 16) & 1) == 0)
                        *px += (((rnd >> 8) & 3) * 0x28) << 12;
                    else
                        *px -= (((rnd >> 8) & 3) * 0x28) << 12;
                    *pz += 0x19000;
                }
                *py += ((*(u16 *)(c + 0x4d4) * 0xa) + 0x23) << 12;
            } else {
                if (*(s16 *)(c + 0x8e) != 0) {
                    vin.z = (0x7e - (((rnd >> 8) & 0x3f) << 2)) << 12;
                    Matrix4x3_FromRotationY(data_020a0e68, (s16)(*(s16 *)(c + 0x8e) - 0x4000));
                    MulVec3Mat4x3(&vin, data_020a0e68, &vout);
                    *px += vout.x;
                    *pz += vout.z;
                } else {
                    int a = ((rnd >> 24) & 7) * 0x1e;
                    int b = ((rnd >> 16) & 7) * 0x1e;
                    *pz -= 0x64000;
                    *px += (0x69 - a) << 12;
                    *pz += (0x69 - b) << 12;
                }
                *py += 0x96000;
            }
        }

        {
            int i = 0;
            char *cur = c;
            u32 id0 = 0x13a;
            u32 id1 = 0x13b;
            for (; i < 0x14; i++) {
                if (*(int *)(cur + 0x4dc) != 0 || *(int *)(cur + 0x4e0) != 0 || *(int *)(cur + 0x4e4) != 0) {
                    *(u32 *)(c + 0x5cc + (i << 2)) =
                        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                            *(u32 *)(c + 0x5cc + (i << 2)), id0,
                            *(int *)(cur + 0x4dc), *(int *)(cur + 0x4e0), *(int *)(cur + 0x4e4),
                            (void *)0, (void *)0);
                    *(u32 *)(c + 0x61c + (i << 2)) =
                        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                            *(u32 *)(c + 0x61c + (i << 2)), id1,
                            *(int *)(cur + 0x4dc), *(int *)(cur + 0x4e0), *(int *)(cur + 0x4e4),
                            (void *)0, (void *)0);
                }
                cur += 0xc;
            }
        }

        {
            u16 *p = (u16 *)(c + 0x4d4);
            char *c400 = c + 0x400;
            *p = (u16)(*p + 1);
            if (*(u16 *)(c400 + 0xd4) > 0x26) {
                int j = 0;
                char *q = c;
                *(u16 *)(c400 + 0xd4) = 0;
                for (; j < 0x14; j++) {
                    *(int *)(q + 0x4dc) = 0;
                    *(int *)(q + 0x4e0) = 0;
                    *(int *)(q + 0x4e4) = 0;
                    q += 0xc;
                }
            }
        }
    }

    if (*(int *)(c + 0x49c) == 0) {
        func_ov066_021194fc(c);
        if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x674) != 0)
            func_ov066_021194a4(c);
        return 1;
    }

    {
        Vector3 vrel;
        *(int *)(c + 0x4a8) = *(int *)(c + 0x4b4) + 0x8000;
        _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
        *(int *)(c + 0x354) = *(int *)(c + 0x5c);
        *(int *)(c + 0x358) = *(int *)(c + 0x60);
        *(int *)(c + 0x35c) = *(int *)(c + 0x64);
        vrel.x = data_ov066_0211ad18[0];
        vrel.y = data_ov066_0211ad18[1];
        vrel.z = data_ov066_0211ad18[2];
        _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(c + 0x320, &vrel);
        func_ov066_021194fc(c);
        if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x674) != 0)
            func_ov066_021194a4(c);
        _ZN12CylinderClsn5ClearEv(c + 0x320);
        _ZN12CylinderClsn6UpdateEv(c + 0x320);
        _ZN14BlendModelAnim7AdvanceEv(c + 0x360);
        _ZN9Animation7AdvanceEv(c + 0x448);
    }
    return 1;
}

}  /* extern "C" */
