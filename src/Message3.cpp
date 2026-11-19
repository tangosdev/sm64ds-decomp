//cpp
/* VS-mode text. LoadFont3D brings in the font, LoadTextVS picks the language
 * table, and the display entry points draw into the main and sub backgrounds.
 *
 * This is the single-player Message text code built a second time for the VS
 * mode, against its own globals. With relocations masked, nine functions here
 * are word-identical to their single-player twins, in the same order:
 * GetLanguageIndexVS, AddChoiceVS, AddCharPairVS, AddCharVS, LayoutLineVS,
 * SetTextGlobalsVS, func_02033a80 (func_0201cfb8), Message_DrawLineBG2VS and
 * Message_DrawCenteredLineVS. The other functions are VS variants of other
 * sizes. Twins take the single-player name plus `VS` where that name exists.
 *
 * leftovers:
 * - defer_codegen is off so this file emits in source order. The per-function
 *   optimization pragmas are brackets.
 * - data_0209fc78 is a volatile macro in func_02033464 only. decl_common.h
 *   spells that byte plain, and the other functions in this file read it plain.
 * - LayoutLineVS calls the handler as (data_02092830 + idx)[-1](), the
 *   spelling of its single-player twin; data_02092830[idx - 1]() subtracts
 *   first, a 6-word DIFF.
 * - func_020319fc writes the spill-over store as (&dst[wordIdx])[8] and keeps
 *   the char-cast data_0209fd5c read: dst[wordIdx + 8] grows the function by
 *   4 bytes, and indexing data_0209fd5c shrinks it by 8.
 *   Kept (#821 r82): data_0209fd5c[i + j * 16], *(data_0209fd5c + ...) and
 *   (data_0209fd5c + j * 16)[i] -> 0x180 or 55 words off of 0x188.
 * - SetTextGlobalsVS keeps the r2/r3 temps. One expression reorders the load
 *   of *(r2 + 4) against the store of r3.
 * - Address-named helpers stay address-named.
 */

#pragma defer_codegen off
#include "types.h"
#include "nitro/hw/registers.h"
#include "decl_common.h"
#include "PlayerInput.h"
#include "private/controller_input.h"
#include "SaveData.h"
#include "Message.h"
#include "MessageBank.h"
#include "nitro/gx/g2.h"
#include "nitro/gx/gx_bgcnt.h"
#include "nitro/gx/gx.h"
#include "private/mb_views.h"

struct OamAttr;

