/* Overlay 8 carries no code. Each object is the cartridge symbol
 * of that name; a named section places it at its ROM address.
 */
#include "types.h"

extern u32 data_ov008_021111c0[];
extern u32 data_ov008_021111c4[];
extern u32 data_ov008_021111c8[];
extern u32 data_ov008_021111d0[];
extern u32 data_ov008_021111d8[];
extern u32 data_ov008_021111e0[];
extern u32 data_ov008_021111ec[];
extern u32 data_ov008_021111fc[];
extern u32 data_ov008_02111204[];
extern u32 data_ov008_02111210[];
extern u32 data_ov008_02111218[];
extern u32 data_ov008_02111228[];
extern u32 data_ov008_0211122c[];
extern u32 data_ov008_02111244[];
extern u32 data_ov008_0211124c[];
extern u32 data_ov008_0211126c[];
extern u32 data_ov008_0211128c[];
extern u32 data_ov008_021112ac[];
extern u32 data_ov008_021112b0[];
extern u32 data_ov008_021112d0[];
extern u32 data_ov008_021112f8[];
extern u32 data_ov008_02111320[];
extern u32 data_ov008_02111350[];
extern u32 data_ov008_02111380[];
extern u32 data_ov008_021113ac[];
extern u32 data_ov008_021113b0[];
extern u32 data_ov008_021113c0[];
extern u32 data_ov008_021113d0[];
extern u32 data_ov008_02111400[];
extern u32 data_ov008_02111438[];
extern u32 data_ov008_02111450[];
extern u32 data_ov008_021114a8[];
extern u32 data_ov008_02111508[];
extern u32 data_ov008_0211150c[];
extern u32 data_ov008_02111520[];
extern u32 data_ov008_0211153c[];
extern u32 data_ov008_02111548[];
extern u32 data_ov008_0211155c[];
extern u32 data_ov008_02111560[];
extern u32 data_ov008_0211159c[];
extern u32 data_ov008_021115bc[];
extern u32 data_ov008_021115c8[];
extern u32 data_ov008_021115e4[];
extern u32 data_ov008_021115f4[];
extern u32 data_ov008_02111624[];
extern u32 data_ov008_02111640[];
extern u32 data_ov008_02111660[];
extern u32 data_ov008_02111680[];
extern u32 data_ov008_021116a0[];
extern u32 data_ov008_021116c0[];
extern u32 data_ov008_021116e0[];
extern u32 data_ov008_02111700[];
extern u32 data_ov008_02111720[];
extern u32 data_ov008_02111740[];
extern u32 data_ov008_02111760[];
extern u32 data_ov008_02111768[];
extern u32 data_ov008_02111834[];
extern u32 data_ov008_02111844[];
extern u32 data_ov008_021118b8[];
extern u32 data_ov008_0211190c[];
extern u32 data_ov008_02111928[];
extern u32 data_ov008_0211192c[];
extern u32 data_ov008_02111944[];
extern u32 data_ov008_02111960[];
extern u32 data_ov008_02111994[];
extern u32 data_ov008_02111a4c[];
extern u32 data_ov008_02111a58[];
extern u32 data_ov008_02111a78[];
extern u32 data_ov008_02111abc[];
extern u32 data_ov008_02111ad4[];
extern u32 data_ov008_02111af4[];
extern u32 data_ov008_02111b14[];
extern u32 data_ov008_02111b34[];
extern u32 data_ov008_02111b44[];
extern u32 data_ov008_02111b64[];
extern u32 data_ov008_02111b68[];
extern u32 data_ov008_02111b74[];
extern u32 data_ov008_02111b88[];
extern u32 data_ov008_02111bd4[];
extern u32 data_ov008_02111be0[];

#pragma define_section s_021111c0 ".data.data_ov008_021111c0" ".data.data_ov008_021111c0" ".data.data_ov008_021111c0" far_data
#pragma section s_021111c0 begin
u32 data_ov008_021111c0[1] = {
    0x000007d6,
};
#pragma section s_021111c0 end

#pragma define_section s_021111c4 ".data.data_ov008_021111c4" ".data.data_ov008_021111c4" ".data.data_ov008_021111c4" far_data
#pragma section s_021111c4 begin
u32 data_ov008_021111c4[1] = {
    0x00000004,
};
#pragma section s_021111c4 end

#pragma define_section s_021111c8 ".data.data_ov008_021111c8" ".data.data_ov008_021111c8" ".data.data_ov008_021111c8" far_data
#pragma section s_021111c8 begin
u32 data_ov008_021111c8[2] = {
    0x092ee225,
    0x05780000,
};
#pragma section s_021111c8 end

#pragma define_section s_021111d0 ".data.data_ov008_021111d0" ".data.data_ov008_021111d0" ".data.data_ov008_021111d0" far_data
#pragma section s_021111d0 begin
u32 data_ov008_021111d0[2] = {
    0x00000008,
    (u32)data_ov008_02111380,
};
#pragma section s_021111d0 end

#pragma define_section s_021111d8 ".data.data_ov008_021111d8" ".data.data_ov008_021111d8" ".data.data_ov008_021111d8" far_data
#pragma section s_021111d8 begin
u32 data_ov008_021111d8[2] = {
    0x0000000b,
    (u32)data_ov008_02111450,
};
#pragma section s_021111d8 end

