/* Overlay 20 course data the haunted-house actors never name: the .rodata
 * table and the .data word tables. The BookGen/Book/Chair vtables and
 * profiles at 0x0211482c stay unclaimed. */
#include "types.h"

extern u32 data_ov020_021134e4[];
extern u32 data_ov020_021134e8[];
extern u32 data_ov020_021134ec[];
extern u32 data_ov020_02113508[];
extern u32 data_ov020_02113518[];
extern u32 data_ov020_02113534[];
extern u32 data_ov020_02113554[];
extern u32 data_ov020_021135d4[];
extern u32 data_ov020_021135f4[];
extern u32 data_ov020_02113780[];
extern u32 data_ov020_02113784[];
extern u32 data_ov020_0211378c[];
extern u32 data_ov020_02113794[];
extern u32 data_ov020_0211379c[];
extern u32 data_ov020_021137a4[];
extern u32 data_ov020_021137ac[];
extern u32 data_ov020_021137b4[];
extern u32 data_ov020_021137bc[];
extern u32 data_ov020_021137c4[];
extern u32 data_ov020_021137cc[];
extern u32 data_ov020_021137d8[];
extern u32 data_ov020_021137e4[];
extern u32 data_ov020_021137f0[];
extern u32 data_ov020_021137f8[];
extern u32 data_ov020_021137fc[];
extern u32 data_ov020_02113800[];
extern u32 data_ov020_02113804[];
extern u32 data_ov020_0211380c[];
extern u32 data_ov020_0211381c[];
extern u32 data_ov020_02113820[];
extern u32 data_ov020_0211382c[];
extern u32 data_ov020_0211383c[];
extern u32 data_ov020_0211384c[];
extern u32 data_ov020_0211385c[];
extern u32 data_ov020_0211386c[];
extern u32 data_ov020_0211387c[];
extern u32 data_ov020_0211388c[];
extern u32 data_ov020_0211389c[];
extern u32 data_ov020_021138a8[];
extern u32 data_ov020_021138ac[];
extern u32 data_ov020_021138bc[];
extern u32 data_ov020_021138c4[];
extern u32 data_ov020_021138cc[];
extern u32 data_ov020_021138d8[];
extern u32 data_ov020_021138e4[];
extern u32 data_ov020_021138f0[];
extern u32 data_ov020_021138fc[];
extern u32 data_ov020_02113918[];
extern u32 data_ov020_02113938[];
extern u32 data_ov020_02113958[];
extern u32 data_ov020_02113978[];
extern u32 data_ov020_02113980[];
extern u32 data_ov020_02113998[];
extern u32 data_ov020_021139b0[];
extern u32 data_ov020_021139b8[];
extern u32 data_ov020_021139d8[];
extern u32 data_ov020_021139f4[];
extern u32 data_ov020_021139f8[];
extern u32 data_ov020_02113a00[];
extern u32 data_ov020_02113a18[];
extern u32 data_ov020_02113a38[];
extern u32 data_ov020_02113a50[];
extern u32 data_ov020_02113a58[];
extern u32 data_ov020_02113a74[];
extern u32 data_ov020_02113a78[];
extern u32 data_ov020_02113a98[];
extern u32 data_ov020_02113aa0[];
extern u32 data_ov020_02113ab8[];
extern u32 data_ov020_02113abc[];
extern u32 data_ov020_02113ad8[];
extern u32 data_ov020_02113af8[];
extern u32 data_ov020_02113b10[];
extern u32 data_ov020_02113b18[];
extern u32 data_ov020_02113b2c[];
extern u32 data_ov020_02113b40[];
extern u32 data_ov020_02113b50[];
extern u32 data_ov020_02113b68[];
extern u32 data_ov020_02113b7c[];
extern u32 data_ov020_02113b80[];
extern u32 data_ov020_02113b90[];
extern u32 data_ov020_02113b98[];
extern u32 data_ov020_02113bd8[];
extern u32 data_ov020_02113c08[];
extern u32 data_ov020_02113c14[];
extern u32 data_ov020_02113c18[];
extern u32 data_ov020_02113c20[];
extern u32 data_ov020_02113c28[];
extern u32 data_ov020_02113c48[];
extern u32 data_ov020_02113c60[];
extern u32 data_ov020_02113ca8[];
extern u32 data_ov020_02113ccc[];
extern u32 data_ov020_02113cd4[];
extern u32 data_ov020_02113cec[];
extern u32 data_ov020_02113cf0[];
extern u32 data_ov020_02113cf4[];
extern u32 data_ov020_02113d30[];
extern u32 data_ov020_02113d38[];
extern u32 data_ov020_02113d80[];
extern u32 data_ov020_02113d90[];
extern u32 data_ov020_02113da8[];
extern u32 data_ov020_02113dc4[];
extern u32 data_ov020_02113dd0[];
extern u32 data_ov020_02113e00[];
extern u32 data_ov020_02113e20[];
extern u32 data_ov020_02113e50[];
extern u32 data_ov020_02113e6c[];
extern u32 data_ov020_02113e70[];
extern u32 data_ov020_02113e88[];
extern u32 data_ov020_02113ec0[];
extern u32 data_ov020_02113f10[];
extern u32 data_ov020_02113f20[];
extern u32 data_ov020_02113f4c[];
extern u32 data_ov020_02113f60[];
extern u32 data_ov020_02113f78[];
extern u32 data_ov020_02113fb0[];
extern u32 data_ov020_02113ff4[];
extern u32 data_ov020_02114004[];
extern u32 data_ov020_02114010[];
extern u32 data_ov020_02114064[];
extern u32 data_ov020_021140b8[];
extern u32 data_ov020_021140d4[];
extern u32 data_ov020_021140ec[];
extern u32 data_ov020_021140f8[];
extern u32 data_ov020_02114108[];
extern u32 data_ov020_0211417c[];
extern u32 data_ov020_02114184[];
extern u32 data_ov020_021141cc[];
extern u32 data_ov020_021142a0[];
extern u32 data_ov020_021142ac[];
extern u32 data_ov020_0211433c[];
extern u32 data_ov020_02114350[];
extern u32 data_ov020_02114380[];
extern u32 data_ov020_021143d8[];
extern u32 data_ov020_021143fc[];
extern u32 data_ov020_02114458[];
extern u32 data_ov020_02114498[];
extern u32 data_ov020_02114508[];
extern u32 data_ov020_0211454c[];
extern u32 data_ov020_02114568[];
extern u32 data_ov020_02114598[];
extern u32 data_ov020_021145e4[];
extern u32 data_ov020_0211462c[];
extern u32 data_ov020_02114668[];
extern u32 data_ov020_021146f0[];
extern u32 data_ov020_02114750[];
extern u32 data_ov020_02114768[];
extern u32 data_ov020_021147c4[];
extern u32 data_ov020_02114824[];


