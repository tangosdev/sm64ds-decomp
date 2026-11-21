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
#pragma define_section s_02113b40 ".data.data_ov020_02113b40" ".data.data_ov020_02113b40" ".data.data_ov020_02113b40" far_data
#pragma section s_02113b40 begin
u32 data_ov020_02113b40[4] = {
    0x03e88226,
    0x044c0514,
    0x0a288e26,
    0xfe0c0320,
};
#pragma section s_02113b40 end

#pragma define_section s_02113b50 ".data.data_ov020_02113b50" ".data.data_ov020_02113b50" ".data.data_ov020_02113b50" far_data
#pragma section s_02113b50 begin
u32 data_ov020_02113b50[6] = {
    0x03b1423f,
    0x00fa0352,
    0xffb0083f,
    0x06400352,
    0x0320e5ff,
    0x00c80258,
};
#pragma section s_02113b50 end

#pragma define_section s_02113b68 ".data.data_ov020_02113b68" ".data.data_ov020_02113b68" ".data.data_ov020_02113b68" far_data
#pragma section s_02113b68 begin
u32 data_ov020_02113b68[5] = {
    0x06a44c3f,
    0xfe7006d6,
    0x08fce7ff,
    0x019005dc,
    0x05780227,
};
#pragma section s_02113b68 end

#pragma define_section s_02113b7c ".data.data_ov020_02113b7c" ".data.data_ov020_02113b7c" ".data.data_ov020_02113b7c" far_data
#pragma section s_02113b7c begin
u32 data_ov020_02113b7c[1] = {
    0x04b006a4,
};
#pragma section s_02113b7c end

#pragma define_section s_02113b80 ".data.data_ov020_02113b80" ".data.data_ov020_02113b80" ".data.data_ov020_02113b80" far_data
#pragma section s_02113b80 begin
u32 data_ov020_02113b80[4] = {
    0x07080227,
    0x04b006a4,
    0x00640227,
    0x04b006a4,
};
#pragma section s_02113b80 end

#pragma define_section s_02113b90 ".data.data_ov020_02113b90" ".data.data_ov020_02113b90" ".data.data_ov020_02113b90" far_data
#pragma section s_02113b90 begin
u32 data_ov020_02113b90[2] = {
    0xfe700227,
    0x04b006a4,
};
#pragma section s_02113b90 end

#pragma define_section s_02113b98 ".data.data_ov020_02113b98" ".data.data_ov020_02113b98" ".data.data_ov020_02113b98" far_data
#pragma section s_02113b98 begin
u32 data_ov020_02113b98[16] = {
    0x0af08226,
    0x08980000,
    0x09928226,
    0x08980320,
    0x04b0443f,
    0x00c8f6d2,
    0x0af0e3ff,
    0x06a4fc18,
    0x0af0e5ff,
    0x05dc0258,
    0x02bcce3f,
    0x06d6f768,
    0x04b0ee3c,
    0x06a4f768,
    0xf60ae225,
    0x0672f7fe,
};
#pragma section s_02113b98 end

#pragma define_section s_02113bd8 ".data.data_ov020_02113bd8" ".data.data_ov020_02113bd8" ".data.data_ov020_02113bd8" far_data
#pragma section s_02113bd8 begin
u32 data_ov020_02113bd8[12] = {
    0xf830001e,
    0x000000be,
    0x00000000,
    0xff060000,
    0xf8f80126,
    0x00500000,
    0x40000000,
    0xffff0000,
    0xfbb40126,
    0x00000000,
    0xc0000000,
    0xffff0000,
};
#pragma section s_02113bd8 end

#pragma define_section s_02113c08 ".data.data_ov020_02113c08" ".data.data_ov020_02113c08" ".data.data_ov020_02113c08" far_data
#pragma section s_02113c08 begin
u32 data_ov020_02113c08[3] = {
    0xfa88011f,
    0x03b60113,
    0x80000000,
};
#pragma section s_02113c08 end

#pragma define_section s_02113c14 ".data.data_ov020_02113c14" ".data.data_ov020_02113c14" ".data.data_ov020_02113c14" far_data
#pragma section s_02113c14 begin
u32 data_ov020_02113c14[1] = {
    0xffff0000,
};
#pragma section s_02113c14 end

#pragma define_section s_02113c18 ".data.data_ov020_02113c18" ".data.data_ov020_02113c18" ".data.data_ov020_02113c18" far_data
#pragma section s_02113c18 begin
u32 data_ov020_02113c18[2] = {
    0x00000400,
    (u32)data_ov020_02113bd8,
};
#pragma section s_02113c18 end

#pragma define_section s_02113c20 ".data.data_ov020_02113c20" ".data.data_ov020_02113c20" ".data.data_ov020_02113c20" far_data
#pragma section s_02113c20 begin
u32 data_ov020_02113c20[2] = {
    0x00000205,
    (u32)data_ov020_0211385c,
};
#pragma section s_02113c20 end

