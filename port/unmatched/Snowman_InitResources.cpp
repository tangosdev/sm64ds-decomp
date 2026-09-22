/* HOST COPY of src/game/actors/d_a_bg_snwmn.cpp -- the TextureSequence::Prepare
 * calling-convention seam (the SHORT-1 argsweep row
 * __ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File), applied to
 * daBgSnwmn_c's ("SNOWMAN", actor id 272 -- NOT SnowmanBody or
 * SnowmanHead, see port/ov072_syms.txt's own header for the class-
 * identity derivation) InitResources.
 *
 * THE BUG, SAME SHAPE AS MOTHERPENGUIN'S (gate 191, port/unmatched/
 * MotherPenguin_InitResources.cpp). TextureSequence::Prepare is a real
 * non-static C++ method, Prepare(BMD_File &model, BTP_File &animFile),
 * consuming THREE ARM registers at the ABI level: r0=this, r1=&model,
 * r2=&animFile. Its ROM body is a tail-call veneer that does not touch
 * r0-r2, so whatever the caller already has loaded rides straight
 * through. The matched src/game/actors/d_a_bg_snwmn.cpp calls it as a
 * TWO-argument free function --
 *
 *   _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(
 *       (void *)data_ov072_02122c48[1], (void *)data_ov072_02122c50[1]);
 *
 * -- decl=2/push=2, consumed=3, the SHORT-1 row. On the DS this is byte-
 * identical (the enclosing InitResources's own `this` already sits in r0
 * at the call site, model in r1, animFile in r2, regardless of the C
 * declaration's arity). On the host, cdecl passes only the two declared
 * arguments; the real three-argument bridge's `self` parameter then reads
 * whatever the model pointer happens to be, and its own `bmd`/`btp`
 * parameters shift one slot short.
 *
 * THE FIX (MotherPenguin's own empirically-derived shape, re-applied):
 * self=model, bmd=animFile, btp=animFile -- func_02046d50's own matched
 * body (src/func_02046d50.c) only reads r0(arg)/r1(t), and its `struct
 * Tbl` layout matches BTP_File's, not BMD_File's; the model's own name
 * table is what gets searched (arg=model) using names walked out of the
 * animation file's table (t=animFile). This host copy is the matched src
 * line for line otherwise; only the Prepare call gains its real
 * three-argument shape.
 *
 * src/game/actors/d_a_bg_snwmn.cpp is dropped from slice_gate193.txt in favour
 * of this file; the byte-locked source is unchanged.
 */
#include "decl_common.h"

extern "C" {
extern int IsStarCollectedInLevel(signed char levelID, int starID);
extern void _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int id, unsigned int param, void *pos, void *ang, int a, int b);
extern void _ZN7fBase_c18MarkForDestructionEv(void *self);
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern void _ZN15TextureSequence8LoadFileER13SharedFilePtr(void *f);
/* the real three-argument bridge (hal/player_bridges.cpp): self, model, animFile */
extern void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void *self, void *bmd, void *btp);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *self, void *btp, int a, int fix, unsigned int u);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *self, void *act, void *pos, int f1, int f2, unsigned int u1, unsigned int u2);
extern void _ZN9dBgCh_GndC1Ev(void *self);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(void *self, void *pos, void *act);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(void *self);
extern void _ZN9dBgCh_GndD1Ev(void *self);
extern void func_ov072_021208d8(void *c);

extern int data_ov072_02122c70[];
extern int data_ov072_02122c48[];
extern int data_ov072_02122c50[];
extern int data_ov072_02122c40[];

/* PORT_HOST_ABI: TextureSequence::Prepare calling-convention seam (the
   SHORT-1 argsweep row, MotherPenguin's own shape re-applied); cdecl needs
   the third argument spelled explicitly. */
/* _ZN11daBgSnwmn_c13InitResourcesEv RETIRED (run link100, lane SEAT6, batch B6).
   The SHORT-1 argsweep row is one declaration and one argument per call
   site, which is all this copy ever changed.
   The matched TU src/_ZN11daBgSnwmn_c13InitResourcesEv.c is seated in its place: port/tools/hostgen.py's REG_RIDE_ARG table, self=model, bmd=animFile,
   btp=animFile -- this file's own derivation, applied to the
   generated TU.
   Per-row ROM evidence (referrer, RTTI name, kind:function record, the
   dispatch instruction read at its own address) is in port/slice_seat6.txt. */
}
