/* ov053 is data-only: 4,556 bytes of .rodata at 0x021111a0 and 480 bytes
   of .data at 0x02112380, no code. mwccarm puts every plain initialized
   object in its own same-named input section ordered by size, and the lcf
   can only select by section name, so one TU cannot place separated runs
   as separate arrays. The whole .rodata range (the zero runs are interior
   elements) and the whole .data range are one array each; the
   data_ov053_* rows stay symbols.txt rows and are emitted as lcf labels.
   A word is either a value or a relocated pointer, so each array is a
   union and the ROM relocations are .ptr initializers. */
#include "types.h"

typedef union Ov053Word {
    u32 value;
    const void *ptr;
} Ov053Word;

extern u32 data_ov053_021111a4[];
extern u32 data_ov053_021111a8[];
extern u32 data_ov053_021111ac[];
extern u32 data_ov053_021111b0[];
extern u32 data_ov053_021111b4[];
extern u32 data_ov053_021111b8[];
extern u32 data_ov053_021111bc[];
extern u32 data_ov053_021111c0[];
extern u32 data_ov053_021111c4[];
extern u32 data_ov053_021111c8[];
extern u32 data_ov053_021111cc[];
extern u32 data_ov053_021111d0[];
extern u32 data_ov053_021111d4[];
extern u32 data_ov053_021111d8[];
extern u32 data_ov053_021111dc[];
extern u32 data_ov053_021111e0[];
extern u32 data_ov053_021111fc[];
extern u32 data_ov053_02111234[];
extern u32 data_ov053_0211126c[];
extern u32 data_ov053_021112a4[];
extern u32 data_ov053_021112dc[];
extern u32 data_ov053_02111314[];
extern u32 data_ov053_0211134c[];
extern u32 data_ov053_02111384[];
extern u32 data_ov053_021113c0[];
extern u32 data_ov053_02111404[];
extern u32 data_ov053_021114f8[];
extern u32 data_ov053_021115ec[];
extern u32 data_ov053_02111750[];
extern u32 data_ov053_0211194c[];
extern u32 data_ov053_02111b68[];
extern u32 data_ov053_02111dc4[];
extern u32 data_ov053_02112098[];
extern u32 data_ov053_02112384[];
extern u32 data_ov053_0211238c[];
extern u32 data_ov053_02112394[];
extern u32 data_ov053_0211239c[];
extern u32 data_ov053_021123a4[];
extern u32 data_ov053_021123b0[];
extern u32 data_ov053_021123bc[];
extern u32 data_ov053_021123c8[];
extern u32 data_ov053_021123d4[];
extern u32 data_ov053_021123e4[];
extern u32 data_ov053_0211241c[];
extern u32 data_ov053_0211247c[];
extern u32 data_ov053_021124dc[];
extern u32 data_ov053_02112514[];

/* .rodata: 0x21111a0..0x211236c, 4556 bytes. */



/* .data: 0x2112380..0x2112560, 480 bytes. */

#pragma define_section s_021111c0 ".rodata.data_ov053_021111c0" ".rodata.data_ov053_021111c0" ".rodata.data_ov053_021111c0" far_data
#pragma section s_021111c0 begin
u32 data_ov053_021111c0[1] = {
    0x00001000,
};
#pragma section s_021111c0 end

#pragma define_section s_021111c4 ".rodata.data_ov053_021111c4" ".rodata.data_ov053_021111c4" ".rodata.data_ov053_021111c4" far_data
#pragma section s_021111c4 begin
u32 data_ov053_021111c4[1] = {
    0x00001000,
};
#pragma section s_021111c4 end

#pragma define_section s_021111c8 ".rodata.data_ov053_021111c8" ".rodata.data_ov053_021111c8" ".rodata.data_ov053_021111c8" far_data
#pragma section s_021111c8 begin
u32 data_ov053_021111c8[1] = {
    0x00001000,
};
#pragma section s_021111c8 end

#pragma define_section s_021111cc ".rodata.data_ov053_021111cc" ".rodata.data_ov053_021111cc" ".rodata.data_ov053_021111cc" far_data
#pragma section s_021111cc begin
u32 data_ov053_021111cc[1] = {
    0x00001000,
};
#pragma section s_021111cc end

#pragma define_section s_021111d0 ".rodata.data_ov053_021111d0" ".rodata.data_ov053_021111d0" ".rodata.data_ov053_021111d0" far_data
#pragma section s_021111d0 begin
u32 data_ov053_021111d0[1] = {
    0x00001000,
};
#pragma section s_021111d0 end

