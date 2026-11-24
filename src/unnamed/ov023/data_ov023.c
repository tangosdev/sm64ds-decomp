#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef union {
    u32 w;
    void *p;
} Word;

extern Word data_ov023_02111b30[];
extern const Word data_ov023_02111760[];
extern const Word data_ov023_02111764[];
extern const Word data_ov023_02111768[];
extern const Word data_ov023_02111784[];

const Word data_ov023_02111760[1] = {{.w = 0x00000000}};

const Word data_ov023_02111764[1] = {{.w = 0x00001000}};

const Word data_ov023_02111768[7] = {
    {.w = 0x0000ffff}, {.p = data_ov023_02111b30}, {.w = 0x00000001}, {.w = 0x00000001},
    {.w = 0x00000001}, {.w = 0x00000001},          {.w = 0x000100c8}};

const Word data_ov023_02111784[201] = {
    {.w = 0x00000000}, {.w = 0x00000000}, {.w = 0xffffffeb}, {.w = 0xffffffd7}, {.w = 0xffffffc2},
    {.w = 0xffffffae}, {.w = 0xffffff99}, {.w = 0xffffff85}, {.w = 0xffffff70}, {.w = 0xffffff5b},
    {.w = 0xffffff47}, {.w = 0xffffff32}, {.w = 0xffffff1e}, {.w = 0xffffff09}, {.w = 0xfffffef4},
    {.w = 0xfffffee0}, {.w = 0xfffffecb}, {.w = 0xfffffeb7}, {.w = 0xfffffea2}, {.w = 0xfffffe8e},
    {.w = 0xfffffe79}, {.w = 0xfffffe64}, {.w = 0xfffffe50}, {.w = 0xfffffe3b}, {.w = 0xfffffe27},
    {.w = 0xfffffe12}, {.w = 0xfffffdfd}, {.w = 0xfffffde9}, {.w = 0xfffffdd4}, {.w = 0xfffffdc0},
    {.w = 0xfffffdab}, {.w = 0xfffffd97}, {.w = 0xfffffd82}, {.w = 0xfffffd6d}, {.w = 0xfffffd59},
    {.w = 0xfffffd44}, {.w = 0xfffffd30}, {.w = 0xfffffd1b}, {.w = 0xfffffd06}, {.w = 0xfffffcf2},
    {.w = 0xfffffcdd}, {.w = 0xfffffcc9}, {.w = 0xfffffcb4}, {.w = 0xfffffca0}, {.w = 0xfffffc8b},
    {.w = 0xfffffc76}, {.w = 0xfffffc62}, {.w = 0xfffffc4d}, {.w = 0xfffffc39}, {.w = 0xfffffc24},
    {.w = 0xfffffc0f}, {.w = 0xfffffbfb}, {.w = 0xfffffbe6}, {.w = 0xfffffbd2}, {.w = 0xfffffbbd},
    {.w = 0xfffffba9}, {.w = 0xfffffb94}, {.w = 0xfffffb7f}, {.w = 0xfffffb6b}, {.w = 0xfffffb56},
    {.w = 0xfffffb42}, {.w = 0xfffffb2d}, {.w = 0xfffffb18}, {.w = 0xfffffb04}, {.w = 0xfffffaef},
    {.w = 0xfffffadb}, {.w = 0xfffffac6}, {.w = 0xfffffab2}, {.w = 0xfffffa9d}, {.w = 0xfffffa88},
    {.w = 0xfffffa74}, {.w = 0xfffffa5f}, {.w = 0xfffffa4b}, {.w = 0xfffffa36}, {.w = 0xfffffa21},
    {.w = 0xfffffa0d}, {.w = 0xfffff9f8}, {.w = 0xfffff9e4}, {.w = 0xfffff9cf}, {.w = 0xfffff9bb},
    {.w = 0xfffff9a6}, {.w = 0xfffff991}, {.w = 0xfffff97d}, {.w = 0xfffff968}, {.w = 0xfffff954},
    {.w = 0xfffff93f}, {.w = 0xfffff92a}, {.w = 0xfffff916}, {.w = 0xfffff901}, {.w = 0xfffff8ed},
    {.w = 0xfffff8d8}, {.w = 0xfffff8c4}, {.w = 0xfffff8af}, {.w = 0xfffff89a}, {.w = 0xfffff886},
    {.w = 0xfffff871}, {.w = 0xfffff85d}, {.w = 0xfffff848}, {.w = 0xfffff833}, {.w = 0xfffff81f},
    {.w = 0xfffff80a}, {.w = 0xfffff7f6}, {.w = 0xfffff7e1}, {.w = 0xfffff7cd}, {.w = 0xfffff7b8},
    {.w = 0xfffff7a3}, {.w = 0xfffff78f}, {.w = 0xfffff77a}, {.w = 0xfffff766}, {.w = 0xfffff751},
    {.w = 0xfffff73c}, {.w = 0xfffff728}, {.w = 0xfffff713}, {.w = 0xfffff6ff}, {.w = 0xfffff6ea},
    {.w = 0xfffff6d6}, {.w = 0xfffff6c1}, {.w = 0xfffff6ac}, {.w = 0xfffff698}, {.w = 0xfffff683},
    {.w = 0xfffff66f}, {.w = 0xfffff65a}, {.w = 0xfffff645}, {.w = 0xfffff631}, {.w = 0xfffff61c},
    {.w = 0xfffff608}, {.w = 0xfffff5f3}, {.w = 0xfffff5df}, {.w = 0xfffff5ca}, {.w = 0xfffff5b5},
    {.w = 0xfffff5a1}, {.w = 0xfffff58c}, {.w = 0xfffff578}, {.w = 0xfffff563}, {.w = 0xfffff54e},
    {.w = 0xfffff53a}, {.w = 0xfffff525}, {.w = 0xfffff511}, {.w = 0xfffff4fc}, {.w = 0xfffff4e8},
    {.w = 0xfffff4d3}, {.w = 0xfffff4be}, {.w = 0xfffff4aa}, {.w = 0xfffff495}, {.w = 0xfffff481},
    {.w = 0xfffff46c}, {.w = 0xfffff457}, {.w = 0xfffff443}, {.w = 0xfffff42e}, {.w = 0xfffff41a},
    {.w = 0xfffff405}, {.w = 0xfffff3f1}, {.w = 0xfffff3dc}, {.w = 0xfffff3c7}, {.w = 0xfffff3b3},
    {.w = 0xfffff39e}, {.w = 0xfffff38a}, {.w = 0xfffff375}, {.w = 0xfffff360}, {.w = 0xfffff34c},
    {.w = 0xfffff337}, {.w = 0xfffff323}, {.w = 0xfffff30e}, {.w = 0xfffff2fa}, {.w = 0xfffff2e5},
    {.w = 0xfffff2d0}, {.w = 0xfffff2bc}, {.w = 0xfffff2a7}, {.w = 0xfffff293}, {.w = 0xfffff27e},
    {.w = 0xfffff269}, {.w = 0xfffff255}, {.w = 0xfffff240}, {.w = 0xfffff22c}, {.w = 0xfffff217},
    {.w = 0xfffff203}, {.w = 0xfffff1ee}, {.w = 0xfffff1d9}, {.w = 0xfffff1c5}, {.w = 0xfffff1b0},
    {.w = 0xfffff19c}, {.w = 0xfffff187}, {.w = 0xfffff172}, {.w = 0xfffff15e}, {.w = 0xfffff149},
    {.w = 0xfffff135}, {.w = 0xfffff120}, {.w = 0xfffff10c}, {.w = 0xfffff0f7}, {.w = 0xfffff0e2},
    {.w = 0xfffff0ce}, {.w = 0xfffff0b9}, {.w = 0xfffff0a5}, {.w = 0xfffff090}, {.w = 0xfffff07b},
    {.w = 0xfffff067}, {.w = 0xfffff052}, {.w = 0xfffff03e}, {.w = 0xfffff029}, {.w = 0xfffff015},
    {.w = 0xfffff000}};
