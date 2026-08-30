/* HOST COPIES for TREASURE_CHEST (13, ov064, "11daObjTbox_c"), run rel0215
 * wave 3, lane w3-c -- the two pointer-to-member dispatchers, the ONE ROM body
 * the decomp does not have, and the seat of the six SOURCE records.
 *
 * ================= THE BODY THE DECOMP DOES NOT HAVE =====================
 *
 * hal/actor_classes_ov064_w12.cpp's header (run linkw wave 12) declined this
 * class and gave the measurement: "one of the six records, func_ov064_0211a4c4
 * (state 0's tick, 540 bytes at 0x0211a4c4), HAS NO MATCHED TU anywhere in src/
 * and is in no port host copy. State 0 is the state its InitResources leaves
 * the chest in, so that body runs on the first frame a chest exists.
 * Registering the class without it would seat a DS code address in a live
 * dispatch table." Both halves of that re-verified on this tip:
 *
 *   - config/arm9/overlays/ov064/symbols.txt names func_ov064_0211a4c4
 *     kind:function(arm,size=0x21c) addr:0x0211a4c4, and
 *     config/arm9/overlays/ov064/delinks.txt has NO block for it -- the TU
 *     before it, src/func_ov064_0211a49c.c, ends at 0x0211a4c4 and the next,
 *     src/func_ov064_0211a6e0.c, starts at 0x0211a6e0. Nothing in src/ or in
 *     port/ defines the symbol.
 *   - the ONLY reference to 0x0211a4c4 anywhere in ov064's relocs is the data
 *     word at 0x0211c4bc, one of the six { function, 0 } source pairs, and
 *     __sinit_ov064_0211b59c's own disassembly stores that pair at DEST+0x08 --
 *     Entry[0].tick. func_ov064_0211a6ec calls Entry[idx].pmf[0] (the enter)
 *     and func_ov064_0211a734 calls Entry[idx].pmf[1] (the tick), and the
 *     chest's idx (this+0x16c) is bss zero until something sets it. So it is
 *     state 0's tick and it is on the first Behavior frame of every chest.
 *
 * A LOUD FACE IS NOT AVAILABLE HERE and that is why this body is transcribed
 * rather than declined. The tree has a precedent for facing a missing state
 * body -- GOOMBOSS (198, ov074, func_ov074_021201f0) is seated with the class
 * registered and quarantines on its first Behavior frame, carried by a
 * tools/battery.py LEVEL_SKIPS row. This lane may not take that route: the
 * campaign plan's step 15 says a lane that needs a skip has not finished, and
 * this one would need a skip on THREE levels (8, 9 and 18, the levels whose
 * spawn lists carry id 13).
 *
 * TRANSCRIBED FROM THE ROM, INSTRUCTION BY INSTRUCTION, out of
 * extracted/overlays/overlay_0064.bin at base 0x02115ee0 (T4 -- ov064 is
 * compressed:true and the dsd export is the wrong image). 135 instructions,
 * 0x0211a4c4..0x0211a6e0. Every call it makes is resolved through
 * config/arm9/overlays/ov064/relocs.txt and every one of the nine is ALREADY
 * in build/port/walk_window.map before this lane -- the external closure gap
 * for this body is zero, which is checked rather than assumed:
 *     0x0203adbc DecIfAbove0_Short          0x02012790 func_02012790
 *     0x02010f3c Actor::FindWithID          0x0203b7ac Vec3_HorzAngle
 *     0x0203b0e8 AngleDiff                  0x02010ef0 Actor::FindWithActorID
 *     0x0200f97c Actor::SpawnSoundObj       0x02012694 func_02012694
 *     0x020d5a1c Player::Shock              0x0211a6ec (this file, below)
 *
 * WHAT IT IS. The Jolly Roger Bay chest puzzle. Four chests carry an order
 * number in this+0x172 (their spawn parameter's low byte); the player must open
 * them in that order. Each frame in state 0 the tick counts down the chest's
 * own timer, plays sound 14 when it reads 0x58, and once the timer expires and
 * the chest has a linked actor id in +0x15c, finds that actor, requires it to
 * be a PLAYER (id 0xbf), and requires the player to be within a quarter turn of
 * the chest's facing. Then it walks every id-13 actor: r8 counts them all and
 * sb counts the OTHER chests already in state 1 or 2. If (sb + 1) equals this
 * chest's own order number the open is correct -- if every chest is now open it
 * spawns the sound object and sets +0x173, otherwise it plays sound 0x26 -- and
 * the chest enters state 1. If it does NOT match, the chest re-arms its timer
 * to 0x5a, shocks the player, and puts every OTHER id-13 chest back to state 0.
 *
 * ================= THE TWO DISPATCHERS ====================================
 *
 * func_ov064_0211a6ec and func_ov064_0211a734 each form an mwcc
 * pointer-to-member over a FORWARD-DECLARED struct, so MSVC hands them the
 * general worst-case-inheritance representation where the ROM has a plain
 * { function, 0 } pair; dispatching that mangles `this`. The KnockDownPlank /
 * MontyMole / Scuttlebug case, the same shape as gate 179's two Piece
 * dispatchers. Both also index the table, which MSVC strides at its own
 * sizeof(PMF) rather than the ROM's 8 -- the second half of the same fault.
 * Read here as a plain { fn, 0 } array of three sixteen-byte Entries.
 *
 * TreasureChest::Behavior and its Render are NOT here: Behavior is a plain
 * three-call body that reaches the state machine only through
 * func_ov064_0211a734 (so it stays in the slice), and Render is the ModelAnim
 * slot-5 shadow, host-copied in unmatched/Ov064_Clam.cpp where that file's own
 * header says it belongs.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

struct PortPmf { unsigned fn; int delta; };
struct PortEntry { PortPmf pmf[2]; };      /* 16 bytes, the ROM's own stride */
struct PortVec3 { int x, y, z; };