#pragma define_section s_021111e0 ".data.data_ov008_021111e0" ".data.data_ov008_021111e0" ".data.data_ov008_021111e0" far_data
#pragma section s_021111e0 begin
u32 data_ov008_021111e0[3] = {
    (u32)data_ov008_021111d8,
    0x00000000,
    0x1f400000,
};
#pragma section s_021111e0 end

#pragma define_section s_021111ec ".data.data_ov008_021111ec" ".data.data_ov008_021111ec" ".data.data_ov008_021111ec" far_data
#pragma section s_021111ec begin
u32 data_ov008_021111ec[4] = {
    0x073aff02,
    0x173e0258,
    0x871d0000,
    0x00000000,
};
#pragma section s_021111ec end

#pragma define_section s_021111fc ".data.data_ov008_021111fc" ".data.data_ov008_021111fc" ".data.data_ov008_021111fc" far_data
#pragma section s_021111fc begin
u32 data_ov008_021111fc[2] = {
    0xff020000,
    0x00027fff,
};
#pragma section s_021111fc end

#pragma define_section s_02111204 ".data.data_ov008_02111204" ".data.data_ov008_02111204" ".data.data_ov008_02111204" far_data
#pragma section s_02111204 begin
u32 data_ov008_02111204[3] = {
    0x7fffff02,
    0xff020004,
    0x00007fff,
};
#pragma section s_02111204 end

#pragma define_section s_02111210 ".data.data_ov008_02111210" ".data.data_ov008_02111210" ".data.data_ov008_02111210" far_data
#pragma section s_02111210 begin
u32 data_ov008_02111210[2] = {
    (u32)data_ov008_02111508,
    (u32)data_ov008_021111d0,
};
#pragma section s_02111210 end

#pragma define_section s_02111218 ".data.data_ov008_02111218" ".data.data_ov008_02111218" ".data.data_ov008_02111218" far_data
#pragma section s_02111218 begin
u32 data_ov008_02111218[4] = {
    0x07d507d7,
    0x07d907d8,
    (u32)data_ov008_021111e0,
    0x06400101,
};
#pragma section s_02111218 end

#pragma define_section s_02111228 ".data.data_ov008_02111228" ".data.data_ov008_02111228" ".data.data_ov008_02111228" far_data
#pragma section s_02111228 begin
u32 data_ov008_02111228[1] = {
    0x0000002f,
};
#pragma section s_02111228 end

#pragma define_section s_0211122c ".data.data_ov008_0211122c" ".data.data_ov008_0211122c" ".data.data_ov008_0211122c" far_data
#pragma section s_0211122c begin
u32 data_ov008_0211122c[6] = {
    0x0000e5a2,
    0x0000e5a2,
    0x00001a5e,
    0x10001a5e,
    0x00001a5e,
    0x2000e5a2,
};
#pragma section s_0211122c end

#pragma define_section s_02111244 ".data.data_ov008_02111244" ".data.data_ov008_02111244" ".data.data_ov008_02111244" far_data
#pragma section s_02111244 begin
u32 data_ov008_02111244[2] = {
    0x0000e5a2,
    0x30001a5e,
};
#pragma section s_02111244 end

#pragma define_section s_0211124c ".data.data_ov008_0211124c" ".data.data_ov008_0211124c" ".data.data_ov008_0211124c" far_data
#pragma section s_0211124c begin
u32 data_ov008_0211124c[8] = {
    0x00001a5e,
    0x00001a5e,
    0x0000e5a2,
    0x0000e5a2,
    0x0000e5a2,
    0x00001a5e,
    0x00001a5e,
    0x0000e5a2,
};
#pragma section s_0211124c end

#pragma define_section s_0211126c ".data.data_ov008_0211126c" ".data.data_ov008_0211126c" ".data.data_ov008_0211126c" far_data
#pragma section s_0211126c begin
u32 data_ov008_0211126c[8] = {
    0x044cc63d,
    0xef020032,
    0x0866e225,
    0x05780000,
    0x0866e225,
    0x05140000,
    0x0866e225,
    0x04b00000,
};
#pragma section s_0211126c end

#pragma define_section s_0211128c ".data.data_ov008_0211128c" ".data.data_ov008_0211128c" ".data.data_ov008_0211128c" far_data
#pragma section s_0211128c begin
u32 data_ov008_0211128c[8] = {
    0x0802e225,
    0x05780000,
    0x0802e225,
    0x05140000,
    0x0802e225,
    0x04b00000,
    0x0802e225,
    0x044c0000,
};
#pragma section s_0211128c end

#pragma define_section s_021112ac ".data.data_ov008_021112ac" ".data.data_ov008_021112ac" ".data.data_ov008_021112ac" far_data
#pragma section s_021112ac begin
u32 data_ov008_021112ac[1] = {
    0x0000ec78,
};
#pragma section s_021112ac end

