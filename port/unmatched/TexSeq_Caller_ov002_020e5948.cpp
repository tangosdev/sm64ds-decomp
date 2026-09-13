/* HOST COPY of src/func_ov002_020e5948.c -- one of the TextureSequence::Prepare callers.
 *
 * BYTE-IDENTICAL to the byte-locked source except for the Prepare extern
 * declaration (given its real third parameter) and each Prepare CALL (given its
 * real third argument). Produced by copying the source verbatim and applying
 * exact string edits, so it cannot carry a transcription error.
 *
 * THE CALLING-CONVENTION SEAM:
 *
 * TextureSequence::Prepare's ROM body is a 0xc tail-call veneer into
 * func_02046d50, whose real definition (src/func_02046d50.c) is
 *     void func_02046d50(void* arg, struct Tbl* t)
 * -- TWO arguments: a name-resolution context and a table it dereferences. At
 * every Prepare call the ROM sets exactly r0 and r1 (proven by disasm; e.g.
 * func_ov002_020e5948 0x020e60f4, LakituBro 0x0212ec44, func_ov091 0x02133300),
 * and func_02046d50 uses r0 as `arg` and r1 as `t`.
 *
 * The host wrapper (hal/player_bridges.cpp) is a THREE-parameter thiscall face:
 *     Prepare(void* self, void* bmd, void* btp)
 *       -> ((TextureSequence*)self)->Prepare(*bmd, *btp)
 *       -> func_02046d50(self, bmd, btp)      // btp ignored by func_02046d50
 * so it needs self = r0 = the caller's first value, bmd = r1 = the caller's
 * SECOND value. The matched C passes only TWO arguments, so the wrapper reads a
 * third (btp) from an unwritten stack slot and DEREFERENCES it (`*(BTP_File*)btp`)
 * -> reads garbage / faults, before func_02046d50 (which never uses it) runs.
 *
 * THE FIX gives the call its third argument: the caller's own second value
 * again (a known-valid BTP_File pointer). func_02046d50 ignores it; the wrapper's
 * deref is now of a real object. Nothing else changes.
 *
 * src/func_ov002_020e5948.c is dropped from slice_gate10.txt in favour of this file; the byte-locked source
 * is unchanged.
 */
#include "types.h"
#pragma opt_strength_reduction off
#pragma opt_common_subs off
#pragma opt_propagation off
#pragma opt_loop_invariants off
/* The source TU is a .c -- every symbol here (the function itself and all
   its data/function externs) is C-linkage. Compiled as .cpp, MSVC would
   mangle the lot (and callers of func_ov002_020e5948's flat name would no
   longer resolve), so the whole body is wrapped extern "C" -- the only
   edit beyond A's handoff content, which was generated from the .c. */
extern "C" {
extern void _ZN9Animation8LoadFileER13SharedFilePtr(void* sfp);
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* sfp);
extern void _ZN15TextureSequence8LoadFileER13SharedFilePtr(void* sfp);
extern void LoadSilverStarAndNumber(void);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* bmd, s32 a, s32 b);
extern void func_ov002_020bec2c(void* p);
extern void* _Znwj(u32 sz);
extern void* _ZN10ModelAnim2C1Ev(void* thiz);
extern void _ZN10ModelAnim213Func_020162C4Eji5Fix12IiEt(void* thiz, u32 a, s32 b, Fix12i c, u16 d);
extern s32 func_ov002_020e6bb0(void* p);
extern void func_ov002_020e6780(void* p);
extern void* _ZN9ModelAnimC1Ev(void* thiz);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, void* f, s32 i, Fix12i fx, u32 j);
extern void* _ZN5ModelC1Ev(void* thiz);
extern void _ZN11ShadowModel12InitCylinderEv(void* thiz);
extern s32 _ZNK6Player14GetBodyModelIDEjb(void* thiz, u32 a, s32 b);
extern void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void* bmd, void* btp, void* btp2);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void* thiz, void* f, s32 i, Fix12i fx, u32 j);

extern s8 data_0209f310[];
extern u8 data_0209f2d8;
extern s8 data_0209f2f8;