/* Words the overlay relocates point here. */

/* 0x021134e4..0x02113674 (400 bytes, 100 words) */
#pragma explicit_zero_data on
#pragma define_section s_021134e4 ".rodata.data_ov020_021134e4" ".rodata.data_ov020_021134e4" ".rodata.data_ov020_021134e4" far_data
#pragma section s_021134e4 begin
u32 data_ov020_021134e4[1] = {
    0x00000000,
};
#pragma section s_021134e4 end
#pragma explicit_zero_data off

#pragma define_section s_021134e8 ".rodata.data_ov020_021134e8" ".rodata.data_ov020_021134e8" ".rodata.data_ov020_021134e8" far_data
#pragma section s_021134e8 begin
u32 data_ov020_021134e8[1] = {
    0x00001000,
};
#pragma section s_021134e8 end

#pragma define_section s_021134ec ".rodata.data_ov020_021134ec" ".rodata.data_ov020_021134ec" ".rodata.data_ov020_021134ec" far_data
#pragma section s_021134ec begin
u32 data_ov020_021134ec[7] = {
    0x0000ffff,
    (u32)data_ov020_021137cc,
    0x00000001,
    0x00000001,
    0x00000001,
    0x0000005b,
    0x0000005b,
};
#pragma section s_021134ec end

#pragma define_section s_02113508 ".rodata.data_ov020_02113508" ".rodata.data_ov020_02113508" ".rodata.data_ov020_02113508" far_data
#pragma section s_02113508 begin
u32 data_ov020_02113508[4] = {
    0x00000000,
    0x0000002e,
    0x0000005b,
    0x00000089,
};
#pragma section s_02113508 end

#pragma define_section s_02113518 ".rodata.data_ov020_02113518" ".rodata.data_ov020_02113518" ".rodata.data_ov020_02113518" far_data
#pragma section s_02113518 begin
u32 data_ov020_02113518[7] = {
    0x000000b6,
    0x000000e4,
    0x00000111,
    0x0000013f,
    0x0000016c,
    0x0000019a,
    0x000001c7,
};
#pragma section s_02113518 end