#pragma define_section s_021112b0 ".data.data_ov008_021112b0" ".data.data_ov008_021112b0" ".data.data_ov008_021112b0" far_data
#pragma section s_021112b0 begin
u32 data_ov008_021112b0[8] = {
    0xec7812f2,
    0xfce00000,
    0x0000e890,
    0xe89012f2,
    0xfce00000,
    0xffe7e4a8,
    0xe4a812f2,
    0xfce0ffe7,
};
#pragma section s_021112b0 end

#pragma define_section s_021112d0 ".data.data_ov008_021112d0" ".data.data_ov008_021112d0" ".data.data_ov008_021112d0" far_data
#pragma section s_021112d0 begin
u32 data_ov008_021112d0[10] = {
    0xfd430c3f,
    0x0cde0064,
    0x0a28ff0d,
    0x0fa00000,
    0x0b54ff0e,
    0x0fa00000,
    0x0c80ff0f,
    0x0fa00000,
    0x0dacff10,
    0x0fa00000,
};
#pragma section s_021112d0 end

#pragma define_section s_021112f8 ".data.data_ov008_021112f8" ".data.data_ov008_021112f8" ".data.data_ov008_021112f8" far_data
#pragma section s_021112f8 begin
u32 data_ov008_021112f8[10] = {
    0x079ee225,
    0x05780000,
    0x079ee225,
    0x05140000,
    0x079ee225,
    0x04b00000,
    0x079ee225,
    0x044c0000,
    0x079ee225,
    0x03e80000,
};
#pragma section s_021112f8 end

#pragma define_section s_02111320 ".data.data_ov008_02111320" ".data.data_ov008_02111320" ".data.data_ov008_02111320" far_data
#pragma section s_02111320 begin
u32 data_ov008_02111320[12] = {
    0x161a000f,
    0x19690000,
    0x00000000,
    0xffff0000,
    0xfa36000f,
    0x0aa40000,
    0x00000000,
    0xf12f0000,
    0xfce0004c,
    0x15ae0000,
    0x4000ffff,
    0xffff0000,
};
#pragma section s_02111320 end

#pragma define_section s_02111350 ".data.data_ov008_02111350" ".data.data_ov008_02111350" ".data.data_ov008_02111350" far_data
#pragma section s_02111350 begin
u32 data_ov008_02111350[12] = {
    0x073ae225,
    0x05780000,
    0x073ae225,
    0x05140000,
    0x073ae225,
    0x04b00000,
    0x073ae225,
    0x044c0000,
    0x073ae225,
    0x03e80000,
    0x073ae225,
    0x03840000,
};
#pragma section s_02111350 end

#pragma define_section s_02111380 ".data.data_ov008_02111380" ".data.data_ov008_02111380" ".data.data_ov008_02111380" far_data
#pragma section s_02111380 begin
u32 data_ov008_02111380[11] = {
    0x00000602,
    (u32)data_ov008_021112ac,
    0x00000303,
    (u32)data_ov008_021111fc,
    0x00000104,
    (u32)data_ov008_021111ec,
    0x0000010b,
    (u32)data_ov008_021111c0,
    0x00000401,
    (u32)data_ov008_021113c0,
    0x00000406,
};
#pragma section s_02111380 end

#pragma define_section s_021113ac ".data.data_ov008_021113ac" ".data.data_ov008_021113ac" ".data.data_ov008_021113ac" far_data
#pragma section s_021113ac begin
u32 data_ov008_021113ac[1] = {
    (u32)data_ov008_0211122c,
};
#pragma section s_021113ac end

#pragma define_section s_021113b0 ".data.data_ov008_021113b0" ".data.data_ov008_021113b0" ".data.data_ov008_021113b0" far_data
#pragma section s_021113b0 begin
u32 data_ov008_021113b0[4] = {
    0x00000407,
    (u32)data_ov008_0211124c,
    0x0000010e,
    (u32)data_ov008_021111c4,
};
#pragma section s_021113b0 end

#pragma define_section s_021113c0 ".data.data_ov008_021113c0" ".data.data_ov008_021113c0" ".data.data_ov008_021113c0" far_data
#pragma section s_021113c0 begin
u32 data_ov008_021113c0[4] = {
    0x04e20000,
    0x11f80000,
    0x80000000,
    0x00000000,
};
#pragma section s_021113c0 end

#pragma define_section s_021113d0 ".data.data_ov008_021113d0" ".data.data_ov008_021113d0" ".data.data_ov008_021113d0" far_data
#pragma section s_021113d0 begin
u32 data_ov008_021113d0[12] = {
    0x06400000,
    0x11f80000,
    0x80000000,
    0x00000000,
    0x03840000,
    0x11f80000,
    0x80000000,
    0x00000000,
    0x07d00000,
    0x11f80000,
    0x80000000,
    0x00000000,
};
#pragma section s_021113d0 end

#pragma define_section s_02111400 ".data.data_ov008_02111400" ".data.data_ov008_02111400" ".data.data_ov008_02111400" far_data
#pragma section s_02111400 begin
u32 data_ov008_02111400[14] = {
    0x09c4000f,
    0x154a0000,
    0x00000000,
    0xff1f0000,
    0x07d0000f,
    0x154a0000,
    0x00000000,
    0xff1f0000,
    0x05dc000f,
    0x154a0000,
    0x00000000,
    0xff1f0000,
    0x03b6000f,
    0x154a0000,
};
#pragma section s_02111400 end