#pragma define_section s_02113c28 ".data.data_ov020_02113c28" ".data.data_ov020_02113c28" ".data.data_ov020_02113c28" far_data
#pragma section s_02113c28 begin
u32 data_ov020_02113c28[8] = {
    0x00000220,
    (u32)data_ov020_02113a18,
    0x00000240,
    (u32)data_ov020_02113a38,
    0x00000260,
    (u32)data_ov020_02113a58,
    0x00000280,
    (u32)data_ov020_02113a78,
};
#pragma section s_02113c28 end

#pragma define_section s_02113c48 ".data.data_ov020_02113c48" ".data.data_ov020_02113c48" ".data.data_ov020_02113c48" far_data
#pragma section s_02113c48 begin
u32 data_ov020_02113c48[6] = {
    0x000002a0,
    (u32)data_ov020_02113ab8,
    0x000002c0,
    (u32)data_ov020_02113ad8,
    0x000002e0,
    (u32)data_ov020_02113af8,
};
#pragma section s_02113c48 end

#pragma define_section s_02113c60 ".data.data_ov020_02113c60" ".data.data_ov020_02113c60" ".data.data_ov020_02113c60" far_data
#pragma section s_02113c60 begin
u32 data_ov020_02113c60[18] = {
    0x00000b00,
    (u32)data_ov020_021140d4,
    0x00000505,
    (u32)data_ov020_02113b40,
    0x00000220,
    (u32)data_ov020_021139d8,
    0x00000240,
    (u32)data_ov020_02113918,
    0x00000260,
    (u32)data_ov020_021139b8,
    0x00000280,
    (u32)data_ov020_02113938,
    0x000002a0,
    (u32)data_ov020_021139f8,
    0x000002c0,
    (u32)data_ov020_02113958,
    0x000002e0,
    (u32)data_ov020_02113978,
};
#pragma section s_02113c60 end

#pragma define_section s_02113ca8 ".data.data_ov020_02113ca8" ".data.data_ov020_02113ca8" ".data.data_ov020_02113ca8" far_data
#pragma section s_02113ca8 begin
u32 data_ov020_02113ca8[9] = {
    0x00000200,
    (u32)data_ov020_02113a98,
    0x00000205,
    (u32)data_ov020_0211383c,
    0x00000120,
    (u32)data_ov020_021138ac,
    0x00000140,
    (u32)data_ov020_021138bc,
    0x00000160,
};
#pragma section s_02113ca8 end

#pragma define_section s_02113ccc ".data.data_ov020_02113ccc" ".data.data_ov020_02113ccc" ".data.data_ov020_02113ccc" far_data
#pragma section s_02113ccc begin
u32 data_ov020_02113ccc[2] = {
    (u32)data_ov020_0211384c,
    0x00000180,
};
#pragma section s_02113ccc end

#pragma define_section s_02113cd4 ".data.data_ov020_02113cd4" ".data.data_ov020_02113cd4" ".data.data_ov020_02113cd4" far_data
#pragma section s_02113cd4 begin
u32 data_ov020_02113cd4[6] = {
    (u32)data_ov020_0211389c,
    0x000001a0,
    (u32)data_ov020_0211380c,
    0x000001c0,
    (u32)data_ov020_0211381c,
    0x000001e0,
};
#pragma section s_02113cd4 end

#pragma define_section s_02113cec ".data.data_ov020_02113cec" ".data.data_ov020_02113cec" ".data.data_ov020_02113cec" far_data
#pragma section s_02113cec begin
u32 data_ov020_02113cec[1] = {
    (u32)data_ov020_0211382c,
};
#pragma section s_02113cec end

#pragma define_section s_02113cf0 ".data.data_ov020_02113cf0" ".data.data_ov020_02113cf0" ".data.data_ov020_02113cf0" far_data
#pragma section s_02113cf0 begin
u32 data_ov020_02113cf0[1] = {
    0x00000202,
};
#pragma section s_02113cf0 end

#pragma define_section s_02113cf4 ".data.data_ov020_02113cf4" ".data.data_ov020_02113cf4" ".data.data_ov020_02113cf4" far_data
#pragma section s_02113cf4 begin
u32 data_ov020_02113cf4[15] = {
    (u32)data_ov020_021137f0,
    0x00000103,
    (u32)data_ov020_02113784,
    0x00001504,
    (u32)data_ov020_02114184,
    0x0000050b,
    (u32)data_ov020_021137e4,
    0x00000501,
    (u32)data_ov020_02113f60,
    0x00001909,
    (u32)data_ov020_021142ac,
    0x0000050c,
    (u32)data_ov020_021137d8,
    0x0000010e,
    (u32)data_ov020_02113780,
};
#pragma section s_02113cf4 end

#pragma define_section s_02113d30 ".data.data_ov020_02113d30" ".data.data_ov020_02113d30" ".data.data_ov020_02113d30" far_data
#pragma section s_02113d30 begin
u32 data_ov020_02113d30[2] = {
    0x0000010a,
    (u32)data_ov020_021137fc,
};
#pragma section s_02113d30 end