extern "C" {
extern u8 data_0209fc6c;
extern u8 data_0209fc70;
extern u8 data_0209fc74;
extern u8 data_0209fc7c;
extern u8 data_0209fc80;
extern u8 data_0209fc84;
extern u8 data_0209fc88;
extern u8 data_0209fc8c;
extern u8 data_0209fc90;
extern u8 data_0209fc98;
extern u8 data_0209fca0;
extern u8 data_0209fca4;
extern u8 data_0209fca8;
extern u8 data_0209fcac;
extern u8 data_0209fcb0;
extern u8 data_0209fcb4;
extern u8 data_0209fcb8;
extern u8 data_0209fcbc;
extern u8 data_0209fcc0;
extern u8 data_0209fcc4;
extern u8 data_0209fcc8;
extern u8 data_0209fccc;
extern u8 data_0209fcd0;
extern u8 data_0209fcd4;
extern u8 data_0209fcd8;
extern u8 data_0209fce0;
extern u8 data_0209fce4;
extern u8 data_0209fcf4[];
extern s16 data_0209fce8;
extern s16 data_0209fcec;
extern s16 data_0209fcf0;
extern int *data_0209fcf8;
extern int data_0209fcfc;
extern int data_0209fd00;
extern int data_0209fd04;
extern int data_0209fd08;
extern u8 *data_0209fd0c;
extern void *data_0209fd10;
extern int data_0209fd14;
extern int data_0209fd18;
extern int data_0209fd1c;
extern u8 data_0209fd20[];
extern u32 data_0209fd5c[];
extern u8 data_02092818[];
extern int data_0209285c[];
extern void (*data_02092830[])(void);
extern u8 data_0208f074[];
extern u8 data_0208f174[];
extern s32 data_0208ee44;
extern u32 gNextActorUpdateMask;
extern u8 gSubScreenLayers;
extern u8 gMainScreenLayers;
extern u16 data_020a0e5a[];
extern s32 data_020a0db0;
extern struct OamAttr data_ov002_0210c390;
extern struct OamAttr data_ov002_0210c398;
extern struct OamAttr data_ov002_0210c3a0;
extern struct OamAttr data_ov002_0210c3a8;
extern int data_0209289c[296];
/* VS message banks. Each blob is one int table; the loader takes its address. */
int data_02092d3c[328] = {
    0x4d455347, 0x626d6731, 0x00000521, 0x00000002, 0x00000003, 0x00000000, 0x00000000, 0x00000000,
    0x494e4631, 0x00000140, 0x00400023, 0x00000000, 0x00000001, 0x000100c8, 0x0000000a, 0x000100c8,
    0x00000016, 0x000100c8, 0x00000027, 0x000100c8, 0x00000030, 0x000100c8, 0x00000035, 0x00010098,
    0x00000043, 0x00010098, 0x0000004e, 0x00010098, 0x0000005d, 0x000100c8, 0x00000060, 0x00010060,
    0x00000069, 0x00010060, 0x00000074, 0x00010060, 0x0000007b, 0x00010020, 0x0000007e, 0x00010020,
    0x00000082, 0x000100c8, 0x0000008a, 0x000100c8, 0x0000008e, 0x000100c8, 0x00000091, 0x000200a0,
    0x000000a1, 0x000700a0, 0x000000ff, 0x000700a0, 0x00000183, 0x000700a0, 0x000001e3, 0x000700a0,
    0x00000260, 0x000400a0, 0x00000280, 0x00070078, 0x00000290, 0x000100c8, 0x0000029f, 0x000200a0,
    0x000002b7, 0x000700a0, 0x000002e8, 0x000400a0, 0x0000030e, 0x000400a0, 0x00000324, 0x000200a0,
    0x00000339, 0x000200a0, 0x00000370, 0x000100c8, 0x00000379, 0x000100c8, 0x00000385, 0x000200a0,
    0x000003ad, 0x000100c8, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x44415431, 0x000003c1, 0x17180cff, 0x1e17121d, 0x210eff0e, 0x0c4d1d12, 0x1c1b1e18, 0x180cff0e,
    0x181b1d17, 0x1b0e1515, 0x0d18164d, 0x0cff1c0e, 0x121d1718, 0xff0e1e17, 0x1d121e1a, 0x0a1d1cff,
    0x1b0a0d17, 0x18164d0d, 0x1dff0e0d, 0x110c1e18, 0x0d18164d, 0x1e0dff0e, 0x1147150a, 0x4d0d170a,
    0x0e0d1816, 0xff1418ff, 0x1b1b1e1c, 0x0d171e18, 0x0a0e11ff, 0x1811190d, 0xff1c0e17, 0x1b0e1d1c,
    0x18ff180e, 0x0f18ff17, 0x1918ff0f, 0x1718121d, 0x0e22ff1c, 0x1817ff1c, 0x2d3e19ff, 0x2f35402f,
    0x382d4d31, 0x26313a3b, 0x171215ff, 0x10171214, 0x1b1b0e4d, 0xfd271b18, 0x1d0e10fd, 0x18150c4d,
    0x4d1b0e1c, 0x4d0d170a, 0x0e0c0a0f, 0x0e111d4d, 0x171215fd, 0x0a194d14, 0x0e171d1b, 0x170a4d1b,
    0x1b1d4d0d, 0x100a4d22, 0x4817120a, 0x1519fdfd, 0x0e1c0a0e, 0x0e1b194d, 0x0a4d1c1c, 0x0b4d2217,
    0x181d1d1e, 0x20ff4817, 0x38313e35, 0x4d3f3f31, 0x39393b0c, 0x2f353a41, 0x3b35402d, 0x0f4d3f3a,
    0x3138352d, 0xfdfd2730, 0x3a3e411d, 0x433b3c4d, 0x184d3e31, 0x2d4d0f0f, 0x184d303a, 0x3b404d17,
    0x313efd4d, 0x4d40313f, 0x403f453f, 0xfd483931, 0x312f2d0f, 0x413b454d, 0x1c0d4d3e, 0x3e2d3c4d,
    0x3e313a40, 0x3b394d29, 0x2ffd3142, 0x313f3b38, 0x3b404d3e, 0x34403133, 0x4d293e31, 0x4d303a2d,
    0x4d453e40, 0x352d332d, 0x0dff483a, 0x15172018, 0x4d0d0a18, 0x15120a0f, 0xfd270d0e, 0x1d0e10fd,
    0x18150c4d, 0x4d1b0e1c, 0x4d0d170a, 0x0e0c0a0f, 0x0e111d4d, 0x1712154d, 0x0a19fd14, 0x0e171d1b,
    0x170a4d1b, 0x1b1d4d0d, 0x100a4d22, 0x4817120a, 0x1519fdfd, 0x0e1c0a0e, 0x0e1b194d, 0x0a4d1c1c,
    0x0b4d2217, 0x181d1d1e, 0x0dff4817, 0x3b0d4d1c, 0x3b383a43, 0x194d302d, 0x4d452d38, 0x38352d0f,
    0xfd273031, 0x3e411dfd, 0x3b3c4d3a, 0x4d3e3143, 0x4d0f0f18, 0x4d303a2d, 0x404d1718, 0x3efd4d3b,
    0x40313f31, 0x3f453f4d, 0x48393140, 0x2f2d0ffd, 0x3b454d31, 0x0d4d3e41, 0x2d3c4d1c, 0x313a403e,
    0x394d293e, 0xfd31423b, 0x3f3b382f, 0x404d3e31, 0x4031333b, 0x293e3134, 0x303a2d4d, 0x453e404d,
    0x2d332d4d, 0xff483a35, 0x2f3a2d0c, 0x434d3831, 0x38313e35, 0x4d3f3f31, 0x39393b2f, 0x2f353a41,
    0x3b35402d, 0xff263f3a, 0x3a3e411d, 0x433b3c4d, 0x184d3e31, 0xff480f0f, 0x0c1e181d, 0x181d4d11,
    0x0a1d1c4d, 0x1bff1d1b, 0x3e414031, 0x3b404d3a, 0x40351d4d, 0x1c4d3138, 0x31313e2f, 0x0aff263a,
    0x3c4d3838, 0x31452d38, 0x344d3f3e, 0x4d31422d, 0x40323138, 0x3134404d, 0x2d33fd4d, 0x4d293139,
    0x1f4d3b3f, 0x35434d1c, 0x314d3838, 0xff48303a, 0x000105fe, 0x31384d01, 0x404d4032, 0xfd4d3134,
    0x31392d33, 0x3b3f4d29, 0x4d1c1f4d, 0x38383543, 0x303a314d, 0x05feff48, 0x4d010001, 0x40323138,
    0x3134404d, 0x2d33fd4d, 0xff483139, 0x4d313a18, 0x31393b39, 0x3c4d403a, 0x3f2d3138, 0x48484831,
    0x3e2f1cff, 0x382e392d, 0x3b404d31, 0x4031334d, 0x3134404d, 0x2d401c4d, 0xfd273f3e, 0x4d31341d,
    0x3f3c2d2f, 0x3835434d, 0x31344d38, 0x454d3c38, 0x3b4d413b, 0xff484041, 0x1d17180c, 0x0e1e1712,
    0x0105feff, 0x204d0100, 0x271c1712, 0x150e1cff, 0x4a1d0c0e, 0x3a3b0c4d, 0x383b3e40, 0x4d3e3138,
    0x31303b16, 0x1d1cfd3f, 0x4a1d1b0a, 0x403c184d, 0x3f3a3b35, 0x121e1aff, 0x1c1f4d1d, 0xffffffff,
};
int data_0209325c[344] = {
    0x4d455347, 0x626d6731, 0x00000561, 0x00000002, 0x00000001, 0x00000000, 0x00000000, 0x00000000,
    0x494e4631, 0x00000140, 0x00400023, 0x00000000, 0x00000001, 0x000100c8, 0x0000000a, 0x000100c8,
    0x00000014, 0x000100c8, 0x0000001c, 0x000100c8, 0x00000025, 0x000100c8, 0x0000002a, 0x00010098,
    0x00000033, 0x00010098, 0x00000039, 0x00010098, 0x00000044, 0x000100c8, 0x00000047, 0x00010060,
    0x00000050, 0x00010060, 0x00000057, 0x00010060, 0x0000005e, 0x00010020, 0x00000061, 0x00010020,
    0x00000064, 0x000100c8, 0x0000006c, 0x000100c8, 0x0000006f, 0x000100c8, 0x00000072, 0x000200a0,
    0x00000090, 0x000700a0, 0x000000ee, 0x000700a0, 0x00000173, 0x000700a0, 0x000001d3, 0x000700a0,
    0x00000255, 0x000400a0, 0x0000027a, 0x00070078, 0x0000028d, 0x000100c8, 0x000002a0, 0x000200a0,
    0x000002c3, 0x000700a0, 0x00000313, 0x000400a0, 0x0000034d, 0x000400a0, 0x0000036c, 0x000200a0,
    0x00000379, 0x000200a0, 0x000003a8, 0x000100c8, 0x000003b1, 0x000100c8, 0x000003c1, 0x000200a0,
    0x000003e0, 0x000100c8, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x44415431, 0x00000401, 0x17180cff, 0x1e17121d, 0x0b0aff0a, 0x0d170a0b, 0xff0a1718, 0x0a16180c,
    0xff120d17, 0x1d17180c, 0x0a1e1712, 0x0c1c0eff, 0x1d1cff12, 0x0a0d170a, 0x1dff0d1b, 0x110c1e18,
    0x16121cff, 0x1b1d0e16, 0xff0a0c12, 0x1cff1418, 0x181b1b1e, 0xff0d171e, 0x0f0f1e0c, 0x1cff0e12,
    0x0e1b0e1d, 0x9c1cff18, 0xff1817ff, 0x12231918, 0xff121718, 0x17ff9c1c, 0x411fff18, 0x314d353b,
    0x2f3e313f, 0x3e2d4035, 0x3ffd3540, 0x2d463a31, 0x35392d4d, 0xff26352f, 0x14171215, 0x4d101712,
    0x181b1b0e, 0xfdfd271b, 0x4d1d0e10, 0x1c18150c, 0x0a4d1b0e, 0x0f4d0d17, 0x4d0e0c0a, 0xfd0e111d,
    0x14171215, 0x1b0a194d, 0x1b0e171d, 0x0d170a4d, 0x221b1d4d, 0x0a100a4d, 0xfd481712, 0x0e1519fd,
    0x4d0e1c0a, 0x1c0e1b19, 0x170a4d1c, 0x1e0b4d22, 0x17181d1d, 0x3b0cff48, 0x353a4139, 0x35462d2f,
    0x4d313a3b, 0x313e3543, 0x3f3f3138, 0x3b3afd4d, 0x353e4d3a, 0x352f3f41, 0xfd272d40, 0x313c1cfd,
    0x4d353a33, 0x353e4d31, 0x312f2f2d, 0x4d35303a, 0x2f4d2d38, 0x3b3f3a3b, 0xfd483138, 0x3542420a,
    0x2d3a352f, 0x2d4d3540, 0x2d493838, 0x3b3e4038, 0x3a35174d, 0x303a3140, 0x1c0d4d3b, 0x3139fd29,
    0x40354040, 0x35304d35, 0x3b3e324d, 0x4d31403a, 0x353e4d31, 0x423b3e3c, 0x0dff482d, 0x15172018,
    0x4d0d0a18, 0x15120a0f, 0xfd270d0e, 0x1d0e10fd, 0x18150c4d, 0x4d1b0e1c, 0x4d0d170a, 0x0e0c0a0f,
    0x0e111d4d, 0x1712154d, 0x0a19fd14, 0x0e171d1b, 0x170a4d1b, 0x1b1d4d0d, 0x100a4d22, 0x4817120a,
    0x1519fdfd, 0x0e1c0a0e, 0x0e1b194d, 0x0a4d1c1c, 0x0b4d2217, 0x181d1d1e, 0x16ff4817, 0x382d303b,
    0x4db04035, 0x3a433b30, 0x302d3b38, 0xfd1c0d4d, 0x4d3a3b3a, 0x3f41353e, 0x2d40352f, 0x1cfdfd27,
    0x3a33313c, 0x4d314d35, 0x2f2d353e, 0x303a312f, 0x2d384d35, 0x3a3b2f4d, 0x31383b3f, 0x420afd48,
    0x352f3542, 0x35402d3a, 0x38382d4d, 0x40382d49, 0x174d3b3e, 0x31403a35, 0x4d3b303a, 0xfd291c0d,
    0x40403139, 0x4d354035, 0x324d3530, 0x403a3b3e, 0x4d314d31, 0x3e3c353e, 0x482d423b, 0x3a3a0aff,
    0x2d383841, 0x384d313e, 0x3b2f4d2d, 0x353a4139, 0x35462d2f, 0xfd313a3b, 0x313e3543, 0x3f3f3138,
    0x3c1cff26, 0x353a3331, 0x4d2d384d, 0x3f3a3b2f, 0x4831383b, 0x0c181dff, 0x194d0a0c, 0x124d1b0e,
    0x12231217, 0xff0e1b0a, 0x3b40351b, 0x3e2d3a3e, 0x382d4d31, 0x3ffd3b38, 0x3e31342f, 0x304d3b39,
    0x1d4d3831, 0x383b4035, 0x1dff263b, 0x35404041, 0x334d354d, 0x2d2f3b35, 0x353e3b40, 0x3a3b3f4d,
    0x3f414d3b, 0x3540352f, 0x382d30fd, 0x394d2d38, 0x382d303b, 0x4db04035, 0x3035321c, 0x384d292d,
    0x2d3c4d2d, 0x4035403e, 0x2d3ffd2d, 0x354db03e, 0x3e31403a, 0x40403b3e, 0xfeff482d, 0x01000105,
    0x3a3b3a4d, 0x3c4db84d, 0xfd4dc935, 0x3f313e3c, 0x31403a31, 0x2d384d29, 0x303b394d, 0x4035382d,
    0x321cfdb0, 0x4d2d3035, 0x2f4d353f, 0x382f3a3b, 0x48313041, 0x0105feff, 0x344d0100, 0x2d384d2d,
    0x2d352f3f, 0xfd4d3b40, 0x3c4d2d38, 0x35403e2d, 0xff482d40, 0x3140400a, 0x3e31303a, 0x48484831,
    0x32320aff, 0x2d3e3e31, 0x4d31384d, 0x3831401c, 0xfd273138, 0x2d2f4d12, 0x38313c3c, 0x404d3538,
    0x3b3f4d35, 0x304d3b3a, 0x41352d49, 0xff483b40, 0x1d17180c, 0x0a1e1712, 0x0105feff, 0x114d0100,
    0x121f4d0a, 0x27181d17, 0x150e1cff, 0x4a1d0c0e, 0x393b2f4d, 0x35303a2d, 0x0a1d1cfd, 0x4d4a1d1b,
    0x35463c3b, 0xff353a3b, 0x120c1c0e, 0x4d0a0d4d, 0x0d120f1c, 0xffffff0a, 0xffffffff, 0xffffffff,
};
int data_020937bc[368] = {
    0x4d455347, 0x626d6731, 0x000005c1, 0x00000002, 0x00000001, 0x00000000, 0x00000000, 0x00000000,
    0x494e4631, 0x00000140, 0x00400023, 0x00000000, 0x00000001, 0x000100c8, 0x00000008, 0x000100c8,
    0x00000016, 0x000100c8, 0x00000027, 0x000100c8, 0x0000002e, 0x000100c8, 0x00000034, 0x00010098,
    0x00000042, 0x00010098, 0x0000004d, 0x00010098, 0x00000058, 0x000100c8, 0x0000005e, 0x00010060,
    0x00000069, 0x00010060, 0x00000075, 0x00010060, 0x0000007d, 0x00010020, 0x00000080, 0x00010020,
    0x00000083, 0x000100c8, 0x0000008c, 0x000100c8, 0x0000008f, 0x000100c8, 0x00000092, 0x000200a0,
    0x000000a3, 0x000700a0, 0x00000116, 0x000700a0, 0x000001a2, 0x000700a0, 0x00000215, 0x000700a0,
    0x00000296, 0x000400a0, 0x000002c4, 0x00070078, 0x000002d6, 0x000100c8, 0x000002e8, 0x000200a0,
    0x00000306, 0x000700a0, 0x00000351, 0x000400a0, 0x00000384, 0x000400a0, 0x0000039f, 0x000200a0,
    0x000003b8, 0x000200a0, 0x000003ea, 0x000100c8, 0x000003f1, 0x000100c8, 0x00000403, 0x000200a0,
    0x0000042c, 0x000100c8, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x44415431, 0x00000461, 0x100e1cff, 0xff1b121e, 0x12150a1c, 0x0e0d4d1b, 0x1e1a0a4d, 0x1816ff9d,
    0x4d1c180d, 0x0c4d0e0d, 0x1b1d1718, 0x1cff1518, 0x121e100e, 0x0a1cff1b, 0xff1b1215, 0x180d1816,
    0x1d1c0e4d, 0x0a0d1791, 0x1816ff1b, 0x1d4d180d, 0x1b0a0c18, 0x0d1816ff, 0x91154d18, 0xff231219,
    0x1d1c1215, 0x170eff18, 0x1f15181f, 0x0e1d170e, 0x1b1e0aff, 0x151e0c12, 0x1c0e1b0a, 0x1d1c0eff,
    0x180e1b99, 0xff9d1cff, 0x18ff1817, 0x18120c19, 0xff1c0e17, 0x17ff9d1c, 0x198fff18, 0x402f2d3e,
    0x3e2d2f35, 0x383b3f4d, 0x71ff263b, 0x181b1b0e, 0x0e0d4d1b, 0x17180c4d, 0xa312210e, 0xfdfd2717,
    0x991d1712, 0x150a1d17, 0x1d184d18, 0x1f4d0a1b, 0xfd4d230e, 0x1815180c, 0x0d17910c, 0x4d0e1d18,
    0x4d1c9116, 0x0c1b0e0c, 0xfd224d0a, 0x0f4d0e0d, 0x1d170e1b, 0x4d0a4d0e, 0x0c4d1e1d, 0x0a191618,
    0x181b0ea1, 0x19fdfd48, 0x0a1c151e, 0x0a1e0c4d, 0x121e1a15, 0x0b4d1b0e, 0x17a31d18, 0x2d0fff48,
    0x4d3b3838, 0x2f4d3130, 0x3a41393b, 0x2f2d2f35, 0x4d3ac335, 0x382d3a35, 0x3e2e39b1, 0xfd2d2f35,
    0x2d3c0afd, 0x454d2d33, 0x2f3a314d, 0x303a3135, 0x2d384d31, 0x3a3b2f4d, 0x2d383b3f, 0x3e2d3cfd,
    0x313e4d2d, 0x2f353a35, 0x383e2d35, 0xfd4d482d, 0xb9403a12, 0x382d403a, 0x403b4d3b, 0x424d2d3e,
    0x394d4631, 0x2f4d3fb1, 0x2d2f3e31, 0x4d45fd4d, 0x324d3130, 0x403a313e, 0x4d2d4d31, 0x2f4d4140,
    0x2d3c393b, 0x3b3e31c1, 0x0f71ff48, 0x1815150a, 0x4d0e0d4d, 0x0c1c0e0d, 0x0a101b0a, 0x12fdfd27,
    0x17991d17, 0x18150a1d, 0x1b1d184d, 0x0e1f4d0a, 0x0cfd4d23, 0x0c181518, 0x180d1791, 0x164d0e1d,
    0x0c4d1c91, 0x0a0c1b0e, 0x0dfd224d, 0x1b0f4d0e, 0x0e1d170e, 0x1d4d0a4d, 0x180c4d1e, 0xa10a1916,
    0x48181b0e, 0x1e19fdfd, 0x4d0a1c15, 0x150a1e0c, 0x0e121e1a, 0x180b4d1b, 0x4817a31d, 0x2d0f71ff,
    0x4d3b3838, 0x304d3130, 0x2d2f3f31, 0x4d2d333e, 0xfd271c0d, 0x2d3c0afd, 0x454d2d33, 0x2f3a314d,
    0x303a3135, 0x2d384d31, 0x3a3b2f4d, 0x2d383b3f, 0x3e2d3cfd, 0x313e4d2d, 0x2f353a35, 0x383e2d35,
    0xfd4d482d, 0xb9403a12, 0x382d403a, 0x403b4d3b, 0x424d2d3e, 0x394d4631, 0x2f4d3fb1, 0x2d2f3e31,
    0x4d45fd4d, 0x324d3130, 0x403a313e, 0x4d2d4d31, 0x2f4d4140, 0x2d3c393b, 0x3b3e31c1, 0x0d8fff48,
    0x2d313f31, 0x2d2f4d3f, 0x38312f3a, 0x384d3e2d, 0x3b2ffd2d, 0x353a4139, 0x352f2d2f, 0x354d3ac3,
    0xb1382d3a, 0x353e2e39, 0xff262d2f, 0x332d3c0a, 0x2d384d2d, 0x3a3b2f4d, 0x2d383b3f, 0x181dff48,
    0x194d0a0c, 0x4d0a1b0a, 0x0e19160e, 0xff1b0a23, 0x4d3e128f, 0x2d384d2d, 0x3a2d3c4d, 0x38382d40,
    0x31304d2d, 0xbd404d38, 0x3b384140, 0x380eff26, 0x303b394d, 0x1c1f4d3b, 0x313f4d48, 0x3e312f4d,
    0xb13e2d3e, 0x3e3b3c4d, 0xfd31413d, 0x3b303b40, 0x3b384d3f, 0x31304d3f, 0x4d3fb139, 0x2d334136,
    0x313e3b30, 0x3b38fd3f, 0x3a2d344d, 0x2d2e2d4d, 0x3a3b303a, 0x483b302d, 0x0105feff, 0x344d0100,
    0x2d3f4d2d, 0x3b303538, 0x3831304d, 0x314136fd, 0x4d4a3b33, 0x394d3831, 0x4d3b303b, 0x4d481c1f,
    0x2d3a3532, 0x2d463538, 0xff48b13e, 0x000105fe, 0x2d344d01, 0x382d3f4d, 0xfd3b3035, 0x4d383130,
    0x33314136, 0x1eff483b, 0x3b394d3a, 0x403a3139, 0x3c4d293b, 0x324d3e3b, 0x3e3b422d, 0xff484848,
    0x462d1171, 0x2f4d3140, 0x384d3a3b, 0x0e4d3f2d, 0x313e403f, 0x3f2d3838, 0x2d15fd27, 0x3b334d3f,
    0x3f2d3e3e, 0x4d31404d, 0x3041452d, 0x3ab13e2d, 0x0e1cff48, 0x1b121e10, 0x0a1171ff, 0x170a104d,
    0x4d180d0a, 0x000105fe, 0x1cff2701, 0x0c0e150e, 0x164d4a1d, 0x3f3b303b, 0x4d31304d, 0x403a3b0c,
    0xfd383b3e, 0x1b0a1d1c, 0x184d4a1d, 0x3b352f3c, 0xff3f313a, 0x12150a1c, 0x0e0d4d1b, 0x481c1f4d,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
};
int data_02093d7c[376] = {
    0x4d455347, 0x626d6731, 0x000005e1, 0x00000002, 0x00000001, 0x00000000, 0x00000000, 0x00000000,
    0x494e4631, 0x00000140, 0x00400023, 0x00000000, 0x00000001, 0x000100c8, 0x00000008, 0x000100c8,
    0x00000018, 0x000100c8, 0x00000022, 0x000100c8, 0x00000029, 0x000100c8, 0x00000031, 0x00010098,
    0x00000040, 0x00010098, 0x0000004c, 0x00010098, 0x00000059, 0x000100c8, 0x0000005f, 0x00010060,
    0x00000068, 0x00010060, 0x00000072, 0x00010060, 0x00000079, 0x00010020, 0x0000007d, 0x00010020,
    0x00000081, 0x000100c8, 0x0000008a, 0x000100c8, 0x0000008d, 0x000100c8, 0x00000092, 0x000200a0,
    0x000000a5, 0x000700a0, 0x00000115, 0x000700a0, 0x000001ac, 0x000700a0, 0x00000221, 0x000700a0,
    0x000002c8, 0x000400a0, 0x000002f1, 0x00070078, 0x0000030f, 0x000100c8, 0x0000031f, 0x000200a0,
    0x0000033b, 0x000700a0, 0x00000382, 0x000400a0, 0x000003be, 0x000400a0, 0x000003dd, 0x000200a0,
    0x000003f3, 0x000200a0, 0x00000424, 0x000100c8, 0x0000042b, 0x000100c8, 0x0000043a, 0x000200a0,
    0x00000463, 0x000100c8, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x44415431, 0x00000481, 0x120e20ff, 0xff1b0e1d, 0x1c1b1e14, 0x1b0e1f4d, 0x1c1c0a15, 0xff26170e,
    0x1e0e1d1c, 0x171e1b0e, 0x0e20ff10, 0x1b0e1d12, 0x0e0e0bff, 0x170e0d17, 0x0a1d1cff, 0x1b0a0d17,
    0x1816470d, 0xff1c1e0d, 0x0c1e181d, 0x18164711, 0xff1c1e0d, 0x0a114702, 0x16470d17, 0x1c1e0d18,
    0x144818ff, 0x1cff2748, 0x181b1b1e, 0xff0d171e, 0x0f191814, 0x0e1ba611, 0x1d1cff1b, 0x180e1b0e,
    0x17120eff, 0x1c1e0aff, 0x1d1918ff, 0x0e171812, 0x0a13ff17, 0x120e17ff, 0x380aff17, 0x3a353138,
    0x2d3e404d, 0x31353a35, 0x263a313e, 0x1b0e1fff, 0x0d17120b, 0x1c10171e, 0x15110e0f, 0xfd271b0e,
    0x171216fd, 0x0e121612, 0x0d4d0e1b, 0x0e4d0e12, 0x0e0f1d17, 0x171e171b, 0x171efd10, 0x121b4d0d,
    0x0e1d110c, 0x0e120d4d, 0x1b0e104d, 0xfd0e1d94, 0x0e0f1e0a, 0x170a1712, 0x4d1b0e0d, 0x481c1e0a,
    0x0bfdfd4d, 0x0e1d1d12, 0x17120e4d, 0x0a1d4d0e, 0x4d0e1d1c, 0x0cac1b0d, 0x48170e14, 0x3e311fff,
    0x303a352e, 0x3f333a41, 0x38343132, 0xfd273e31, 0x342f1cfd, 0x3140382d, 0x3f2d304d, 0x3f451c4d,
    0x4d393140, 0x4d3f412d, 0xfd303a41, 0x30313543, 0x314d3e31, 0x4d293a35, 0x314d3941, 0x3a313a35,
    0x3f311b4d, 0x2dfd4031, 0x41463f41, 0x3e34cc32, 0x4d483a31, 0x353a3516, 0x3e313539, 0x35304d31,
    0x3a0efd31, 0x3e313240, 0x333a413a, 0x303a414d, 0x2f353e4d, 0x4d314034, 0xfd313530, 0xb43e3110,
    0x2d4d3140, 0x35313241, 0x303a2d3a, 0x2d4d3e31, 0xff483f41, 0x1720180d, 0x0d0a1815, 0x110e0f4d,
    0x1c0e1015, 0x0a15110c, 0x27170e10, 0x1216fdfd, 0x12161217, 0x4d0e1b0e, 0x4d0e120d, 0x0f1d170e,
    0x1e171b0e, 0x1efd1017, 0x1b4d0d17, 0x1d110c12, 0x120d4d0e, 0x0e104d0e, 0x0e1d941b, 0x0f1e0afd,
    0x0a17120e, 0x1b0e0d17, 0x1c1e0a4d, 0x0bfdfd48, 0x0e1d1d12, 0x17120e4d, 0x0a1d4d0e, 0x4d0e1d1c,
    0x0cac1b0d, 0x48170e14, 0x471c0dff, 0x3a433b0d, 0x302d3b38, 0x353c1c47, 0x324d3831, 0x47383431,
    0x3f3133fd, 0x2d38342f, 0x273a3133, 0x342f1cfd, 0x3140382d, 0x3f2d304d, 0x3f451c4d, 0x4d393140,
    0x4d3f412d, 0xfd303a41, 0x30313543, 0x314d3e31, 0x4d293a35, 0x314d3941, 0x3a313a35, 0x3f311b4d,
    0x2dfd4031, 0x41463f41, 0x3e34cc32, 0x4d483a31, 0x353a3516, 0x3e313539, 0x35304d31, 0x3a0efd31,
    0x3e313240, 0x333a413a, 0x303a414d, 0x2f353e4d, 0x4d314034, 0xfd313530, 0xb43e3110, 0x2d4d3140,
    0x35313241, 0x303a2d3a, 0x2d4d3e31, 0xff483f41, 0x342d3e0d, 0x3f3b3840, 0x1c0d4d31, 0x402d0d47,
    0x2ecc3a31, 0x3e403e31, 0x3a41332d, 0x2e2dfd33, 0x2f313e2e, 0x263a3134, 0x40350bff, 0x304d3140,
    0x1c4d3f2d, 0x31403f45, 0x412dfd39, 0x342f3f3f, 0x3140382d, 0x1cff483a, 0x1d1b0a1d, 0x0e0b4d4a,
    0x1b11ac1b, 0x23ff170e, 0x2fcc3e41, 0x41464d37, 0x351d4d39, 0x2e383140, 0x3f303835, 0x3e35342f,
    0x0aff2639, 0x4d313838, 0x3835311d, 0x3934313a, 0x344d3e31, 0x3a312e2d, 0x3f2d30fd, 0x353c1c4d,
    0x424d3831, 0x2d383e31, 0x3a313f3f, 0x310d4d48, 0x3f1ffd3e, 0x3b164748, 0x4d3f4130, 0x4d403f35,
    0x3a31312e, 0x48403130, 0x05fefffd, 0xfd010001, 0x312e2d34, 0x2d304d3a, 0x3c1c4d3f, 0x4d383135,
    0x383e3142, 0x313f3f2d, 0x0dfd483a, 0x1f4d3e31, 0x1647483f, 0x3f41303b, 0x403f354d, 0x31312e4d,
    0x4031303a, 0x05feff48, 0xfd010001, 0x4d402d34, 0x4d3f2d30, 0x31353c1c, 0x31424d38, 0x3f2d383e,
    0x483a313f, 0x3a350eff, 0x164d3a31, 0x3a31393b, 0x352e4d40, 0x48314040, 0x10ff4848, 0x3235313e,
    0x4d41464d, 0x4d3a3130, 0x3e31401c, 0x483a313a, 0x31350dfd, 0x40cc164d, 0x344d3146, 0x40323835,
    0x3e35304d, 0x2e2d304d, 0xff273531, 0x1d120e20, 0xfeff1b0e, 0x01000105, 0x200e104d, 0x1d171712,
    0x0e1cff27, 0x1d0c0e15, 0x3b0c4d4a, 0x3b3e403a, 0x3e313838, 0x303b1647, 0x1cfd3f41, 0x1d1b0a1d,
    0x3c184d4a, 0x3a3b3540, 0x1fff3a31, 0x1647483f, 0x1c1e0d18, 0x0d170e4d, 0xffff260e, 0xffffffff,
};

extern const u16 data_02075b14[256];
extern const u16 data_02075d14[256];
extern u16 data_02092808[];
extern u8 data_02092810[];
extern u8 data_02092824[];
extern u32 data_0209283c[];
extern u32 data_0209287c[];

extern MBUserInfo *func_0203d8a0(int idx);
extern void DecompressLZ16(void *src, void *dst);
extern void *LoadFile(int handle);
#include "OAM.h"
#include "nitro/mi/memory.h"

/* Slot 0's nx/ny are data_0209f4a2/data_0209f4a4. The record stride is 0x18. */
extern ControllerInput data_0209f498[];
}
extern u8 NumStars(void);
extern int func_020138dc(void);
int func_020321fc(void);
void func_020326ac(void);
void func_02032f54(void);
void func_02032f9c(int a);
void func_020331fc(int a0, int a1, int a2);
void func_02033390(int sub);
void func_02033464(int arg0, int arg1, int arg2);
void func_020337ec(void);
void func_020338b0(int a, int b, short c, int d);
void func_02033a80(int arg0);
void func_02033bb8(int param0);
void func_02033e50(int p0, int p1);
void Message_DrawLineBG2VS(void);
void func_020345b0(int arg0);
void LoadFont3D(void);