#pragma define_section s_02111438 ".data.data_ov008_02111438" ".data.data_ov008_02111438" ".data.data_ov008_02111438" far_data
#pragma section s_02111438 begin
u32 data_ov008_02111438[6] = {
    0x00000000,
    0xff1f0000,
    0x0190000f,
    0x154a0000,
    0x00000000,
    0xff1f0000,
};
#pragma section s_02111438 end

#pragma define_section s_02111450 ".data.data_ov008_02111450" ".data.data_ov008_02111450" ".data.data_ov008_02111450" far_data
#pragma section s_02111450 begin
u32 data_ov008_02111450[22] = {
    0x00002f00,
    (u32)data_ov008_021115c8,
    0x00000505,
    (u32)data_ov008_021112d0,
    0x00000320,
    (u32)data_ov008_02111320,
    0x00000125,
    (u32)data_ov008_021111c8,
    0x00000c45,
    (u32)data_ov008_021114a8,
    0x00000560,
    (u32)data_ov008_02111400,
    0x00000465,
    (u32)data_ov008_0211126c,
    0x00000485,
    (u32)data_ov008_0211128c,
    0x000005a5,
    (u32)data_ov008_021112f8,
    0x000006c5,
    (u32)data_ov008_02111350,
    0x00006be5,
    (u32)data_ov008_021118b8,
};
#pragma section s_02111450 end

#pragma define_section s_021114a8 ".data.data_ov008_021114a8" ".data.data_ov008_021114a8" ".data.data_ov008_021114a8" far_data
#pragma section s_021114a8 begin
u32 data_ov008_021114a8[24] = {
    0x028a423f,
    0x0dac00fa,
    0x044cc43d,
    0xef020032,
    0x028a603e,
    0xec140064,
    0x044c643e,
    0xe9260064,
    0x0640663e,
    0xec140064,
    0x0834683e,
    0xe9260064,
    0x028a203f,
    0xec140064,
    0x044c243f,
    0xe9260064,
    0x0640263f,
    0xec140064,
    0x0834283f,
    0xe9260064,
    0x08cae225,
    0x05780000,
    0x08cae225,
    0x05140000,
};
#pragma section s_021114a8 end

#pragma define_section s_02111508 ".data.data_ov008_02111508" ".data.data_ov008_02111508" ".data.data_ov008_02111508" far_data
#pragma section s_02111508 begin
u32 data_ov008_02111508[1] = {
    0x53504c43,
};
#pragma section s_02111508 end

#pragma define_section s_0211150c ".data.data_ov008_0211150c" ".data.data_ov008_0211150c" ".data.data_ov008_0211150c" far_data
#pragma section s_0211150c begin
u32 data_ov008_0211150c[5] = {
    0x00150008,
    0x00040fc0,
    0x00000002,
    0x00040fc0,
    0x000000ff,
};
#pragma section s_0211150c end

#pragma define_section s_02111520 ".data.data_ov008_02111520" ".data.data_ov008_02111520" ".data.data_ov008_02111520" far_data
#pragma section s_02111520 begin
u32 data_ov008_02111520[7] = {
    0x00440fc8,
    0x00000001,
    0x00040fc0,
    0x00000000,
    0x00040fc2,
    0x000000ff,
    0x00040fc6,
};
#pragma section s_02111520 end

#pragma define_section s_0211153c ".data.data_ov008_0211153c" ".data.data_ov008_0211153c" ".data.data_ov008_0211153c" far_data
#pragma section s_0211153c begin
u32 data_ov008_0211153c[3] = {
    0x000000ff,
    0x00040fc9,
    0x000000ff,
};
#pragma section s_0211153c end

#pragma define_section s_02111548 ".data.data_ov008_02111548" ".data.data_ov008_02111548" ".data.data_ov008_02111548" far_data
#pragma section s_02111548 begin
u32 data_ov008_02111548[5] = {
    0x00040fc7,
    0x000000ff,
    0x00040fc5,
    0x000000ff,
    0x00040fc1,
};
#pragma section s_02111548 end

#pragma define_section s_0211155c ".data.data_ov008_0211155c" ".data.data_ov008_0211155c" ".data.data_ov008_0211155c" far_data
#pragma section s_0211155c begin
u32 data_ov008_0211155c[1] = {
    0x000000ff,
};
#pragma section s_0211155c end

#pragma define_section s_02111560 ".data.data_ov008_02111560" ".data.data_ov008_02111560" ".data.data_ov008_02111560" far_data
#pragma section s_02111560 begin
u32 data_ov008_02111560[15] = {
    0x00040fc3,
    0x000000ff,
    0x00040fc4,
    0x000000ff,
    0x00040fc8,
    0x000000ff,
    0x00000fc0,
    0x000000ff,
    0x00040fcc,
    0x000000ff,
    0x018c0fcc,
    0x000000ff,
    0x011c0fcc,
    0x000000ff,
    0x001c0fc0,
};
#pragma section s_02111560 end