#pragma define_section s_021111d4 ".rodata.data_ov053_021111d4" ".rodata.data_ov053_021111d4" ".rodata.data_ov053_021111d4" far_data
#pragma section s_021111d4 begin
u32 data_ov053_021111d4[1] = {
    0x00001000,
};
#pragma section s_021111d4 end

#pragma define_section s_021111d8 ".rodata.data_ov053_021111d8" ".rodata.data_ov053_021111d8" ".rodata.data_ov053_021111d8" far_data
#pragma section s_021111d8 begin
u32 data_ov053_021111d8[1] = {
    0x00001000,
};
#pragma section s_021111d8 end

#pragma define_section s_021111dc ".rodata.data_ov053_021111dc" ".rodata.data_ov053_021111dc" ".rodata.data_ov053_021111dc" far_data
#pragma section s_021111dc begin
u32 data_ov053_021111dc[1] = {
    0x00001000,
};
#pragma section s_021111dc end

#pragma define_section s_021111e0 ".rodata.data_ov053_021111e0" ".rodata.data_ov053_021111e0" ".rodata.data_ov053_021111e0" far_data
#pragma section s_021111e0 begin
u32 data_ov053_021111e0[3] = {
    0x0000ffff,
    (u32)&data_ov053_02112394,
    0x00000001,
};
#pragma section s_021111e0 end

#pragma define_section s_021111ec ".rodata.data_ov053_021111ec" ".rodata.data_ov053_021111ec" ".rodata.data_ov053_021111ec" far_data
#pragma section s_021111ec begin
u32 data_ov053_021111ec[4] = {
    0x00000001,
    0x00000001,
    0x000000b5,
    0x000000b5,
};
#pragma section s_021111ec end

#pragma define_section s_021111fc ".rodata.data_ov053_021111fc" ".rodata.data_ov053_021111fc" ".rodata.data_ov053_021111fc" far_data
#pragma section s_021111fc begin
u32 data_ov053_021111fc[2] = {
    0x0000ffff,
    (u32)&data_ov053_021123b0,
};
#pragma section s_021111fc end

#pragma define_section s_02111204 ".rodata.data_ov053_02111204" ".rodata.data_ov053_02111204" ".rodata.data_ov053_02111204" far_data
#pragma section s_02111204 begin
u32 data_ov053_02111204[3] = {
    0x00000001,
    0x00000001,
    0x00000001,
};
#pragma section s_02111204 end

#pragma define_section s_02111210 ".rodata.data_ov053_02111210" ".rodata.data_ov053_02111210" ".rodata.data_ov053_02111210" far_data
#pragma section s_02111210 begin
u32 data_ov053_02111210[2] = {
    0x00000001,
    0x0001005a,
};
#pragma section s_02111210 end

#pragma define_section s_02111218 ".rodata.data_ov053_02111218" ".rodata.data_ov053_02111218" ".rodata.data_ov053_02111218" far_data
#pragma section s_02111218 begin
u32 data_ov053_02111218[4] = {
    0x0000ffff,
    (u32)&data_ov053_021123bc,
    0x00000001,
    0x00000001,
};
#pragma section s_02111218 end

#pragma define_section s_02111228 ".rodata.data_ov053_02111228" ".rodata.data_ov053_02111228" ".rodata.data_ov053_02111228" far_data
#pragma section s_02111228 begin
u32 data_ov053_02111228[3] = {
    0x00000001,
    0x00000001,
    0x005b005a,
};
#pragma section s_02111228 end

#pragma define_section s_02111234 ".rodata.data_ov053_02111234" ".rodata.data_ov053_02111234" ".rodata.data_ov053_02111234" far_data
#pragma section s_02111234 begin
u32 data_ov053_02111234[4] = {
    0x0000ffff,
    (u32)&data_ov053_021123b0,
    0x00000001,
    0x00000001,
};
#pragma section s_02111234 end

#pragma define_section s_02111244 ".rodata.data_ov053_02111244" ".rodata.data_ov053_02111244" ".rodata.data_ov053_02111244" far_data
#pragma section s_02111244 begin
u32 data_ov053_02111244[10] = {
    0x00000001,
    0x00000001,
    0x0001002c,
    0x0000ffff,
    (u32)&data_ov053_021123bc,
    0x00000001,
    0x00000001,
    0x00000001,
    0x00000001,
    0x002d002c,
};
#pragma section s_02111244 end

#pragma define_section s_0211126c ".rodata.data_ov053_0211126c" ".rodata.data_ov053_0211126c" ".rodata.data_ov053_0211126c" far_data
#pragma section s_0211126c begin
u32 data_ov053_0211126c[14] = {
    0x0000ffff,
    (u32)&data_ov053_021123b0,
    0x00000001,
    0x00000001,
    0x00000001,
    0x00000001,
    0x0001001e,
    0x0000ffff,
    (u32)&data_ov053_021123bc,
    0x00000001,
    0x00000001,
    0x00000001,
    0x00000001,
    0x001f001e,
};
#pragma section s_0211126c end