#pragma define_section s_02113534 ".rodata.data_ov020_02113534" ".rodata.data_ov020_02113534" ".rodata.data_ov020_02113534" far_data
#pragma section s_02113534 begin
u32 data_ov020_02113534[8] = {
    0x000001f5,
    0x00000222,
    0x00000250,
    0x0000027d,
    0x000002ab,
    0x000002d8,
    0x00000306,
    0x00000333,
};
#pragma section s_02113534 end

#pragma define_section s_02113554 ".rodata.data_ov020_02113554" ".rodata.data_ov020_02113554" ".rodata.data_ov020_02113554" far_data
#pragma section s_02113554 begin
u32 data_ov020_02113554[32] = {
    0x00000361,
    0x0000038e,
    0x000003bc,
    0x000003e9,
    0x00000417,
    0x00000444,
    0x00000472,
    0x0000049f,
    0x000004cd,
    0x000004fa,
    0x00000528,
    0x00000555,
    0x00000583,
    0x000005b0,
    0x000005de,
    0x0000060b,
    0x00000639,
    0x00000666,
    0x00000694,
    0x000006c1,
    0x000006ef,
    0x0000071c,
    0x0000074a,
    0x00000777,
    0x000007a5,
    0x000007d2,
    0x00000800,
    0x0000082e,
    0x0000085b,
    0x00000889,
    0x000008b6,
    0x000008e4,
};
#pragma section s_02113554 end

#pragma define_section s_021135d4 ".rodata.data_ov020_021135d4" ".rodata.data_ov020_021135d4" ".rodata.data_ov020_021135d4" far_data
#pragma section s_021135d4 begin
u32 data_ov020_021135d4[8] = {
    0x00000911,
    0x0000093f,
    0x0000096c,
    0x0000099a,
    0x000009c7,
    0x000009f5,
    0x00000a22,
    0x00000a50,
};
#pragma section s_021135d4 end

#pragma define_section s_021135f4 ".rodata.data_ov020_021135f4" ".rodata.data_ov020_021135f4" ".rodata.data_ov020_021135f4" far_data
#pragma section s_021135f4 begin
u32 data_ov020_021135f4[32] = {
    0x00000a7d,
    0x00000aab,
    0x00000ad8,
    0x00000b06,
    0x00000b33,
    0x00000b61,
    0x00000b8e,
    0x00000bbc,
    0x00000be9,
    0x00000c17,
    0x00000c44,
    0x00000c72,
    0x00000c9f,
    0x00000ccd,
    0x00000cfa,
    0x00000d28,
    0x00000d55,
    0x00000d83,
    0x00000db0,
    0x00000dde,
    0x00000e0b,
    0x00000e39,
    0x00000e66,
    0x00000e94,
    0x00000ec1,
    0x00000eef,
    0x00000f1c,
    0x00000f4a,
    0x00000f77,
    0x00000fa5,
    0x00000fd2,
    0x00001000,
};
#pragma section s_021135f4 end


/* 0x02113780..0x02114828 (4264 bytes, 1066 words) */
#pragma define_section s_02113780 ".data.data_ov020_02113780" ".data.data_ov020_02113780" ".data.data_ov020_02113780" far_data
#pragma section s_02113780 begin
u32 data_ov020_02113780[1] = {
    0x00000004,
};
#pragma section s_02113780 end

#pragma define_section s_02113784 ".data.data_ov020_02113784" ".data.data_ov020_02113784" ".data.data_ov020_02113784" far_data
#pragma section s_02113784 begin
u32 data_ov020_02113784[2] = {
    0xff020000,
    0x00004832,
};
#pragma section s_02113784 end

#pragma define_section s_0211378c ".data.data_ov020_0211378c" ".data.data_ov020_0211378c" ".data.data_ov020_0211378c" far_data
#pragma section s_0211378c begin
u32 data_ov020_0211378c[2] = {
    0x00000002,
    (u32)data_ov020_0211388c,
};
#pragma section s_0211378c end

#pragma define_section s_02113794 ".data.data_ov020_02113794" ".data.data_ov020_02113794" ".data.data_ov020_02113794" far_data
#pragma section s_02113794 begin
u32 data_ov020_02113794[2] = {
    0x00000009,
    (u32)data_ov020_02113c18,
};
#pragma section s_02113794 end

#pragma define_section s_0211379c ".data.data_ov020_0211379c" ".data.data_ov020_0211379c" ".data.data_ov020_0211379c" far_data
#pragma section s_0211379c begin
u32 data_ov020_0211379c[2] = {
    0x00000009,
    (u32)data_ov020_02113c60,
};
#pragma section s_0211379c end