#pragma define_section s_0211159c ".data.data_ov008_0211159c" ".data.data_ov008_0211159c" ".data.data_ov008_0211159c" far_data
#pragma section s_0211159c begin
u32 data_ov008_0211159c[8] = {
    0x000000ff,
    0x04040fc0,
    0x000000ff,
    0x01040fc0,
    0x000000ff,
    0x00040fe0,
    0x000000ff,
    0xffffffff,
};
#pragma section s_0211159c end

#pragma define_section s_021115bc ".data.data_ov008_021115bc" ".data.data_ov008_021115bc" ".data.data_ov008_021115bc" far_data
#pragma section s_021115bc begin
u32 data_ov008_021115bc[3] = {
    0xffffffff,
    0xffffffff,
    0xffffffff,
};
#pragma section s_021115bc end

#pragma define_section s_021115c8 ".data.data_ov008_021115c8" ".data.data_ov008_021115c8" ".data.data_ov008_021115c8" far_data
#pragma section s_021115c8 begin
u32 data_ov008_021115c8[6] = {
    0xf5a600f3,
    0x09f60000,
    0x00000000,
    0x01310000,
    0xfc7c0017,
    0x08fc0000,
};
#pragma section s_021115c8 end

#pragma define_section s_021115e4 ".data.data_ov008_021115e4" ".data.data_ov008_021115e4" ".data.data_ov008_021115e4" far_data
#pragma section s_021115e4 begin
u32 data_ov008_021115e4[4] = {
    0xffff0000,
    0xfe0c0017,
    0x08fc0000,
    0x00000000,
};
#pragma section s_021115e4 end

#pragma define_section s_021115f4 ".data.data_ov008_021115f4" ".data.data_ov008_021115f4" ".data.data_ov008_021115f4" far_data
#pragma section s_021115f4 begin
u32 data_ov008_021115f4[12] = {
    0xffff0000,
    0xfd440017,
    0x08fc0000,
    0x00000000,
    0xffff0000,
    0xfc1800f5,
    0x0af00000,
    0x00000000,
    0xffff0000,
    0xfe7000f5,
    0x0af00000,
    0x00000000,
};
#pragma section s_021115f4 end

#pragma define_section s_02111624 ".data.data_ov008_02111624" ".data.data_ov008_02111624" ".data.data_ov008_02111624" far_data
#pragma section s_02111624 begin
u32 data_ov008_02111624[7] = {
    0xffff0000,
    0xfd4400f5,
    0x0af00000,
    0x00000000,
    0xffff0000,
    0xf7cc00f3,
    0x09f60000,
};
#pragma section s_02111624 end

#pragma define_section s_02111640 ".data.data_ov008_02111640" ".data.data_ov008_02111640" ".data.data_ov008_02111640" far_data
#pragma section s_02111640 begin
u32 data_ov008_02111640[8] = {
    0x00000000,
    0x01630000,
    0xfce0004c,
    0x10cc0000,
    0x0000ffff,
    0x03e80000,
    0x0960012e,
    0x0a280000,
};
#pragma section s_02111640 end

#pragma define_section s_02111660 ".data.data_ov008_02111660" ".data.data_ov008_02111660" ".data.data_ov008_02111660" far_data
#pragma section s_02111660 begin
u32 data_ov008_02111660[8] = {
    0x00000000,
    0x00f30000,
    0x02bc0021,
    0x09f600fa,
    0x00000000,
    0xffff0000,
    0x047e0023,
    0x09f600fa,
};
#pragma section s_02111660 end

#pragma define_section s_02111680 ".data.data_ov008_02111680" ".data.data_ov008_02111680" ".data.data_ov008_02111680" far_data
#pragma section s_02111680 begin
u32 data_ov008_02111680[8] = {
    0x00000000,
    0xffff0000,
    0x06400022,
    0x09f600fa,
    0x00000000,
    0xffff0000,
    0x015e001e,
    0x0dac00fa,
};
#pragma section s_02111680 end

#pragma define_section s_021116a0 ".data.data_ov008_021116a0" ".data.data_ov008_021116a0" ".data.data_ov008_021116a0" far_data
#pragma section s_021116a0 begin
u32 data_ov008_021116a0[8] = {
    0x00000000,
    0x05000000,
    0x028a001e,
    0x0dac00fa,
    0x00000000,
    0x01010000,
    0x03b6001e,
    0x0dac00fa,
};
#pragma section s_021116a0 end

#pragma define_section s_021116c0 ".data.data_ov008_021116c0" ".data.data_ov008_021116c0" ".data.data_ov008_021116c0" far_data
#pragma section s_021116c0 begin
u32 data_ov008_021116c0[8] = {
    0x00000000,
    0xff020000,
    0x04e2001e,
    0x0dac00fa,
    0x00000000,
    0xff030000,
    0x060e001e,
    0x0dac00fa,
};
#pragma section s_021116c0 end

#pragma define_section s_021116e0 ".data.data_ov008_021116e0" ".data.data_ov008_021116e0" ".data.data_ov008_021116e0" far_data
#pragma section s_021116e0 begin
u32 data_ov008_021116e0[8] = {
    0x00000000,
    0xff040000,
    0x073a001e,
    0x0dac00fa,
    0x00000000,
    0xff050000,
    0x0866001e,
    0x0dac00fa,
};
#pragma section s_021116e0 end