#pragma define_section s_021112a4 ".rodata.data_ov053_021112a4" ".rodata.data_ov053_021112a4" ".rodata.data_ov053_021112a4" far_data
#pragma section s_021112a4 begin
u32 data_ov053_021112a4[3] = {
    0x0000ffff,
    (u32)&data_ov053_021123b0,
    0x00000001,
};
#pragma section s_021112a4 end

#pragma define_section s_021112b0 ".rodata.data_ov053_021112b0" ".rodata.data_ov053_021112b0" ".rodata.data_ov053_021112b0" far_data
#pragma section s_021112b0 begin
u32 data_ov053_021112b0[11] = {
    0x00000001,
    0x00000001,
    0x00000001,
    0x0001003f,
    0x0000ffff,
    (u32)&data_ov053_021123bc,
    0x00000001,
    0x00000001,
    0x00000001,
    0x00000001,
    0x0040003f,
};
#pragma section s_021112b0 end

#pragma define_section s_021112dc ".rodata.data_ov053_021112dc" ".rodata.data_ov053_021112dc" ".rodata.data_ov053_021112dc" far_data
#pragma section s_021112dc begin
u32 data_ov053_021112dc[14] = {
    0x0000ffff,
    (u32)&data_ov053_021123b0,
    0x00000001,
    0x00000001,
    0x00000001,
    0x00000001,
    0x00010008,
    0x0000ffff,
    (u32)&data_ov053_021123bc,
    0x00000001,
    0x00000001,
    0x00000001,
    0x00000001,
    0x00090008,
};
#pragma section s_021112dc end

#pragma define_section s_02111314 ".rodata.data_ov053_02111314" ".rodata.data_ov053_02111314" ".rodata.data_ov053_02111314" far_data
#pragma section s_02111314 begin
u32 data_ov053_02111314[14] = {
    0x0000ffff,
    (u32)&data_ov053_021123b0,
    0x00000001,
    0x00000001,
    0x00000001,
    0x00000001,
    0x0001004b,
    0x0000ffff,
    (u32)&data_ov053_021123bc,
    0x00000001,
    0x00000001,
    0x00000001,
    0x00000001,
    0x004c004b,
};
#pragma section s_02111314 end

#pragma define_section s_0211134c ".rodata.data_ov053_0211134c" ".rodata.data_ov053_0211134c" ".rodata.data_ov053_0211134c" far_data
#pragma section s_0211134c begin
u32 data_ov053_0211134c[14] = {
    0x0000ffff,
    (u32)&data_ov053_021123b0,
    0x00000001,
    0x00000001,
    0x00000001,
    0x00000001,
    0x0001001e,
    0x0000ffff,
    (u32)&data_ov053_021123bc,
    0x00000001,
    0x00000001,
    0x00000001,
    0x00000001,
    0x001f001e,
};
#pragma section s_0211134c end

#pragma define_section s_02111384 ".rodata.data_ov053_02111384" ".rodata.data_ov053_02111384" ".rodata.data_ov053_02111384" far_data
#pragma section s_02111384 begin
u32 data_ov053_02111384[10] = {
    0x0000ffff,
    (u32)&data_ov053_021123a4,
    0x00000101,
    0x005a0101,
    0x00b40101,
    0x010e0101,
    0x01680101,
    0x01c20101,
    0x00b00001,
    0x00b00001,
};
#pragma section s_02111384 end

#pragma define_section s_021113ac ".rodata.data_ov053_021113ac" ".rodata.data_ov053_021113ac" ".rodata.data_ov053_021113ac" far_data
#pragma section s_021113ac begin
u32 data_ov053_021113ac[1] = {
    0x00b00001,
};
#pragma section s_021113ac end

#pragma define_section s_021113b0 ".rodata.data_ov053_021113b0" ".rodata.data_ov053_021113b0" ".rodata.data_ov053_021113b0" far_data
#pragma section s_021113b0 begin
u32 data_ov053_021113b0[4] = {
    0x00b00001,
    0x00b00001,
    0x00b00001,
    0x00520001,
};
#pragma section s_021113b0 end

#pragma define_section s_021113c0 ".rodata.data_ov053_021113c0" ".rodata.data_ov053_021113c0" ".rodata.data_ov053_021113c0" far_data
#pragma section s_021113c0 begin
u32 data_ov053_021113c0[4] = {
    0x00000000,
    0xfffffe1d,
    0x000000be,
    0x00000360,
};
#pragma section s_021113c0 end