#pragma define_section s_02113d38 ".data.data_ov020_02113d38" ".data.data_ov020_02113d38" ".data.data_ov020_02113d38" far_data
#pragma section s_02113d38 begin
u32 data_ov020_02113d38[18] = {
    0x00001600,
    (u32)data_ov020_02114508,
    0x00000805,
    (u32)data_ov020_02113b98,
    0x00000220,
    (u32)data_ov020_02113998,
    0x00000540,
    (u32)data_ov020_02113d80,
    0x00000560,
    (u32)data_ov020_02113dd0,
    0x00000680,
    (u32)data_ov020_02114004,
    0x000005a0,
    (u32)data_ov020_02113e20,
    0x000005c0,
    (u32)data_ov020_02113e70,
    0x000005e0,
    (u32)data_ov020_02113ec0,
};
#pragma section s_02113d38 end

#pragma define_section s_02113d80 ".data.data_ov020_02113d80" ".data.data_ov020_02113d80" ".data.data_ov020_02113d80" far_data
#pragma section s_02113d80 begin
u32 data_ov020_02113d80[4] = {
    0x0bb80043,
    0x01900032,
    0x00000000,
    0x01000000,
};
#pragma section s_02113d80 end

#pragma define_section s_02113d90 ".data.data_ov020_02113d90" ".data.data_ov020_02113d90" ".data.data_ov020_02113d90" far_data
#pragma section s_02113d90 begin
u32 data_ov020_02113d90[6] = {
    0x0abe0043,
    0x0866003c,
    0x95560000,
    0x01000000,
    0xfbbe0044,
    0x0442f6a0,
};
#pragma section s_02113d90 end

#pragma define_section s_02113da8 ".data.data_ov020_02113da8" ".data.data_ov020_02113da8" ".data.data_ov020_02113da8" far_data
#pragma section s_02113da8 begin
u32 data_ov020_02113da8[7] = {
    0x60000000,
    0x02010000,
    0x029e010a,
    0xfdb2f72c,
    0xe0000000,
    0xffff0000,
    0xfbbe0043,
};
#pragma section s_02113da8 end

#pragma define_section s_02113dc4 ".data.data_ov020_02113dc4" ".data.data_ov020_02113dc4" ".data.data_ov020_02113dc4" far_data
#pragma section s_02113dc4 begin
u32 data_ov020_02113dc4[3] = {
    0x0442f6d2,
    0x60000000,
    0xff030000,
};
#pragma section s_02113dc4 end

#pragma define_section s_02113dd0 ".data.data_ov020_02113dd0" ".data.data_ov020_02113dd0" ".data.data_ov020_02113dd0" far_data
#pragma section s_02113dd0 begin
u32 data_ov020_02113dd0[12] = {
    0x0bb80043,
    0x01900032,
    0x00000000,
    0x01000000,
    0x0abe0043,
    0x0866003c,
    0x95560000,
    0x01000000,
    0xfbbe0044,
    0x0442f6a0,
    0x60000000,
    0x02010000,
};
#pragma section s_02113dd0 end

#pragma define_section s_02113e00 ".data.data_ov020_02113e00" ".data.data_ov020_02113e00" ".data.data_ov020_02113e00" far_data
#pragma section s_02113e00 begin
u32 data_ov020_02113e00[8] = {
    0x029e010a,
    0xfdb2f72c,
    0xe0000000,
    0xffff0000,
    0xfbbe0043,
    0x0442f6d2,
    0x60000000,
    0xff030000,
};
#pragma section s_02113e00 end

#pragma define_section s_02113e20 ".data.data_ov020_02113e20" ".data.data_ov020_02113e20" ".data.data_ov020_02113e20" far_data
#pragma section s_02113e20 begin
u32 data_ov020_02113e20[12] = {
    0xfbbe0044,
    0x0442f6a0,
    0x60000000,
    0x02010000,
    0x029e010a,
    0xfdb2f72c,
    0xe0000000,
    0xffff0000,
    0xfbbe0043,
    0x0442f6d2,
    0x60000000,
    0xff030000,
};
#pragma section s_02113e20 end

#pragma define_section s_02113e50 ".data.data_ov020_02113e50" ".data.data_ov020_02113e50" ".data.data_ov020_02113e50" far_data
#pragma section s_02113e50 begin
u32 data_ov020_02113e50[7] = {
    0x0bb80043,
    0x01900032,
    0x00000000,
    0x01000000,
    0x0abe0043,
    0x0866003c,
    0x95560000,
};
#pragma section s_02113e50 end

#pragma define_section s_02113e6c ".data.data_ov020_02113e6c" ".data.data_ov020_02113e6c" ".data.data_ov020_02113e6c" far_data
#pragma section s_02113e6c begin
u32 data_ov020_02113e6c[1] = {
    0x01000000,
};
#pragma section s_02113e6c end

#pragma define_section s_02113e70 ".data.data_ov020_02113e70" ".data.data_ov020_02113e70" ".data.data_ov020_02113e70" far_data
#pragma section s_02113e70 begin
u32 data_ov020_02113e70[6] = {
    0x0bb80043,
    0x01900032,
    0x00000000,
    0x01000000,
    0x0abe0043,
    0x0866003c,
};
#pragma section s_02113e70 end