#pragma define_section s_02111700 ".data.data_ov008_02111700" ".data.data_ov008_02111700" ".data.data_ov008_02111700" far_data
#pragma section s_02111700 begin
u32 data_ov008_02111700[8] = {
    0x00000000,
    0xff060000,
    0x05aa001b,
    0x0fd20000,
    0x00000000,
    0xffff0000,
    0x0000002c,
    0x10d80000,
};
#pragma section s_02111700 end

#pragma define_section s_02111720 ".data.data_ov008_02111720" ".data.data_ov008_02111720" ".data.data_ov008_02111720" far_data
#pragma section s_02111720 begin
u32 data_ov008_02111720[8] = {
    0x00000000,
    0xff060000,
    0x0991001e,
    0x0da300fa,
    0x00000000,
    0xff070000,
    0x04b0001c,
    0x00320000,
};
#pragma section s_02111720 end

#pragma define_section s_02111740 ".data.data_ov008_02111740" ".data.data_ov008_02111740" ".data.data_ov008_02111740" far_data
#pragma section s_02111740 begin
u32 data_ov008_02111740[8] = {
    0x00000000,
    0xffff0000,
    0x04b0001c,
    0xfda80000,
    0x00000000,
    0xffff0000,
    0x06720017,
    0x00320000,
};
#pragma section s_02111740 end

#pragma define_section s_02111760 ".data.data_ov008_02111760" ".data.data_ov008_02111760" ".data.data_ov008_02111760" far_data
#pragma section s_02111760 begin
u32 data_ov008_02111760[2] = {
    0x00000000,
    0xffff0000,
};
#pragma section s_02111760 end

#pragma define_section s_02111768 ".data.data_ov008_02111768" ".data.data_ov008_02111768" ".data.data_ov008_02111768" far_data
#pragma section s_02111768 begin
u32 data_ov008_02111768[51] = {
    0x06720017,
    0xfda80000,
    0x00000000,
    0xffff0000,
    0x0834011c,
    0x00320000,
    0x00000000,
    0xffff0000,
    0x0834011c,
    0xfda8012c,
    0x00000000,
    0xffff0000,
    0x09c4011d,
    0x00320000,
    0x00000000,
    0xffff0000,
    0x09c4011d,
    0xfda80000,
    0x00000000,
    0xffff0000,
    0x0834011c,
    0xfda80000,
    0x00000000,
    0xffff0000,
    0x06720017,
    0xfda800c8,
    0x00000000,
    0xffff0000,
    0x09c4011d,
    0xfda8012c,
    0x00000000,
    0xffff0000,
    0x148800fe,
    0xfd470000,
    0x00000000,
    0x01040000,
    0x089800fe,
    0x0a280000,
    0xc0000000,
    0x01040000,
    0x15ef00fe,
    0xfd4d0000,
    0x00000000,
    0x12040000,
    0x089800fe,
    0x0b220000,
    0xc0000000,
    0x02040000,
    0x134400fe,
    0xfd420000,
    0x00000000,
};
#pragma section s_02111768 end

#pragma define_section s_02111834 ".data.data_ov008_02111834" ".data.data_ov008_02111834" ".data.data_ov008_02111834" far_data
#pragma section s_02111834 begin
u32 data_ov008_02111834[4] = {
    0x00040000,
    0x089800fe,
    0x08fc0000,
    0xc0000000,
};
#pragma section s_02111834 end

#pragma define_section s_02111844 ".data.data_ov008_02111844" ".data.data_ov008_02111844" ".data.data_ov008_02111844" far_data
#pragma section s_02111844 begin
u32 data_ov008_02111844[29] = {
    0x10040000,
    0x1398000f,
    0x084b0000,
    0x00000000,
    0xf0f10000,
    0x06bc000f,
    0x17640000,
    0x00000000,
    0xf0f10000,
    0xf64a000f,
    0x1bbe0000,
    0x00000000,
    0xf1f10000,
    0x0a280017,
    0x0fa00000,
    0x00000000,
    0xffff0000,
    0x0b540017,
    0x0fa00000,
    0x00000000,
    0xffff0000,
    0x0c800017,
    0x0fa00000,
    0x00000000,
    0xffff0000,
    0x0dac0017,
    0x0fa00000,
    0x00000000,
    0xffff0000,
};
#pragma section s_02111844 end

#pragma define_section s_021118b8 ".data.data_ov008_021118b8" ".data.data_ov008_021118b8" ".data.data_ov008_021118b8" far_data
#pragma section s_021118b8 begin
u32 data_ov008_021118b8[21] = {
    0x0d48e225,
    0x136a0000,
    0x0d48e225,
    0x13060000,
    0x0d48e225,
    0x12a20000,
    0x0d48e225,
    0x123e0000,
    0x0d48e225,
    0x11da0000,
    0x0d48e225,
    0x155e0000,
    0x0d48e225,
    0x14fa0000,
    0x0d48e225,
    0x14960000,
    0x0d48e225,
    0x14320000,
    0x0d48e225,
    0x13ce0000,
    0x0dace225,
};
#pragma section s_021118b8 end