#pragma define_section s_021137a4 ".data.data_ov020_021137a4" ".data.data_ov020_021137a4" ".data.data_ov020_021137a4" far_data
#pragma section s_021137a4 begin
u32 data_ov020_021137a4[2] = {
    0x00000002,
    (u32)data_ov020_0211386c,
};
#pragma section s_021137a4 end

#pragma define_section s_021137ac ".data.data_ov020_021137ac" ".data.data_ov020_021137ac" ".data.data_ov020_021137ac" far_data
#pragma section s_021137ac begin
u32 data_ov020_021137ac[2] = {
    0x00000009,
    (u32)data_ov020_02113ca8,
};
#pragma section s_021137ac end

#pragma define_section s_021137b4 ".data.data_ov020_021137b4" ".data.data_ov020_021137b4" ".data.data_ov020_021137b4" far_data
#pragma section s_021137b4 begin
u32 data_ov020_021137b4[2] = {
    0x00000009,
    (u32)data_ov020_02113cf0,
};
#pragma section s_021137b4 end

#pragma define_section s_021137bc ".data.data_ov020_021137bc" ".data.data_ov020_021137bc" ".data.data_ov020_021137bc" far_data
#pragma section s_021137bc begin
u32 data_ov020_021137bc[2] = {
    0x00000009,
    (u32)data_ov020_02113d38,
};
#pragma section s_021137bc end

#pragma define_section s_021137c4 ".data.data_ov020_021137c4" ".data.data_ov020_021137c4" ".data.data_ov020_021137c4" far_data
#pragma section s_021137c4 begin
u32 data_ov020_021137c4[2] = {
    0x00000002,
    (u32)data_ov020_0211387c,
};
#pragma section s_021137c4 end

#pragma define_section s_021137cc ".data.data_ov020_021137cc" ".data.data_ov020_021137cc" ".data.data_ov020_021137cc" far_data
#pragma section s_021137cc begin
u32 data_ov020_021137cc[3] = {
    0x775f6874,
    0x72657461,
    0x00000000,
};
#pragma section s_021137cc end

#pragma define_section s_021137d8 ".data.data_ov020_021137d8" ".data.data_ov020_021137d8" ".data.data_ov020_021137d8" far_data
#pragma section s_021137d8 begin
u32 data_ov020_021137d8[3] = {
    0x07d00af0,
    0x050007d0,
    0x000007d0,
};
#pragma section s_021137d8 end

#pragma define_section s_021137e4 ".data.data_ov020_021137e4" ".data.data_ov020_021137e4" ".data.data_ov020_021137e4" far_data
#pragma section s_021137e4 begin
u32 data_ov020_021137e4[3] = {
    0x07ce07cd,
    0x07d007cf,
    0x000007d1,
};
#pragma section s_021137e4 end

#pragma define_section s_021137f0 ".data.data_ov020_021137f0" ".data.data_ov020_021137f0" ".data.data_ov020_021137f0" far_data
#pragma section s_021137f0 begin
u32 data_ov020_021137f0[2] = {
    0x062cfbb4,
    0xf7fefa29,
};
#pragma section s_021137f0 end

#pragma define_section s_021137f8 ".data.data_ov020_021137f8" ".data.data_ov020_021137f8" ".data.data_ov020_021137f8" far_data
#pragma section s_021137f8 begin
u32 data_ov020_021137f8[1] = {
    0xfa29062c,
};
#pragma section s_021137f8 end

#pragma define_section s_021137fc ".data.data_ov020_021137fc" ".data.data_ov020_021137fc" ".data.data_ov020_021137fc" far_data
#pragma section s_021137fc begin
u32 data_ov020_021137fc[1] = {
    0x070807d0,
};
#pragma section s_021137fc end

#pragma define_section s_02113800 ".data.data_ov020_02113800" ".data.data_ov020_02113800" ".data.data_ov020_02113800" far_data
#pragma section s_02113800 begin
u32 data_ov020_02113800[1] = {
    0x000003b6,
};
#pragma section s_02113800 end

#pragma define_section s_02113804 ".data.data_ov020_02113804" ".data.data_ov020_02113804" ".data.data_ov020_02113804" far_data
#pragma section s_02113804 begin
u32 data_ov020_02113804[2] = {
    0x002ec000,
    0x00002204,
};
#pragma section s_02113804 end