#pragma define_section s_021113d0 ".rodata.data_ov053_021113d0" ".rodata.data_ov053_021113d0" ".rodata.data_ov053_021113d0" far_data
#pragma section s_021113d0 begin
u32 data_ov053_021113d0[13] = {
    0x000000be,
    0xfffffe1d,
    0x000000be,
    0x00000360,
    0x000000be,
    0x0000036b,
    0x00000079,
    0xfffffd86,
    0x00000079,
    0x0000036b,
    0x00000079,
    0xfffffd86,
    0x00000079,
};
#pragma section s_021113d0 end

#pragma define_section s_02111404 ".rodata.data_ov053_02111404" ".rodata.data_ov053_02111404" ".rodata.data_ov053_02111404" far_data
#pragma section s_02111404 begin
u32 data_ov053_02111404[13] = {
    0x00000000,
    0xfffffe1d,
    0xfffffe2e,
    0xfffffe5e,
    0xfffffea7,
    0xffffff03,
    0xffffff6d,
    0xffffffe0,
    0x00000056,
    0x000000c8,
    0x00000133,
    0x0000018f,
    0x000001d8,
};
#pragma section s_02111404 end

#pragma define_section s_02111438 ".rodata.data_ov053_02111438" ".rodata.data_ov053_02111438" ".rodata.data_ov053_02111438" far_data
#pragma section s_02111438 begin
u32 data_ov053_02111438[48] = {
    0x00000208,
    0x00000219,
    0x0000020f,
    0x000001f2,
    0x000001c5,
    0x0000018a,
    0x00000144,
    0x000000f5,
    0x000000a0,
    0x00000048,
    0xffffffee,
    0xffffff95,
    0xffffff40,
    0xfffffef2,
    0xfffffeac,
    0xfffffe71,
    0xfffffe44,
    0xfffffe27,
    0x000001e3,
    0x000001d2,
    0x000001a2,
    0x00000159,
    0x000000fd,
    0x00000093,
    0x00000020,
    0xffffffaa,
    0xffffff38,
    0xfffffecd,
    0xfffffe71,
    0xfffffe28,
    0xfffffdf8,
    0xfffffde7,
    0xfffffdf1,
    0xfffffe0e,
    0xfffffe3b,
    0xfffffe76,
    0xfffffebc,
    0xffffff0b,
    0xffffff60,
    0xffffffb8,
    0x00000012,
    0x0000006b,
    0x000000c0,
    0x0000010e,
    0x00000154,
    0x0000018f,
    0x000001bc,
    0x000001d9,
};
#pragma section s_02111438 end

#pragma define_section s_021114f8 ".rodata.data_ov053_021114f8" ".rodata.data_ov053_021114f8" ".rodata.data_ov053_021114f8" far_data
#pragma section s_021114f8 begin
u32 data_ov053_021114f8[5] = {
    0x00000000,
    0xfffffe1d,
    0xfffffea9,
    0xfffffff7,
    0x00000186,
};
#pragma section s_021114f8 end

#pragma define_section s_0211150c ".rodata.data_ov053_0211150c" ".rodata.data_ov053_0211150c" ".rodata.data_ov053_0211150c" far_data
#pragma section s_0211150c begin
u32 data_ov053_0211150c[5] = {
    0x000002d4,
    0x00000360,
    0x00000315,
    0x00000255,
    0x0000014e,
};
#pragma section s_0211150c end

#pragma define_section s_02111520 ".rodata.data_ov053_02111520" ".rodata.data_ov053_02111520" ".rodata.data_ov053_02111520" far_data
#pragma section s_02111520 begin
u32 data_ov053_02111520[7] = {
    0x0000002f,
    0xffffff28,
    0xfffffe67,
    0xfffffe1d,
    0xfffffe67,
    0xffffff28,
    0x00000030,
};
#pragma section s_02111520 end

#pragma define_section s_0211153c ".rodata.data_ov053_0211153c" ".rodata.data_ov053_0211153c" ".rodata.data_ov053_0211153c" far_data
#pragma section s_0211153c begin
u32 data_ov053_0211153c[3] = {
    0x0000014f,
    0x00000256,
    0x00000316,
};
#pragma section s_0211153c end

#pragma define_section s_02111548 ".rodata.data_ov053_02111548" ".rodata.data_ov053_02111548" ".rodata.data_ov053_02111548" far_data
#pragma section s_02111548 begin
u32 data_ov053_02111548[5] = {
    0x00000360,
    0x00000201,
    0xffffff79,
    0xfffffe1d,
    0xfffffea9,
};
#pragma section s_02111548 end