#pragma define_section s_0211190c ".data.data_ov008_0211190c" ".data.data_ov008_0211190c" ".data.data_ov008_0211190c" far_data
#pragma section s_0211190c begin
u32 data_ov008_0211190c[7] = {
    0x136a0000,
    0x0dace225,
    0x13060000,
    0x0dace225,
    0x12a20000,
    0x0dace225,
    0x123e0000,
};
#pragma section s_0211190c end

#pragma define_section s_02111928 ".data.data_ov008_02111928" ".data.data_ov008_02111928" ".data.data_ov008_02111928" far_data
#pragma section s_02111928 begin
u32 data_ov008_02111928[1] = {
    0x0dace225,
};
#pragma section s_02111928 end

#pragma define_section s_0211192c ".data.data_ov008_0211192c" ".data.data_ov008_0211192c" ".data.data_ov008_0211192c" far_data
#pragma section s_0211192c begin
u32 data_ov008_0211192c[6] = {
    0x11da0000,
    0x0dace225,
    0x155e0000,
    0x0dace225,
    0x14fa0000,
    0x0dace225,
};
#pragma section s_0211192c end

#pragma define_section s_02111944 ".data.data_ov008_02111944" ".data.data_ov008_02111944" ".data.data_ov008_02111944" far_data
#pragma section s_02111944 begin
u32 data_ov008_02111944[7] = {
    0x14960000,
    0x0dace225,
    0x14320000,
    0x0dace225,
    0x13ce0000,
    0x0e10e225,
    0x136a0000,
};
#pragma section s_02111944 end

#pragma define_section s_02111960 ".data.data_ov008_02111960" ".data.data_ov008_02111960" ".data.data_ov008_02111960" far_data
#pragma section s_02111960 begin
u32 data_ov008_02111960[13] = {
    0x0e10e225,
    0x13060000,
    0x0e10e225,
    0x12a20000,
    0x0e10e225,
    0x123e0000,
    0x0e10e225,
    0x11da0000,
    0x0e10e225,
    0x155e0000,
    0x0e10e225,
    0x14fa0000,
    0x0e10e225,
};
#pragma section s_02111960 end

#pragma define_section s_02111994 ".data.data_ov008_02111994" ".data.data_ov008_02111994" ".data.data_ov008_02111994" far_data
#pragma section s_02111994 begin
u32 data_ov008_02111994[46] = {
    0x14960000,
    0x0e10e225,
    0x14320000,
    0x0e10e225,
    0x13ce0000,
    0x0e74e225,
    0x136a0000,
    0x0e74e225,
    0x13060000,
    0x0e74e225,
    0x12a20000,
    0x0e74e225,
    0x123e0000,
    0x0e74e225,
    0x11da0000,
    0x0e74e225,
    0x155e0000,
    0x0e74e225,
    0x14fa0000,
    0x0e74e225,
    0x14960000,
    0x0e74e225,
    0x14320000,
    0x0e74e225,
    0x13ce0000,
    0x0ed8e225,
    0x136a0000,
    0x0ed8e225,
    0x13060000,
    0x0ed8e225,
    0x12a20000,
    0x0ed8e225,
    0x123e0000,
    0x0ed8e225,
    0x11da0000,
    0x0ed8e225,
    0x155e0000,
    0x0ed8e225,
    0x14fa0000,
    0x0ed8e225,
    0x14960000,
    0x0ed8e225,
    0x14320000,
    0x0ed8e225,
    0x13ce0000,
    0x0f3ce225,
};
#pragma section s_02111994 end

#pragma define_section s_02111a4c ".data.data_ov008_02111a4c" ".data.data_ov008_02111a4c" ".data.data_ov008_02111a4c" far_data
#pragma section s_02111a4c begin
u32 data_ov008_02111a4c[3] = {
    0x136a0000,
    0x0f3ce225,
    0x13060000,
};
#pragma section s_02111a4c end

#pragma define_section s_02111a58 ".data.data_ov008_02111a58" ".data.data_ov008_02111a58" ".data.data_ov008_02111a58" far_data
#pragma section s_02111a58 begin
u32 data_ov008_02111a58[8] = {
    0x0f3ce225,
    0x12a20000,
    0x0f3ce225,
    0x123e0000,
    0x0f3ce225,
    0x11da0000,
    0x0f3ce225,
    0x155e0000,
};
#pragma section s_02111a58 end

#pragma define_section s_02111a78 ".data.data_ov008_02111a78" ".data.data_ov008_02111a78" ".data.data_ov008_02111a78" far_data
#pragma section s_02111a78 begin
u32 data_ov008_02111a78[17] = {
    0x0f3ce225,
    0x14fa0000,
    0x0f3ce225,
    0x14960000,
    0x0f3ce225,
    0x14320000,
    0x0f3ce225,
    0x13ce0000,
    0x0fa0e225,
    0x136a0000,
    0x0fa0e225,
    0x13060000,
    0x0fa0e225,
    0x12a20000,
    0x0fa0e225,
    0x123e0000,
    0x0fa0e225,
};
#pragma section s_02111a78 end