#pragma define_section s_0211380c ".data.data_ov020_0211380c" ".data.data_ov020_0211380c" ".data.data_ov020_0211380c" far_data
#pragma section s_0211380c begin
u32 data_ov020_0211380c[4] = {
    0xff9c0043,
    0xfe0c0032,
    0x00000000,
    0x01000000,
};
#pragma section s_0211380c end

#pragma define_section s_0211381c ".data.data_ov020_0211381c" ".data.data_ov020_0211381c" ".data.data_ov020_0211381c" far_data
#pragma section s_0211381c begin
u32 data_ov020_0211381c[1] = {
    0xff9c0043,
};
#pragma section s_0211381c end

#pragma define_section s_02113820 ".data.data_ov020_02113820" ".data.data_ov020_02113820" ".data.data_ov020_02113820" far_data
#pragma section s_02113820 begin
u32 data_ov020_02113820[3] = {
    0xfe0c0032,
    0x00000000,
    0x01000000,
};
#pragma section s_02113820 end

#pragma define_section s_0211382c ".data.data_ov020_0211382c" ".data.data_ov020_0211382c" ".data.data_ov020_0211382c" far_data
#pragma section s_0211382c begin
u32 data_ov020_0211382c[4] = {
    0xff9c0043,
    0xfe0c0032,
    0x00000000,
    0x01000000,
};
#pragma section s_0211382c end

#pragma define_section s_0211383c ".data.data_ov020_0211383c" ".data.data_ov020_0211383c" ".data.data_ov020_0211383c" far_data
#pragma section s_0211383c begin
u32 data_ov020_0211383c[4] = {
    0x00c88226,
    0xfa880320,
    0xfe0ce5ff,
    0xfc7c0258,
};
#pragma section s_0211383c end

#pragma define_section s_0211384c ".data.data_ov020_0211384c" ".data.data_ov020_0211384c" ".data.data_ov020_0211384c" far_data
#pragma section s_0211384c begin
u32 data_ov020_0211384c[4] = {
    0xff9c0043,
    0xfe0c0032,
    0x00000000,
    0x01000000,
};
#pragma section s_0211384c end

#pragma define_section s_0211385c ".data.data_ov020_0211385c" ".data.data_ov020_0211385c" ".data.data_ov020_0211385c" far_data
#pragma section s_0211385c begin
u32 data_ov020_0211385c[4] = {
    0xfe708226,
    0x003201f4,
    0xfa888226,
    0x03b601f4,
};
#pragma section s_0211385c end

#pragma define_section s_0211386c ".data.data_ov020_0211386c" ".data.data_ov020_0211386c" ".data.data_ov020_0211386c" far_data
#pragma section s_0211386c begin
u32 data_ov020_0211386c[4] = {
    0x00000700,
    (u32)data_ov020_02114064,
    0x00000305,
    (u32)data_ov020_021138e4,
};
#pragma section s_0211386c end

#pragma define_section s_0211387c ".data.data_ov020_0211387c" ".data.data_ov020_0211387c" ".data.data_ov020_0211387c" far_data
#pragma section s_0211387c begin
u32 data_ov020_0211387c[4] = {
    0x00000500,
    (u32)data_ov020_02113f10,
    0x00000605,
    (u32)data_ov020_02113b68,
};
#pragma section s_0211387c end

#pragma define_section s_0211388c ".data.data_ov020_0211388c" ".data.data_ov020_0211388c" ".data.data_ov020_0211388c" far_data
#pragma section s_0211388c begin
u32 data_ov020_0211388c[4] = {
    0x00001300,
    (u32)data_ov020_021143d8,
    0x00000505,
    (u32)data_ov020_02113b18,
};
#pragma section s_0211388c end

#pragma define_section s_0211389c ".data.data_ov020_0211389c" ".data.data_ov020_0211389c" ".data.data_ov020_0211389c" far_data
#pragma section s_0211389c begin
u32 data_ov020_0211389c[3] = {
    0xff9c0043,
    0xfe0c0032,
    0x00000000,
};
#pragma section s_0211389c end

#pragma define_section s_021138a8 ".data.data_ov020_021138a8" ".data.data_ov020_021138a8" ".data.data_ov020_021138a8" far_data
#pragma section s_021138a8 begin
u32 data_ov020_021138a8[1] = {
    0x01000000,
};
#pragma section s_021138a8 end