#pragma define_section s_0211155c ".rodata.data_ov053_0211155c" ".rodata.data_ov053_0211155c" ".rodata.data_ov053_0211155c" far_data
#pragma section s_0211155c begin
u32 data_ov053_0211155c[1] = {
    0xfffffff7,
};
#pragma section s_0211155c end

#pragma define_section s_02111560 ".rodata.data_ov053_02111560" ".rodata.data_ov053_02111560" ".rodata.data_ov053_02111560" far_data
#pragma section s_02111560 begin
u32 data_ov053_02111560[15] = {
    0x00000186,
    0x000002d4,
    0x00000360,
    0x00000203,
    0xffffff7a,
    0x0000036b,
    0x000002ce,
    0x00000158,
    0xffffff99,
    0xfffffe23,
    0xfffffd86,
    0xfffffdda,
    0xfffffeb1,
    0xffffffd8,
    0x00000119,
};
#pragma section s_02111560 end

#pragma define_section s_0211159c ".rodata.data_ov053_0211159c" ".rodata.data_ov053_0211159c" ".rodata.data_ov053_0211159c" far_data
#pragma section s_0211159c begin
u32 data_ov053_0211159c[8] = {
    0x00000240,
    0x00000318,
    0x0000036b,
    0x00000318,
    0x00000240,
    0x00000119,
    0xffffffd7,
    0xfffffeb0,
};
#pragma section s_0211159c end

#pragma define_section s_021115bc ".rodata.data_ov053_021115bc" ".rodata.data_ov053_021115bc" ".rodata.data_ov053_021115bc" far_data
#pragma section s_021115bc begin
u32 data_ov053_021115bc[9] = {
    0xfffffdd9,
    0xfffffd86,
    0xffffff10,
    0x000001e5,
    0x0000036b,
    0x000002ce,
    0x00000158,
    0xffffff99,
    0xfffffe23,
};
#pragma section s_021115bc end

#pragma define_section s_021115e0 ".rodata.data_ov053_021115e0" ".rodata.data_ov053_021115e0" ".rodata.data_ov053_021115e0" far_data
#pragma section s_021115e0 begin
u32 data_ov053_021115e0[1] = {
    0xfffffd86,
};
#pragma section s_021115e0 end

#pragma define_section s_021115e4 ".rodata.data_ov053_021115e4" ".rodata.data_ov053_021115e4" ".rodata.data_ov053_021115e4" far_data
#pragma section s_021115e4 begin
u32 data_ov053_021115e4[2] = {
    0xffffff0d,
    0x000001e4,
};
#pragma section s_021115e4 end

#pragma define_section s_021115ec ".rodata.data_ov053_021115ec" ".rodata.data_ov053_021115ec" ".rodata.data_ov053_021115ec" far_data
#pragma section s_021115ec begin
u32 data_ov053_021115ec[2] = {
    0x00000000,
    0xfffffe1d,
};
#pragma section s_021115ec end

#pragma define_section s_021115f4 ".rodata.data_ov053_021115f4" ".rodata.data_ov053_021115f4" ".rodata.data_ov053_021115f4" far_data
#pragma section s_021115f4 begin
u32 data_ov053_021115f4[12] = {
    0xffffff17,
    0x000000e9,
    0x000001e3,
    0x000000e9,
    0xffffff17,
    0xfffffe1d,
    0xffffff17,
    0x000000e9,
    0x000001e3,
    0x0000014c,
    0x00000000,
    0xfffffeb4,
};
#pragma section s_021115f4 end

#pragma define_section s_02111624 ".rodata.data_ov053_02111624" ".rodata.data_ov053_02111624" ".rodata.data_ov053_02111624" far_data
#pragma section s_02111624 begin
u32 data_ov053_02111624[7] = {
    0xfffffe1d,
    0xfffffe81,
    0xffffff71,
    0x0000008f,
    0x0000017f,
    0x000001e3,
    0x000001ae,
};
#pragma section s_02111624 end

#pragma define_section s_02111640 ".rodata.data_ov053_02111640" ".rodata.data_ov053_02111640" ".rodata.data_ov053_02111640" far_data
#pragma section s_02111640 begin
u32 data_ov053_02111640[8] = {
    0x00000124,
    0x00000067,
    0xffffff99,
    0xfffffedc,
    0xfffffe52,
    0xfffffe1d,
    0xfffffe46,
    0xfffffeb4,
};
#pragma section s_02111640 end

#pragma define_section s_02111660 ".rodata.data_ov053_02111660" ".rodata.data_ov053_02111660" ".rodata.data_ov053_02111660" far_data
#pragma section s_02111660 begin
u32 data_ov053_02111660[8] = {
    0xffffff4f,
    0x00000000,
    0x000000b1,
    0x0000014c,
    0x000001ba,
    0x000001e3,
    0x000001c8,
    0x0000017f,
};
#pragma section s_02111660 end

