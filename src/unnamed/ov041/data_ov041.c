/* Overlay 41 carries no code: .rodata and .data course data. The relocated
 * words are the interior data_ov041_* labels defined by the lcf. The runs
 * spell mat_kira, water_mat and a CLPS block at 0x021119f8. */
#include "types.h"

extern u32 data_ov041_021111a0[];
extern u32 data_ov041_021111a4[];
extern u32 data_ov041_021111a8[];
extern u32 data_ov041_021111e0[];
extern u32 data_ov041_021111ec[];
extern u32 data_ov041_021111fc[];
extern u32 data_ov041_02111204[];
extern u32 data_ov041_02111210[];
extern u32 data_ov041_02111218[];
extern u32 data_ov041_02111228[];
extern u32 data_ov041_02111244[];
extern u32 data_ov041_021112b0[];
extern u32 data_ov041_021113ac[];
extern u32 data_ov041_021113b0[];
extern u32 data_ov041_021113d0[];
extern u32 data_ov041_02111438[];
extern u32 data_ov041_0211150c[];
extern u32 data_ov041_02111520[];
extern u32 data_ov041_0211153c[];
extern u32 data_ov041_02111548[];
extern u32 data_ov041_0211155c[];
extern u32 data_ov041_02111560[];
extern u32 data_ov041_0211159c[];
extern u32 data_ov041_021115bc[];
extern u32 data_ov041_021115e0[];
extern u32 data_ov041_021115e4[];
extern u32 data_ov041_021115f4[];
extern u32 data_ov041_02111624[];
extern u32 data_ov041_02111640[];
extern u32 data_ov041_02111660[];
extern u32 data_ov041_02111680[];
extern u32 data_ov041_021116a0[];
extern u32 data_ov041_021116c0[];
extern u32 data_ov041_021116e0[];
extern u32 data_ov041_02111700[];
extern u32 data_ov041_02111720[];
extern u32 data_ov041_02111740[];
extern u32 data_ov041_02111760[];
extern u32 data_ov041_02111768[];
extern u32 data_ov041_02111834[];
extern u32 data_ov041_02111844[];
extern u32 data_ov041_02111880[];
extern u32 data_ov041_02111884[];
extern u32 data_ov041_02111888[];
extern u32 data_ov041_0211188c[];
extern u32 data_ov041_02111894[];
extern u32 data_ov041_0211189c[];
extern u32 data_ov041_021118a8[];
extern u32 data_ov041_021118b4[];
extern u32 data_ov041_021118c0[];
extern u32 data_ov041_021118d0[];
extern u32 data_ov041_021118f8[];
extern u32 data_ov041_0211190c[];
extern u32 data_ov041_02111910[];
extern u32 data_ov041_02111928[];
extern u32 data_ov041_0211192c[];
extern u32 data_ov041_02111944[];
extern u32 data_ov041_02111948[];
extern u32 data_ov041_02111960[];
extern u32 data_ov041_02111978[];
extern u32 data_ov041_02111994[];
extern u32 data_ov041_021119b8[];
extern u32 data_ov041_021119f8[];
extern u32 data_ov041_02111a4c[];
extern u32 data_ov041_02111a58[];
extern u32 data_ov041_02111a78[];
extern u32 data_ov041_02111abc[];
extern u32 data_ov041_02111ad4[];
extern u32 data_ov041_02111af4[];
extern u32 data_ov041_02111b14[];
extern u32 data_ov041_02111b34[];
extern u32 data_ov041_02111b44[];
extern u32 data_ov041_02111b64[];
extern u32 data_ov041_02111b68[];
extern u32 data_ov041_02111b6c[];
extern u32 data_ov041_02111b74[];
extern u32 data_ov041_02111b88[];
extern u32 data_ov041_02111bd4[];
extern u32 data_ov041_02111be0[];
extern u32 data_ov041_02111c20[];
extern u32 data_ov041_02111c54[];
extern u32 data_ov041_02111cc8[];


/* Words the overlay relocates point here. */
extern int data_ov041_021118e0;

/* 0x021111a0..0x02111870 (1744 bytes, 436 words) */
#pragma explicit_zero_data on
#pragma define_section s_021111a0 ".rodata.data_ov041_021111a0" ".rodata.data_ov041_021111a0" ".rodata.data_ov041_021111a0" far_data
#pragma section s_021111a0 begin
u32 data_ov041_021111a0[1] = {
    0x00000000,
};
#pragma section s_021111a0 end
#pragma explicit_zero_data off

#pragma define_section s_021111a4 ".rodata.data_ov041_021111a4" ".rodata.data_ov041_021111a4" ".rodata.data_ov041_021111a4" far_data
#pragma section s_021111a4 begin
u32 data_ov041_021111a4[1] = {
    0x00001000,
};
#pragma section s_021111a4 end

#pragma define_section s_021111a8 ".rodata.data_ov041_021111a8" ".rodata.data_ov041_021111a8" ".rodata.data_ov041_021111a8" far_data
#pragma section s_021111a8 begin
u32 data_ov041_021111a8[14] = {
    0x0000ffff,
    (u32)data_ov041_021118a8,
    0x00000001,
    0x00000001,
    0x00000001,
    0x000000d2,
    0x000000d2,
    0x0000ffff,
    (u32)data_ov041_0211189c,
    0x00000001,
    0x00000001,
    0x00000001,
    0x00d200d2,
    0x00000001,
};
#pragma section s_021111a8 end