/* The text cursor's current control record (data_0209fd0c). Bytes 3 and 4 are
 * a big-endian argument; 5 and 6 are the number-format mode and base. */
struct MsgTextChar {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 indexHi; /* 3 */
    u8 indexLo; /* 4 */
    u8 mode;    /* 5 */
    u8 base;    /* 6 */
};

inline bool CharDiffers(u16 a, u16 b) {
    return a != b;
}

s32 GetLanguageIndexVS(void) {
    s32 lang = GetOwnerLanguage();
    switch (lang) {
    case 2:
        return 1;
    case 3:
        return 2;
    case 4:
        return 3;
    case 5:
        return 4;
    default:
        return 0;
    }
}

void Message::func_02031028(int arg) {
    MBUserInfo *p;
    int i;
    int j;
    u8 *out;
    int n;
    u16 val;
    p = func_0203d8a0(data_0209fc84);
    i = 0;
    data_0209fca0 = p->nameLength;
    n = data_0209fca0;
    if (n <= 0) {
        return;
    }
    out = data_02092818;
    do {
        val = p->name[i];
        j = 0;
        while (CharDiffers(data_02075b14[j], val) && CharDiffers(data_02075d14[j], val) &&
               j < 256) {
            j++;
        }

        if (j == 256) {
            *out = 0x47;
        } else {
            *out = j;
        }
        if (arg >= 0) {
            if (arg == 0) {
                data_0209fc88 += data_0208f074[*out];
            } else {
                data_0209fc88 += data_0208f174[*out];
            }
            data_0209fcd8 += 1;
        }
        i++;
        out++;
    } while (i < n);
}

