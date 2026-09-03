/* THE OPENING'S SCRIPT ARGUMENT RECORDS, hosted with REAL HOST POINTERS.
 *
 * Run lvled, lane intro-cutscene. Same job and same shape as table 2 of
 * hal/ptr_tables.cpp (the 39 camera-script records): the ROM's own two-word
 * record, redefined here so the pointer word holds a HOST address instead of
 * a DS one.
 *
 * WHY NOT romdata.py. That list byte-copies, and its header banks what that
 * costs on a kuppa table: data_020876e4 was removed from it because it held
 * six relocated words, so the byte copy handed ProcessKuppaScript six raw DS
 * addresses and it faulted walking them. Each record below has exactly ONE
 * relocated word, at +4, verified against config/arm9/relocs.txt
 * (out/intro-cutscene/reloc_audit.txt). Word 0 is a plain value and is
 * carried through from arm9_dec.bin unchanged.
 *
 * The opening's scripts reach these through arguments the sinit patches in,
 * so the pointer IS dereferenced and a DS address here is a fault, not a
 * cosmetic wrong number.
 */
#include "dsstate_seg.h"

/* ---- TWO THINGS THE SINIT NEEDS THAT ARE NOT RECORDS ----------------------
 *
 * data_0209b278: BSS, 12 bytes (0x0209b278..0x0209b284 by the symbol table).
 * It is in the 0x0209xxxx WORKING region, not romdata -- byte-copying it there
 * would be a category error and its span comes out negative anyway, because the
 * next symbol is below it. Nothing in the tree defined it yet; the sinit is the
 * first thing to reference it. Zero-initialised, which is what BSS means.
 * (Its neighbour data_0209b294, the FaderBrightness object ProcessKuppaScript
 * drives, is already hosted elsewhere and is deliberately NOT defined here.)
 *
 * _ZN10FaderColorD1Ev: the sinit hands this to func_020731dc, the ROM's
 * global-destructor registrar, as the destructor for a static FaderColor. A
 * no-op host body used to live here under the claim that the symbol had no
 * matched src TU. That was stale: src/engine/fader/_ZN10FaderColorD1Ev.c is
 * matched (vptr store, base-subobject dtor func_020177c4, returns self) and
 * is on slice_intro.txt now, so the registrar stores the ROM's own body. The
 * port never runs global destructors, so nothing observable changes; the
 * stand-in is simply gone (lane shadow-A).
 */