#pragma define_section s_021111e0 ".rodata.data_ov041_021111e0" ".rodata.data_ov041_021111e0" ".rodata.data_ov041_021111e0" far_data
#pragma section s_021111e0 begin
u32 data_ov041_021111e0[3] = {
    0x00000000,
    0x00000089,
    0x00000112,
};
#pragma section s_021111e0 end

#pragma define_section s_021111ec ".rodata.data_ov041_021111ec" ".rodata.data_ov041_021111ec" ".rodata.data_ov041_021111ec" far_data
#pragma section s_021111ec begin
u32 data_ov041_021111ec[4] = {
    0x0000019c,
    0x00000225,
    0x000002ae,
    0x00000337,
};
#pragma section s_021111ec end

#pragma define_section s_021111fc ".rodata.data_ov041_021111fc" ".rodata.data_ov041_021111fc" ".rodata.data_ov041_021111fc" far_data
#pragma section s_021111fc begin
u32 data_ov041_021111fc[2] = {
    0x000003c0,
    0x00000449,
};
#pragma section s_021111fc end

#pragma define_section s_02111204 ".rodata.data_ov041_02111204" ".rodata.data_ov041_02111204" ".rodata.data_ov041_02111204" far_data
#pragma section s_02111204 begin
u32 data_ov041_02111204[3] = {
    0x000004d3,
    0x0000055c,
    0x000005e5,
};
#pragma section s_02111204 end

#pragma define_section s_02111210 ".rodata.data_ov041_02111210" ".rodata.data_ov041_02111210" ".rodata.data_ov041_02111210" far_data
#pragma section s_02111210 begin
u32 data_ov041_02111210[2] = {
    0x0000066e,
    0x000006f7,
};
#pragma section s_02111210 end

#pragma define_section s_02111218 ".rodata.data_ov041_02111218" ".rodata.data_ov041_02111218" ".rodata.data_ov041_02111218" far_data
#pragma section s_02111218 begin
u32 data_ov041_02111218[4] = {
    0x00000781,
    0x0000080a,
    0x00000893,
    0x0000091c,
};
#pragma section s_02111218 end

#pragma define_section s_02111228 ".rodata.data_ov041_02111228" ".rodata.data_ov041_02111228" ".rodata.data_ov041_02111228" far_data
#pragma section s_02111228 begin
u32 data_ov041_02111228[7] = {
    0x000009a5,
    0x00000a2f,
    0x00000ab8,
    0x00000b41,
    0x00000bca,
    0x00000c53,
    0x00000cdc,
};
#pragma section s_02111228 end

#pragma define_section s_02111244 ".rodata.data_ov041_02111244" ".rodata.data_ov041_02111244" ".rodata.data_ov041_02111244" far_data
#pragma section s_02111244 begin
u32 data_ov041_02111244[27] = {
    0x00000d66,
    0x00000def,
    0x00000e78,
    0x00000f01,
    0x00000f8a,
    0x00001014,
    0x0000109d,
    0x00001126,
    0x000011af,
    0x00001238,
    0x000012c2,
    0x0000134b,
    0x000013d4,
    0x0000145d,
    0x000014e6,
    0x0000156f,
    0x000015f9,
    0x00001682,
    0x0000170b,
    0x00001794,
    0x0000181d,
    0x000018a7,
    0x00001930,
    0x000019b9,
    0x00001a42,
    0x00001acb,
    0x00001b55,
};
#pragma section s_02111244 end

#pragma define_section s_021112b0 ".rodata.data_ov041_021112b0" ".rodata.data_ov041_021112b0" ".rodata.data_ov041_021112b0" far_data
#pragma section s_021112b0 begin
u32 data_ov041_021112b0[63] = {
    0x00001bde,
    0x00001c67,
    0x00001cf0,
    0x00001d79,
    0x00001e02,
    0x00001e8c,
    0x00001f15,
    0x00001f9e,
    0x00002027,
    0x000020b0,
    0x0000213a,
    0x000021c3,
    0x0000224c,
    0x000022d5,
    0x0000235e,
    0x000023e8,
    0x00002471,
    0x000024fa,
    0x00002583,
    0x0000260c,
    0x00002695,
    0x0000271f,
    0x000027a8,
    0x00002831,
    0x000028ba,
    0x00002943,
    0x000029cd,
    0x00002a56,
    0x00002adf,
    0x00002b68,
    0x00002bf1,
    0x00002c7a,
    0x00002d04,
    0x00002d8d,
    0x00002e16,
    0x00002e9f,
    0x00002f28,
    0x00002fb2,
    0x0000303b,
    0x000030c4,
    0x0000314d,
    0x000031d6,
    0x00003260,
    0x000032e9,
    0x00003372,
    0x000033fb,
    0x00003484,
    0x0000350d,
    0x00003597,
    0x00003620,
    0x000036a9,
    0x00003732,
    0x000037bb,
    0x00003845,
    0x000038ce,
    0x00003957,
    0x000039e0,
    0x00003a69,
    0x00003af3,
    0x00003b7c,
    0x00003c05,
    0x00003c8e,
    0x00003d17,
};
#pragma section s_021112b0 end

#pragma define_section s_021113ac ".rodata.data_ov041_021113ac" ".rodata.data_ov041_021113ac" ".rodata.data_ov041_021113ac" far_data
#pragma section s_021113ac begin
u32 data_ov041_021113ac[1] = {
    0x00003da0,
};
#pragma section s_021113ac end