void Message::AddChoiceVS() {
    struct S {
        u8 b[8];
    };
    struct S *p = (struct S *)data_0209fd0c;
    u16 v = p->b[4] | (p->b[3] << 8);
    u8 idx;
    switch (v) {
    case 0:
        data_0209fc80 = data_0209fcbc;
        break;
    case 1:
        idx = data_0209fcd0;
        data_0209fcd0 = idx + 1;
        data_0209fcf4[idx] = data_0209fcd8;
        AddCharVS(0x4d);
        data_0209fca4 = 1;
        break;
    case 2:
        idx = data_0209fcd0;
        data_0209fcd0 = idx + 1;
        data_0209fcf4[idx] = data_0209fcbc;
        AddCharVS(0x4d);
        data_0209fca4 = 2;
        break;
    case 3:
        idx = data_0209fcd0;
        data_0209fcd0 = idx + 1;
        data_0209fcf4[idx] = data_0209fcbc;
        AddCharVS(0x4d);
        data_0209fca4 = 3;
        break;
    }
}

void Message::func_0203128c(void) {
    u8 *g;
    int mode;
    int sub;
    int num;
    int seen;
    int i;
    int q;
    int cnt;
    int n2;
    g = data_0209fd0c;
    mode = (u16)(g[4] | (g[3] << 8));
    switch (mode) {
    case 0:
        sub = g[5];
        num = 0;
        seen = num;
        cnt = g[6];
        switch (sub) {
        case 0:
            num = (cnt - NumStars()) & 0xff;
            break;
        case 1:
            num = func_020138dc() & 0xff;
            break;
        }
        for (i = 0; i < 3; i++) {
            q = num / data_02092808[i];
            if (q != 0 || seen != 0 || i == 2) {
                Message::AddCharVS(data_02092824[q]);
                seen = 1;
            }
            num = (num % data_02092808[i]) & 0xff;
        }
        break;
    case 1:
        func_02031028(-1);
        n2 = 0;
        if ((int)data_0209fca0 > 0) {
            u8 *pp = data_02092818;
            do {
                Message::AddCharVS(*pp);
                n2++;
                pp++;
            } while (n2 < data_0209fca0);
        }
        break;
    }
}

void Message::AddCharPairVS() {
    struct MsgTextChar *state = (struct MsgTextChar *)data_0209fd0c;
    u16 idx = state->indexLo | (state->indexHi << 8);
    AddCharVS(data_02092810[idx]);
    AddCharVS((data_02092810[idx] + 1) & 0xff);
}

/* opt_common_subs stays off: the ROM re-reads every mask and row word per use
 * and spills the scaled row indices, a CSE-off recompute no spelling reaches.
 * Re-measured (#821 r17): without the bracket it is 0x23c against 0x2b0, the
 * same with the row indices assigned inside the subscripts. */
#pragma push
/* Kept (#821 r79): rolled k loops over the four words and a u32 *src row pointer are 0x1ec and
   0x22c against 0x2b0. */
#pragma opt_common_subs off
void Message::func_02031428(void) {
    int cnt;
    int i;
    int j;
    u32 *base;

    base = (u32 *)((int)G2_GetBG3CharPtr() + 0x4000);

    for (i = 0; i < (cnt = ((MessageEntry *)data_0209fd00)->lineCount * 2); i++) {
        for (j = 0; j < data_0209fc78; j++) {
            /* scroll the tile up four pixel rows (4 bytes per 4bpp row) */
            MI_CpuCopy32(base + 4, base, 0x10);
            if (i == ((MessageEntry *)data_0209fd00)->lineCount * 2 - 1) {
                base[4] = 0;
                base[5] = 0;
                base[6] = 0;
                base[7] = 0;
            } else {
                MI_CpuCopy32(base + data_0209fc78 * 8, base + 4, 0x10);
            }
            base += 8;
        }
    }

    {
        u32 col;
        int rowIdx;
        u8 rem;
        int count;
        int j;
        int blk;
        int wordIdx;
        u8 *inc;
        u32 *dst;
        int i2;
        int i3;

        dst = (u32 *)((int)G2_GetBG3CharPtr() + ((data_0209fc78 * (cnt - 1) + 0x200) << 5) + 0x10);
        col = data_0209fc7c;
        count = data_0209fcd8;

        j = 0;
        if (count > 0) {
            rowIdx = data_0209fc70;
            inc = data_0209fd20;
            do {
                u32 *q;
                rem = col & 7;
                blk = col >> 3;
                wordIdx = blk << 3;
                q = &dst[wordIdx];
                q[0] |= data_0209287c[rem] & data_0209fd5c[rowIdx + 0];
                q[1] |= data_0209287c[rem] & data_0209fd5c[rowIdx + 1];
                i2 = rowIdx + 2;
                q[2] |= data_0209287c[rem] & data_0209fd5c[i2];
                i3 = rowIdx + 3;
                q[3] |= data_0209287c[rem] & data_0209fd5c[i3];
                if (rem != 0) {
                    /* the next tile's first four rows (8 words on) */
                    q[8] = data_0209283c[rem] & data_0209fd5c[rowIdx + 0];
                    q[9] = data_0209283c[rem] & data_0209fd5c[rowIdx + 1];
                    q[10] = data_0209283c[rem] & data_0209fd5c[i2];
                    q[11] = data_0209283c[rem] & data_0209fd5c[i3];
                }
                col = (col + *inc) & 0xff;
                rowIdx += 16;
                inc++;
                j++;
            } while (j < count);
        }
    }

    data_0209fc70 += 3;
}
#pragma pop

void Message::func_020316d8(int screen, int c, u32 x, void *buf) {
    u32 *chr;
    int pitch;
    if (screen == 1)
        chr = (u32 *)((char *)G2_GetBG2CharPtr() + ((data_0209fcdc + 0x280 + (x >> 3)) << 5));
    else if (screen == 0)
        chr = (u32 *)((char *)G2S_GetBG0CharPtr() + ((data_0209fc78 + 0x200 + (x >> 3)) << 5));
    else
        chr = (u32 *)((char *)G2_GetBG3CharPtr() + ((data_0209fc78 + 0x200 + (x >> 3)) << 5));

    if ((x & 0xf) == 0) {
        MI_CpuCopy32((char *)buf + ((((c & 0xf) << 1) + ((c & 0xf0) << 2)) << 5), chr, 0x40);
        pitch = (data_0209fc88 + 7) / 8;
        MI_CpuCopy32((char *)buf + (((c & 0xf) + (((c & 0xf0) << 1) + 0x10)) << 6),
                     (chr + pitch * 8), 0x40);
    } else {
        int lo;
        int hi;
        u32 *src;
        int m;
        u32 *src2;
        int idx;
        int i;
        short rem;
        short n;
        int stride;
        int k;
        int sh;
        int rsh;
        lo = c & 0xf;
        hi = c & 0xf0;
        src = (u32 *)((char *)buf + (((lo << 1) + (hi << 2)) << 5));
        src2 = src + 8;
        n = (int)data_0208f174[c];
        i = 0;
        stride = ((data_0209fc88 + 7) / 8) << 3;
        m = x & 7;
        sh = m << 2;
        k = 8 - m;
        rsh = k << 2;
        for (; i < 16; i++) {
            idx = (i >> 3) * stride + (i & 7);
            chr[idx] |= *src << sh;
            rem = n;
            rem -= k;
            if (rem > 0) {
                (chr + idx)[8] = (*src >> rsh) | (*src2 << sh);
                rem = rem - 8;
            }
            if (rem > 0)
                *(u32 *)((char *)chr + (idx << 2) + 0x40) = *src2 >> rsh;
            src += 1;
            src2 += 1;
            if (i == 7) {
                src += 0xf8;
                src2 += 0xf8;
            }
        }
    }
}

void Message::func_020318a4(int sl) {
    int base;
    int acc;
    int m;
    int z;
    int i;
    u8 *cmd;
    u8 op;

    acc = data_0209fc7c;
    base = (int)G2_GetBG3CharPtr() + 0x8000;
    m = -1;
    z = 0;

    for (;;) {
        cmd = data_0209fd0c;
        op = cmd[0];
        switch (op) {
        case 0xfd:
            return;
        case 0xfe: {
            int param = cmd[2];
            int val;
            data_0209fd18 = param;
            val = (u16)(cmd[4] | (cmd[3] << 8));
            if (param == 1 && val == 1) {
                func_02031028(m);
                i = z;
                if (i < data_0209fca0) {
                    u8 *p = data_02092818;
                    do {
                        func_020316d8(sl, *p, acc, (void *)base);
                        acc = (acc + data_0208f174[*p]) & 0xff;
                        p++;
                        i++;
                    } while (i < data_0209fca0);
                }
            }
            data_0209fd0c = data_0209fd0c + data_0209fd0c[1];
            break;
        }
        case 0xff:
            return;
        default:
            func_020316d8(sl, op, acc, (void *)base);
            acc = (acc + data_0208f174[op]) & 0xff;
            data_0209fd0c = data_0209fd0c + 1;
            break;
        }
    }
}

