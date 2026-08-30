/* HOST COPIES of three ov096 POKEY (actors 240 and 241, daSanbo_c) sites MSVC
 * cannot deliver from the matched source, run rel0215 wave 2 lane
 * cast-lvl16pair. Each matched TU stays byte-locked in src/ and is simply not
 * referenced from port/slice_ov096.txt, which carries every refusal in full.
 *
 * (1) src/_ZN5PokeyD1Ev.cpp -- THE C++-MANGLING WALL. The matched TU is a real
 *     C++ class definition ending `virtual ~Pokey();` with an empty body.
 *     mwccarm mangles that Itanium and emits _ZN5PokeyD1Ev, the name vtable
 *     slot 16 needs; MSVC emits ??1Pokey@@UAE@XZ and nothing by the flat name,
 *     plus its own COMDAT vtable which the destructor would store at this[0]
 *     over the ROM-shaped table the registry fills, plus member-destructor
 *     calls under MSVC mangling this port does not define. The ov077
 *     Lakitu/Spiny case.
 *
 *     THE REPLACEMENT BODY IS RELOCATION-CONFIRMED, not reasoned from D0. The
 *     ROM says PokeyD1 makes exactly PokeyD0 calls in PokeyD0 order, minus the
 *     final Memory::Deallocate and its data_020a0eac argument
 *     (config/arm9/overlays/ov096/relocs.txt):
 *
 *       PokeyD1 0x02135700..0x02135748     PokeyD0 0x02135748..0x021357a4
 *         0x02135714 -> 0x020373f8           0x0213575c -> 0x020373f8
 *         0x0213571c -> 0x020149a4           0x02135764 -> 0x020149a4
 *         0x02135724 -> 0x02015ff8           0x0213576c -> 0x02015ff8
 *         0x0213572c -> 0x02016d20           0x02135774 -> 0x02016d20
 *         0x02135734 -> 0x020112c8           0x0213577c -> 0x020112c8
 *         0x02135744 (load) 0x021379d8       0x0213578c -> 0x0203c1e8  <- only
 *                                            0x0213579c (load) 0x021379d8
 *                                            0x021357a0 (load) 0x020a0eac
 *
 *     Resolved against src/_ZN5PokeyD0Ev.c own member list, which names the
 *     four members and their offsets: WithMeshClsn +0x180 (0x020373f8),
 *     MovingCylinderClsn +0x14c (0x020149a4), ShadowModel +0x124 (0x02015ff8),
 *     Model +0xd4 (0x02016d20), then _ZN5ActorD2Ev (0x020112c8). Three of
 *     those five arm9 addresses appear again, identically, in TORNADO
 *     destructor relocations for the same three member types -- two
 *     independently recovered destructors agreeing is the cross-check.
 *
 *     WHICH TABLE IT STORES. The load at 0x02135744 reads 0x021379d8, the one
 *     Pokey table. src spells it two ways -- both factories say _ZTV5Pokey and
 *     D0 says _ZTV9daSanbo_c -- and port/ov096_syms.txt proves they are one
 *     table by reading its own vtable[-1] typeinfo through to the string
 *     "9daSanbo_c". This copy stores the host array _ZTV5Pokey; the
 *     /alternatename bridge in hal/actor_classes_ov096.cpp carries the other
 *     spelling to the same storage.
 *
 * (2) and (3) src/func_ov096_021368f0.cpp and src/func_ov096_021368b4.cpp --
 *     THE PMF DISEASE, the ENTER and TICK halves of one two-record cell:
 *         struct C; typedef void (C::*PMF)();
 *         struct C { char pad[0x384]; PMF *pp; };
 *         021368f0:  PMF *p = c->pp;     (c->**p)();
 *         021368b4:  PMF *p = c->pp + 1; (c->**p)();
 *     MSVC unknown-inheritance general form is 16 bytes against the ROM 8, so
 *     both the call representation AND the `+ 1` stride are wrong -- the tick
 *     dispatch would land in the middle of the enter record.
 *
 *     WHERE THE CELL POINTER COMES FROM, and it is NOT refused:
 *     src/func_ov096_02136928.c stays in the slice and computes it with plain
 *     integer arithmetic, `*(int *)(c + 0x384) = (int)&data_ov096_02137b48 +
 *     (a << 4)`. The `a << 4` is the proof that the cells are PAIRS of 8-byte
 *     records: six states, twelve records.
 *
 * THE STATE MAP, transcribed from src/__sinit_ov096_0213770c.c and
 * cross-checked against the ROM relocations of the twelve source records
 * (config/arm9/overlays/ov096/relocs.txt, from:0x02137920 .. from:0x02137978):
 *
 *   state  cell[0] ENTER (source record)     cell[1] TICK (source record)
 *     0    0x021368a4 (data..._02137948)     0x02136754 (data..._02137940)
 *     1    0x0213670c (data..._02137950)     0x021365d4 (data..._02137928)
 *     2    0x02136534 (data..._02137930)     0x02136434 (data..._02137920)
 *     3    0x0213640c (data..._02137978)     0x021363c4 (data..._02137970)
 *     4    0x021363b4 (data..._02137938)     0x02136264 (data..._02137968)
 *     5    0x02136134 (data..._02137960)     0x021360c4 (data..._02137958)
 *
 * All twelve handler addresses fall inside POKEY own .text range
 * (0x021360c4..0x021368a4), which is the independent confirmation that this
 * machine is Pokey alone -- TORNADO has none, and its vtable slot 12 is the
 * arm9 ActorBase default rather than an own OnPendingDestroy.
 *
 * THE SEAT: port_ov096_states_seat() in hal/actor_classes_ov096.cpp rewrites
 * each mounted source record fn word with its HOST body address, VALIDATED
 * against the ROM own address first so a wrong mount aborts loudly instead of
 * seating garbage, and it runs BEFORE __sinit_ov096_0213770c copies the twelve
 * records into data_ov096_02137b48. Both dispatchers below therefore read a
 * HOST function pointer out of the cell and call it directly. The MrBlizzard/
 * BabyPenguin/Unagi/HootTheOwl/ov077/ov092 order.
 *
 * THE RETURN-TYPE NOTE. The ROM own typedef is `void (C::*PMF)()`, and the
 * twelve handlers do not agree with each other: ten return int and
 * func_ov096_0213670c returns void. On the DS that is r0, which this dispatch
 * discards. The host cell is typed void-returning to match the ROM typedef and
 * the ten int-returning bodies are reached through it on plain cdecl, where an
 * ignored eax is exactly the same discard. The same call this lane made for
 * ov092 and for the same reason.
 */