#pragma define_section s_02113e88 ".data.data_ov020_02113e88" ".data.data_ov020_02113e88" ".data.data_ov020_02113e88" far_data
#pragma section s_02113e88 begin
u32 data_ov020_02113e88[14] = {
    0x95560000,
    0x01000000,
    0x029e010a,
    0xfdb2f72c,
    0xe0000000,
    0xffff0000,
    0xfbbe0043,
    0x0442f6d2,
    0x60000000,
    0xff030000,
    0xfbbe0044,
    0x0442f6a0,
    0x60000000,
    0x02010000,
};
#pragma section s_02113e88 end

#pragma define_section s_02113ec0 ".data.data_ov020_02113ec0" ".data.data_ov020_02113ec0" ".data.data_ov020_02113ec0" far_data
#pragma section s_02113ec0 begin
u32 data_ov020_02113ec0[20] = {
    0x0bb80043,
    0x01900032,
    0x00000000,
    0x01000000,
    0x0abe0043,
    0x0866003c,
    0x95560000,
    0x01000000,
    0x029e010a,
    0xfdb2f72c,
    0xe0000000,
    0xffff0000,
    0xfbbe0043,
    0x0442f6d2,
    0x60000000,
    0xff030000,
    0xfbbe0044,
    0x0442f6a0,
    0x60000000,
    0x02010000,
};
#pragma section s_02113ec0 end

#pragma define_section s_02113f10 ".data.data_ov020_02113f10" ".data.data_ov020_02113f10" ".data.data_ov020_02113f10" far_data
#pragma section s_02113f10 begin
u32 data_ov020_02113f10[4] = {
    0x02bc0131,
    0x032006a4,
    0x00000000,
    0xff000000,
};
#pragma section s_02113f10 end

#pragma define_section s_02113f20 ".data.data_ov020_02113f20" ".data.data_ov020_02113f20" ".data.data_ov020_02113f20" far_data
#pragma section s_02113f20 begin
u32 data_ov020_02113f20[11] = {
    0x05140097,
    0xfea2076c,
    0xc0000000,
    0x00060000,
    0x01d5001e,
    0xfd70079e,
    0x00000000,
    0xff060000,
    0x07d0002a,
    0x03b60708,
    0xc0000000,
};
#pragma section s_02113f20 end

#pragma define_section s_02113f4c ".data.data_ov020_02113f4c" ".data.data_ov020_02113f4c" ".data.data_ov020_02113f4c" far_data
#pragma section s_02113f4c begin
u32 data_ov020_02113f4c[5] = {
    0x0f220000,
    0x03370043,
    0xff3e06d6,
    0xc0000000,
    0xff000000,
};
#pragma section s_02113f4c end

#pragma define_section s_02113f60 ".data.data_ov020_02113f60" ".data.data_ov020_02113f60" ".data.data_ov020_02113f60" far_data
#pragma section s_02113f60 begin
u32 data_ov020_02113f60[6] = {
    0x03840000,
    0x16a805dc,
    0x80000000,
    0x05000000,
    0x02bc0000,
    0x15e005dc,
};
#pragma section s_02113f60 end

#pragma define_section s_02113f78 ".data.data_ov020_02113f78" ".data.data_ov020_02113f78" ".data.data_ov020_02113f78" far_data
#pragma section s_02113f78 begin
u32 data_ov020_02113f78[14] = {
    0x80000000,
    0x05000000,
    0x01f40000,
    0x16a805dc,
    0x80000000,
    0x05000000,
    0x02bc0000,
    0x177005dc,
    0x80000000,
    0x05000000,
    0x07d00000,
    0x03b6076c,
    0xc0000000,
    0x059c0000,
};
#pragma section s_02113f78 end

#pragma define_section s_02113fb0 ".data.data_ov020_02113fb0" ".data.data_ov020_02113fb0" ".data.data_ov020_02113fb0" far_data
#pragma section s_02113fb0 begin
u32 data_ov020_02113fb0[17] = {
    (u32)data_ov020_0211378c,
    0x00000000,
    0x10cc0000,
    (u32)data_ov020_0211379c,
    0x00000000,
    0x06400101,
    (u32)data_ov020_021137bc,
    (u32)data_ov020_021138cc,
    0x0c8b0104,
    (u32)data_ov020_021137a4,
    0x00000000,
    0x06400101,
    (u32)data_ov020_021137c4,
    0x00000000,
    0x02bc0103,
    (u32)data_ov020_021137ac,
    0x00000000,
};
#pragma section s_02113fb0 end

#pragma define_section s_02113ff4 ".data.data_ov020_02113ff4" ".data.data_ov020_02113ff4" ".data.data_ov020_02113ff4" far_data
#pragma section s_02113ff4 begin
u32 data_ov020_02113ff4[4] = {
    0x06400101,
    (u32)data_ov020_02113794,
    0x00000000,
    0x01900101,
};
#pragma section s_02113ff4 end

#pragma define_section s_02114004 ".data.data_ov020_02114004" ".data.data_ov020_02114004" ".data.data_ov020_02114004" far_data
#pragma section s_02114004 begin
u32 data_ov020_02114004[3] = {
    0x0bb80043,
    0x01900032,
    0x00000000,
};
#pragma section s_02114004 end