#pragma define_section s_02111680 ".rodata.data_ov053_02111680" ".rodata.data_ov053_02111680" ".rodata.data_ov053_02111680" far_data
#pragma section s_02111680 begin
u32 data_ov053_02111680[8] = {
    0x00000113,
    0x0000008f,
    0x00000000,
    0xffffff71,
    0xfffffeed,
    0xfffffe81,
    0xfffffe38,
    0xfffffe1d,
};
#pragma section s_02111680 end

#pragma define_section s_021116a0 ".rodata.data_ov053_021116a0" ".rodata.data_ov053_021116a0" ".rodata.data_ov053_021116a0" far_data
#pragma section s_021116a0 begin
u32 data_ov053_021116a0[8] = {
    0x000001e3,
    0x000000e9,
    0xffffff17,
    0xfffffe1d,
    0xffffff17,
    0x000000e9,
    0x000001e3,
    0x000000e9,
};
#pragma section s_021116a0 end

#pragma define_section s_021116c0 ".rodata.data_ov053_021116c0" ".rodata.data_ov053_021116c0" ".rodata.data_ov053_021116c0" far_data
#pragma section s_021116c0 begin
u32 data_ov053_021116c0[8] = {
    0xffffff17,
    0xfffffe1d,
    0xfffffeb4,
    0x00000000,
    0x0000014c,
    0x000001e3,
    0x0000017f,
    0x0000008f,
};
#pragma section s_021116c0 end

#pragma define_section s_021116e0 ".rodata.data_ov053_021116e0" ".rodata.data_ov053_021116e0" ".rodata.data_ov053_021116e0" far_data
#pragma section s_021116e0 begin
u32 data_ov053_021116e0[8] = {
    0xffffff71,
    0xfffffe81,
    0xfffffe1d,
    0xfffffe52,
    0xfffffedc,
    0xffffff99,
    0x00000067,
    0x00000124,
};
#pragma section s_021116e0 end

#pragma define_section s_02111700 ".rodata.data_ov053_02111700" ".rodata.data_ov053_02111700" ".rodata.data_ov053_02111700" far_data
#pragma section s_02111700 begin
u32 data_ov053_02111700[8] = {
    0x000001ae,
    0x000001e3,
    0x000001ba,
    0x0000014c,
    0x000000b1,
    0x00000000,
    0xffffff4f,
    0xfffffeb4,
};
#pragma section s_02111700 end

#pragma define_section s_02111720 ".rodata.data_ov053_02111720" ".rodata.data_ov053_02111720" ".rodata.data_ov053_02111720" far_data
#pragma section s_02111720 begin
u32 data_ov053_02111720[8] = {
    0xfffffe46,
    0xfffffe1d,
    0xfffffe38,
    0xfffffe81,
    0xfffffeed,
    0xffffff71,
    0x00000000,
    0x0000008f,
};
#pragma section s_02111720 end

#pragma define_section s_02111740 ".rodata.data_ov053_02111740" ".rodata.data_ov053_02111740" ".rodata.data_ov053_02111740" far_data
#pragma section s_02111740 begin
u32 data_ov053_02111740[4] = {
    0x00000113,
    0x0000017f,
    0x000001c8,
    0x000001e3,
};
#pragma section s_02111740 end

#pragma define_section s_02111750 ".rodata.data_ov053_02111750" ".rodata.data_ov053_02111750" ".rodata.data_ov053_02111750" far_data
#pragma section s_02111750 begin
u32 data_ov053_02111750[4] = {
    0x00000000,
    0xfffffe1d,
    0xfffffe1d,
    0xfffffe1d,
};
#pragma section s_02111750 end

#pragma define_section s_02111760 ".rodata.data_ov053_02111760" ".rodata.data_ov053_02111760" ".rodata.data_ov053_02111760" far_data
#pragma section s_02111760 begin
u32 data_ov053_02111760[2] = {
    0xfffffe1d,
    0xfffffe1d,
};
#pragma section s_02111760 end