#pragma define_section s_021113b0 ".rodata.data_ov041_021113b0" ".rodata.data_ov041_021113b0" ".rodata.data_ov041_021113b0" far_data
#pragma section s_021113b0 begin
u32 data_ov041_021113b0[8] = {
    0x00003e2a,
    0x00003eb3,
    0x00003f3c,
    0x00003fc5,
    0x0000404e,
    0x000040d8,
    0x00004161,
    0x000041ea,
};
#pragma section s_021113b0 end

#pragma define_section s_021113d0 ".rodata.data_ov041_021113d0" ".rodata.data_ov041_021113d0" ".rodata.data_ov041_021113d0" far_data
#pragma section s_021113d0 begin
u32 data_ov041_021113d0[26] = {
    0x00004273,
    0x000042fc,
    0x00004386,
    0x0000440f,
    0x00004498,
    0x00004521,
    0x000045aa,
    0x00004633,
    0x000046bd,
    0x00004746,
    0x000047cf,
    0x00004858,
    0x000048e1,
    0x0000496b,
    0x000049f4,
    0x00004a7d,
    0x00004b06,
    0x00004b8f,
    0x00004c18,
    0x00004ca2,
    0x00004d2b,
    0x00004db4,
    0x00004e3d,
    0x00004ec6,
    0x00004f50,
    0x00004fd9,
};
#pragma section s_021113d0 end

#pragma define_section s_02111438 ".rodata.data_ov041_02111438" ".rodata.data_ov041_02111438" ".rodata.data_ov041_02111438" far_data
#pragma section s_02111438 begin
u32 data_ov041_02111438[53] = {
    0x00005062,
    0x000050eb,
    0x00005174,
    0x000051fe,
    0x00005287,
    0x00005310,
    0x00005399,
    0x00005422,
    0x000054ab,
    0x00005535,
    0x000055be,
    0x00005647,
    0x000056d0,
    0x00005759,
    0x000057e3,
    0x0000586c,
    0x000058f5,
    0x0000597e,
    0x00005a07,
    0x00005a91,
    0x00005b1a,
    0x00005ba3,
    0x00005c2c,
    0x00005cb5,
    0x00005d3e,
    0x00005dc8,
    0x00005e51,
    0x00005eda,
    0x00005f63,
    0x00005fec,
    0x00006076,
    0x000060ff,
    0x00006188,
    0x00006211,
    0x0000629a,
    0x00006324,
    0x000063ad,
    0x00006436,
    0x000064bf,
    0x00006548,
    0x000065d1,
    0x0000665b,
    0x000066e4,
    0x0000676d,
    0x000067f6,
    0x0000687f,
    0x00006909,
    0x00006992,
    0x00006a1b,
    0x00006aa4,
    0x00006b2d,
    0x00006bb7,
    0x00006c40,
};
#pragma section s_02111438 end

#pragma define_section s_0211150c ".rodata.data_ov041_0211150c" ".rodata.data_ov041_0211150c" ".rodata.data_ov041_0211150c" far_data
#pragma section s_0211150c begin
u32 data_ov041_0211150c[5] = {
    0x00006cc9,
    0x00006d52,
    0x00006ddb,
    0x00006e64,
    0x00006eee,
};
#pragma section s_0211150c end

#pragma define_section s_02111520 ".rodata.data_ov041_02111520" ".rodata.data_ov041_02111520" ".rodata.data_ov041_02111520" far_data
#pragma section s_02111520 begin
u32 data_ov041_02111520[7] = {
    0x00006f77,
    0x00007000,
    0x00000000,
    0x00000014,
    0x00000027,
    0x0000003b,
    0x0000004e,
};
#pragma section s_02111520 end

#pragma define_section s_0211153c ".rodata.data_ov041_0211153c" ".rodata.data_ov041_0211153c" ".rodata.data_ov041_0211153c" far_data
#pragma section s_0211153c begin
u32 data_ov041_0211153c[3] = {
    0x00000062,
    0x00000076,
    0x00000089,
};
#pragma section s_0211153c end

#pragma define_section s_02111548 ".rodata.data_ov041_02111548" ".rodata.data_ov041_02111548" ".rodata.data_ov041_02111548" far_data
#pragma section s_02111548 begin
u32 data_ov041_02111548[5] = {
    0x0000009d,
    0x000000b0,
    0x000000c4,
    0x000000d8,
    0x000000eb,
};
#pragma section s_02111548 end

#pragma define_section s_0211155c ".rodata.data_ov041_0211155c" ".rodata.data_ov041_0211155c" ".rodata.data_ov041_0211155c" far_data
#pragma section s_0211155c begin
u32 data_ov041_0211155c[1] = {
    0x000000ff,
};
#pragma section s_0211155c end

#pragma define_section s_02111560 ".rodata.data_ov041_02111560" ".rodata.data_ov041_02111560" ".rodata.data_ov041_02111560" far_data
#pragma section s_02111560 begin
u32 data_ov041_02111560[15] = {
    0x00000112,
    0x00000126,
    0x0000013a,
    0x0000014d,
    0x00000161,
    0x00000174,
    0x00000188,
    0x0000019c,
    0x000001af,
    0x000001c3,
    0x000001d6,
    0x000001ea,
    0x000001fe,
    0x00000211,
    0x00000225,
};
#pragma section s_02111560 end

#pragma define_section s_0211159c ".rodata.data_ov041_0211159c" ".rodata.data_ov041_0211159c" ".rodata.data_ov041_0211159c" far_data
#pragma section s_0211159c begin
u32 data_ov041_0211159c[8] = {
    0x00000238,
    0x0000024c,
    0x00000260,
    0x00000273,
    0x00000287,
    0x0000029a,
    0x000002ae,
    0x000002c2,
};
#pragma section s_0211159c end