void Message::func_020319fc(int n) {
    u32 *dst;
    int i;
    u32 col;
    int j;
    int col0;
    int skip;
    int count;
    int blk;
    int wordIdx;
    u8 rem;

    if (data_0209fc94 != 0)
        dst = (u32 *)((char *)G2S_GetBG0CharPtr() +
                      (((data_0209fcb0 * data_0209fc78 << 1) + 0x200) << 5));
    else
        dst = (u32 *)((char *)G2_GetBG3CharPtr() +
                      (((data_0209fcb0 * data_0209fc78 << 1) + 0x200) << 5));

    skip = (n - 1) << 5;
    col0 = data_0209fc7c;
    count = data_0209fcd8;
    col = col0;

    for (i = 0; i < 16; i++) {
        for (j = 0; j < count; j++) {
            blk = col >> 3;
            wordIdx = blk << 3;
            rem = col & 7;
            dst[wordIdx] = (data_0209287c[rem] & data_0209fd5c[i + j * 16]) | dst[wordIdx];
            if (rem != 0)
                (&dst[wordIdx])[8] =
                    data_0209283c[rem] & *(u32 *)((char *)data_0209fd5c + ((i + j * 16) << 2));
            col = (col + data_0209fd20[j]) & 0xff;
        }
        dst++;
        if (i == 7)
            dst = (u32 *)((char *)dst + skip);
        col = col0;
    }
    data_0209fcb0++;
}

void Message::AddCharVS(u8 c) {
    int i;
    int *ip;
    unsigned int word;
    unsigned int masked;
    int mask;
    int sh;
    sh = data_0209fca8;
    if (sh == 2) {
        ip = (int *)(((char *)data_0209fd10) + (((c & 0x1f) + ((c & 0xe0) << 1)) << 5));
    } else if (sh == 1) {
        ip = (int *)(((char *)G2S_GetBG1CharPtr()) + (((c & 0x1f) + ((c & 0xe0) << 1)) << 5));
    } else {
        ip = (int *)(((char *)G2_GetBG3CharPtr()) + (((c & 0x1f) + ((c & 0xe0) << 1)) << 5));
    }
    mask = data_0209285c[data_0209fcd4];
    sh = data_0209fccc;
    for (i = 0; i < 16; i++) {
        word = *(ip++);
        masked = word;
        masked = masked & mask;
        if (sh != 0) {
            masked = (masked >> ((8 - sh) << 2)) | (masked << (sh << 2));
            data_0209fd5c[data_0209fd1c] = masked;
        } else {
            data_0209fd5c[data_0209fd1c] = masked;
        }
        if (i == 7) {
            ip += 0xf8;
        }
        data_0209fd1c = data_0209fd1c + 1;
    }

    {
        unsigned char width = data_0208f074[c];
        data_0209fccc = (sh + width) & 7;
        data_0209fd20[data_0209fcd8] = width;
        data_0209fc88 = data_0209fc88 + width;
        data_0209fcd8 = data_0209fcd8 + 1;
    }
}

void Message::func_02031cd4(void *arg) {
    int i;
    u8 *base;
    u8 cmd;
    u8 *entry;
    u32 type;
    u16 v;

    data_0209fcd8 = 0;
    data_0209fc88 = 0;
    i = 0;

    for (;;) {
        base = data_0209fd0c;
        cmd = base[i];
        switch (cmd) {
        case 0xff:
            data_0209fcc8 = 8;
            return;
        case 0xfe:
            entry = base + i;
            type = entry[2];
            data_0209fd18 = type;
            v = entry[4] | (entry[3] << 8);
            if (type == 1 && v == 1)
                func_02031028((int)arg);
            entry = data_0209fd0c + i;
            i += entry[1];
            break;
        case 0xfd:
            data_0209fcc8 = 2;
            return;
        default:
            if (arg == 0)
                data_0209fc88 += data_0208f074[cmd];
            else
                data_0209fc88 += data_0208f174[cmd];
            i += 1;
            data_0209fcd8 += 1;
            break;
        }
    }
}

void Message::LayoutLineVS() {
    int flag;
    u8 *p;
    u8 op;
    u8 m;

    data_0209fccc = data_0209fc7c & 7;
    flag = 0;
    if (data_0209fc90 == 0) {
        volatile int zero = 0;
        data_0209fd1c = 0;
        data_0209fcd8 = 0;
        data_0209fc88 = 0;
        MI_CpuFill32(data_0209fd5c, zero, 0xf00);
    }

    for (;;) {
        p = data_0209fd0c;
        op = *p;
        switch (op) {
        case 0xff:
            if (flag != 0) {
                if (data_0209fcc4 == 0) {
                    data_0209fcc8 = 2;
                } else {
                    data_0209fcc8 = 3;
                    data_0209fc70 = 0;
                }
                data_0209fcbc++;
                return;
            } else {
                if (data_0209fcc4 != 0) {
                    u8 *s = (u8 *)data_0209fd00;
                    u8 cc = data_0209fcbc;
                    if (cc < s[6]) {
                        data_0209fcbc = cc + 1;
                        data_0209fc70 = 0;
                        data_0209fcc8 = 3;
                        flag = 0;
                        return;
                    }
                    m = data_0209fca4;
                    if (m == 1) {
                        data_0209fcc8 = 4;
                        data_0209fcd0 = 0;
                    } else if (m == 2) {
                        data_0209fcc8 = 6;
                        data_0209fcd0 = 0;
                    } else if (m == 3) {
                        data_0209fcc8 = 5;
                        data_0209fcd0 = 0;
                    } else {
                        data_0209fcc8 = 8;
                    }
                    return;
                } else {
                    m = data_0209fca4;
                    if (m == 1) {
                        data_0209fcc8 = 4;
                        data_0209fcd0 = 0;
                    } else if (m == 2) {
                        data_0209fcc8 = 6;
                        data_0209fcd0 = 0;
                    } else if (m == 3) {
                        data_0209fcc8 = 5;
                        data_0209fcd0 = 0;
                    } else {
                        data_0209fcc8 = 8;
                    }
                    return;
                }
            }
        case 0xfe: {
            u8 idx = p[2];
            data_0209fd18 = idx;
            (data_02092830 + idx)[-1]();
            p = data_0209fd0c;
            data_0209fd0c = p + p[1];
            break;
        }
        case 0xfd:
            if (data_0209fcc4 != 0) {
                u8 *s = (u8 *)data_0209fd00;
                u8 cc = data_0209fcbc;
                if (cc != s[6]) {
                    data_0209fcbc = cc + 1;
                    data_0209fcc8 = 3;
                    data_0209fc70 = 0;
                    if (data_0209fcbc != s[6])
                        data_0209fd0c = p + 1;
                } else {
                    data_0209fcc8 = 7;
                }
                return;
            } else {
                u8 *s = (u8 *)data_0209fd00;
                u8 cc = data_0209fcbc;
                if (cc != s[6]) {
                    data_0209fcbc = cc + 1;
                    if (data_0209fcbc != s[6])
                        data_0209fd0c = p + 1;
                    data_0209fcc8 = 2;
                } else {
                    data_0209fcc8 = 7;
                }
                return;
            }
        default:
            AddCharVS(op);
            data_0209fd0c++;
            flag = 1;
            break;
        }
    }
}

int Message::func_020321fc(void) {
    if (data_0209fcb8 == 0) {
        if (data_0209fc8c > data_0209fc6c && data_0209fc74 > data_0209fce0) {
            int t;
            s16 v;
            int mask;

            data_0209fcac++;
            t = data_0209fcac * (data_0209fce4 - data_0209fc6c + 1);
            t = data_0209fce4 - t / 8;
            v = t;
            if (v < 0)
                data_0209fc8c = 0;
            else
                data_0209fc8c = t;
            t = data_0209fcac * (data_0209fcc0 - data_0209fce0 + 1);
            data_0209fc74 = data_0209fcc0 - t / 8;
            mask = 0;
            if (data_0209fc8c <= data_0209fc6c) {
                data_0209fc8c = data_0209fc6c;
                mask = 1;
            }
            if (data_0209fc74 <= data_0209fce0) {
                mask |= 2;
                data_0209fc74 = data_0209fce0;
            }
            if (mask == 3)
                data_0209fcc8 = data_0209fcb4;

            if (data_0209fca8 != 0) {
                G2S_SetWnd0Position(data_0209fc8c, data_0209fc74,
                                    data_0209fce4 * 2 - data_0209fc8c + 1,
                                    data_0209fcc0 * 2 - data_0209fc74 + 1);
            } else {
                G2_SetWnd0Position(data_0209fc8c, data_0209fc74,
                                   data_0209fce4 * 2 - data_0209fc8c + 1,
                                   data_0209fcc0 * 2 - data_0209fc74 + 1);
            }
            return 0;
        }
        return 1;
    }

    if (data_0209fc8c < data_0209fce4 && data_0209fc74 < data_0209fcc0) {
        int t;
        int t2;

        data_0209fcac--;
        t = data_0209fcac * (data_0209fce4 - data_0209fc6c + 1);
        t2 = data_0209fcac * (data_0209fcc0 - data_0209fce0 + 1);
        data_0209fc8c = data_0209fce4 - t / 8;
        data_0209fc74 = data_0209fcc0 - t2 / 8;
        data_0209fc8c += 7;
        data_0209fc74 += 5;
        if (data_0209fc8c >= data_0209fce4)
            data_0209fc8c = data_0209fce4;
        if (data_0209fc74 >= data_0209fcc0)
            data_0209fc74 = data_0209fcc0;

        if (data_0209fca8 != 0) {
            G2S_SetWnd0Position(data_0209fc8c, data_0209fc74, data_0209fce4 * 2 - data_0209fc8c + 1,
                                data_0209fcc0 * 2 - data_0209fc74 + 1);
        } else {
            G2_SetWnd0Position(data_0209fc8c, data_0209fc74, data_0209fce4 * 2 - data_0209fc8c + 1,
                               data_0209fcc0 * 2 - data_0209fc74 + 1);
        }
        return 0;
    }

    data_0209fc9c = 0;
    data_0209fcb8 = 0;
    if (data_0209fca8 != 0) {
        gSubScreenLayers &= ~2;
        GXS_SetVisiblePlane(gSubScreenLayers);
        G2S_BlendNone();
        G2S_SetWnd0InsidePlane(GX_WND_PLANEMASK_NONE, 0);
        G2S_SetWndOutsidePlane(GX_WND_PLANEMASK_NONE, 0);
        GXS_SetVisibleWnd(GX_WNDMASK_NONE);
    } else {
        gMainScreenLayers &= ~DISPLAY_LAYER_BG3;
        GX_SetVisiblePlane(gMainScreenLayers);
        G2_BlendNone();
        G2_SetWnd0InsidePlane(GX_WND_PLANEMASK_NONE, 0);
        G2_SetWndOutsidePlane(GX_WND_PLANEMASK_NONE, 0);
        GX_SetVisibleWnd(GX_WNDMASK_NONE);
    }
    return 0;
}

