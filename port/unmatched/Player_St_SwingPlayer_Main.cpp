// PORT_HOST_ABI. Player::St_SwingPlayer_Main, transcribed from the ROM.
// ov002 0x020d9fec, size 0x3c4. Run rel0215: the Wario carry-and-spin freeze.
//
// ---- WHY A HOST COPY, AND WHY THIS ONE IS OWED --------------------------
// St_SwingPlayer_Main is a MATCHING FLOOR: it has NO matched src TU anywhere
// in the tree, so stategen never emitted a case for it and hal_call_state_fn
// no-ops it (`unhosted state fn 0x020d9fec`). Its two siblings ARE hosted --
// St_SwingPlayer_Init (0x020da3b0) and St_SwingPlayer_Cleanup (0x020d9fc4) are
// matched Player methods, seated in hal/player_bridges.cpp's run-linkw block --
// so the state is ENTERED and Init runs, but Main never advances it and
// Cleanup never runs.
//
// That is exactly Tango's "both stuck and sliding" on a Wario carry-and-spin.
// The entry gate is src/func_ov002_020d9dcc.c: when the ACTIVE CHARACTER INDEX
// is 2 (data_020a0e40 selects the versus slot; index 2 == Wario, cap OR plain)
// and the held partner's tag is 0xbf, it ChangeState's the carrier into
// SwingPlayer (data_ov002_02110604). St_SwingPlayer_Init sets the victim's PIN
// bit -- *(u32 *)(mHeldObj + 0xb0) |= 0x800 -- which ONLY St_SwingPlayer_Cleanup
// clears. With Main dead, mStateStep never leaves 0, so the state never reaches
// the throw phase, never ChangeState's out, so Cleanup never runs: the victim
// stays pinned (stuck) and the carrier keeps an mAngleYSpeed nothing here ever
// consumes to zero (sliding). Climb (port/unmatched/Player_St_Climb.cpp) is the
// worked precedent for what a silently no-op'd state body costs and for the
// remedy -- a faithful host transcription -- which is what this is.
//
// It is a matching floor rather than an oversight: a regalloc two-web rank tie,
// ~185 permuter variants over ~7000 iterations, none byte-exact. Per the Climb
// precedent a matching-floor state gets a ROM-accurate host transcription, not
// a match attempt and not a stand-in.
//
// ---- THE ROM BODY, read out of extracted/overlays/overlay_0002.bin at base
// 0x020ad660 (the .text start in config/arm9/overlays/ov002/delinks.txt), never
// dsd's shifted copy. It is an mStateStep (Player+0x6e3) step machine:
//
//   020d9fec  push {r4,lr}; sub sp,#0x28; mov r4,r0
//   020d9ff8  ldrb r1,[r4,#0x6e3]            mStateStep
//   020d9ffc  cmp r1,#3; addls pc,pc,r1,lsl#2 ; b default   jump table 0..3
//             0 -> 0x020da018   windup
//             1 -> 0x020da064   spin
//             2 -> 0x020da344   throw  (wait anim, ChangeState)
//             3 -> 0x020da360   throw2 (wait anim, ChangeState)
//             default -> 0x020da378 (tail: Player_AdvanceAnims; return 1)
//
// Every call target is confirmed against config/arm9/overlays/ov002/relocs.txt
// and .../symbols.txt (and config/arm9/symbols.txt for the arm9 ones):
//   0x020bed98 _ZN6Player12FinishedAnimEv
//   0x020bef2c _ZN6Player7SetAnimEji5Fix12IiEj
//   0x020d9c70 func_ov002_020d9c70   (release: detach the held partner)
//   0x020da95c func_ov002_020da95c
//   0x020da9d4 func_ov002_020da9d4
//   0x0203aed8 _Z15ApproachLinear2Rsss
//   0x02022d80 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE
//   0x02012328 _ZN5Sound8PlayLongEjjjRK7Vector3j
//   0x0201264c _ZN5Sound9PlayBank0EjRK7Vector3
//   0x020dc174 func_ov002_020dc174
//   0x02015024 _ZN12CylinderClsn5ClearEv
//   0x02014ff0 _ZN12CylinderClsn6UpdateEv
//   0x020e30a0 _ZN6Player11ChangeStateERNS_5StateE
//   0x020bedd4 Player_AdvanceAnims
// and the data words in the literal pool at 0x020da390..0x020da3ac:
//   data_020a0e40 (u8 versus-slot index), data_0209f49e / data_0209f4a0 (the
//   per-slot versus table, stride 0x18), 0x69c / 0xf99 / 0x132 / 0x133 (the
//   spin field offset, the whoosh threshold and the two particle ids), and
//   data_ov002_0211013c (the State the throw exits to). All are already mounted
//   or hosted in the port (ov002_syms.txt / actor_vtables.cpp / the gate-10,
//   gate-29 slices), so this transcription adds no new externals.
//
// Field offsets are include/Player.h and include/Actor.h:
//   +0x08e mAngleY        +0x314 mAttackClsn (a CylinderClsn)
//   +0x358 mHeldObj       +0x620 mLoopingSoundHandle   +0x628 unk_628 (particle)
//   +0x69c mAngleYSpeed   +0x6a4 mStateTimer            +0x6d2 mDesiredAngleY
//   +0x6d4 unk_6d4        +0x6e3 mStateStep
// The ROM forms +0x69c etc. as `add base,#0x600; [base,#0x9c]` only because the
// halfword load offset is 8-bit; the effective address is +0x69c. strh stores
// wrap in 16 bits, written here as explicit (short) truncations.