#pragma define_section s_021115bc ".rodata.data_ov041_021115bc" ".rodata.data_ov041_021115bc" ".rodata.data_ov041_021115bc" far_data
#pragma section s_021115bc begin
u32 data_ov041_021115bc[9] = {
    0x000002d5,
    0x000002e9,
    0x000002fc,
    0x00000310,
    0x00000324,
    0x00000337,
    0x0000034b,
    0x0000035e,
    0x00000372,
};
#pragma section s_021115bc end

#pragma define_section s_021115e0 ".rodata.data_ov041_021115e0" ".rodata.data_ov041_021115e0" ".rodata.data_ov041_021115e0" far_data
#pragma section s_021115e0 begin
u32 data_ov041_021115e0[1] = {
    0x00000386,
};
#pragma section s_021115e0 end

#pragma define_section s_021115e4 ".rodata.data_ov041_021115e4" ".rodata.data_ov041_021115e4" ".rodata.data_ov041_021115e4" far_data
#pragma section s_021115e4 begin
u32 data_ov041_021115e4[4] = {
    0x00000399,
    0x000003ad,
    0x000003c0,
    0x000003d4,
};
#pragma section s_021115e4 end

#pragma define_section s_021115f4 ".rodata.data_ov041_021115f4" ".rodata.data_ov041_021115f4" ".rodata.data_ov041_021115f4" far_data
#pragma section s_021115f4 begin
u32 data_ov041_021115f4[12] = {
    0x000003e8,
    0x000003fb,
    0x0000040f,
    0x00000422,
    0x00000436,
    0x00000449,
    0x0000045d,
    0x00000471,
    0x00000484,
    0x00000498,
    0x000004ab,
    0x000004bf,
};
#pragma section s_021115f4 end

#pragma define_section s_02111624 ".rodata.data_ov041_02111624" ".rodata.data_ov041_02111624" ".rodata.data_ov041_02111624" far_data
#pragma section s_02111624 begin
u32 data_ov041_02111624[7] = {
    0x000004d3,
    0x000004e6,
    0x000004fa,
    0x0000050d,
    0x00000521,
    0x00000535,
    0x00000548,
};
#pragma section s_02111624 end

#pragma define_section s_02111640 ".rodata.data_ov041_02111640" ".rodata.data_ov041_02111640" ".rodata.data_ov041_02111640" far_data
#pragma section s_02111640 begin
u32 data_ov041_02111640[8] = {
    0x0000055c,
    0x0000056f,
    0x00000583,
    0x00000597,
    0x000005aa,
    0x000005be,
    0x000005d1,
    0x000005e5,
};
#pragma section s_02111640 end

#pragma define_section s_02111660 ".rodata.data_ov041_02111660" ".rodata.data_ov041_02111660" ".rodata.data_ov041_02111660" far_data
#pragma section s_02111660 begin
u32 data_ov041_02111660[8] = {
    0x000005f9,
    0x0000060c,
    0x00000620,
    0x00000633,
    0x00000647,
    0x0000065b,
    0x0000066e,
    0x00000682,
};
#pragma section s_02111660 end

#pragma define_section s_02111680 ".rodata.data_ov041_02111680" ".rodata.data_ov041_02111680" ".rodata.data_ov041_02111680" far_data
#pragma section s_02111680 begin
u32 data_ov041_02111680[8] = {
    0x00000695,
    0x000006a9,
    0x000006bd,
    0x000006d0,
    0x000006e4,
    0x000006f7,
    0x0000070b,
    0x0000071f,
};
#pragma section s_02111680 end

#pragma define_section s_021116a0 ".rodata.data_ov041_021116a0" ".rodata.data_ov041_021116a0" ".rodata.data_ov041_021116a0" far_data
#pragma section s_021116a0 begin
u32 data_ov041_021116a0[8] = {
    0x00000732,
    0x00000746,
    0x00000759,
    0x0000076d,
    0x00000781,
    0x00000794,
    0x000007a8,
    0x000007bb,
};
#pragma section s_021116a0 end

#pragma define_section s_021116c0 ".rodata.data_ov041_021116c0" ".rodata.data_ov041_021116c0" ".rodata.data_ov041_021116c0" far_data
#pragma section s_021116c0 begin
u32 data_ov041_021116c0[8] = {
    0x000007cf,
    0x000007e3,
    0x000007f6,
    0x0000080a,
    0x0000081d,
    0x00000831,
    0x00000845,
    0x00000858,
};
#pragma section s_021116c0 end

#pragma define_section s_021116e0 ".rodata.data_ov041_021116e0" ".rodata.data_ov041_021116e0" ".rodata.data_ov041_021116e0" far_data
#pragma section s_021116e0 begin
u32 data_ov041_021116e0[8] = {
    0x0000086c,
    0x0000087f,
    0x00000893,
    0x000008a7,
    0x000008ba,
    0x000008ce,
    0x000008e1,
    0x000008f5,
};
#pragma section s_021116e0 end

#pragma define_section s_02111700 ".rodata.data_ov041_02111700" ".rodata.data_ov041_02111700" ".rodata.data_ov041_02111700" far_data
#pragma section s_02111700 begin
u32 data_ov041_02111700[8] = {
    0x00000909,
    0x0000091c,
    0x00000930,
    0x00000943,
    0x00000957,
    0x0000096b,
    0x0000097e,
    0x00000992,
};
#pragma section s_02111700 end