extern "C" {

DSSTATE_BEGIN
unsigned char data_0209b278[12];

extern unsigned char data_02087798[];
extern unsigned char data_020877c8[];
extern unsigned char data_020877f8[];
extern unsigned char data_02087828[];
extern unsigned char data_02087c68[];
extern unsigned char data_02087ca0[];
extern unsigned char data_02087cd8[];
extern unsigned char data_02087d10[];
extern unsigned char data_02087d48[];
extern unsigned char data_02087d80[];
extern unsigned char data_02087db8[];
extern unsigned char data_02087df0[];
extern unsigned char data_02087e28[];
extern unsigned char data_02087e60[];
extern unsigned char data_02087e98[];
extern unsigned char data_02087ed0[];
extern unsigned char data_02087f08[];
extern unsigned char data_02087f80[];
extern unsigned char data_02087fc0[];
extern unsigned char data_02088000[];
extern unsigned char data_02088040[];
extern unsigned char data_02088080[];
extern unsigned char data_020880c0[];
extern unsigned char data_02088100[];
extern unsigned char data_02088140[];
extern unsigned char data_02088180[];
extern unsigned char data_020881c0[];
extern unsigned char data_02088200[];
extern unsigned char data_02088240[];
extern unsigned char data_02088280[];
extern unsigned char data_020882c0[];
extern unsigned char data_02088300[];
extern unsigned char data_020883d0[];
extern unsigned char data_02088418[];
extern unsigned char data_02088460[];
extern unsigned char data_020884a8[];
extern unsigned char data_020884f0[];
extern unsigned char data_02088538[];
extern unsigned char data_02088580[];
extern unsigned char data_020885c8[];
extern unsigned char data_02088660[];
extern unsigned char data_020886b0[];
extern unsigned char data_02088700[];
extern unsigned char data_02088750[];
extern unsigned char data_020887a0[];
extern unsigned char data_020887f0[];
extern unsigned char data_02088840[];
extern unsigned char data_02088898[];
extern unsigned char data_02088c2c[];
extern unsigned char data_02088d80[];
extern unsigned char data_02088e38[];
extern unsigned char data_020891a8[];
extern unsigned char data_020892c0[];
extern unsigned char data_020893d8[];
extern unsigned char data_020894f0[];

/* 0x0208751c: word0 0x00000008, pointer -> 0x02088140 */
void *data_0208751c[2] = { (void *)0x00000008u, (void *)&data_02088140 };
/* 0x02087524: word0 0x00000023, pointer -> 0x020894f0 */
void *data_02087524[2] = { (void *)0x00000023u, (void *)&data_020894f0 };
/* 0x0208752c: word0 0x0000000a, pointer -> 0x02088660 */
void *data_0208752c[2] = { (void *)0x0000000au, (void *)&data_02088660 };
/* 0x02087534: word0 0x00000007, pointer -> 0x02087e60 */
void *data_02087534[2] = { (void *)0x00000007u, (void *)&data_02087e60 };
/* 0x0208753c: word0 0x00000023, pointer -> 0x020893d8 */
void *data_0208753c[2] = { (void *)0x00000023u, (void *)&data_020893d8 };
/* 0x02087544: word0 0x00000009, pointer -> 0x020883d0 */
void *data_02087544[2] = { (void *)0x00000009u, (void *)&data_020883d0 };
/* 0x0208754c: word0 0x00000006, pointer -> 0x02087828 */
void *data_0208754c[2] = { (void *)0x00000006u, (void *)&data_02087828 };
/* 0x02087554: word0 0x00000007, pointer -> 0x02087f08 */
void *data_02087554[2] = { (void *)0x00000007u, (void *)&data_02087f08 };
/* 0x0208755c: word0 0x00000008, pointer -> 0x02088080 */
void *data_0208755c[2] = { (void *)0x00000008u, (void *)&data_02088080 };
/* 0x02087564: word0 0x00000008, pointer -> 0x020880c0 */
void *data_02087564[2] = { (void *)0x00000008u, (void *)&data_020880c0 };
/* 0x0208756c: word0 0x00000009, pointer -> 0x02088538 */
void *data_0208756c[2] = { (void *)0x00000009u, (void *)&data_02088538 };
/* 0x02087574: word0 0x00000009, pointer -> 0x02088460 */
void *data_02087574[2] = { (void *)0x00000009u, (void *)&data_02088460 };
/* 0x0208757c: word0 0x00000009, pointer -> 0x020884a8 */
void *data_0208757c[2] = { (void *)0x00000009u, (void *)&data_020884a8 };
/* 0x02087584: word0 0x00000007, pointer -> 0x02087d80 */
void *data_02087584[2] = { (void *)0x00000007u, (void *)&data_02087d80 };
/* 0x0208758c: word0 0x0000000b, pointer -> 0x02088898 */
void *data_0208758c[2] = { (void *)0x0000000bu, (void *)&data_02088898 };
/* 0x02087594: word0 0x00000009, pointer -> 0x020884f0 */
void *data_02087594[2] = { (void *)0x00000009u, (void *)&data_020884f0 };
/* 0x0208759c: word0 0x00000006, pointer -> 0x020877c8 */
void *data_0208759c[2] = { (void *)0x00000006u, (void *)&data_020877c8 };
/* 0x020875a4: word0 0x0000000a, pointer -> 0x020887a0 */
void *data_020875a4[2] = { (void *)0x0000000au, (void *)&data_020887a0 };
/* 0x020875ac: word0 0x00000008, pointer -> 0x02088180 */
void *data_020875ac[2] = { (void *)0x00000008u, (void *)&data_02088180 };
/* 0x020875b4: word0 0x00000007, pointer -> 0x02087e28 */
void *data_020875b4[2] = { (void *)0x00000007u, (void *)&data_02087e28 };
/* 0x020875bc: word0 0x00000023, pointer -> 0x020891a8 */
void *data_020875bc[2] = { (void *)0x00000023u, (void *)&data_020891a8 };
/* 0x020875c4: word0 0x0000000b, pointer -> 0x02088840 */
void *data_020875c4[2] = { (void *)0x0000000bu, (void *)&data_02088840 };
/* 0x020875cc: word0 0x00000008, pointer -> 0x02088300 */
void *data_020875cc[2] = { (void *)0x00000008u, (void *)&data_02088300 };
/* 0x020875d4: word0 0x0000000a, pointer -> 0x020886b0 */
void *data_020875d4[2] = { (void *)0x0000000au, (void *)&data_020886b0 };
/* 0x020875dc: word0 0x00000008, pointer -> 0x02088040 */
void *data_020875dc[2] = { (void *)0x00000008u, (void *)&data_02088040 };
/* 0x020875e4: word0 0x00000007, pointer -> 0x02087c68 */
void *data_020875e4[2] = { (void *)0x00000007u, (void *)&data_02087c68 };
/* 0x020875ec: word0 0x00000007, pointer -> 0x02087ed0 */
void *data_020875ec[2] = { (void *)0x00000007u, (void *)&data_02087ed0 };
/* 0x020875f4: word0 0x00000009, pointer -> 0x02088418 */
void *data_020875f4[2] = { (void *)0x00000009u, (void *)&data_02088418 };
/* 0x020875fc: word0 0x00000009, pointer -> 0x020885c8 */
void *data_020875fc[2] = { (void *)0x00000009u, (void *)&data_020885c8 };
/* 0x02087604: word0 0x00000014, pointer -> 0x02088c2c */
void *data_02087604[2] = { (void *)0x00000014u, (void *)&data_02088c2c };
/* 0x0208760c: word0 0x00000007, pointer -> 0x02087d10 */
void *data_0208760c[2] = { (void *)0x00000007u, (void *)&data_02087d10 };
/* 0x02087614: word0 0x0000000a, pointer -> 0x020887f0 */
void *data_02087614[2] = { (void *)0x0000000au, (void *)&data_020887f0 };
/* 0x0208761c: word0 0x00000007, pointer -> 0x02087e98 */
void *data_0208761c[2] = { (void *)0x00000007u, (void *)&data_02087e98 };
/* 0x02087624: word0 0x00000008, pointer -> 0x02088100 */
void *data_02087624[2] = { (void *)0x00000008u, (void *)&data_02088100 };
/* 0x0208762c: word0 0x00000008, pointer -> 0x020881c0 */
void *data_0208762c[2] = { (void *)0x00000008u, (void *)&data_020881c0 };
/* 0x02087634: word0 0x00000007, pointer -> 0x02087d48 */
void *data_02087634[2] = { (void *)0x00000007u, (void *)&data_02087d48 };
/* 0x0208763c: word0 0x00000007, pointer -> 0x02087ca0 */
void *data_0208763c[2] = { (void *)0x00000007u, (void *)&data_02087ca0 };
/* 0x02087644: word0 0x00000017, pointer -> 0x02088d80 */
void *data_02087644[2] = { (void *)0x00000017u, (void *)&data_02088d80 };
/* 0x0208764c: word0 0x00000008, pointer -> 0x02087fc0 */
void *data_0208764c[2] = { (void *)0x00000008u, (void *)&data_02087fc0 };
/* 0x02087654: word0 0x00000007, pointer -> 0x02087db8 */
void *data_02087654[2] = { (void *)0x00000007u, (void *)&data_02087db8 };
/* 0x0208765c: word0 0x00000008, pointer -> 0x02088240 */
void *data_0208765c[2] = { (void *)0x00000008u, (void *)&data_02088240 };
/* 0x02087664: word0 0x00000017, pointer -> 0x02088e38 */
void *data_02087664[2] = { (void *)0x00000017u, (void *)&data_02088e38 };
/* 0x0208766c: word0 0x00000023, pointer -> 0x020892c0 */
void *data_0208766c[2] = { (void *)0x00000023u, (void *)&data_020892c0 };
/* 0x02087674: word0 0x0000000a, pointer -> 0x02088750 */
void *data_02087674[2] = { (void *)0x0000000au, (void *)&data_02088750 };
/* 0x0208767c: word0 0x00000008, pointer -> 0x02087f80 */
void *data_0208767c[2] = { (void *)0x00000008u, (void *)&data_02087f80 };
/* 0x02087684: word0 0x00000008, pointer -> 0x020882c0 */
void *data_02087684[2] = { (void *)0x00000008u, (void *)&data_020882c0 };
/* 0x0208768c: word0 0x00000008, pointer -> 0x02088200 */
void *data_0208768c[2] = { (void *)0x00000008u, (void *)&data_02088200 };
/* 0x02087694: word0 0x00000007, pointer -> 0x02087df0 */
void *data_02087694[2] = { (void *)0x00000007u, (void *)&data_02087df0 };
/* 0x0208769c: word0 0x00000006, pointer -> 0x020877f8 */
void *data_0208769c[2] = { (void *)0x00000006u, (void *)&data_020877f8 };
/* 0x020876a4: word0 0x00000009, pointer -> 0x02088580 */
void *data_020876a4[2] = { (void *)0x00000009u, (void *)&data_02088580 };
/* 0x020876ac: word0 0x00000008, pointer -> 0x02088280 */
void *data_020876ac[2] = { (void *)0x00000008u, (void *)&data_02088280 };
/* 0x020876b4: word0 0x0000000a, pointer -> 0x02088700 */
void *data_020876b4[2] = { (void *)0x0000000au, (void *)&data_02088700 };
/* 0x020876bc: word0 0x00000006, pointer -> 0x02087798 */
void *data_020876bc[2] = { (void *)0x00000006u, (void *)&data_02087798 };
/* 0x020876c4: word0 0x00000008, pointer -> 0x02088000 */
void *data_020876c4[2] = { (void *)0x00000008u, (void *)&data_02088000 };
/* 0x020876cc: word0 0x00000007, pointer -> 0x02087cd8 */
void *data_020876cc[2] = { (void *)0x00000007u, (void *)&data_02087cd8 };
DSSTATE_END

}  /* extern "C" */