#pragma define_section s_02111768 ".rodata.data_ov053_02111768" ".rodata.data_ov053_02111768" ".rodata.data_ov053_02111768" far_data
#pragma section s_02111768 begin
u32 data_ov053_02111768[51] = {
    0xfffffe1d,
    0xfffffe1d,
    0xfffffe1d,
    0xfffffe1d,
    0xfffffe1d,
    0xffffff17,
    0x000000e9,
    0x000001e3,
    0x000000e9,
    0xffffff17,
    0xfffffe1d,
    0xfffffeb4,
    0x00000001,
    0x0000014d,
    0x000001e3,
    0x0000014c,
    0x00000000,
    0xfffffeb4,
    0xfffffe1d,
    0xfffffe25,
    0xfffffe3e,
    0xfffffe64,
    0xfffffe97,
    0xfffffed3,
    0xffffff17,
    0xffffff62,
    0xffffffb0,
    0x00000000,
    0x00000050,
    0x0000009e,
    0x000000e9,
    0x0000012d,
    0x00000169,
    0x0000019c,
    0x000001c2,
    0x000001db,
    0x000001e3,
    0x000001dd,
    0x000001cb,
    0x000001ae,
    0x00000188,
    0x00000159,
    0x00000124,
    0x000000e9,
    0x000000aa,
    0x00000067,
    0x00000023,
    0xffffffde,
    0xffffff99,
    0xffffff57,
    0xffffff17,
};
#pragma section s_02111768 end

#pragma define_section s_02111834 ".rodata.data_ov053_02111834" ".rodata.data_ov053_02111834" ".rodata.data_ov053_02111834" far_data
#pragma section s_02111834 begin
u32 data_ov053_02111834[4] = {
    0xfffffedc,
    0xfffffea7,
    0xfffffe79,
    0xfffffe52,
};
#pragma section s_02111834 end

#pragma define_section s_02111844 ".rodata.data_ov053_02111844" ".rodata.data_ov053_02111844" ".rodata.data_ov053_02111844" far_data
#pragma section s_02111844 begin
u32 data_ov053_02111844[50] = {
    0xfffffe35,
    0xfffffe23,
    0xfffffe1d,
    0x000001e3,
    0x000001e3,
    0x000001e3,
    0x000001e3,
    0x000001e3,
    0x000001e3,
    0x000001e3,
    0x000001e3,
    0x000001e3,
    0x000001e3,
    0x000000e9,
    0xffffff17,
    0xfffffe1d,
    0xffffff17,
    0x000000e9,
    0x000001e3,
    0x0000014c,
    0xffffffff,
    0xfffffeb3,
    0xfffffe1d,
    0xfffffeb4,
    0x00000000,
    0x0000014c,
    0x000001e3,
    0x000001dc,
    0x000001c5,
    0x000001a3,
    0x00000175,
    0x0000013e,
    0x000000ff,
    0x000000ba,
    0x00000072,
    0x00000026,
    0xffffffda,
    0xffffff8e,
    0xffffff46,
    0xffffff01,
    0xfffffec2,
    0xfffffe8b,
    0xfffffe5d,
    0xfffffe3b,
    0xfffffe24,
    0xfffffe1d,
    0xfffffe24,
    0xfffffe38,
    0xfffffe57,
    0xfffffe81,
};
#pragma section s_02111844 end

#pragma define_section s_0211190c ".rodata.data_ov053_0211190c" ".rodata.data_ov053_0211190c" ".rodata.data_ov053_0211190c" far_data
#pragma section s_0211190c begin
u32 data_ov053_0211190c[7] = {
    0xfffffeb4,
    0xfffffeed,
    0xffffff2d,
    0xffffff71,
    0xffffffb8,
    0x00000000,
    0x00000048,
};
#pragma section s_0211190c end

#pragma define_section s_02111928 ".rodata.data_ov053_02111928" ".rodata.data_ov053_02111928" ".rodata.data_ov053_02111928" far_data
#pragma section s_02111928 begin
u32 data_ov053_02111928[1] = {
    0x0000008f,
};
#pragma section s_02111928 end

#pragma define_section s_0211192c ".rodata.data_ov053_0211192c" ".rodata.data_ov053_0211192c" ".rodata.data_ov053_0211192c" far_data
#pragma section s_0211192c begin
u32 data_ov053_0211192c[6] = {
    0x000000d3,
    0x00000113,
    0x0000014c,
    0x0000017f,
    0x000001a9,
    0x000001c8,
};
#pragma section s_0211192c end

#pragma define_section s_02111944 ".rodata.data_ov053_02111944" ".rodata.data_ov053_02111944" ".rodata.data_ov053_02111944" far_data
#pragma section s_02111944 begin
u32 data_ov053_02111944[2] = {
    0x000001dc,
    0x000001e3,
};
#pragma section s_02111944 end

#pragma define_section s_0211194c ".rodata.data_ov053_0211194c" ".rodata.data_ov053_0211194c" ".rodata.data_ov053_0211194c" far_data
#pragma section s_0211194c begin
u32 data_ov053_0211194c[5] = {
    0x19191919,
    0x19191919,
    0x1a191919,
    0x1a1a1a1a,
    0x1a1a1a1a,
};
#pragma section s_0211194c end