#pragma define_section s_02111720 ".rodata.data_ov041_02111720" ".rodata.data_ov041_02111720" ".rodata.data_ov041_02111720" far_data
#pragma section s_02111720 begin
u32 data_ov041_02111720[8] = {
    0x000009a5,
    0x000009b9,
    0x000009cd,
    0x000009e0,
    0x000009f4,
    0x00000a07,
    0x00000a1b,
    0x00000a2f,
};
#pragma section s_02111720 end

#pragma define_section s_02111740 ".rodata.data_ov041_02111740" ".rodata.data_ov041_02111740" ".rodata.data_ov041_02111740" far_data
#pragma section s_02111740 begin
u32 data_ov041_02111740[8] = {
    0x00000a42,
    0x00000a56,
    0x00000a69,
    0x00000a7d,
    0x00000a91,
    0x00000aa4,
    0x00000ab8,
    0x00000acb,
};
#pragma section s_02111740 end

#pragma define_section s_02111760 ".rodata.data_ov041_02111760" ".rodata.data_ov041_02111760" ".rodata.data_ov041_02111760" far_data
#pragma section s_02111760 begin
u32 data_ov041_02111760[2] = {
    0x00000adf,
    0x00000af3,
};
#pragma section s_02111760 end

#pragma define_section s_02111768 ".rodata.data_ov041_02111768" ".rodata.data_ov041_02111768" ".rodata.data_ov041_02111768" far_data
#pragma section s_02111768 begin
u32 data_ov041_02111768[51] = {
    0x00000b06,
    0x00000b1a,
    0x00000b2d,
    0x00000b41,
    0x00000b55,
    0x00000b68,
    0x00000b7c,
    0x00000b8f,
    0x00000ba3,
    0x00000bb7,
    0x00000bca,
    0x00000bde,
    0x00000bf1,
    0x00000c05,
    0x00000c18,
    0x00000c2c,
    0x00000c40,
    0x00000c53,
    0x00000c67,
    0x00000c7a,
    0x00000c8e,
    0x00000ca2,
    0x00000cb5,
    0x00000cc9,
    0x00000cdc,
    0x00000cf0,
    0x00000d04,
    0x00000d17,
    0x00000d2b,
    0x00000d3e,
    0x00000d52,
    0x00000d66,
    0x00000d79,
    0x00000d8d,
    0x00000da0,
    0x00000db4,
    0x00000dc8,
    0x00000ddb,
    0x00000def,
    0x00000e02,
    0x00000e16,
    0x00000e2a,
    0x00000e3d,
    0x00000e51,
    0x00000e64,
    0x00000e78,
    0x00000e8c,
    0x00000e9f,
    0x00000eb3,
    0x00000ec6,
    0x00000eda,
};
#pragma section s_02111768 end

#pragma define_section s_02111834 ".rodata.data_ov041_02111834" ".rodata.data_ov041_02111834" ".rodata.data_ov041_02111834" far_data
#pragma section s_02111834 begin
u32 data_ov041_02111834[4] = {
    0x00000eee,
    0x00000f01,
    0x00000f15,
    0x00000f28,
};
#pragma section s_02111834 end

#pragma define_section s_02111844 ".rodata.data_ov041_02111844" ".rodata.data_ov041_02111844" ".rodata.data_ov041_02111844" far_data
#pragma section s_02111844 begin
u32 data_ov041_02111844[11] = {
    0x00000f3c,
    0x00000f50,
    0x00000f63,
    0x00000f77,
    0x00000f8a,
    0x00000f9e,
    0x00000fb2,
    0x00000fc5,
    0x00000fd9,
    0x00000fec,
    0x00001000,
};
#pragma section s_02111844 end


/* 0x02111880..0x02111d40 (1216 bytes, 304 words) */
#pragma define_section s_02111880 ".data.data_ov041_02111880" ".data.data_ov041_02111880" ".data.data_ov041_02111880" far_data
#pragma section s_02111880 begin
u32 data_ov041_02111880[1] = {
    0x00000960,
};
#pragma section s_02111880 end

#pragma define_section s_02111884 ".data.data_ov041_02111884" ".data.data_ov041_02111884" ".data.data_ov041_02111884" far_data
#pragma section s_02111884 begin
u32 data_ov041_02111884[1] = {
    0x000007a0,
};
#pragma section s_02111884 end

#pragma define_section s_02111888 ".data.data_ov041_02111888" ".data.data_ov041_02111888" ".data.data_ov041_02111888" far_data
#pragma section s_02111888 begin
u32 data_ov041_02111888[1] = {
    0x00000224,
};
#pragma section s_02111888 end

#pragma define_section s_0211188c ".data.data_ov041_0211188c" ".data.data_ov041_0211188c" ".data.data_ov041_0211188c" far_data
#pragma section s_0211188c begin
u32 data_ov041_0211188c[2] = {
    0x00000002,
    (u32)data_ov041_021118d0,
};
#pragma section s_0211188c end

#pragma define_section s_02111894 ".data.data_ov041_02111894" ".data.data_ov041_02111894" ".data.data_ov041_02111894" far_data
#pragma section s_02111894 begin
u32 data_ov041_02111894[2] = {
    0x00000008,
    (u32)data_ov041_02111978,
};
#pragma section s_02111894 end

#pragma define_section s_0211189c ".data.data_ov041_0211189c" ".data.data_ov041_0211189c" ".data.data_ov041_0211189c" far_data
#pragma section s_0211189c begin
u32 data_ov041_0211189c[3] = {
    0x5f74616d,
    0x6172696b,
    0x00000000,
};
#pragma section s_0211189c end