#pragma define_section s_02114010 ".data.data_ov020_02114010" ".data.data_ov020_02114010" ".data.data_ov020_02114010" far_data
#pragma section s_02114010 begin
u32 data_ov020_02114010[21] = {
    0x01000000,
    0x0abe0043,
    0x0866003c,
    0x95560000,
    0x01000000,
    0x051b0014,
    0xfaf90000,
    0x20000003,
    0xffff0000,
    0xfbbe0044,
    0x0442f6a0,
    0x60000000,
    0x02010000,
    0x029e010a,
    0xfdb2f72c,
    0xe0000000,
    0xffff0000,
    0xfbbe0043,
    0x0442f6d2,
    0x60000000,
    0xff030000,
};
#pragma section s_02114010 end

#pragma define_section s_02114064 ".data.data_ov020_02114064" ".data.data_ov020_02114064" ".data.data_ov020_02114064" far_data
#pragma section s_02114064 begin
u32 data_ov020_02114064[21] = {
    0xf8620047,
    0x00c80320,
    0x00000000,
    0xffff0000,
    0xfaec003a,
    0x08340000,
    0xe71d0000,
    0xffff0000,
    0xfa0d0126,
    0x07860000,
    0x238e0000,
    0xffff0000,
    0xfa880120,
    0x012c0352,
    0x40000000,
    0xffff0000,
    0xfd440120,
    0xff9c0352,
    0xc0000000,
    0xffff0000,
    0xfa880120,
};
#pragma section s_02114064 end

#pragma define_section s_021140b8 ".data.data_ov020_021140b8" ".data.data_ov020_021140b8" ".data.data_ov020_021140b8" far_data
#pragma section s_021140b8 begin
u32 data_ov020_021140b8[7] = {
    0xfda80352,
    0x40000000,
    0xffff0000,
    0xfd440120,
    0xfd120352,
    0xc0000000,
    0xffff0000,
};
#pragma section s_021140b8 end

#pragma define_section s_021140d4 ".data.data_ov020_021140d4" ".data.data_ov020_021140d4" ".data.data_ov020_021140d4" far_data
#pragma section s_021140d4 begin
u32 data_ov020_021140d4[6] = {
    0x0a2800ab,
    0xfb500320,
    0x00000000,
    0xff010000,
    0x0b5400ab,
    0xfb500320,
};
#pragma section s_021140d4 end

#pragma define_section s_021140ec ".data.data_ov020_021140ec" ".data.data_ov020_021140ec" ".data.data_ov020_021140ec" far_data
#pragma section s_021140ec begin
u32 data_ov020_021140ec[3] = {
    0x00000000,
    0xffff0000,
    0x0c8000ab,
};
#pragma section s_021140ec end

#pragma define_section s_021140f8 ".data.data_ov020_021140f8" ".data.data_ov020_021140f8" ".data.data_ov020_021140f8" far_data
#pragma section s_021140f8 begin
u32 data_ov020_021140f8[4] = {
    0xfb500320,
    0x00000000,
    0xff010000,
    0x0a2800ab,
};
#pragma section s_021140f8 end

#pragma define_section s_02114108 ".data.data_ov020_02114108" ".data.data_ov020_02114108" ".data.data_ov020_02114108" far_data
#pragma section s_02114108 begin
u32 data_ov020_02114108[29] = {
    0xfe0c0320,
    0x80000000,
    0xffff0000,
    0x0b5400ab,
    0xfe0c0320,
    0x80000000,
    0xff010000,
    0x0c8000ab,
    0xfe0c0320,
    0x80000000,
    0xffff0000,
    0x06400096,
    0xfce00398,
    0x00000000,
    0xffff0000,
    0x04b0001e,
    0x0708041a,
    0x00000000,
    0xff060000,
    0x080c0108,
    0xfbb404b0,
    0xc0000000,
    0xffff0000,
    0x080c0108,
    0xfe0c04b0,
    0xc0000000,
    0xffff0000,
    0x05140108,
    0xfaec03ce,
};
#pragma section s_02114108 end

#pragma define_section s_0211417c ".data.data_ov020_0211417c" ".data.data_ov020_0211417c" ".data.data_ov020_0211417c" far_data
#pragma section s_0211417c begin
u32 data_ov020_0211417c[2] = {
    0x20000000,
    0xffff0000,
};
#pragma section s_0211417c end

#pragma define_section s_02114184 ".data.data_ov020_02114184" ".data.data_ov020_02114184" ".data.data_ov020_02114184" far_data
#pragma section s_02114184 begin
u32 data_ov020_02114184[18] = {
    0x028aff02,
    0x189c0137,
    0x80000000,
    0x01040000,
    0xf7680d5c,
    0x0000fa60,
    0x00000000,
    0xfd62ff04,
    0x012c0a8c,
    0x40000000,
    0x01020000,
    0x079e044c,
    0x000003ca,
    0x00004000,
    0x00d70104,
    0x0dc00140,
    0x71c70000,
    0x01040000,
};
#pragma section s_02114184 end