extern "C" {

struct PortOv096Pmf { unsigned int fn; int delta; };
typedef void (*PortOv096StateFn)(void *);

/* ---- (1) the destructor ------------------------------------------------ */
extern int _ZTV5Pokey[];
void _ZN12WithMeshClsnD1Ev(void *);          /* 0x020373f8, member +0x180 */
void _ZN18MovingCylinderClsnD1Ev(void *);    /* 0x020149a4, member +0x14c */
void _ZN11ShadowModelD1Ev(void *);           /* 0x02015ff8, member +0x124 */
void _ZN5ModelD1Ev(void *);                  /* 0x02016d20, member +0xd4  */
void _ZN5ActorD2Ev(void *);                  /* 0x020112c8               */

/* PORT_HOST_ABI: mwcc Itanium D1 that MSVC emits no flat name for. */
int *_ZN5PokeyD1Ev(int *t)
{
    t[0] = (int)(size_t)_ZTV5Pokey;
    _ZN12WithMeshClsnD1Ev((char *)t + 0x180);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x14c);
    _ZN11ShadowModelD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}

/* ---- (2) the ENTER dispatch -------------------------------------------- */
/* PORT_HOST_ABI: mwcc pointer-to-member through an incomplete class. */
void func_ov096_021368f0(void *cv)
{
    char *c = (char *)cv;
    PortOv096Pmf *p = *(PortOv096Pmf **)(c + 0x384);
    ((PortOv096StateFn)(size_t)p[0].fn)(cv);
}

/* ---- (3) the TICK dispatch --------------------------------------------- */
/* PORT_HOST_ABI: the same, plus the ROM 8-byte record stride MSVC widens to
   16. `c->pp + 1` in the matched source is record[1], the tick half. */