#pragma define_section s_021138ac ".data.data_ov020_021138ac" ".data.data_ov020_021138ac" ".data.data_ov020_021138ac" far_data
#pragma section s_021138ac begin
u32 data_ov020_021138ac[4] = {
    0xff9c0043,
    0xfe0c0032,
    0x00000000,
    0x01020000,
};
#pragma section s_021138ac end

#pragma define_section s_021138bc ".data.data_ov020_021138bc" ".data.data_ov020_021138bc" ".data.data_ov020_021138bc" far_data
#pragma section s_021138bc begin
u32 data_ov020_021138bc[2] = {
    0xff9c0043,
    0xfe0c0032,
};
#pragma section s_021138bc end

#pragma define_section s_021138c4 ".data.data_ov020_021138c4" ".data.data_ov020_021138c4" ".data.data_ov020_021138c4" far_data
#pragma section s_021138c4 begin
u32 data_ov020_021138c4[2] = {
    0x00000000,
    0x01000000,
};
#pragma section s_021138c4 end

#pragma define_section s_021138cc ".data.data_ov020_021138cc" ".data.data_ov020_021138cc" ".data.data_ov020_021138cc" far_data
#pragma section s_021138cc begin
u32 data_ov020_021138cc[3] = {
    0x0000005b,
    (u32)data_ov020_021134e8,
    (u32)data_ov020_021134e4,
};
#pragma section s_021138cc end

#pragma define_section s_021138d8 ".data.data_ov020_021138d8" ".data.data_ov020_021138d8" ".data.data_ov020_021138d8" far_data
#pragma section s_021138d8 begin
u32 data_ov020_021138d8[3] = {
    (u32)data_ov020_02113508,
    0x00000001,
    (u32)data_ov020_021134ec,
};
#pragma section s_021138d8 end

#pragma define_section s_021138e4 ".data.data_ov020_021138e4" ".data.data_ov020_021138e4" ".data.data_ov020_021138e4" far_data
#pragma section s_021138e4 begin
u32 data_ov020_021138e4[3] = {
    0xfb508226,
    0x08fc0000,
    0xf830063d,
};
#pragma section s_021138e4 end

#pragma define_section s_021138f0 ".data.data_ov020_021138f0" ".data.data_ov020_021138f0" ".data.data_ov020_021138f0" far_data
#pragma section s_021138f0 begin
u32 data_ov020_021138f0[3] = {
    0x076c04b0,
    0xf830e5ff,
    0x05780258,
};
#pragma section s_021138f0 end

#pragma define_section s_021138fc ".data.data_ov020_021138fc" ".data.data_ov020_021138fc" ".data.data_ov020_021138fc" far_data
#pragma section s_021138fc begin
u32 data_ov020_021138fc[7] = {
    (u32)data_ov020_02114668,
    (u32)data_ov020_021137b4,
    0x07cc07d2,
    0x07d407d3,
    (u32)data_ov020_02113fb0,
    0x05820007,
    0x0000005f,
};
#pragma section s_021138fc end

#pragma define_section s_02113918 ".data.data_ov020_02113918" ".data.data_ov020_02113918" ".data.data_ov020_02113918" far_data
#pragma section s_02113918 begin
u32 data_ov020_02113918[7] = {
    0x060e0014,
    0x06a40000,
    0xc00000ff,
    0x0b270000,
    0x03b60046,
    0x03840000,
    0x00000000,
};
#pragma section s_02113918 end

#pragma define_section s_02113938 ".data.data_ov020_02113938" ".data.data_ov020_02113938" ".data.data_ov020_02113938" far_data
#pragma section s_02113938 begin
u32 data_ov020_02113938[8] = {
    0x03b60046,
    0x03840000,
    0x00000000,
    0x00000000,
    0x060e0014,
    0x06a40000,
    0xc00000ff,
    0x0b270000,
};
#pragma section s_02113938 end

#pragma define_section s_02113958 ".data.data_ov020_02113958" ".data.data_ov020_02113958" ".data.data_ov020_02113958" far_data
#pragma section s_02113958 begin
u32 data_ov020_02113958[8] = {
    0x03b60046,
    0x03840000,
    0x00000000,
    0x00000000,
    0x060e0014,
    0x06a40000,
    0xc00000ff,
    0x0b270000,
};
#pragma section s_02113958 end

#pragma define_section s_02113978 ".data.data_ov020_02113978" ".data.data_ov020_02113978" ".data.data_ov020_02113978" far_data
#pragma section s_02113978 begin
u32 data_ov020_02113978[2] = {
    0x03b60046,
    0x03840000,
};
#pragma section s_02113978 end