#pragma define_section s_021141cc ".data.data_ov020_021141cc" ".data.data_ov020_021141cc" ".data.data_ov020_021141cc" far_data
#pragma section s_021141cc begin
u32 data_ov020_021141cc[53] = {
    0x06a40d93,
    0x00000226,
    0x0000c000,
    0x0d8e0104,
    0x049c0514,
    0xc0000000,
    0x01040000,
    0x02bc0384,
    0x000007bc,
    0x00008000,
    0xf8670104,
    0x08ed0320,
    0x60000000,
    0x01040000,
    0x0226f786,
    0x0000028a,
    0x00004000,
    0xfa240104,
    0xff1f01c2,
    0x80000000,
    0x00060000,
    0x05dcfc7c,
    0x0000fa24,
    0x00000000,
    0x05140104,
    0xfaec0546,
    0x11c70000,
    0x01040000,
    0x044c073a,
    0x0000fce0,
    0x00004000,
    0xfdf80104,
    0xfad803e8,
    0x1c710000,
    0x01040000,
    0x00a0f3dd,
    0x00001194,
    0x00002000,
    0xff380104,
    0x00c8f95c,
    0x80000000,
    0x01040000,
    0x0320f894,
    0x000008fc,
    0x00006000,
    0x0a5a0104,
    0x047ef63c,
    0x00000000,
    0x01040000,
    0x0960012c,
    0x0000fed4,
    0x00004000,
    0x0d930104,
};
#pragma section s_021141cc end

#pragma define_section s_021142a0 ".data.data_ov020_021142a0" ".data.data_ov020_021142a0" ".data.data_ov020_021142a0" far_data
#pragma section s_021142a0 begin
u32 data_ov020_021142a0[3] = {
    0x01900514,
    0xc0000000,
    0x00000000,
};
#pragma section s_021142a0 end

#pragma define_section s_021142ac ".data.data_ov020_021142ac" ".data.data_ov020_021142ac" ".data.data_ov020_021142ac" far_data
#pragma section s_021142ac begin
u32 data_ov020_021142ac[36] = {
    0x000001f4,
    0x00000802,
    0x00120100,
    0x00000320,
    0x80000802,
    0x00121000,
    0x0000f9c0,
    0x0000f9f2,
    0x00126000,
    0xff38f7a4,
    0xe000110f,
    0x00122000,
    0x00000802,
    0xc0000546,
    0x00121200,
    0x000005dc,
    0x0000ffce,
    0x00121200,
    0x0000015e,
    0x0000ffce,
    0x00121500,
    0x0000ffce,
    0x400002bc,
    0x00121600,
    0x0000ffce,
    0x400005dc,
    0x00121300,
    0x03200802,
    0xc00006a4,
    0x00121200,
    0x03200802,
    0xc0000190,
    0x00121200,
    0x03200064,
    0x0000ffce,
    0x00121500,
};
#pragma section s_021142ac end

#pragma define_section s_0211433c ".data.data_ov020_0211433c" ".data.data_ov020_0211433c" ".data.data_ov020_0211433c" far_data
#pragma section s_0211433c begin
u32 data_ov020_0211433c[5] = {
    0x0320fd12,
    0x400005aa,
    0x00121300,
    0x075401f4,
    0x0000073a,
};
#pragma section s_0211433c end

#pragma define_section s_02114350 ".data.data_ov020_02114350" ".data.data_ov020_02114350" ".data.data_ov020_02114350" far_data
#pragma section s_02114350 begin
u32 data_ov020_02114350[12] = {
    0x00120400,
    0x07540320,
    0x8000073a,
    0x00124000,
    0x0000f894,
    0x8000041a,
    0x00126300,
    0x0320fd12,
    0x400002bc,
    0x00121300,
    0x06a40802,
    0x40000226,
};
#pragma section s_02114350 end

#pragma define_section s_02114380 ".data.data_ov020_02114380" ".data.data_ov020_02114380" ".data.data_ov020_02114380" far_data
#pragma section s_02114380 begin
u32 data_ov020_02114380[22] = {
    0x00122400,
    0x032005dc,
    0x0000ffce,
    0x00121100,
    0x00000c80,
    0x0000041a,
    0x00122200,
    0x03e8f830,
    0x0000041a,
    0x00123300,
    0xf6a0ffb5,
    0x000011c6,
    0x00122200,
    0xf6a0ffb5,
    0x00000802,
    0x00122200,
    0xf6a0f862,
    0xc000007d,
    0x00122200,
    0xf6a0060e,
    0x40000113,
    0x00122200,
};
#pragma section s_02114380 end

#pragma define_section s_021143d8 ".data.data_ov020_021143d8" ".data.data_ov020_021143d8" ".data.data_ov020_021143d8" far_data
#pragma section s_021143d8 begin
u32 data_ov020_021143d8[9] = {
    0x041a0044,
    0x09c407ea,
    0xa0000000,
    0x05000000,
    0x0285001f,
    0x044c0be9,
    0x00000000,
    0xff020000,
    0xfe0c0098,
};
#pragma section s_021143d8 end