/* the shared ring, every one already linked before this lane */
unsigned short DecIfAbove0_Short(unsigned short *p);
void func_02012790(int id);
void func_02012694(int id, void *pos);
void *_ZN5Actor10FindWithIDEj(unsigned id);
void *_ZN5Actor15FindWithActorIDEjPS_(unsigned id, void *prev);
void _ZN5Actor13SpawnSoundObjEj(void *self, unsigned a);
short Vec3_HorzAngle(const void *v0, const void *v1);
int AngleDiff(int a, int b);
void _ZN6Player5ShockEj(void *self, unsigned a);

/* the five state bodies that DO have matched TUs (slice_w3c.txt) */
int func_ov064_0211a2c4(void *c);   /* Entry[2].tick  */
int func_ov064_0211a380(void *c);   /* Entry[2].enter */
int func_ov064_0211a39c(void *c);   /* Entry[1].tick  */
int func_ov064_0211a49c(void *c);   /* Entry[1].enter */
int func_ov064_0211a6e0(void *c);   /* Entry[0].enter */

/* the six SOURCE records the sinit copies from (data, { fn, 0 }) */
extern PortPmf data_ov064_0211c49c[];   /* {0211a2c4, 0} -> Entry[2].tick  */
extern PortPmf data_ov064_0211c4a4[];   /* {0211a39c, 0} -> Entry[1].tick  */
extern PortPmf data_ov064_0211c4ac[];   /* {0211a380, 0} -> Entry[2].enter */
extern PortPmf data_ov064_0211c4b4[];   /* {0211a49c, 0} -> Entry[1].enter */
extern PortPmf data_ov064_0211c4bc[];   /* {0211a4c4, 0} -> Entry[0].tick  */
extern PortPmf data_ov064_0211c4c4[];   /* {0211a6e0, 0} -> Entry[0].enter */

/* the RUNTIME table, three 16-byte Entries, filled by __sinit_ov064_0211b59c */
extern PortEntry data_ov064_0211c98c[3];

int func_ov064_0211a4c4(void *self);

}  /* extern "C" */

/* PORT_HOST_ABI: mwcc pointer-to-member on a forward-declared struct, indexed.
   The matched func_ov064_0211a6ec writes the new index to this+0x16c, reads it
   back and calls Entry[idx].pmf[0] -- the state's ENTER half. */
extern "C" void func_ov064_0211a6ec(void *self, int i)
{
    char *c = (char *)self;
    *(int *)(c + 0x16c) = i;
    int j = *(int *)(c + 0x16c);
    unsigned fn = data_ov064_0211c98c[j].pmf[0].fn;
    if (fn)
        ((int (*)(void *))(size_t)fn)(c);
}

/* PORT_HOST_ABI: the same, reading Entry[idx].pmf[1] -- the state's TICK half.
   TreasureChest::Behavior calls this every frame. */
extern "C" void func_ov064_0211a734(void *self)
{
    char *c = (char *)self;
    int j = *(int *)(c + 0x16c);
    unsigned fn = data_ov064_0211c98c[j].pmf[1].fn;
    if (fn)
        ((int (*)(void *))(size_t)fn)(c);
}