#pragma define_section s_02113980 ".data.data_ov020_02113980" ".data.data_ov020_02113980" ".data.data_ov020_02113980" far_data
#pragma section s_02113980 begin
u32 data_ov020_02113980[6] = {
    0x00000000,
    0x00000000,
    0x060e0014,
    0x06a40000,
    0xc00000ff,
    0x0b270000,
};
#pragma section s_02113980 end

#pragma define_section s_02113998 ".data.data_ov020_02113998" ".data.data_ov020_02113998" ".data.data_ov020_02113998" far_data
#pragma section s_02113998 begin
u32 data_ov020_02113998[6] = {
    0x0bb80043,
    0x01900032,
    0x00000000,
    0x01020000,
    0x0abe0043,
    0x0866003c,
};
#pragma section s_02113998 end

#pragma define_section s_021139b0 ".data.data_ov020_021139b0" ".data.data_ov020_021139b0" ".data.data_ov020_021139b0" far_data
#pragma section s_021139b0 begin
u32 data_ov020_021139b0[2] = {
    0x95550000,
    0x01020000,
};
#pragma section s_021139b0 end

#pragma define_section s_021139b8 ".data.data_ov020_021139b8" ".data.data_ov020_021139b8" ".data.data_ov020_021139b8" far_data
#pragma section s_021139b8 begin
u32 data_ov020_021139b8[8] = {
    0x03b60046,
    0x03840000,
    0x00000000,
    0x00000000,
    0x060e0014,
    0x06a40000,
    0xc00000ff,
    0x0b270000,
};
#pragma section s_021139b8 end

#pragma define_section s_021139d8 ".data.data_ov020_021139d8" ".data.data_ov020_021139d8" ".data.data_ov020_021139d8" far_data
#pragma section s_021139d8 begin
u32 data_ov020_021139d8[7] = {
    0x03b60046,
    0x03840000,
    0x00000000,
    0x00010000,
    0x03b60044,
    0x03840096,
    0x00000000,
};
#pragma section s_021139d8 end

#pragma define_section s_021139f4 ".data.data_ov020_021139f4" ".data.data_ov020_021139f4" ".data.data_ov020_021139f4" far_data
#pragma section s_021139f4 begin
u32 data_ov020_021139f4[1] = {
    0x01020000,
};
#pragma section s_021139f4 end

#pragma define_section s_021139f8 ".data.data_ov020_021139f8" ".data.data_ov020_021139f8" ".data.data_ov020_021139f8" far_data
#pragma section s_021139f8 begin
u32 data_ov020_021139f8[2] = {
    0x03b60046,
    0x03840000,
};
#pragma section s_021139f8 end

#pragma define_section s_02113a00 ".data.data_ov020_02113a00" ".data.data_ov020_02113a00" ".data.data_ov020_02113a00" far_data
#pragma section s_02113a00 begin
u32 data_ov020_02113a00[6] = {
    0x00000000,
    0x00000000,
    0x060e0014,
    0x06a40000,
    0xc00000ff,
    0x0b270000,
};
#pragma section s_02113a00 end

#pragma define_section s_02113a18 ".data.data_ov020_02113a18" ".data.data_ov020_02113a18" ".data.data_ov020_02113a18" far_data
#pragma section s_02113a18 begin
u32 data_ov020_02113a18[8] = {
    0xf8f80043,
    0xfce00032,
    0x00000000,
    0x2f020000,
    0xfa100043,
    0xfb500032,
    0x00000000,
    0x01020000,
};
#pragma section s_02113a18 end

#pragma define_section s_02113a38 ".data.data_ov020_02113a38" ".data.data_ov020_02113a38" ".data.data_ov020_02113a38" far_data
#pragma section s_02113a38 begin
u32 data_ov020_02113a38[6] = {
    0xf8f80043,
    0xfce00032,
    0x00000000,
    0x21000000,
    0xfa100043,
    0xfb500032,
};
#pragma section s_02113a38 end

#pragma define_section s_02113a50 ".data.data_ov020_02113a50" ".data.data_ov020_02113a50" ".data.data_ov020_02113a50" far_data
#pragma section s_02113a50 begin
u32 data_ov020_02113a50[2] = {
    0x00000000,
    0x01000000,
};
#pragma section s_02113a50 end

#pragma define_section s_02113a58 ".data.data_ov020_02113a58" ".data.data_ov020_02113a58" ".data.data_ov020_02113a58" far_data
#pragma section s_02113a58 begin
u32 data_ov020_02113a58[7] = {
    0xf8f80043,
    0xfce00032,
    0x00000000,
    0x21000000,
    0xfa100043,
    0xfb500032,
    0x00000000,
};
#pragma section s_02113a58 end