#pragma define_section s_021143fc ".data.data_ov020_021143fc" ".data.data_ov020_021143fc" ".data.data_ov020_021143fc" far_data
#pragma section s_021143fc begin
u32 data_ov020_021143fc[23] = {
    0x170cff38,
    0x80000000,
    0xffff0000,
    0x08980092,
    0x12c0ff38,
    0x00000000,
    0x00000000,
    0x01900092,
    0xf4acff38,
    0x00000000,
    0x00000000,
    0x07080092,
    0xf6a0ff38,
    0x00000000,
    0x00000000,
    0x04b0001f,
    0xf4480031,
    0x00000000,
    0x0a000000,
    0xf286011c,
    0x1068ff38,
    0x20000000,
    0xffff0000,
};
#pragma section s_021143fc end

#pragma define_section s_02114458 ".data.data_ov020_02114458" ".data.data_ov020_02114458" ".data.data_ov020_02114458" far_data
#pragma section s_02114458 begin
u32 data_ov020_02114458[16] = {
    0xf358011c,
    0x0f96ff38,
    0x20000000,
    0xffff0000,
    0x0753004c,
    0x0b0eff38,
    0x0000ffff,
    0x073a0000,
    0x013f004c,
    0x0b52ff38,
    0x0000ffff,
    0x04b00000,
    0xfddc004c,
    0x0ae9ff38,
    0x0000ffff,
    0x04b10000,
};
#pragma section s_02114458 end

#pragma define_section s_02114498 ".data.data_ov020_02114498" ".data.data_ov020_02114498" ".data.data_ov020_02114498" far_data
#pragma section s_02114498 begin
u32 data_ov020_02114498[28] = {
    0xfa35004c,
    0x1212ff38,
    0x5d27ffff,
    0x07420000,
    0x044c0108,
    0x0857015e,
    0x00000000,
    0xffff0000,
    0x00c80108,
    0x0857015e,
    0x00000000,
    0xffff0000,
    0x047e001e,
    0x08ca08da,
    0x00000000,
    0xff060000,
    0x00960031,
    0x012c0c80,
    0x00000000,
    0x00fc0000,
    0x01c20031,
    0x012c0c80,
    0x00000000,
    0x00170000,
    0xf47a0031,
    0x14b4044c,
    0x60000000,
    0x00f30000,
};
#pragma section s_02114498 end

#pragma define_section s_02114508 ".data.data_ov020_02114508" ".data.data_ov020_02114508" ".data.data_ov020_02114508" far_data
#pragma section s_02114508 begin
u32 data_ov020_02114508[17] = {
    0xff420048,
    0x00bef66e,
    0x00000000,
    0xffff0000,
    0x0af00049,
    0x073a0320,
    0x00000000,
    0xffff0000,
    0xf49a00e7,
    0x14a6ff38,
    0xe0000857,
    0x0000ffff,
    0x0b5400d2,
    0xfb820000,
    0x00000000,
    0xffff0000,
    0x0b5400d2,
};
#pragma section s_02114508 end

#pragma define_section s_0211454c ".data.data_ov020_0211454c" ".data.data_ov020_0211454c" ".data.data_ov020_0211454c" far_data
#pragma section s_0211454c begin
u32 data_ov020_0211454c[7] = {
    0xfbe60000,
    0x00000000,
    0xffff0000,
    0x0b5400d2,
    0xfc4a0000,
    0x00000000,
    0xffff0000,
};
#pragma section s_0211454c end

#pragma define_section s_02114568 ".data.data_ov020_02114568" ".data.data_ov020_02114568" ".data.data_ov020_02114568" far_data
#pragma section s_02114568 begin
u32 data_ov020_02114568[12] = {
    0x0b5400d2,
    0xfcae0000,
    0x00000000,
    0xffff0000,
    0x0b5400d2,
    0xfd120000,
    0x00000000,
    0xffff0000,
    0x0b5400d2,
    0xfd760000,
    0x00000000,
    0xffff0000,
};
#pragma section s_02114568 end

#pragma define_section s_02114598 ".data.data_ov020_02114598" ".data.data_ov020_02114598" ".data.data_ov020_02114598" far_data
#pragma section s_02114598 begin
u32 data_ov020_02114598[19] = {
    0x0b5400d2,
    0xfdda0000,
    0x00000000,
    0xffff0000,
    0x0b5400d2,
    0xfe3e0000,
    0x00000000,
    0xffff0000,
    0x0b5400d2,
    0xfea20000,
    0x00000000,
    0xffff0000,
    0xf7680096,
    0x1388ffb0,
    0x00000000,
    0xffff0000,
    0x0cf800fe,
    0xfbb4f5d8,
    0xe0000000,
};
#pragma section s_02114598 end

#pragma define_section s_021145e4 ".data.data_ov020_021145e4" ".data.data_ov020_021145e4" ".data.data_ov020_021145e4" far_data
#pragma section s_021145e4 begin
u32 data_ov020_021145e4[18] = {
    0x10040000,
    0x0960011f,
    0x03480429,
    0x80000000,
    0xffff0000,
    0x0af0011f,
    0x03c00429,
    0x80000000,
    0xffff0000,
    0xf60a002c,
    0x0672f6a0,
    0x00000000,
    0x20070000,
    0x0cf800fe,
    0xfbb4f5d8,
    0xe0000000,
    0x00040000,
    0x07d900fe,
};
#pragma section s_021145e4 end