#pragma define_section s_021118a8 ".data.data_ov041_021118a8" ".data.data_ov041_021118a8" ".data.data_ov041_021118a8" far_data
#pragma section s_021118a8 begin
u32 data_ov041_021118a8[3] = {
    0x65746177,
    0x616d5f72,
    0x00000074,
};
#pragma section s_021118a8 end

#pragma define_section s_021118b4 ".data.data_ov041_021118b4" ".data.data_ov041_021118b4" ".data.data_ov041_021118b4" far_data
#pragma section s_021118b4 begin
u32 data_ov041_021118b4[3] = {
    (u32)data_ov041_0211188c,
    (u32)data_ov041_021118f8,
    0x10040200,
};
#pragma section s_021118b4 end

#pragma define_section s_021118c0 ".data.data_ov041_021118c0" ".data.data_ov041_021118c0" ".data.data_ov041_021118c0" far_data
#pragma section s_021118c0 begin
u32 data_ov041_021118c0[4] = {
    0xfe3e0000,
    0xc0001068,
    0x09010000,
    0x00006cff,
};
#pragma section s_021118c0 end

#pragma define_section s_021118d0 ".data.data_ov041_021118d0" ".data.data_ov041_021118d0" ".data.data_ov041_021118d0" far_data
#pragma section s_021118d0 begin
u32 data_ov041_021118d0[4] = {
    0x00001300,
    (u32)data_ov041_02111a58,
    0x00003405,
    (u32)data_ov041_02111b88,
};
#pragma section s_021118d0 end

#pragma define_section s_021118f8 ".data.data_ov041_021118f8" ".data.data_ov041_021118f8" ".data.data_ov041_021118f8" far_data
#pragma section s_021118f8 begin
u32 data_ov041_021118f8[5] = {
    0x000000d2,
    (u32)data_ov041_021111a4,
    (u32)data_ov041_021111a0,
    (u32)data_ov041_021111e0,
    0x00000002,
};
#pragma section s_021118f8 end

#pragma define_section s_0211190c ".data.data_ov041_0211190c" ".data.data_ov041_0211190c" ".data.data_ov041_0211190c" far_data
#pragma section s_0211190c begin
u32 data_ov041_0211190c[1] = {
    (u32)data_ov041_021111a8,
};
#pragma section s_0211190c end

#pragma define_section s_02111910 ".data.data_ov041_02111910" ".data.data_ov041_02111910" ".data.data_ov041_02111910" far_data
#pragma section s_02111910 begin
u32 data_ov041_02111910[6] = {
    0x01b80306,
    0x04e2015e,
    0x00000000,
    0xff020000,
    0x01f4ede4,
    0x0000ff9c,
};
#pragma section s_02111910 end

#pragma define_section s_02111928 ".data.data_ov041_02111928" ".data.data_ov041_02111928" ".data.data_ov041_02111928" far_data
#pragma section s_02111928 begin
u32 data_ov041_02111928[1] = {
    0x00004000,
};
#pragma section s_02111928 end

#pragma define_section s_0211192c ".data.data_ov041_0211192c" ".data.data_ov041_0211192c" ".data.data_ov041_0211192c" far_data
#pragma section s_0211192c begin
u32 data_ov041_0211192c[6] = {
    (u32)data_ov041_021119f8,
    (u32)data_ov041_02111894,
    0x079f07a1,
    0x07a307a2,
    (u32)data_ov041_021118b4,
    0x05dc0001,
};
#pragma section s_0211192c end

#pragma define_section s_02111944 ".data.data_ov041_02111944" ".data.data_ov041_02111944" ".data.data_ov041_02111944" far_data
#pragma section s_02111944 begin
u32 data_ov041_02111944[1] = {
    0x0000000f,
};
#pragma section s_02111944 end

#pragma define_section s_02111948 ".data.data_ov041_02111948" ".data.data_ov041_02111948" ".data.data_ov041_02111948" far_data
#pragma section s_02111948 begin
u32 data_ov041_02111948[6] = {
    0x000000c8,
    0x00c8e3e0,
    0x07d00000,
    0x00000000,
    0x000006a4,
    0x0af00000,
};
#pragma section s_02111948 end

#pragma define_section s_02111960 ".data.data_ov041_02111960" ".data.data_ov041_02111960" ".data.data_ov041_02111960" far_data
#pragma section s_02111960 begin
u32 data_ov041_02111960[6] = {
    0x0000ff38,
    0xff380bb8,
    0x0ce40000,
    0x015efe48,
    0x01b804e2,
    0x04e2015e,
};
#pragma section s_02111960 end

#pragma define_section s_02111978 ".data.data_ov041_02111978" ".data.data_ov041_02111978" ".data.data_ov041_02111978" far_data
#pragma section s_02111978 begin
u32 data_ov041_02111978[7] = {
    0x00000802,
    (u32)data_ov041_02111948,
    0x00000403,
    (u32)&data_ov041_021118e0,
    0x00000204,
    (u32)data_ov041_02111910,
    0x0000010b,
};
#pragma section s_02111978 end

#pragma define_section s_02111994 ".data.data_ov041_02111994" ".data.data_ov041_02111994" ".data.data_ov041_02111994" far_data
#pragma section s_02111994 begin
u32 data_ov041_02111994[9] = {
    (u32)data_ov041_02111884,
    0x00000401,
    (u32)data_ov041_021119b8,
    0x0000010c,
    (u32)data_ov041_02111880,
    0x0000010e,
    (u32)data_ov041_02111888,
    0x0000010a,
    (u32)data_ov041_021118c0,
};
#pragma section s_02111994 end