/* PORT_HOST_ABI: the ROM body at ov064 0x0211a4c4 (0x21c bytes), state 0's
   tick. No matched TU exists anywhere in the tree; this is transcribed from the
   overlay image. See this file's header for the derivation and for why a loud
   face is not an option for this particular slot. */
extern "C" int func_ov064_0211a4c4(void *self)
{
    char *c = (char *)self;

    DecIfAbove0_Short((unsigned short *)(c + 0x170));
    if (*(unsigned short *)(c + 0x170) == 0x58)
        func_02012790(0xe);

    /* not linked to anything yet, or the arming timer is still running */
    if (*(unsigned int *)(c + 0x15c) == 0)
        return 0;
    if (*(unsigned short *)(c + 0x170) != 0)
        return 0;

    char *other = (char *)_ZN5Actor10FindWithIDEj(*(unsigned int *)(c + 0x15c));
    if (other == 0)
        return 0;
    if (*(unsigned short *)(other + 0xc) != 0xbf)   /* must be a PLAYER */
        return 0;

    {
        PortVec3 pv;
        pv.x = *(int *)(other + 0x5c);
        pv.y = *(int *)(other + 0x60);
        pv.z = *(int *)(other + 0x64);
        if (AngleDiff(Vec3_HorzAngle(c + 0x5c, &pv),
                      *(short *)(c + 0x8e)) >= 0x4000)
            return 0;
    }

    /* walk every id-13 actor: total, and how many OTHERS are open (state 1|2) */
    int opened = 0;
    int total = 0;
    char *it = (char *)_ZN5Actor15FindWithActorIDEjPS_(0xd, 0);
    while (it != 0) {
        ++total;
        if (it != c) {
            int st = *(int *)(it + 0x16c);
            if (st == 1 || st == 2)
                ++opened;
        }
        it = (char *)_ZN5Actor15FindWithActorIDEjPS_(0xd, it);
    }

    unsigned order = *(unsigned char *)(c + 0x172);
    if ((unsigned)(opened + 1) == order) {
        if ((unsigned)total == order) {
            _ZN5Actor13SpawnSoundObjEj(c, 0);
            *(unsigned char *)(c + 0x173) = 1;
        } else {
            func_02012790(0x26);
        }
        if (*(unsigned char *)(other + 0x706) != 0)
            func_02012694(0x22, c + 0x74);
        else
            func_02012694(0x20, c + 0x74);
        func_ov064_0211a6ec(c, 1);
        return 0;
    }

    /* wrong chest: re-arm, shock the player, reset every OTHER chest */
    *(unsigned short *)(c + 0x170) = 0x5a;
    if (*(unsigned char *)(other + 0x6f9) != 0)
        _ZN6Player5ShockEj(other, 0);
    else
        _ZN6Player5ShockEj(other, 1);
    {
        char *r = 0;
        while ((r = (char *)_ZN5Actor15FindWithActorIDEjPS_(0xd, r)) != 0) {
            if (r != c)
                func_ov064_0211a6ec(r, 0);
        }
    }
    return 0;
}

/* ---- the seat ------------------------------------------------------------ */

static const struct { PortPmf *slot; unsigned rom; int (*host)(void *); }
g_treasure_chest_states[] = {
    {data_ov064_0211c4c4, 0x0211a6e0, func_ov064_0211a6e0},  /* Entry[0].enter */
    {data_ov064_0211c4bc, 0x0211a4c4, func_ov064_0211a4c4},  /* Entry[0].tick  */
    {data_ov064_0211c4b4, 0x0211a49c, func_ov064_0211a49c},  /* Entry[1].enter */
    {data_ov064_0211c4a4, 0x0211a39c, func_ov064_0211a39c},  /* Entry[1].tick  */
    {data_ov064_0211c4ac, 0x0211a380, func_ov064_0211a380},  /* Entry[2].enter */
    {data_ov064_0211c49c, 0x0211a2c4, func_ov064_0211a2c4},  /* Entry[2].tick  */
};

extern "C" void port_treasure_chest_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_treasure_chest_states /
                             sizeof g_treasure_chest_states[0]; ++i) {
        PortPmf *p = g_treasure_chest_states[i].slot;
        if (p->fn != g_treasure_chest_states[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: TreasureChest source %u: the mount "
                         "holds %08x/%d, the ROM's own table says %08x/0 -- "
                         "WRONG BYTES\n", i, p->fn, p->delta,
                         g_treasure_chest_states[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_treasure_chest_states[i].host;
    }
}
