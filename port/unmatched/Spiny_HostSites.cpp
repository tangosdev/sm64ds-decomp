/* HOST COPIES of three ov077 SPINY (actor 260, daTgz_c) sites MSVC cannot
 * deliver from the matched source, run rel0215 lane cast-ov077. Each matched
 * TU stays byte-locked in src/ and is simply not referenced from
 * port/slice_ov077.txt. The reasoning is Lakitu_HostSites.cpp's, class for
 * class; only the offsets and the ROM addresses differ. Read that file's
 * header for the full derivation of each of the three shapes.
 *
 * (1) src/_ZN5SpinyD1Ev.cpp -- the C++-mangling wall. That TU is a real C++
 *     class ending `virtual ~Spiny();` with the body `Spiny::~Spiny() {}`;
 *     MSVC emits ??1Spiny@@UAE@XZ, its own COMDAT vtable, and member-dtor
 *     calls under MSVC mangling -- none of which is what vtable slot 16 needs.
 *
 *     ROM-CONFIRMED BODY. D1 is 0x02124b64..0x02124bb4, D0 is
 *     0x02124bb4..0x02124c18. config/arm9/overlays/ov077/relocs.txt on D1:
 *       0x02124b78 0x020373f8 _ZN12WithMeshClsnD1Ev
 *       0x02124b80 0x020149a4 _ZN18MovingCylinderClsnD1Ev
 *       0x02124b88 0x02015ff8 _ZN11ShadowModelD1Ev
 *       0x02124b90 0x0201691c _ZN9ModelAnimD1Ev
 *       0x02124b98 0x02016d20 _ZN5ModelD1Ev
 *       0x02124ba0 0x020112c8 _ZN5ActorD2Ev
 *       0x02124bb0 load       0x02127984  (_ZTV5Spiny)
 *     -- the same six calls in the same order as src/_ZN5SpinyD0Ev.c, which
 *     adds only _ZN6Memory10DeallocateEPvP4Heap. Offsets are that D0's own
 *     (0x1e4 / 0x1b0 / 0x188 / 0x124 / 0xd4), and note Spiny takes the PLAIN
 *     MovingCylinderClsn where Lakitu takes the WithPos child -- relocation
 *     confirmed, not assumed from the sibling.
 *
 *     D1 stores _ZTV5Spiny and D0 stores _ZTV7daTgz_c: one table, two
 *     spellings (the typeinfo record at 0x02127954 reads "7daTgz_c").
 *
 * (2) src/_ZN5Spiny6RenderEv.cpp -- the ModelAnim slot-5 collision. States 0
 *     and 4 draw the plain Model (whose host table IS dual-filled and would
 *     have been safe); every other state draws &mModelAnim through the same
 *     six-virtual local shadow, and _ZTV9ModelAnim's MSVC slot 5 is Virtual18.
 *     Offsets are Spiny.h's own: unk_0b0 0xb0, mModel 0xd4, mModelAnim 0x124,
 *     unk_3d8 0x3d8.
 *
 * (3) src/func_ov077_02125e20.cpp and src/func_ov077_02125e5c.cpp -- the PMF
 *     disease, state cell at self+0x3d0 (Lakitu's is 0x3f0). 02125e5c
 *     dispatches cell[0] (enter), 02125e20 cell[1] (tick).
 *
 * Spiny's twelve source records are seated by port_ov077_states_seat() before
 * __sinit_ov077_0212749c copies them into data_ov077_02127c28 -- the bss array
 * this lane had to PIN at 0x60, because dsd split it in three at the two
 * addresses arm9 cites for ov079's Whomp pair. See port/ov077_syms.txt.
 */
#include "Model.h"
#include "ModelAnim.h"

extern "C" {

/* ---- (1) the destructor ------------------------------------------------ */
extern int _ZTV5Spiny[];
void _ZN12WithMeshClsnD1Ev(void *);
void _ZN18MovingCylinderClsnD1Ev(void *);
void _ZN11ShadowModelD1Ev(void *);
void _ZN9ModelAnimD1Ev(void *);
void _ZN5ModelD1Ev(void *);
void _ZN5ActorD2Ev(void *);

/* PORT_HOST_ABI: mwcc-Itanium destructor name MSVC cannot emit; ROM call
 * order and offsets from relocs.txt + src/_ZN5SpinyD0Ev.c. */
int *_ZN5SpinyD1Ev(int *t)
{
    t[0] = (int)(size_t)_ZTV5Spiny;
    _ZN12WithMeshClsnD1Ev((char *)t + 0x1e4);
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x1b0);
    _ZN11ShadowModelD1Ev((char *)t + 0x188);
    _ZN9ModelAnimD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}

/* ---- (2) Render -------------------------------------------------------- */
/* PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Whomp/Fish case. */
int _ZN5Spiny6RenderEv(void *selfv)
{
    char *c = (char *)selfv;
    if ((*(unsigned int *)(c + 0xb0) & 0x40000) != 0)
        return 1;
    int s = *(int *)(c + 0x3d8);
    if (s == 0 || s == 4)
        ((Model *)(c + 0xd4))->Model::Render(0);
    else
        ((ModelAnim *)(c + 0x124))->ModelAnim::Render(0);
    return 1;
}

/* ---- (3) the two state dispatch sites ---------------------------------- */
struct PortOv077Pmf { unsigned int fn; int delta; };
typedef void (*PortOv077StateFn)(void *);

/* PORT_HOST_ABI: mwcc pointer-to-member through an incomplete class. */
void func_ov077_02125e5c(void *cv)   /* cell[0], the ENTER half */
{
    PortOv077Pmf *p = *(PortOv077Pmf **)((char *)cv + 0x3d0);
    ((PortOv077StateFn)(size_t)p[0].fn)(cv);
}

/* PORT_HOST_ABI: mwcc pointer-to-member through an incomplete class. */
void func_ov077_02125e20(void *cv)   /* cell[1], the TICK half */
{
    PortOv077Pmf *p = *(PortOv077Pmf **)((char *)cv + 0x3d0);
    ((PortOv077StateFn)(size_t)p[1].fn)(cv);
}

}  /* extern "C" */
