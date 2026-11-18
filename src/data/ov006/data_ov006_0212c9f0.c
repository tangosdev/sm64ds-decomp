//cpp
/*
 * Minigame-overlay read-only table at 0x0212c9f0..0x0212ddd0.
 *
 * Values are the cartridge words; a word the module relocates
 * is written as its target symbol so the link re-creates it.
 */
#include "types.h"
#include "nitro/fx/fx.h"
#include "Heyho.h"

#pragma define_section obj ".rodata.before_coin_tables" ".rodata.before_coin_tables" ".rodata.before_coin_tables" far_data
#pragma section obj begin
struct HeyhoPath sHeyhoFlyInPaths[4][4] = {
    {
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00068000, 0x00078000, 0x00000000 },       /* goal */
            0x00000001,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00000000, 0x00001000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfffa0000, 0x00060000 }, /* range */
            { 0x00068000, 0x00078000, 0x00000000 },       /* goal */
            0x00000002,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00001000, 0x00000000 },       /* startVel */
            { 0xfff98000, 0x00068000, 0x00018000, 0x00078000 }, /* range */
            { 0x00068000, 0x00078000, 0x00000000 },       /* goal */
            0x00000004,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00000000, 0xfffff000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00068000, 0x00078000, 0x00000000 },       /* goal */
            0x00000008,                             /* moveKind */
        },
    },
    {
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0xfff98000, 0x00078000, 0x00000000 },       /* goal */
            0x00000001,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00000000, 0x00001000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfffa0000, 0x00060000 }, /* range */
            { 0xfff98000, 0x00078000, 0x00000000 },       /* goal */
            0x00000002,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00001000, 0x00000000 },       /* startVel */
            { 0xfff98000, 0x00068000, 0x00018000, 0x00078000 }, /* range */
            { 0xfff98000, 0x00078000, 0x00000000 },       /* goal */
            0x00000004,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00000000, 0x00001000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0xfff98000, 0x00078000, 0x00000000 },       /* goal */
            0x00000008,                             /* moveKind */
        },
    },
    {
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00068000, 0x00018000, 0x00000000 },       /* goal */
            0x00000001,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00000000, 0x00001000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfffa0000, 0x00060000 }, /* range */
            { 0x00068000, 0x00018000, 0x00000000 },       /* goal */
            0x00000002,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00001000, 0x00000000 },       /* startVel */
            { 0xfff98000, 0x00068000, 0xfffe8000, 0x00018000 }, /* range */
            { 0x00068000, 0x00018000, 0x00000000 },       /* goal */
            0x00000004,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0xfffff000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00068000, 0x00018000, 0x00000000 },       /* goal */
            0x00000008,                             /* moveKind */
        },
    },
    {
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0xfff98000, 0x00018000, 0x00000000 },       /* goal */
            0x00000001,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00000000, 0x00001000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfffa0000, 0x00060000 }, /* range */
            { 0xfff98000, 0x00018000, 0x00000000 },       /* goal */
            0x00000002,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00001000, 0x00000000 },       /* startVel */
            { 0xfff98000, 0x00068000, 0xfffe8000, 0x00018000 }, /* range */
            { 0xfff98000, 0x00018000, 0x00000000 },       /* goal */
            0x00000004,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0xfff98000, 0x00018000, 0x00000000 },       /* goal */
            0x00000008,                             /* moveKind */
        },
    },
};
#pragma section obj end