void func_ov096_021368b4(void *cv)
{
    char *c = (char *)cv;
    PortOv096Pmf *p = *(PortOv096Pmf **)(c + 0x384);
    ((PortOv096StateFn)(size_t)p[1].fn)(cv);
}

/* ---- (4) THE FALL-OFF-THE-END BODY -------------------------------------
 * src/func_ov096_02135e2c.cpp is declared `int` and has NO return statement.
 * mwccarm accepts that and lets r0 carry whatever the last call left;
 * MSVC refuses the translation unit outright with
 *     error C4716: 'func_ov096_02135e2c': must return a value
 * so this is a COMPILE failure, not an ABI one -- the fourth refusal shape in
 * this file and the only one that is a language-strictness difference rather
 * than a codegen difference.
 *
 * THE ROM AGREES THAT NOTHING IS RETURNED, checked rather than assumed. The
 * body's own tail at 0x02135ee4..0x02135ef8 is
 *     e28d100c  add r1,sp,#0xc
 *     e2800004  add r0,r0,#4
 *     ebfc07b6  bl  SurfaceInfo::CopyNormalTo   (returns void)
 *     e28dd01c  add sp,sp,#0x1c
 *     e8bd40f0  pop {r4-r7,lr}
 *     e12fff1e  bx  lr
 * -- no instruction sets r0 on the way out. And the ONE caller,
 * src/func_ov096_021365d4.c:12, declares it `void func_ov096_02135e2c(int *,
 * void *)` and discards the result at line 47. The `int` in this TU's own
 * signature is the recovery's placeholder, not something the ROM produces, so
 * the returned value is unobservable and 0 is as faithful as any other.
 *
 * WHY A HOST COPY RATHER THAN THE TREE'S OWN MECHANISM. port/tools/hostgen.py
 * carries a FALLS_OFF_RETURN table for exactly this failure, added by lane
 * w5-c for func_ov070_0211f0a4 (port/CMakeLists.txt:2421), and a two-line
 * entry there would keep this TU in the linkage count instead of costing it.
 * hostgen.py is not in this lane's append scope, so the cheap route was not
 * taken and the TU is refused instead -- ONE matched TU of the 49, and the
 * only one this lane gives up for a reason that is not a real ABI wall.
 * A later lane that owns hostgen.py should convert this: add
 *     "func_ov096_02135e2c": [(<the closing brace anchor>, ... "return 0;")]
 * to FALLS_OFF_RETURN, drop this copy, and put src/func_ov096_02135e2c.cpp
 * back into port/slice_ov096.txt. Every other line below is transcribed from
 * the matched body unchanged.
 */
int func_02038414(void *c);
int _ZNK12WithMeshClsn10IsOnGroundEv(void *c);
void *_ZNK12WithMeshClsn14GetFloorResultEv(void *c);
void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *s, int *out);
int _ZN4cstd4fdivEii(int a, int b);
int _ZNK12WithMeshClsn8IsOnWallEv(void *c);
void *_ZNK12WithMeshClsn13GetWallResultEv(void *c);

/* PORT_HOST_ABI: mwcc lets an int body fall off the end; MSVC refuses to
   compile it. The ROM sets no r0 at the tail and the one caller declares this
   void, so the value is unobservable. */
int func_ov096_02135e2c(int *self, void *clsn)
{
    int n0[3];
    int n1[3];
    func_02038414(clsn);
    if (_ZNK12WithMeshClsn10IsOnGroundEv(clsn)) {
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3(
            (char *)_ZNK12WithMeshClsn14GetFloorResultEv(clsn) + 4, n0);
        if (n0[1] != 0) {
            long long a = (long long)n0[0] * (long long)self[0xa4 / 4];
            long long b = (long long)n0[2] * (long long)self[0xac / 4];
            int x = (int)((a + 0x800) >> 12);
            int y = (int)((b + 0x800) >> 12);
            self[0xa8 / 4] = -(_ZN4cstd4fdivEii(x + y, n0[1]) + 0x8000);
        }
    }
    if (_ZNK12WithMeshClsn8IsOnWallEv(clsn)) {
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3(
            (char *)_ZNK12WithMeshClsn13GetWallResultEv(clsn) + 4, n1);
    }
    return 0;   /* the ROM returns nothing; the caller declares this void */
}

}  /* extern "C" */