void Message::func_020326ac(void) {
    switch (data_0209fcc8) {
    case 0:
    case 8:
        break;
    case 1: {
        data_0209fc7c = 0;
        Message::LayoutLineVS();
        return;
    }
    case 2: {
        func_020319fc(data_0209fc78);
        data_0209fcc8 = 1;
        return;
    }
    case 3: {
        data_0209fc7c = 0;
        func_02031428();
        data_0209fc70 += 1;
        if (data_0209fc70 != 16) {
            return;
        }
        data_0209fcc8 = 1;
        data_0209fc7c = 0;
        Message::LayoutLineVS();
        return;
    }
    case 4: {
        s16 t;
        if (IsButtonInputValid() != 0) {
            data_0209fc98 = data_0209fcd0 + 1;
            data_0209fcb8 = 1;
            return;
        }
        t = data_0209f498[gActivePlayerSlot].nx;
        if (t > 0x80) {
            data_0209fcd0 = 1;
        } else if (t < -0x80) {
            data_0209fcd0 = 0;
        } else if (data_0209caa0.file.controllerMode == 0 &&
                   (data_020a0e5a[gActivePlayerSlot * 2] & 0x30)) {
            data_0209fcd0 ^= 1;
        }
        {
            int r5 = data_0209fcf4[data_0209fcd0] * 9 + data_0209fc6c + 12;
            int r4 = data_0209fce0 + (data_0209fc80 * 16) + 14;
            if (data_020a0db0 & (0x10 / data_0208ee44)) {
                OAM::Render(0, &data_ov002_0210c3a8, r5, r4, -1, -1, 0x1000, 0x1000, 0, -1);
            } else {
                OAM::Render(0, &data_ov002_0210c3a0, r5, r4, -1, -1, 0x1000, 0x1000, 0, -1);
            }
            return;
        }
    }
    case 5: {
        s16 t;
        if (IsButtonInputValid() != 0) {
            data_0209fc98 = data_0209fcd0 + 1;
            data_0209fcb8 = 1;
            return;
        }
        t = data_0209f498[gActivePlayerSlot].ny;
        if (t > 0x80) {
            data_0209fcd0 = 1;
        } else if (t < -0x80) {
            data_0209fcd0 = 0;
        } else if (data_0209caa0.file.controllerMode == 0 &&
                   (data_020a0e5a[gActivePlayerSlot * 2] & 0xC0)) {
            data_0209fcd0 ^= 1;
        }
        {
            int r5 = data_0209fc6c + 12;
            int r4 = data_0209fce0 + (data_0209fcf4[data_0209fcd0] * 16) + 14;
            if (data_020a0db0 & (0x10 / data_0208ee44)) {
                OAM::Render(0, &data_ov002_0210c3a8, r5, r4, -1, -1, 0x1000, 0x1000, 0, -1);
            } else {
                OAM::Render(0, &data_ov002_0210c3a0, r5, r4, -1, -1, 0x1000, 0x1000, 0, -1);
            }
            return;
        }
    }
    case 6: {
        int hasf1;
        int idx;
        u8 f0;
        if (IsButtonInputValid() != 0) {
            data_0209fc98 = data_0209fcd0 + 1;
            data_0209fcb8 = 1;
            return;
        }
        idx = gActivePlayerSlot;
        hasf1 = 0;
        f0 = gTouchInfo[idx].held;
        if (f0 != 0) {
            if (gTouchInfo[idx].edge != 0) {
                hasf1 = 1;
            }
        }
        if (hasf1 != 0) {
            data_0209fcec = gTouchInfo[idx].y;
            data_0209fcec -= (s16)(data_0209fcd0 * 8);
        } else if (f0 != 0) {
            s16 debval = gTouchInfo[idx].y;
            data_0209fcf0 = debval - data_0209fcec;
            if (data_0209fcf0 > 16) {
                data_0209fcf0 = 16;
                data_0209fcec = debval - 16;
            } else if (data_0209fcf0 < 0) {
                data_0209fcf0 = 0;
                data_0209fcec = debval;
            }
            switch (data_0209fcd0) {
            case 0:
                if (data_0209fcf0 > 0 && data_0209fcf0 >= 8) {
                    data_0209fcd0 += 1;
                }
                break;
            case 1:
                if (data_0209fcf0 > 0) {
                    if (data_0209fcf0 >= 16) {
                        data_0209fcd0 += 1;
                    } else if (data_0209fcf0 <= 6) {
                        data_0209fcd0 -= 1;
                    }
                }
                break;
            case 2:
                if (data_0209fcf0 > 0 && data_0209fcf0 <= 14) {
                    data_0209fcd0 -= 1;
                }
                break;
            }
        } else if (data_0209caa0.file.controllerMode == 0) {
            u16 mask = data_020a0e5a[idx * 2];
            if (mask & 0xC0) {
                u8 target = (mask & 0x40) ? 0 : 2;
                if (data_0209fcd0 != target) {
                    if (target != 0) {
                        data_0209fcd0 += 1;
                    } else {
                        data_0209fcd0 -= 1;
                    }
                }
            }
        }
        {
            int r5 = data_0209fc6c + 12;
            int r4 = data_0209fce0 + (data_0209fcf4[data_0209fcd0] * 16) + 14;
            if (data_020a0db0 & (0x10 / data_0208ee44)) {
                OAM::Render(0, &data_ov002_0210c3a8, r5, r4, -1, -1, 0x1000, 0x1000, 0, -1);
            } else {
                OAM::Render(0, &data_ov002_0210c3a0, r5, r4, -1, -1, 0x1000, 0x1000, 0, -1);
            }
            return;
        }
    }
    case 7: {
        if ((data_0209caa0.file.flags2 & FILE_FLAGS2_INTRO_DONE) && IsButtonInputValid() != 0) {
            data_0209fd0c += 1;
            data_0209fcc4 = 1;
            data_0209fcbc = 0;
            data_0209fcc8 = 1;
        }
        if (!(data_0209caa0.file.flags2 & FILE_FLAGS2_INTRO_DONE)) {
            return;
        }
        {
            int r5 = ((data_0209fce4 * 2) - 6) - data_0209fc6c;
            int r4 = ((data_0209fcc0 * 2) + 2) - data_0209fce0;
            if (!(data_020a0db0 & (0x10 / data_0208ee44))) {
                return;
            }
            if (data_0209fcd4 != 0) {
                OAM::Render(0, &data_ov002_0210c390, r5, r4, -1, -1, 0x1000, 0x1000, 0, -1);
            } else {
                OAM::Render(0, &data_ov002_0210c398, r5, r4, -1, -1, 0x1000, 0x1000, 0, -1);
            }
            return;
        }
    }
    }
}

/* Splits the VS-mode text blob at 0x0209fd14 into the four pointers the renderer
 * reads. The blob's own header carries the offset from its second section to the
 * third, so only three of the four are fixed strides:
 *
 *     +0x00  header      -> data_0209fd04
 *     +0x20  info section -> data_0209fcf8; its sectionSize is the distance
 *                            to the data section
 *     info + sectionSize -> data_0209fcfc
 *     +0x30              -> data_0209fd08
 *
 * The intermediate infoSection/dataSection locals are kept because the
 * sectionSize load has to happen after infoSection is formed and before
 * dataSection is stored; collapsing them into one expression reorders the
 * loads. Same shape as Message::SetTextGlobals.
 */
void Message::SetTextGlobalsVS() {
    int bankBase = data_0209fd14;
    int infoSection = bankBase + 0x20;
    data_0209fd04 = bankBase;
    data_0209fcf8 = (int *)infoSection;
    int dataSection = infoSection + ((MessageInfoSection *)infoSection)->sectionSize;
    data_0209fcfc = dataSection;
    data_0209fd08 = bankBase + 0x30;
}

void Message::func_02032f54(void) {
    if (data_0209fc9c == 0)
        return;
    if (func_020321fc() == 0)
        return;
    func_020326ac();
}

void Message::func_02032f9c(int a) {
    volatile int li;
    volatile unsigned short ls;
    int idx;
    int p, s;
    int v;
    int i;
    u16 *q;
    int v2;
    int w, n;
    u8 c;
    u8 old94;

    data_0209fce8 = 0x20;
    data_0209fc9c = 0;
    func_02034504();

    idx = data_0209fce8;
    v = data_0209fd14;
    data_0209fc94 = 1;
    data_0209fc78 = 0;
    data_0209fd00 = (int)((char *)data_0209fd08 + idx * 8);
    data_0209fd0c = (u8 *)(v + (sizeof(MessageBankHeader) + sizeof(MessageDataSection)) +
                           ((MessageInfoSection *)data_0209fcf8)->sectionSize +
                           ((MessageEntry *)((char *)data_0209fd08 + idx * 8))->textOffset);

    p = (int)G2_GetBG3CharPtr() + 0x4000;
    li = 0;
    MI_CpuFill32((void *)p, li, 0x2000);

    s = (int)G2_GetBG3ScrPtr();
    ls = 0x2ff;
    MI_CpuFill16((void *)s, ls, 0x800);

    if (a < 0) {
        return;
    }

    data_0209fc84 = a;
    func_02031cd4(0);

    w = 256 - data_0209fc88;
    data_0209fc7c = ((w >> 1) & 7);
    q = (u16 *)((char *)G2_GetBG3ScrPtr() + 0x40) + (u16)(w / 8 >> 1);
    c = data_0209fc7c;
    n = (data_0209fc88 + c + 7) >> 3;
    i = 0;
    if (n > 0) {
        v2 = n + 0x200;
        do {
            q[i] = (i + 0x200);
            (q + i)[0x20] = v2;
            v2++;
            i++;
        } while (i < n);
    }

    data_0209fc90 = 1;
    data_0209fcd4 = 4;
    data_0209fccc = c;
    data_0209fcd8 = 0;
    data_0209fc88 = 0;
    old94 = data_0209fc94;
    data_0209fc94 = 0;
    Message::LayoutLineVS();

    data_0209fc88 = data_0209fc88 + data_0209fc7c;
    func_020319fc((u8)((data_0209fc88 + 7) >> 3));

    data_0209fc94 = old94;
    data_0209fcd4 = 0;
    data_0209fcdc = (((data_0209fc88 + 7) >> 3) << 1);
}

void Message::func_020331fc(int a0, int a1, int a2) {
    volatile int li;
    short *scr;
    int base;
    int cnt;
    int acc;
    int i;
    int old;
    unsigned char *p;

    base = (int)G2_GetBG3CharPtr() + 0x8000;
    data_0209fc78 = 0;
    data_0209fc88 = 0;
    data_0209fc84 = a2;

    {
        int cp = (u32)G2S_GetBG0CharPtr() + 0x4000;
        li = 0;
        MI_CpuFill32((void *)(cp + (data_0209fc78 << 5)), li, 0x800);
    }
    scr = (short *)((u32)G2S_GetBG0ScrPtr() + a0 * 2);
    func_02031028(1);

    old = data_0209fc88;
    data_0209fc7c = (((a1 << 3) - old) >> 1) & 7;
    acc = data_0209fc7c;
    cnt = (unsigned char)((a1 - (old + 7) / 8) >> 1);
    data_0209fc88 = old + acc;
    i = 0;
    if (i < data_0209fca0) {
        p = data_02092818;
        do {
            func_020316d8(0, *p, acc, (void *)base);
            acc = (acc + data_0208f174[*p]) & 0xff;
            p++;
            i++;
        } while (i < data_0209fca0);
    }

    scr += cnt;
    {
        int j = 0;
        int v2;
        int n = (data_0209fc88 + 7) / 8;
        if (n > 0) {
            v2 = n + 0x200;
            do {
                scr[j] = j + 0x200;
                (scr + j)[0x20] = v2;
                v2++;
                j++;
            } while (j < n);
        }
    }
}

void Message::func_02033390(int sub) {
    data_0209fc9c = 0;
    func_02034504();
    data_0209fc94 = 1;
    data_0209fc78 = 0;

    if (sub != 0) {
        volatile u16 fill;
        volatile s32 v;
        int *dst = (int *)((int)G2_GetBG3CharPtr() + 0x4000);
        v = 0;
        MI_CpuFill32(dst, v, 0x2000);
        MI_CpuFill16(G2_GetBG3ScrPtr(), fill = 0x2ff, 0x800);
    } else {
        volatile u16 fill;
        volatile s32 v;
        int *dst = (int *)((int)G2S_GetBG0CharPtr() + 0x4000);
        v = 0;
        MI_CpuFill32(dst, v, 0x2000);
        MI_CpuFill16(G2S_GetBG0ScrPtr(), fill = 0x2ff, 0x800);
    }
}