extern "C" {

int  _ZN6Player12FinishedAnimEv(void *self);
void _ZN6Player7SetAnimEji5Fix12IiEj(void *self, unsigned int id, int a, int b,
                                     unsigned int e);
void func_ov002_020d9c70(char *o);
int  func_ov002_020da95c(char *c);
int  func_ov002_020da9d4(char *self);
int  _Z15ApproachLinear2Rsss(short *ref, short target, short step);
unsigned int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int a, unsigned int b, int x, int y, int z, const void *pos,
    void *cb);
unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b,
                                               unsigned int c, const void *pos,
                                               unsigned int e);
void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int id, const void *pos);
void func_ov002_020dc174(char *c, void *r1, int r2, int r3, unsigned int a5,
                         unsigned int a6);
void _ZN12CylinderClsn5ClearEv(void *self);
void _ZN12CylinderClsn6UpdateEv(void *self);
void _ZN6Player11ChangeStateERNS_5StateE(void *self, void *state);
void Player_AdvanceAnims(char *c);

extern unsigned char data_020a0e40;      /* versus-slot index (u8) */
extern unsigned char data_0209f49e[];    /* versus table, u16 field at +0, stride 0x18 */
extern unsigned char data_0209f4a0[];    /* versus table, s16 field at +2, stride 0x18 */
extern unsigned char data_ov002_0211013c[];  /* the State the throw exits to */

/* The tail every non-early path takes: advance the anims and return 1. */
static int swing_tail(char *c)
{
    Player_AdvanceAnims(c);
    return 1;
}