#pragma define_section s_0211462c ".data.data_ov020_0211462c" ".data.data_ov020_0211462c" ".data.data_ov020_0211462c" far_data
#pragma section s_0211462c begin
u32 data_ov020_0211462c[15] = {
    0x064af4ac,
    0x80000000,
    0x02040000,
    0x0ce4001e,
    0x00b404e2,
    0x00000000,
    0xff060000,
    0xf60a011c,
    0x0672f6a0,
    0x000000b6,
    0xffff0000,
    0x07d900fe,
    0x064af4ac,
    0x80000000,
    0x12040000,
};
#pragma section s_0211462c end

#pragma define_section s_02114668 ".data.data_ov020_02114668" ".data.data_ov020_02114668" ".data.data_ov020_02114668" far_data
#pragma section s_02114668 begin
u32 data_ov020_02114668[34] = {
    0x53504c43,
    0x00360008,
    0x00000fe3,
    0x000000ff,
    0x00040fc4,
    0x000000ff,
    0x00040fc0,
    0x000000ff,
    0x00040105,
    0x000000ff,
    0x01000fc0,
    0x000000ff,
    0x00040fcc,
    0x000000ff,
    0x000403c5,
    0x000000ff,
    0x01000fc4,
    0x000000ff,
    0x00000fc0,
    0x000000ff,
    0x0004040b,
    0x000000ff,
    0x00030fca,
    0x000000ff,
    0x00030fc4,
    0x000000ff,
    0x00030fc0,
    0x000000ff,
    0x0004040a,
    0x000000ff,
    0x018c0fcc,
    0x000000ff,
    0x00040fca,
    0x000000ff,
};
#pragma section s_02114668 end

#pragma define_section s_021146f0 ".data.data_ov020_021146f0" ".data.data_ov020_021146f0" ".data.data_ov020_021146f0" far_data
#pragma section s_021146f0 begin
u32 data_ov020_021146f0[24] = {
    0x00043fc0,
    0x000000ff,
    0x008c0fc0,
    0x000000ff,
    0x000304c5,
    0x000000ff,
    0x04000fc0,
    0x000000ff,
    0x00003fc0,
    0x000000ff,
    0x00040085,
    0x000000ff,
    0x00040084,
    0x000000ff,
    0x00000fc4,
    0x000000ff,
    0x00043fc4,
    0x000000ff,
    0x00040384,
    0x000000ff,
    0x0004038c,
    0x000000ff,
    0x00040304,
    0x000000ff,
};
#pragma section s_021146f0 end

#pragma define_section s_02114750 ".data.data_ov020_02114750" ".data.data_ov020_02114750" ".data.data_ov020_02114750" far_data
#pragma section s_02114750 begin
u32 data_ov020_02114750[6] = {
    0x00030344,
    0x000000ff,
    0x000302c5,
    0x000000ff,
    0x000402c5,
    0x000000ff,
};
#pragma section s_02114750 end

#pragma define_section s_02114768 ".data.data_ov020_02114768" ".data.data_ov020_02114768" ".data.data_ov020_02114768" far_data
#pragma section s_02114768 begin
u32 data_ov020_02114768[23] = {
    0x00040240,
    0x000000ff,
    0x000302c0,
    0x000000ff,
    0x00040244,
    0x000000ff,
    0x00040fc5,
    0x000000ff,
    0x00040245,
    0x000000ff,
    0x00050285,
    0x000000ff,
    0x00040044,
    0x000000ff,
    0x01043fc4,
    0x000000ff,
    0x00003205,
    0x000000ff,
    0x04000fc5,
    0x000000ff,
    0x01043fc5,
    0x000000ff,
    0x00040205,
};
#pragma section s_02114768 end

#pragma define_section s_021147c4 ".data.data_ov020_021147c4" ".data.data_ov020_021147c4" ".data.data_ov020_021147c4" far_data
#pragma section s_021147c4 begin
u32 data_ov020_021147c4[24] = {
    0x000000ff,
    0x00040445,
    0x000000ff,
    0x00040200,
    0x000000ff,
    0x00040440,
    0x000000ff,
    0x00042484,
    0x000000ff,
    0x00040145,
    0x000000ff,
    0x00040185,
    0x000000ff,
    0x00040505,
    0x000000ff,
    0x000401c0,
    0x000000ff,
    0x01040fc4,
    0x000000ff,
    0x000401c5,
    0x000000ff,
    0x000401c4,
    0x000000ff,
    0xffffffff,
};
#pragma section s_021147c4 end

#pragma define_section s_02114824 ".data.data_ov020_02114824" ".data.data_ov020_02114824" ".data.data_ov020_02114824" far_data
#pragma section s_02114824 begin
u32 data_ov020_02114824[1] = {
    0xffffffff,
};
#pragma section s_02114824 end