/* byte-forced: this function reloads data_0209fc78 at every use;
   decl_common.h spells it plain. The macro expands each use to a deref
   through a cast of its address -- the inner name paints blue and resolves to
   the extern. Measured (#821 r68): without it the function is 0x380 of 0x388. */
/* Kept (#821 r76): a reload pin; no block-copy or type lever (r72-r75) applies to this scalar pin;
   r68: 0x380 of 0x388 without it.
   Kept (#821 r80): getter tried -> a u8 or int accessor on the three reads is 0x380, as plain. */
#define data_0209fc78 (*(u8 *)&data_0209fc78)
void Message::func_02033464(int arg0, int arg1, int arg2) {
    volatile unsigned short sv0;
    volatile unsigned short sv2;
    volatile int li4;
    volatile int li8;
    volatile int liC;
    u32 t;
    char *d;
    u16 *base;
    u32 x;
    int i;
    int n;
    u8 *p;
    int v1;

    li4 = 0;
    data_0209fd1c = 0;
    data_0209fcb0 = 1;
    data_0209fc9c = 0;
    data_0209fc88 = 0;
    data_0209fcd8 = 0;
    data_0209fc84 = arg1;

    MI_CpuFill32(data_0209fd5c, li4, 0xf00);

    if (arg2 == 0) {
        x = (arg0 * 2 + 8) << 0x15;
        data_0209fc94 = 1;
        t = x >> 16;
        d = (char *)((u32)G2S_GetBG0ScrPtr() + (((x >> 16) & 0xffe0) << 1));
        sv0 = 0x2ff;
        MI_CpuFill16(d, sv0, 0x80);
        if (arg1 < 0)
            return;
        func_02031028(0);
        data_0209fc7c = ((256 - data_0209fc88) >> 1);
        n = data_0209fca0;
        data_0209fccc = data_0209fc7c & 7;
        data_0209fc88 = 0;
        data_0209fcd8 = 0;
        i = 0;
        if (n > 0) {
            p = data_02092818;
            do {
                Message::AddCharVS(*p);
                i++;
                p++;
            } while (i < data_0209fca0);
        }
        data_0209fc78 = (arg0 * 0x18 + data_0209fcdc);
        {
            int cp = (u32)G2S_GetBG0CharPtr() + 0x4000;
            int *dst = (int *)(cp + (data_0209fc78 << 5));
            li8 = 0;
            MI_CpuFill32(dst, li8, 0x300);
        }
        base = (u16 *)((u32)G2S_GetBG0ScrPtr() + ((t + (data_0209fc7c >> 3)) << 1));
        {
            u32 ch = data_0209fc78;
            x = ch + 0x200;
            v1 = ch + 0x20c;
        }
        i = 0;
        do {
            base[i] = x;
            (base + i)[0x20] = v1;
            i++;
            x++;
            v1++;
        } while (i < 12);
        data_0209fc7c &= 7;
        data_0209fc78 = (data_0209fc78 >> 1);
        data_0209fcb0 = 1;
        func_020319fc(12);
    } else {
        data_0209fc94 = 0;
        d = (char *)G2_GetBG3ScrPtr() + (arg0 << 1);
        sv2 = 0x2ff;
        MI_CpuFill16(d, sv2, 0x80);
        func_02031028(0);
        n = data_0209fca0;
        data_0209fc7c = 4;
        data_0209fccc = 4;
        data_0209fc88 = 0;
        data_0209fcd8 = 0;
        i = 0;
        if (n > 0) {
            p = data_02092818;
            do {
                Message::AddCharVS(*p);
                i++;
                p++;
            } while (i < data_0209fca0);
        }
        data_0209fc78 = (arg1 * 0x18 + data_0209fcdc);
        {
            int cp = (int)G2_GetBG3CharPtr() + 0x4000;
            int *dst = (int *)(cp + (data_0209fc78 << 5));
            liC = 0;
            MI_CpuFill32(dst, liC, 0x300);
        }
        base = (u16 *)((char *)G2_GetBG3ScrPtr() + ((arg0 + (data_0209fc7c >> 3)) << 1));
        {
            u32 ch = data_0209fc78;
            x = ch + 0x200;
            v1 = ch + 0x20c;
        }
        i = 0;
        do {
            base[i] = x;
            (base + i)[0x20] = v1;
            i++;
            x++;
            v1++;
        } while (i < 12);
        data_0209fc7c &= 7;
        data_0209fc78 = (data_0209fc78 >> 1);
        data_0209fcb0 = 1;
        func_020319fc(12);
    }
}
#undef data_0209fc78

void Message::func_020337ec(void) {
    volatile unsigned short v;
    volatile int z;
    data_0209fc9c = 0;
    func_02034504();
    data_0209fc94 = 1;
    data_0209fc78 = 0;
    {
        int *dst = (int *)((u32)G2S_GetBG0CharPtr() + 0x4000);
        z = 0;
        MI_CpuFill32(dst, z, 0x2000);
    }
    {
        char *dst = (char *)G2S_GetBG0ScrPtr();
        v = 0x2ff;
        MI_CpuFill16(dst, v, 0x800);
    }
    data_0209fce8 = 0x18;
    Message_DrawCenteredLineVS(0x280, 0x20);
    data_0209fcdc = data_0209fc78;
    func_02033464(0, 0, 0);
}

void Message::func_020338b0(int a, int b, short c, int d) {
    int p, s;
    volatile int li0, li1;
    volatile unsigned short ls0, ls1;
    data_0209fce8 = c;
    data_0209fc9c = 0;
    func_02034504();
    data_0209fc94 = 1;
    data_0209fc78 = 0;
    if (d >= 0)
        data_0209fc84 = d;
    switch (data_0209fce8) {
    case 0x13:
    case 0x15:
        p = (u32)G2S_GetBG0CharPtr() + 0x4000;
        li0 = 0;
        MI_CpuFill32((void *)p, li0, 0x3000);
        s = (u32)G2S_GetBG0ScrPtr();
        ls0 = 0x37f;
        MI_CpuFill16((void *)s, ls0, 0x800);
        break;
    default:
        p = (u32)G2S_GetBG0CharPtr() + 0x4000;
        li1 = 0;
        MI_CpuFill32((void *)p, li1, 0x2000);
        s = (u32)G2S_GetBG0ScrPtr();
        ls1 = 0x2ff;
        MI_CpuFill16((void *)s, ls1, 0x800);
        break;
    }
    func_02033e50(a, b);
}

void Message::DisplayVsExitText(unsigned short n) {
    volatile int li;
    volatile unsigned short ls;
    int p, s;
    data_0209fce8 = n;
    data_0209fc9c = 0;
    func_02034504();
    data_0209fc94 = 1;
    data_0209fc78 = 0;
    data_0209fcdc = 0;
    p = (u32)G2S_GetBG0CharPtr() + 0x4000;
    li = 0;
    MI_CpuFill32((void *)p, li, 0x2000);
    s = (u32)G2S_GetBG0ScrPtr();
    ls = 0x2ff;
    MI_CpuFill16((void *)s, ls, 0x800);
    Message_DrawCenteredLineVS(0x100, 0x20);
    data_0209fce8 = 0x22;
    Message_DrawCenteredLineVS(0x1c0, 0x20);
}

void Message::func_02033a80(int msgID) {
    data_0209fce8 = msgID;
    data_0209fc9c = 0;
    func_02034504();
    data_0209fc94 = 1;
    data_0209fc78 = 0;
    func_02033e50(0xcf, 0x68);
}

/* The VS twin of Message::DisplayOptionsMenuText: the same two option lines and
   centred footer, drawn from the VS text tables. */
void Message::DisplayOptionsMenuTextVS(s16 arg) {
    volatile int li;
    volatile unsigned short ls;
    int p, s;
    p = (u32)G2S_GetBG0CharPtr() + 0x4000;
    li = 0;
    MI_CpuFill32((void *)p, li, 0x2000);
    s = (u32)G2S_GetBG0ScrPtr();
    ls = 0x2ff;
    MI_CpuFill16((void *)s, ls, 0x800);
    func_02033a80(arg);
    data_0209fce8 = 0xc;
    data_0209fcb0 = 1;
    func_02033e50(0x18f, 0x28);
    data_0209fce8 = data_0209fce8 + 1;
    func_02033e50(0x197, 0x28);
    data_0209fc78 = data_0209fc78 << 1;
    data_0209fce8 = 8;
    Message_DrawCenteredLineVS(0x280, 0x20);
}

/* n holds the line's character count, a u8 like data_0209fc88 it is read from. */
void Message::func_02033bb8(int param0) {
    volatile int li;
    volatile unsigned short sv;
    u8 n;
    int m, i;
    short *scr;

    data_0209fce8 = param0;
    data_0209fc9c = 0;
    func_02034504();

    data_0209fc94 = 1;
    data_0209fc78 = 0;
    data_0209fcdc = 0;

    {
        int *chr = (int *)((char *)G2S_GetBG0CharPtr() + 0x4000);
        li = 0;
        MI_CpuFill32(chr, li, 0x2000);
    }

    {
        char *scr0 = (char *)G2S_GetBG0ScrPtr();
        sv = 0x2ff;
        MI_CpuFill16(scr0, sv, 0x800);
    }

    Message_DrawLineBG2VS();
    n = data_0209fc88;
    m = (n + 7) / 8;
    scr = (short *)((char *)G2_GetBG2ScrPtr() + 0x40 + (16 - m / 2) * 2);
    i = 0;
    if (m > 0) {
        int v2 = m + 0x280;
        do {
            scr[i] = (i + 0x280);
            (scr + i)[0x20] = v2;
            v2++;
            i++;
        } while (i < m);
    }
    func_02033e50(0xa9, 0x70);

    data_0209fce8++;
    Message_DrawLineBG2VS();
    n = data_0209fc88;
    m = (n + 7) / 8;
    scr = (short *)((char *)G2_GetBG2ScrPtr() + 0x840 + (16 - m / 2) * 2);
    i = 0;
    if (m > 0) {
        int mm = (n + 7) / 8;
        int d = data_0209fcdc;
        int v1 = d - mm * 2 + 0x280;
        int v2 = d - mm + 0x280;
        do {
            scr[i] = v1;
            (scr + i)[0x20] = v2;
            v1++;
            v2++;
            i++;
        } while (i < mm);
    }
    func_02033e50(0x149, 0x70);

    data_0209fce8++;
    Message_DrawLineBG2VS();
    n = data_0209fc88;
    m = (n + 7) / 8;
    scr = (short *)((char *)G2_GetBG2ScrPtr() + 0x1040 + (16 - m / 2) * 2);
    i = 0;
    if (m > 0) {
        int mm = (n + 7) / 8;
        int d = data_0209fcdc;
        int v1 = d - mm * 2 + 0x280;
        int v2 = d - mm + 0x280;
        do {
            scr[i] = v1;
            (scr + i)[0x20] = v2;
            v1++;
            v2++;
            i++;
        } while (i < mm);
    }
    func_02033e50(0x1e9, 0x70);

    data_0209fc78 = (data_0209fc78 << 1);
    data_0209fce8++;
    Message_DrawCenteredLineVS(0x280, 0x20);
}