#pragma define_section s_021119b8 ".data.data_ov041_021119b8" ".data.data_ov041_021119b8" ".data.data_ov041_021119b8" far_data
#pragma section s_021119b8 begin
u32 data_ov041_021119b8[16] = {
    0xef980000,
    0xffce01a4,
    0x40000000,
    0x02080000,
    0xeed00000,
    0x006401a4,
    0x40000000,
    0x02780000,
    0xf0600000,
    0xff3801a4,
    0x40000000,
    0x02780000,
    0xf0600000,
    0x006401a4,
    0x40000000,
    0x02780000,
};
#pragma section s_021119b8 end

#pragma define_section s_021119f8 ".data.data_ov041_021119f8" ".data.data_ov041_021119f8" ".data.data_ov041_021119f8" far_data
#pragma section s_021119f8 begin
u32 data_ov041_021119f8[21] = {
    0x53504c43,
    0x00090008,
    0x00030fc4,
    0x000000ff,
    0x00032fc4,
    0x000000ff,
    0x04030000,
    0x000000ff,
    0x04030fc0,
    0x000000ff,
    0x00000023,
    0x000000ff,
    0x00033000,
    0x000000ff,
    0x00030000,
    0x00000002,
    0x00030000,
    0x00000001,
    0x00030fc0,
    0x00000000,
    0xffffffff,
};
#pragma section s_021119f8 end

#pragma define_section s_02111a4c ".data.data_ov041_02111a4c" ".data.data_ov041_02111a4c" ".data.data_ov041_02111a4c" far_data
#pragma section s_02111a4c begin
u32 data_ov041_02111a4c[3] = {
    0xffffffff,
    0xffffffff,
    0xffffffff,
};
#pragma section s_02111a4c end

#pragma define_section s_02111a58 ".data.data_ov041_02111a58" ".data.data_ov041_02111a58" ".data.data_ov041_02111a58" far_data
#pragma section s_02111a58 begin
u32 data_ov041_02111a58[8] = {
    0xff9c004c,
    0x03200013,
    0xc000ffff,
    0x06d80000,
    0x0000001f,
    0x08980032,
    0x00000000,
    0xff020000,
};
#pragma section s_02111a58 end

#pragma define_section s_02111a78 ".data.data_ov041_02111a78" ".data.data_ov041_02111a78" ".data.data_ov041_02111a78" far_data
#pragma section s_02111a78 begin
u32 data_ov041_02111a78[17] = {
    0xfea2001e,
    0xff9c010d,
    0x00000000,
    0xff060000,
    0x0000001e,
    0xe8900269,
    0x00000000,
    0xff060000,
    0xf54000f7,
    0xffcc0013,
    0x00000000,
    0xffff0000,
    0xfb0600f7,
    0x02f30013,
    0x00000000,
    0xffff0000,
    0x015e00f7,
};
#pragma section s_02111a78 end

#pragma define_section s_02111abc ".data.data_ov041_02111abc" ".data.data_ov041_02111abc" ".data.data_ov041_02111abc" far_data
#pragma section s_02111abc begin
u32 data_ov041_02111abc[6] = {
    0xfb260068,
    0x00000000,
    0xffff0000,
    0xfe8500f7,
    0xf4da0083,
    0x00000000,
};
#pragma section s_02111abc end

#pragma define_section s_02111ad4 ".data.data_ov041_02111ad4" ".data.data_ov041_02111ad4" ".data.data_ov041_02111ad4" far_data
#pragma section s_02111ad4 begin
u32 data_ov041_02111ad4[8] = {
    0xffff0000,
    0xfede00fe,
    0xe96f015c,
    0x00000000,
    0x10040000,
    0x019000fe,
    0xf8f8006c,
    0x00000000,
};
#pragma section s_02111ad4 end

#pragma define_section s_02111af4 ".data.data_ov041_02111af4" ".data.data_ov041_02111af4" ".data.data_ov041_02111af4" far_data
#pragma section s_02111af4 begin
u32 data_ov041_02111af4[8] = {
    0x02040000,
    0x019000fe,
    0xfce00061,
    0x00000000,
    0x11040000,
    0x012c000f,
    0xebb00103,
    0x00000000,
};
#pragma section s_02111af4 end

#pragma define_section s_02111b14 ".data.data_ov041_02111b14" ".data.data_ov041_02111b14" ".data.data_ov041_02111b14" far_data
#pragma section s_02111b14 begin
u32 data_ov041_02111b14[8] = {
    0xf1f20000,
    0x03960014,
    0xf22b00fa,
    0xc0000003,
    0xffff0000,
    0xfe5c0090,
    0xe3d60000,
    0x00000000,
};
#pragma section s_02111b14 end

#pragma define_section s_02111b34 ".data.data_ov041_02111b34" ".data.data_ov041_02111b34" ".data.data_ov041_02111b34" far_data
#pragma section s_02111b34 begin
u32 data_ov041_02111b34[4] = {
    0xffff0000,
    0xff740090,
    0xe3fe0000,
    0x00000000,
};
#pragma section s_02111b34 end

#pragma define_section s_02111b44 ".data.data_ov041_02111b44" ".data.data_ov041_02111b44" ".data.data_ov041_02111b44" far_data
#pragma section s_02111b44 begin
u32 data_ov041_02111b44[8] = {
    0xffff0000,
    0x00960090,
    0xe3fe0000,
    0x00000000,
    0xffff0000,
    0x01a40090,
    0xe3d60000,
    0x00000000,
};
#pragma section s_02111b44 end

