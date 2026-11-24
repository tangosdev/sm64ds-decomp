#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef union {
    u32 w;
    void *p;
} Word;

extern const Word data_ov001_020ab5f8[];
extern const Word data_ov001_020ab5fc[];
extern const Word data_ov001_020ab600[];
extern const Word data_ov001_020ab604[];
extern const Word data_ov001_020ab608[];
extern const Word data_ov001_020ab60c[];
extern const Word data_ov001_020ab610[];
extern const Word data_ov001_020ab618[];
extern const Word data_ov001_020ab620[];
extern const Word data_ov001_020ab630[];
extern const Word data_ov001_020ab640[];
extern const Word data_ov001_020ab6b0[];

const Word data_ov001_020ab600[1] = {{.w = 0x00000000}};

const Word data_ov001_020ab604[1] = {{.w = 0x00000000}};

const Word data_ov001_020ab608[1] = {{.w = 0x00000000}};

const Word data_ov001_020ab5f8[1] = {{.w = 0x00000000}};

const Word data_ov001_020ab5fc[1] = {{.w = 0x00000000}};

const Word data_ov001_020ab60c[1] = {{.w = 0x00000000}};

const Word data_ov001_020ab610[2] = {{.w = 0x00000001}, {.p = data_ov001_020ab630}};

const Word data_ov001_020ab618[2] = {{.w = 0x00000001}, {.p = data_ov001_020ab620}};

const Word data_ov001_020ab630[4] = {
    {.w = 0x00000001}, {.p = data_ov001_020ab600}, {.w = 0x00000134}, {.p = data_ov001_020ab6b0}};

const Word data_ov001_020ab620[4] = {
    {.w = 0x00000001}, {.p = data_ov001_020ab604}, {.w = 0x00000070}, {.p = data_ov001_020ab640}};

const Word data_ov001_020ab640[28] = {
    {.w = 0x24201440}, {.w = 0x00000003}, {.w = 0x00000000}, {.w = 0x00000000}, {.w = 0x3fcfe3fc},
    {.w = 0x26262726}, {.w = 0x0100ff00}, {.w = 0xff000000}, {.w = 0x0100ff00}, {.w = 0xff000100},
    {.w = 0x26262726}, {.w = 0x01000100}, {.w = 0xff00fe00}, {.w = 0x01000100}, {.w = 0xff00ff00},
    {.w = 0x27404126}, {.w = 0x0100ff00}, {.w = 0x00000001}, {.w = 0xff000000}, {.w = 0x26252525},
    {.w = 0x00000100}, {.w = 0xfe000100}, {.w = 0xfe00ff00}, {.w = 0x0100ff00}, {.w = 0x41252525},
    {.w = 0xfe000100}, {.w = 0x00000100}, {.w = 0x0000ff00}};

const Word data_ov001_020ab6b0[77] = {
    {.w = 0x23201440}, {.w = 0x00000003}, {.w = 0x00000000}, {.w = 0x00000000}, {.w = 0xfe0000b5},
    {.w = 0x0000ff4b}, {.w = 0x28252425}, {.w = 0x000000b5}, {.w = 0x000fe004}, {.w = 0x00000100},
    {.w = 0x0b5803b5}, {.w = 0x28252425}, {.w = 0x000000b5}, {.w = 0x004fe000}, {.w = 0x00000000},
    {.w = 0x3b58034b}, {.w = 0x28252425}, {.w = 0x0000ff4b}, {.w = 0x000fe3fc}, {.w = 0x0000ff00},
    {.w = 0x34b8004b}, {.w = 0x28252425}, {.w = 0x0000ff4b}, {.w = 0x3fcfe000}, {.w = 0x00000000},
    {.w = 0x04b800b5}, {.w = 0x24404125}, {.w = 0x000000b5}, {.w = 0x00000002}, {.w = 0x3fcfe000},
    {.w = 0x26252626}, {.w = 0xff4b00b5}, {.w = 0x00000000}, {.w = 0xfe000100}, {.w = 0x00b500b5},
    {.w = 0x26274041}, {.w = 0x00000002}, {.w = 0xff4b0000}, {.w = 0xff000000}, {.w = 0x41262626},
    {.w = 0x00000000}, {.w = 0xff4bff4b}, {.w = 0x0000ff00}, {.w = 0x26262540}, {.w = 0x00000002},
    {.w = 0x0000ff00}, {.w = 0x00b5ff4b}, {.w = 0x00000000}, {.w = 0x40412626}, {.w = 0x01000000},
    {.w = 0x00b500b5}, {.w = 0x00000002}, {.w = 0x26262624}, {.w = 0x3fcfe000}, {.w = 0x00000000},
    {.w = 0xff4bff4b}, {.w = 0x0000ff00}, {.w = 0x25254041}, {.w = 0x00000002}, {.w = 0xfe00ff00},
    {.w = 0xfe000000}, {.w = 0x40412626}, {.w = 0x00b5ff4b}, {.w = 0x01000000}, {.w = 0x00000002},
    {.w = 0x26252623}, {.w = 0x000000b5}, {.w = 0x000000b5}, {.w = 0x00000100}, {.w = 0x00000000},
    {.w = 0xff4b00b5}, {.w = 0x26274041}, {.w = 0x00000000}, {.w = 0x00b5fe00}, {.w = 0x01000000},
    {.w = 0x00004126}, {.w = 0x00000000}};