#pragma define_section s_02111abc ".data.data_ov008_02111abc" ".data.data_ov008_02111abc" ".data.data_ov008_02111abc" far_data
#pragma section s_02111abc begin
u32 data_ov008_02111abc[6] = {
    0x11da0000,
    0x0fa0e225,
    0x155e0000,
    0x0fa0e225,
    0x14fa0000,
    0x0fa0e225,
};
#pragma section s_02111abc end

#pragma define_section s_02111ad4 ".data.data_ov008_02111ad4" ".data.data_ov008_02111ad4" ".data.data_ov008_02111ad4" far_data
#pragma section s_02111ad4 begin
u32 data_ov008_02111ad4[8] = {
    0x14960000,
    0x0fa0e225,
    0x14320000,
    0x0fa0e225,
    0x13ce0000,
    0x1004e225,
    0x136a0000,
    0x1004e225,
};
#pragma section s_02111ad4 end

#pragma define_section s_02111af4 ".data.data_ov008_02111af4" ".data.data_ov008_02111af4" ".data.data_ov008_02111af4" far_data
#pragma section s_02111af4 begin
u32 data_ov008_02111af4[8] = {
    0x13060000,
    0x1004e225,
    0x12a20000,
    0x1004e225,
    0x123e0000,
    0x1004e225,
    0x11da0000,
    0x1004e225,
};
#pragma section s_02111af4 end

#pragma define_section s_02111b14 ".data.data_ov008_02111b14" ".data.data_ov008_02111b14" ".data.data_ov008_02111b14" far_data
#pragma section s_02111b14 begin
u32 data_ov008_02111b14[8] = {
    0x155e0000,
    0x1004e225,
    0x14fa0000,
    0x1004e225,
    0x14960000,
    0x1004e225,
    0x14320000,
    0x1004e225,
};
#pragma section s_02111b14 end

#pragma define_section s_02111b34 ".data.data_ov008_02111b34" ".data.data_ov008_02111b34" ".data.data_ov008_02111b34" far_data
#pragma section s_02111b34 begin
u32 data_ov008_02111b34[4] = {
    0x13ce0000,
    0x1068e225,
    0x136a0000,
    0x1068e225,
};
#pragma section s_02111b34 end

#pragma define_section s_02111b44 ".data.data_ov008_02111b44" ".data.data_ov008_02111b44" ".data.data_ov008_02111b44" far_data
#pragma section s_02111b44 begin
u32 data_ov008_02111b44[8] = {
    0x13060000,
    0x1068e225,
    0x12a20000,
    0x1068e225,
    0x123e0000,
    0x1068e225,
    0x11da0000,
    0x1068e225,
};
#pragma section s_02111b44 end

#pragma define_section s_02111b64 ".data.data_ov008_02111b64" ".data.data_ov008_02111b64" ".data.data_ov008_02111b64" far_data
#pragma section s_02111b64 begin
u32 data_ov008_02111b64[1] = {
    0x155e0000,
};
#pragma section s_02111b64 end

#pragma define_section s_02111b68 ".data.data_ov008_02111b68" ".data.data_ov008_02111b68" ".data.data_ov008_02111b68" far_data
#pragma section s_02111b68 begin
u32 data_ov008_02111b68[1] = {
    0x1068e225,
};
#pragma section s_02111b68 end

#pragma define_section s_02111b74 ".data.data_ov008_02111b74" ".data.data_ov008_02111b74" ".data.data_ov008_02111b74" far_data
#pragma section s_02111b74 begin
u32 data_ov008_02111b74[5] = {
    0x14960000,
    0x1068e225,
    0x14320000,
    0x1068e225,
    0x13ce0000,
};
#pragma section s_02111b74 end

#pragma define_section s_02111b88 ".data.data_ov008_02111b88" ".data.data_ov008_02111b88" ".data.data_ov008_02111b88" far_data
#pragma section s_02111b88 begin
u32 data_ov008_02111b88[19] = {
    0x10cce225,
    0x136a0000,
    0x10cce225,
    0x13060000,
    0x10cce225,
    0x12a20000,
    0x10cce225,
    0x123e0000,
    0x10cce225,
    0x11da0000,
    0x10cce225,
    0x155e0000,
    0x10cce225,
    0x14fa0000,
    0x10cce225,
    0x14960000,
    0x10cce225,
    0x14320000,
    0x10cce225,
};
#pragma section s_02111b88 end

#pragma define_section s_02111bd4 ".data.data_ov008_02111bd4" ".data.data_ov008_02111bd4" ".data.data_ov008_02111bd4" far_data
#pragma section s_02111bd4 begin
u32 data_ov008_02111bd4[3] = {
    0x13ce0000,
    0x06d6e225,
    0x05780000,
};
#pragma section s_02111bd4 end

#pragma define_section s_02111be0 ".data.data_ov008_02111be0" ".data.data_ov008_02111be0" ".data.data_ov008_02111be0" far_data
#pragma section s_02111be0 begin
u32 data_ov008_02111be0[16] = {
    0x06d6e225,
    0x05140000,
    0x06d6e225,
    0x04b00000,
    0x06d6e225,
    0x044c0000,
    0x06d6e225,
    0x03e80000,
    0x06d6e225,
    0x03840000,
    0x06d6e225,
    0x03200000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
};
#pragma section s_02111be0 end