extern int data_ov002_0210e798[];
extern int data_ov002_0210e4f0[];
extern int data_ov002_0210e3d8[];
extern int data_ov002_0210e6c0[];
extern int data_ov002_0210eb98[];
extern int data_ov002_0210eb88[];
extern int data_ov002_0210e4c0[];
extern int data_ov002_0210da40[];
extern int data_ov002_0210d9a0[];
extern int data_ov002_0210d9c0[];
extern int data_ov002_0210e500[];
extern int data_ov002_0210e1c8[];
extern int data_ov002_0210e770[];
extern int data_ov002_0210e230[];
extern int data_ov002_0210e288[];
extern int data_ov002_0210e478[];
extern int data_ov002_0210e9d0[];
extern int data_ov002_0210ea10[];
extern int data_ov002_0210e538[];
extern int data_ov002_0210eac8[];
extern int data_ov002_0210e898[];
extern int data_ov002_0210e9e8[];
extern int data_ov002_0210eba8[];
extern int data_ov002_0210eb70[];
extern int data_ov002_0210eaf0[];
extern int data_ov002_0210e640[];
extern int data_ov002_0210e670[];
extern int data_ov002_0210e680[];
extern int data_ov002_0210e438[];
extern int data_ov002_0210e3b0[];
extern int data_ov002_0210e3c8[];
extern int data_ov002_0210e408[];
extern int data_ov002_0210e400[];
extern int data_ov002_0210ecb8[];
extern int data_ov002_0210e8d0[];
extern int data_ov002_0210ebd8[];
extern int data_ov002_0210e750[];
extern int data_ov002_0210d9a8[];
extern int data_ov002_02110aa4[];
extern int data_ov002_0210e588[];
extern int data_ov002_0210e7e8[];
extern int data_ov002_0210e450[];
extern int data_ov002_0210e788[];
extern int data_ov002_0210e1e8[];
extern int data_ov002_0210e1e0[];
extern int data_ov002_0210e4d0[];
extern int data_ov002_0210e4e8[];
extern int data_ov002_0210e1d0[];
extern int data_ov002_0210e800[];
extern int data_ov002_0210e4b8[];
extern int data_ov002_0210e270[];
extern int data_ov002_0210e458[];
extern int data_ov002_0210ea70[];
extern int data_ov002_0210e540[];
extern int data_ov002_0210e8e8[];
extern int data_ov002_0210eca8[];
extern int data_ov002_0210ec98[];
extern int data_ov002_0210e600[];
extern int data_ov002_0210ea88[];
extern int data_ov002_0210e910[];
extern int data_ov002_0210e460[];
extern int data_ov002_0210e2f0[];
extern int data_ov002_0210e958[];
extern int data_ov002_0210eb38[];
extern int data_ov002_0210e660[];
extern int data_ov002_0210ea30[];
extern int data_ov002_0210e3e8[];
extern int data_ov002_0210eb90[];
extern int data_ov002_0210e6b8[];
extern int data_ov002_0210e620[];
extern int data_ov002_0210e6a0[];
extern int data_ov002_0210eb58[];
extern int data_ov002_0210e3e0[];
extern int data_ov002_0210eb10[];
extern int data_ov002_0210e708[];
extern int data_ov002_0210ebc8[];
extern int data_ov002_0210e6e0[];
extern int data_ov002_0210e430[];
extern int data_ov002_0210ec40[];
extern int data_ov002_0210e738[];
extern int data_ov002_0210e250[];
extern int data_ov002_0210ec10[];
extern int data_ov002_0210ec00[];
extern int data_ov002_0210e728[];

extern void* data_ov002_020ff480[];
extern void* data_ov002_0210a68c[];
extern void* data_ov002_0210a69c[];
extern void* data_ov002_0210a6ac[];
extern void* data_ov002_020ff2f0[];

extern int data_ov002_0210da38[];
extern int data_ov002_0210e780[];
extern int data_ov002_0210e6f8[];
extern int data_ov002_0210e7b0[];
extern int data_ov002_0210ebb8[];
extern int data_ov002_0210eb20[];

/* PORT_HOST_ABI: TextureSequence::Prepare calling-convention seam (the
   SHORT-1 argsweep row); the ROM's tail-call veneer rides r0-r2 straight
   through, cdecl needs the third argument spelled explicitly. */
/* func_ov002_020e5948 RETIRED (run link100, lane SEAT6, batch B6).
   This copy's own banner says what it is: 'BYTE-IDENTICAL to the
   byte-locked source except for the Prepare extern declaration (given its
   real third parameter) and each Prepare CALL (given its real third
   argument)'. That is a declaration and three arguments, not a body.
   The matched TU src/func_ov002_020e5948.c is seated in its place: port/tools/hostgen.py's REG_RIDE_ARG table makes exactly those four
   edits in the generated TU, with the third argument the caller's own
   second value again -- this file's own derivation.
   Per-row ROM evidence (referrer, RTTI name, kind:function record, the
   dispatch instruction read at its own address) is in port/slice_seat6.txt. */
}