// PORT_HOST_ABI: matching-floor state with no matched src; faithful ROM transcription per the Climb precedent.
int port_player_st_swingplayer_main(void *self)
{
    char *c = (char *)self;

    switch (*(unsigned char *)(c + 0x6e3)) {     /* mStateStep */
    case 0: {   /* WINDUP: hold until the grab anim finishes, then spin (0x6d) */
        if (_ZN6Player12FinishedAnimEv(c)) {
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x6d, 0x40000000, 0x1000, 0);
            *(unsigned char *)(c + 0x6e3) = 1;
        }
        /* mAngleY += mAngleYSpeed */
        *(short *)(c + 0x8e) =
            (short)(*(short *)(c + 0x8e) + *(short *)(c + 0x69c));
        return swing_tail(c);
    }

    case 1: {   /* SPIN */
        unsigned idx = data_020a0e40;
        unsigned short vs = *(unsigned short *)(&data_0209f49e[idx * 0x18]);
        if (vs & 1) {
            /* versus "release now": let the held partner go */
            char *obj = *(char **)(c + 0x358);      /* mHeldObj */
            if (obj == 0)
                return 1;                            /* early: no tail */
            if (*(unsigned int *)(obj + 0xb0) & 0x200) {
                func_ov002_020da9d4(c);
            } else {
                func_ov002_020d9c70(c);
                func_ov002_020da95c(c);
                _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x6e, 0x40000000, 0x1000, 0);
                *(unsigned char *)(c + 0x6e3) = 3;   /* -> throw2 */
            }
            return 1;                                /* early: no tail */
        }

        /* ongoing spin: steer mAngleYSpeed toward the target, then decay it */
        short w = *(short *)(&data_0209f4a0[idx * 0x18]);
        if (w != 0) {
            short diff = (short)(*(short *)(c + 0x6d2) - *(short *)(c + 0x6d4));
            short spd  = *(short *)(c + 0x69c);
            short absspd = spd < 0 ? (short)(-spd) : spd;
            short delta = (absspd < 0x800) ? (short)(diff << 10)
                                           : (short)(diff << 9);
            if (delta != 0) {
                short v = (short)(*(short *)(c + 0x69c) + delta);
                *(short *)(c + 0x69c) = v;
                if (v >= 0x1800)        *(short *)(c + 0x69c) = 0x1800;
                else if (v <= -0x1800)  *(short *)(c + 0x69c) = -0x1800;
            } else {
                _Z15ApproachLinear2Rsss((short *)(c + 0x69c), 0, 0x20);
            }
        } else {
            _Z15ApproachLinear2Rsss((short *)(c + 0x69c), 0, 0x80);
        }

        /* whoosh particle + looping sound while the spin is fast */
        {
            short spd = *(short *)(c + 0x69c);
            short absspd = spd < 0 ? (short)(-spd) : spd;
            if (absspd >= 0xf99) {
                int px = *(int *)(c + 0x5c);
                int py = *(int *)(c + 0x60) + 0x3c000;
                int pz = *(int *)(c + 0x64);
                unsigned pid = (*(short *)(c + 0x69c) > 0) ? 0x133u : 0x132u;
                *(unsigned int *)(c + 0x628) =
                    _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                        *(unsigned int *)(c + 0x628), pid, px, py, pz, 0, 0);
                *(unsigned int *)(c + 0x620) =
                    _ZN5Sound8PlayLongEjjjRK7Vector3j(
                        *(unsigned int *)(c + 0x620), 0, 0x1c,
                        (const void *)(c + 0x74), 0);
            }
        }

        /* while still turning, hold the throw timer; when it runs out, throw */
        if (*(short *)(c + 0x69c) != 0)
            *(unsigned short *)(c + 0x6a4) = 0xa;     /* mStateTimer */
        if (*(unsigned short *)(c + 0x6a4) == 0) {
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x6f, 0x40000000, 0x1000, 0);
            *(unsigned char *)(c + 0x6e3) = 2;        /* -> throw */
        }

        /* rotate, and a bank whoosh each time the heading wraps a revolution */
        {
            short old = *(short *)(c + 0x8e);
            short spd = *(short *)(c + 0x69c);
            short neu = (short)(old + spd);
            *(short *)(c + 0x8e) = neu;
            if ((spd <= -0x100 && old <  neu) ||
                (spd >=  0x100 && old >  neu))
                _ZN5Sound9PlayBank0EjRK7Vector3(0x1b, (const void *)(c + 0x74));
        }

        /* the swept attack volume this spin carries, and its collision refresh */
        {
            int vec[3] = {0, 0x32000, 0x64000};
            func_ov002_020dc174(c, vec, 0x32000, 0x32000, 0x80, 0);
        }
        _ZN12CylinderClsn5ClearEv((void *)(c + 0x314));
        _ZN12CylinderClsn6UpdateEv((void *)(c + 0x314));
        return swing_tail(c);
    }

    case 2:     /* THROW: when the release anim finishes, leave the state */
        if (_ZN6Player12FinishedAnimEv(c))
            _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211013c);
        return swing_tail(c);

    case 3:     /* THROW2: same exit on the alternate release anim */
        if (_ZN6Player12FinishedAnimEv(c))
            _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211013c);
        return swing_tail(c);

    default:
        return swing_tail(c);
    }
}

}  /* extern "C" */