#pragma define_section s_02111960 ".rodata.data_ov053_02111960" ".rodata.data_ov053_02111960" ".rodata.data_ov053_02111960" far_data
#pragma section s_02111960 begin
u32 data_ov053_02111960[13] = {
    0x1a1a1a1a,
    0x1b1b1b1a,
    0x1b1b1b1b,
    0x1b1b1b1b,
    0x1c1b1b1b,
    0x1c1c1c1c,
    0x1c1c1c1c,
    0x1c1c1c1c,
    0x1d1d1c1c,
    0x1d1d1d1d,
    0x1d1d1d1d,
    0x1d1d1d1d,
    0x1e1e1e1e,
};
#pragma section s_02111960 end

#pragma define_section s_02111994 ".rodata.data_ov053_02111994" ".rodata.data_ov053_02111994" ".rodata.data_ov053_02111994" far_data
#pragma section s_02111994 begin
u32 data_ov053_02111994[46] = {
    0x1e1e1e1e,
    0x1e1e1e1e,
    0x1f1f1e1e,
    0x1f1f1f1f,
    0x0c0c1f1f,
    0x0c0c0c0c,
    0x0b0b0b0c,
    0x0b0b0b0b,
    0x0a0a0a0a,
    0x090a0a0a,
    0x09090909,
    0x08080909,
    0x08080808,
    0x07070808,
    0x07070707,
    0x06060607,
    0x06060606,
    0x05050505,
    0x04050505,
    0x04040404,
    0x03030404,
    0x03030303,
    0x02020303,
    0x02020202,
    0x01010102,
    0x01010101,
    0x00000000,
    0x02020202,
    0x02020202,
    0x02020202,
    0x02020202,
    0x02020202,
    0x02020202,
    0x02020202,
    0x01010101,
    0x01010101,
    0x01010101,
    0x01010101,
    0x01010101,
    0x01010101,
    0x01010101,
    0x01010101,
    0x01010101,
    0x01010101,
    0x00000001,
    0x00000000,
};
#pragma section s_02111994 end

#pragma define_section s_02111a58 ".rodata.data_ov053_02111a58" ".rodata.data_ov053_02111a58" ".rodata.data_ov053_02111a58" far_data
#pragma section s_02111a58 begin
u32 data_ov053_02111a58[8] = {
    0x06060000,
    0x07070707,
    0x08080807,
    0x09090908,
    0x0a0a0909,
    0x0b0a0a0a,
    0x0b0b0b0b,
    0x0c0c0c0c,
};
#pragma section s_02111a58 end

#pragma define_section s_02111a78 ".rodata.data_ov053_02111a78" ".rodata.data_ov053_02111a78" ".rodata.data_ov053_02111a78" far_data
#pragma section s_02111a78 begin
u32 data_ov053_02111a78[17] = {
    0x0d0d0d0c,
    0x0e0e0e0d,
    0x0f0f0e0e,
    0x100f0f0f,
    0x10101010,
    0x11111111,
    0x12121212,
    0x13131312,
    0x14141313,
    0x15141414,
    0x16151515,
    0x16161616,
    0x17171717,
    0x18181817,
    0x19191818,
    0x06060606,
    0x06060606,
};
#pragma section s_02111a78 end

#pragma define_section s_02111abc ".rodata.data_ov053_02111abc" ".rodata.data_ov053_02111abc" ".rodata.data_ov053_02111abc" far_data
#pragma section s_02111abc begin
u32 data_ov053_02111abc[6] = {
    0x05060606,
    0x05050505,
    0x05050505,
    0x05050505,
    0x04040405,
    0x04040404,
};
#pragma section s_02111abc end

#pragma define_section s_02111ad4 ".rodata.data_ov053_02111ad4" ".rodata.data_ov053_02111ad4" ".rodata.data_ov053_02111ad4" far_data
#pragma section s_02111ad4 begin
u32 data_ov053_02111ad4[8] = {
    0x04040404,
    0x04040404,
    0x03030303,
    0x03030303,
    0x03030303,
    0x02030303,
    0x02020202,
    0x02020202,
};
#pragma section s_02111ad4 end

#pragma define_section s_02111af4 ".rodata.data_ov053_02111af4" ".rodata.data_ov053_02111af4" ".rodata.data_ov053_02111af4" far_data
#pragma section s_02111af4 begin
u32 data_ov053_02111af4[8] = {
    0x02020202,
    0x01010102,
    0x01010101,
    0x01010101,
    0x01010101,
    0x00000000,
    0x06060000,
    0x06060606,
};
#pragma section s_02111af4 end