#pragma define_section s_02113a74 ".data.data_ov020_02113a74" ".data.data_ov020_02113a74" ".data.data_ov020_02113a74" far_data
#pragma section s_02113a74 begin
u32 data_ov020_02113a74[1] = {
    0x01000000,
};
#pragma section s_02113a74 end

#pragma define_section s_02113a78 ".data.data_ov020_02113a78" ".data.data_ov020_02113a78" ".data.data_ov020_02113a78" far_data
#pragma section s_02113a78 begin
u32 data_ov020_02113a78[8] = {
    0xf8f80043,
    0xfce00032,
    0x00000000,
    0x21000000,
    0xfa100043,
    0xfb500032,
    0x00000000,
    0x01000000,
};
#pragma section s_02113a78 end

#pragma define_section s_02113a98 ".data.data_ov020_02113a98" ".data.data_ov020_02113a98" ".data.data_ov020_02113a98" far_data
#pragma section s_02113a98 begin
u32 data_ov020_02113a98[2] = {
    0x012c0096,
    0xfce00078,
};
#pragma section s_02113a98 end

#pragma define_section s_02113aa0 ".data.data_ov020_02113aa0" ".data.data_ov020_02113aa0" ".data.data_ov020_02113aa0" far_data
#pragma section s_02113aa0 begin
u32 data_ov020_02113aa0[6] = {
    0x00000000,
    0xffff0000,
    0xfe0c0108,
    0xfaec0112,
    0x20000000,
    0xffff0000,
};
#pragma section s_02113aa0 end

#pragma define_section s_02113ab8 ".data.data_ov020_02113ab8" ".data.data_ov020_02113ab8" ".data.data_ov020_02113ab8" far_data
#pragma section s_02113ab8 begin
u32 data_ov020_02113ab8[1] = {
    0xf8f80043,
};
#pragma section s_02113ab8 end

#pragma define_section s_02113abc ".data.data_ov020_02113abc" ".data.data_ov020_02113abc" ".data.data_ov020_02113abc" far_data
#pragma section s_02113abc begin
u32 data_ov020_02113abc[7] = {
    0xfce00032,
    0x00000000,
    0x21000000,
    0xfa100043,
    0xfb500032,
    0x00000000,
    0x01000000,
};
#pragma section s_02113abc end

#pragma define_section s_02113ad8 ".data.data_ov020_02113ad8" ".data.data_ov020_02113ad8" ".data.data_ov020_02113ad8" far_data
#pragma section s_02113ad8 begin
u32 data_ov020_02113ad8[8] = {
    0xf8f80043,
    0xfce00032,
    0x00000000,
    0x21000000,
    0xfa100043,
    0xfb500032,
    0x00000000,
    0x01000000,
};
#pragma section s_02113ad8 end

#pragma define_section s_02113af8 ".data.data_ov020_02113af8" ".data.data_ov020_02113af8" ".data.data_ov020_02113af8" far_data
#pragma section s_02113af8 begin
u32 data_ov020_02113af8[6] = {
    0xf8f80043,
    0xfce00032,
    0x00000000,
    0x21000000,
    0xfa100043,
    0xfb500032,
};
#pragma section s_02113af8 end

#pragma define_section s_02113b10 ".data.data_ov020_02113b10" ".data.data_ov020_02113b10" ".data.data_ov020_02113b10" far_data
#pragma section s_02113b10 begin
u32 data_ov020_02113b10[2] = {
    0x00000000,
    0x01000000,
};
#pragma section s_02113b10 end

#pragma define_section s_02113b18 ".data.data_ov020_02113b18" ".data.data_ov020_02113b18" ".data.data_ov020_02113b18" far_data
#pragma section s_02113b18 begin
u32 data_ov020_02113b18[5] = {
    0x028a4a3f,
    0x076c0b21,
    0x015ee225,
    0x012c0c80,
    0x0226e225,
};
#pragma section s_02113b18 end

#pragma define_section s_02113b2c ".data.data_ov020_02113b2c" ".data.data_ov020_02113b2c" ".data.data_ov020_02113b2c" far_data
#pragma section s_02113b2c begin
u32 data_ov020_02113b2c[5] = {
    0x012c0c80,
    0x02eee225,
    0x012c0c80,
    0x03b6e225,
    0x012c0c80,
};
#pragma section s_02113b2c end