void Message::func_02033e50(int p0, int p1) {
    int i;
    int idx = data_0209fce8;
    char *base = (char *)data_0209fd08;
    int n = (p1 >> 3) & 0xff;

    data_0209fd00 = (int)(base + idx * 8);
    data_0209fd0c =
        (u8 *)(data_0209fd14 + (sizeof(MessageBankHeader) + sizeof(MessageDataSection)) +
               ((MessageInfoSection *)data_0209fcf8)->sectionSize +
               ((MessageEntry *)(base + idx * 8))->textOffset);
    data_0209fcc8 = 0;

    do {
        func_02031cd4(0);
        idx = data_0209fce8;
        if (idx == 0x13 || idx == 0x15) {
            volatile int li;
            data_0209fc7c = 0xc;
            n = ((data_0209fc88 + 0x13) >> 3) & 0xff;
            {
                int cp = (u32)G2S_GetBG0CharPtr() + 0x4000;
                li = 0;
                MI_CpuFill32((void *)(cp + (data_0209fc78 << 6)), li, n << 6);
            }
            {
                short *scr = (short *)((u32)G2S_GetBG0ScrPtr() + p0 * 2);
                i = 0;
                if (n > 0) {
                    int v1 = (data_0209fc78 << 1) + 0x200;
                    int v2 = n + v1;
                    do {
                        scr[i] = v1;
                        (scr + i)[0x20] = v2;
                        v1++;
                        v2++;
                        i++;
                    } while (i < n);
                }
            }
            Message::LayoutLineVS();
            func_020319fc(n);
            data_0209fcb0 = 1;
            data_0209fc78 = data_0209fc78 + n;
            p0 = (unsigned short)(p0 + 0x40);
        } else {
            volatile int li;
            data_0209fc7c = ((p1 - data_0209fc88) >> 1);
            {
                int cp = (u32)G2S_GetBG0CharPtr() + 0x4000;
                li = 0;
                MI_CpuFill32((void *)(cp + (data_0209fc78 << 6)), li, n << 6);
            }
            {
                int v1, v2;
                short *scr = (short *)((u32)G2S_GetBG0ScrPtr() + p0 * 2);
                i = 0;
                if (n > 0) {
                    v1 = (data_0209fc78 << 1) + 0x200;
                    v2 = n + v1;
                    do {
                        scr[i] = v1;
                        (scr + i)[0x20] = v2;
                        v1++;
                        v2++;
                        i++;
                    } while (i < n);
                }
            }
            Message::LayoutLineVS();
            func_020319fc(n);
            data_0209fcb0 = 1;
            data_0209fc78 = data_0209fc78 + n;
            p0 = (unsigned short)(p0 + 0x40);
        }
    } while (data_0209fcc8 != 8);
    data_0209fcc8 = 0;
}

void Message_DrawLineBG2VS(void) {
    volatile int li;
    int n;
    int idx = data_0209fce8;
    char *base = (char *)data_0209fd08;
    data_0209fd00 = (int)(base + idx * 8);
    data_0209fd0c =
        (u8 *)(data_0209fd14 + (sizeof(MessageBankHeader) + sizeof(MessageDataSection)) +
               ((MessageInfoSection *)data_0209fcf8)->sectionSize +
               ((MessageEntry *)(base + idx * 8))->textOffset);
    Message::func_02031cd4((void *)1);
    data_0209fc7c = 0;
    n = ((data_0209fc88 + 7) / 8) << 6;
    {
        int p = (int)G2_GetBG2CharPtr() + ((data_0209fcdc + 0x280) << 5);
        li = 0;
        MI_CpuFill32((void *)p, li, n);
    }
    Message::func_020318a4(1);
    data_0209fcdc = data_0209fcdc + (u8)(((data_0209fc88 + 7) / 8) << 1);
}

extern "C" void Message_DrawCenteredLineVS(int mapPos, int width) {
    volatile int li;
    int n, i, oldc;
    short *scr;
    int idx = data_0209fce8;
    char *base = (char *)data_0209fd08;
    data_0209fd00 = (int)(base + idx * 8);
    data_0209fd0c =
        (u8 *)(data_0209fd14 + (sizeof(MessageBankHeader) + sizeof(MessageDataSection)) +
               ((MessageInfoSection *)data_0209fcf8)->sectionSize +
               ((MessageEntry *)(base + idx * 8))->textOffset);
    Message::func_02031cd4((void *)1);
    oldc = data_0209fc88;
    data_0209fc7c = (((width << 3) - oldc) & 0xf) >> 1;
    data_0209fc88 = oldc + data_0209fc7c;
    n = ((data_0209fc88 + 7) / 8) << 6;
    {
        int p = (u32)G2S_GetBG0CharPtr() + 0x4000 + (data_0209fc78 << 5);
        li = 0;
        MI_CpuFill32((void *)p, li, n);
    }
    {
        int mapAddr = (u32)G2S_GetBG0ScrPtr() + mapPos * 2;
        scr = (short *)(mapAddr + ((unsigned short)((width - ((oldc + 7) / 8)) >> 1)) * 2);
    }
    for (i = 0; i < (data_0209fc88 + 7) / 8; i++) {
        scr[i] = data_0209fc78 + 0x200 + i;
        (&scr[i])[32] = data_0209fc78 + 0x200 + (data_0209fc88 + 7) / 8 + i;
    }
    Message::func_020318a4(0);
    data_0209fc78 = data_0209fc78 + (u8)(((data_0209fc88 + 7) / 8) << 1);
}

void Message::DisplayPauseTextVS(unsigned short n) {
    volatile int li;
    volatile unsigned short ls;
    int p, s;
    data_0209fce8 = n;
    data_0209fc78 = 0;
    data_0209fc94 = 1;
    p = (u32)G2S_GetBG0CharPtr() + 0x4000;
    li = 0;
    MI_CpuFill32((void *)p, li, 0x2000);
    s = (u32)G2S_GetBG0ScrPtr();
    ls = 0x2ff;
    MI_CpuFill16((void *)s, ls, 0x800);
    Message_DrawCenteredLineVS(0xc0, 0x20);
    data_0209fce8 = data_0209fce8 + 1;
    Message_DrawCenteredLineVS(0x160, 0x20);
    data_0209fce8 = 0x22;
    Message_DrawCenteredLineVS(0x200, 0x20);
}

/* The VS twin of Message::DisplayDontSaveText: message `n` in the box, then the
   two centred answer lines. */
void Message::DisplayDontSaveTextVS(unsigned short n) {
    volatile int li;
    volatile unsigned short ls;
    int p, s;
    data_0209fc9c = 0;
    func_02034504();
    data_0209fc94 = 1;
    data_0209fc78 = 0;
    p = (u32)G2S_GetBG0CharPtr() + 0x4000;
    li = 0;
    MI_CpuFill32((void *)p, li, 0x2000);
    s = (u32)G2S_GetBG0ScrPtr();
    ls = 0x2ff;
    MI_CpuFill16((void *)s, ls, 0x800);
    data_0209fce8 = n;
    func_02033e50(0x105, 0xb0);
    data_0209fce8 = 0xf;
    data_0209fc78 = data_0209fc78 << 1;
    Message_DrawCenteredLineVS(0x285, 0xa);
    data_0209fce8 = data_0209fce8 + 1;
    Message_DrawCenteredLineVS(0x291, 0xa);
}

void Message::func_02034504(void) {
    data_0209fcb0 = 0;
    data_0209fcbc = 0;
    data_0209fcc4 = 0 * 0;
    data_0209fcc8 = 1;
    data_0209fca4 = 0;
    data_0209fcd0 = 0;
    data_0209fc98 = 0;
    data_0209fc90 = 0;
    data_0209fcd4 = 0;
    data_0209fc94 = 0;
    data_0209fc78 = 0;
    data_0209fc7c = 0;
}

void Message::func_020345b0(int msgID) {
    u16 *scr;
    s32 i;
    s32 cols;
    s32 last;
    u8 t;
    volatile s32 li;
    volatile u16 ls;

    if (((MessageInfoSection *)data_0209fcf8)->messageCount <= (u16)msgID) {
        return;
    }

    data_0209fce8 = msgID;
    func_02034504();

    data_0209fc9c = 1;
    data_0209fc8c = 0x7f;
    data_0209fc74 = 0x87;
    data_0209fcd4 = 0;
    data_0209fd00 = (int)((char *)data_0209fd08 + data_0209fce8 * 8);
    data_0209fd0c =
        (u8 *)(data_0209fd14 + (sizeof(MessageBankHeader) + sizeof(MessageDataSection)) +
               ((MessageInfoSection *)data_0209fcf8)->sectionSize +
               ((MessageEntry *)((char *)data_0209fd08 + data_0209fce8 * 8))->textOffset);
    data_0209fc78 = ((((MessageEntry *)data_0209fd00)->width + 7) / 8);
    data_0209fce4 = 0x7f;
    data_0209fc6c = (((0x20 - (data_0209fc78 + 2)) / 2) * 8);
    data_0209fcc0 = 0x63;
    data_0209fce0 = (0x64 - (((((MessageEntry *)data_0209fd00)->lineCount * 2 + 1) * 8) / 2));
    data_0209fc8c = 0x7f;
    data_0209fc74 = 0x63;
    data_0209fcac = 0;
    data_0209fc7c = 0;
    gNextActorUpdateMask |= 0x800000;

    {
        void *p = (char *)G2_GetBG3CharPtr() + 0x4000;
        li = 0;
        MI_CpuFill32(p, li, 0x3000);
    }

    {
        void *s = G2_GetBG3ScrPtr();
        ls = 0x37f;
        MI_CpuFill16(s, ls, 0x800);
    }

    scr = (u16 *)G2_GetBG3ScrPtr() + 0x21;
    SetBg3Offset(-data_0209fc6c, 4 - data_0209fce0);

    i = 0;
    cols = data_0209fc78;
    if ((cols * (((MessageEntry *)data_0209fd00)->lineCount) * 2) > 0) {
        last = cols - 1;
        do {
            s32 rem = i % cols;
            scr[rem] = (i + 0x200);
            if (last == rem) {
                scr += 0x20;
            }
            i++;
        } while (i < (cols * (((MessageEntry *)data_0209fd00)->lineCount) * 2));
    }

    t = data_0209fcc8;
    if (t < 4) {
        do {
            func_020326ac();
            t = data_0209fcc8;
        } while (t < 4);
    }
    data_0209fcb4 = t;

    G2_SetBlendBrightness((gMainScreenLayers ^ 0x10) | 0x20, -8);

    G2_SetWnd0InsidePlane(gMainScreenLayers | GX_WND_PLANEMASK_BG3, 1);
    G2_SetWndOutsidePlane(GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 |
                              GX_WND_PLANEMASK_OBJ,
                          0);
    G2_SetWnd0Position(data_0209fc8c, data_0209fc74, data_0209fce4 * 2 - data_0209fc8c + 1,
                       data_0209fcc0 * 2 - data_0209fc74 + 1);

    data_0209fcc8 = 0;
    GX_SetVisibleWnd(GX_WNDMASK_W0);
    gMainScreenLayers |= DISPLAY_LAYER_BG3;
    GX_SetVisiblePlane(gMainScreenLayers);
}

void LoadFont3D(void) {
    void *r5;
    void *r4;

    r5 = LoadFile(0x980e);
    MI_CpuCopy32(r5, G2_GetBG3CharPtr(), 0x4000);

    r4 = LoadFile(0x980d);
    DecompressLZ16(r4, (u8 *)G2_GetBG3CharPtr() + 0x8000);

    Deallocate(r4);
    Deallocate(r5);

    Message::LoadTextVS();

    data_0209fc9c = 0;
    data_0209fce8 = -1;
}

/* Points the VS-mode text blob at the table for the console's language, then hands
 * to SetTextGlobalsVS to split it into the four pointers the renderer reads.
 * The order is a descending chain rather than a switch, and the final `else` is the
 * default rather than a sixth language -- languages 5..2 have their own tables and
 * everything else falls back to 0x02092d3c. */
void Message::LoadTextVS() {
    if (GetOwnerLanguage() == 5) {
        data_0209fd14 = (int)&data_020937bc;
    } else if (GetOwnerLanguage() == 4) {
        data_0209fd14 = (int)&data_0209325c;
    } else if (GetOwnerLanguage() == 3) {
        data_0209fd14 = (int)&data_02093d7c;
    } else if (GetOwnerLanguage() == 2) {
        data_0209fd14 = (int)&data_0209289c;
    } else {
        data_0209fd14 = (int)&data_02092d3c;
    }
    Message::SetTextGlobalsVS();
}