#pragma define_section s_02111b64 ".data.data_ov041_02111b64" ".data.data_ov041_02111b64" ".data.data_ov041_02111b64" far_data
#pragma section s_02111b64 begin
u32 data_ov041_02111b64[1] = {
    0xffff0000,
};
#pragma section s_02111b64 end

#pragma define_section s_02111b68 ".data.data_ov041_02111b68" ".data.data_ov041_02111b68" ".data.data_ov041_02111b68" far_data
#pragma section s_02111b68 begin
u32 data_ov041_02111b68[1] = {
    0xfede00fe,
};
#pragma section s_02111b68 end

#pragma define_section s_02111b6c ".data.data_ov041_02111b6c" ".data.data_ov041_02111b6c" ".data.data_ov041_02111b6c" far_data
#pragma section s_02111b6c begin
u32 data_ov041_02111b6c[2] = {
    0xe96f015a,
    0x00000000,
};
#pragma section s_02111b6c end

#pragma define_section s_02111b74 ".data.data_ov041_02111b74" ".data.data_ov041_02111b74" ".data.data_ov041_02111b74" far_data
#pragma section s_02111b74 begin
u32 data_ov041_02111b74[5] = {
    0x00040000,
    0x019000fe,
    0xfcae0065,
    0x00000000,
    0x01040000,
};
#pragma section s_02111b74 end

#pragma define_section s_02111b88 ".data.data_ov041_02111b88" ".data.data_ov041_02111b88" ".data.data_ov041_02111b88" far_data
#pragma section s_02111b88 begin
u32 data_ov041_02111b88[19] = {
    0x03ac2226,
    0xf13300fa,
    0x03a92226,
    0xf30c00fa,
    0x00612226,
    0xea8afeec,
    0xfd912226,
    0xe97ffead,
    0xff082226,
    0xe7db015f,
    0x00752226,
    0xe78b0160,
    0x02162226,
    0xe7f1fe9e,
    0xfe602226,
    0xe67dfe7a,
    0xfffbe225,
    0xfaedffc8,
    0xfffbe225,
};
#pragma section s_02111b88 end

#pragma define_section s_02111bd4 ".data.data_ov041_02111bd4" ".data.data_ov041_02111bd4" ".data.data_ov041_02111bd4" far_data
#pragma section s_02111bd4 begin
u32 data_ov041_02111bd4[3] = {
    0xfa42ffbf,
    0xfffbe225,
    0xf98bff9c,
};
#pragma section s_02111bd4 end

#pragma define_section s_02111be0 ".data.data_ov041_02111be0" ".data.data_ov041_02111be0" ".data.data_ov041_02111be0" far_data
#pragma section s_02111be0 begin
u32 data_ov041_02111be0[16] = {
    0xfffbe225,
    0xf8dfff6f,
    0xfffbe225,
    0xf83aff69,
    0x01aae225,
    0xf0d800fa,
    0x01aae225,
    0xf02c00fa,
    0x01aae225,
    0xef7c00fa,
    0x01aae225,
    0xeed600fa,
    0x01aae225,
    0xee2700fa,
    0x0000e225,
    0xf254ff42,
};
#pragma section s_02111be0 end

#pragma define_section s_02111c20 ".data.data_ov041_02111c20" ".data.data_ov041_02111c20" ".data.data_ov041_02111c20" far_data
#pragma section s_02111c20 begin
u32 data_ov041_02111c20[13] = {
    0x0000e225,
    0xf1a7ff3d,
    0x0000e225,
    0xf105ff38,
    0x0000e225,
    0xf067ff34,
    0x0000e225,
    0xefc9ff30,
    0xfeade225,
    0xe5b3fe64,
    0xff59e225,
    0xe67ffe78,
    0x0048e225,
};
#pragma section s_02111c20 end

#pragma define_section s_02111c54 ".data.data_ov041_02111c54" ".data.data_ov041_02111c54" ".data.data_ov041_02111c54" far_data
#pragma section s_02111c54 begin
u32 data_ov041_02111c54[29] = {
    0xe68ffe85,
    0x0116e225,
    0xe617fe77,
    0x011de225,
    0xe523fe47,
    0x0078e225,
    0xe485fe07,
    0xff9ae225,
    0xe468fdff,
    0xfed7e225,
    0xe4d9fe36,
    0xffe2023f,
    0xe57bfe8e,
    0x0000e225,
    0xe89005dc,
    0x0000e225,
    0xe8900672,
    0x0000e225,
    0xe8900708,
    0x0000e225,
    0xe890079e,
    0x0000e225,
    0xe8900834,
    0x0000e225,
    0xe89008ca,
    0x0000e225,
    0xe8900960,
    0xfecae225,
    0xe89007c1,
};
#pragma section s_02111c54 end

#pragma define_section s_02111cc8 ".data.data_ov041_02111cc8" ".data.data_ov041_02111cc8" ".data.data_ov041_02111cc8" far_data
#pragma section s_02111cc8 begin
u32 data_ov041_02111cc8[30] = {
    0xff1ae225,
    0xe8900850,
    0xff6ae225,
    0xe89008e6,
    0x0136e225,
    0xe89007c1,
    0x00e6e225,
    0xe8900850,
    0x0096e225,
    0xe89008e6,
    0x0000043d,
    0xe8900c4e,
    0x0000e225,
    0xe8900384,
    0x0000e225,
    0xe890041a,
    0x0000e225,
    0xe89004b0,
    0x0000e225,
    0xe8900546,
    0xfffff143,
    0xe3910000,
    0xfffff343,
    0x0e10fe4d,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
};
#pragma section s_02111cc8 end

