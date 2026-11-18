//cpp
/* Actor path table, 45 HeyhoPath entries. */
#include "types.h"
#include "nitro/fx/fx.h"
#include "Heyho.h"

#pragma define_section obj ".rodata.before_coin_tables" ".rodata.before_coin_tables" ".rodata.before_coin_tables" far_data
#pragma section obj begin
struct HeyhoPath sHeyhoSettlePaths[9][5] = {
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
        {
            0x00001000,                             /* baseSize */
            { 0xfffff000, 0xfffff000, 0x00000000 },       /* startVel */
            { 0xfff98000, 0x00068000, 0x00030000, 0x00078000 }, /* range */
            { 0x00068000, 0x00078000, 0x00000000 },       /* goal */
            0x00000007,                             /* moveKind */
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
        {
            0x00001000,                             /* baseSize */
            { 0xfffff000, 0xfffff000, 0x00000000 },       /* startVel */
            { 0xfff98000, 0x00068000, 0x00030000, 0x00078000 }, /* range */
            { 0xfff98000, 0x00078000, 0x00000000 },       /* goal */
            0x00000006,                             /* moveKind */
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
        {
            0x00001000,                             /* baseSize */
            { 0xfffff000, 0xfffff000, 0x00000000 },       /* startVel */
            { 0xfff98000, 0x00068000, 0x00018000, 0x00090000 }, /* range */
            { 0x00068000, 0x00018000, 0x00000000 },       /* goal */
            0x00000006,                             /* moveKind */
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
        {
            0x00001000,                             /* baseSize */
            { 0xfffff000, 0xfffff000, 0x00000000 },       /* startVel */
            { 0xfff98000, 0x00068000, 0x00018000, 0x00090000 }, /* range */
            { 0xfff98000, 0x00018000, 0x00000000 },       /* goal */
            0x00000007,                             /* moveKind */
        },
    },
    {
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00068000, 0xfffe8000, 0x00000000 },       /* goal */
            0x00000001,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00000000, 0x00001000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfffa0000, 0x00060000 }, /* range */
            { 0x00068000, 0xfffe8000, 0x00000000 },       /* goal */
            0x00000002,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00001000, 0x00000000 },       /* startVel */
            { 0xfff98000, 0x00068000, 0xfffb8000, 0xfffe8000 }, /* range */
            { 0x00068000, 0xfffe8000, 0x00000000 },       /* goal */
            0x00000004,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00068000, 0xfffe8000, 0x00000000 },       /* goal */
            0x00000009,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0xfffff000, 0xfffff000, 0x00000000 },       /* startVel */
            { 0xfff98000, 0x00068000, 0xfff40000, 0xfffe8000 }, /* range */
            { 0x00068000, 0xfffe8000, 0x00000000 },       /* goal */
            0x00000007,                             /* moveKind */
        },
    },
    {
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0xfff98000, 0xfffe8000, 0x00000000 },       /* goal */
            0x00000001,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00000000, 0x00001000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfffa0000, 0x00060000 }, /* range */
            { 0xfff98000, 0xfffe8000, 0x00000000 },       /* goal */
            0x00000002,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00001000, 0x00000000 },       /* startVel */
            { 0xfff98000, 0x00068000, 0xfffb8000, 0xfffe8000 }, /* range */
            { 0xfff98000, 0xfffe8000, 0x00000000 },       /* goal */
            0x00000004,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0xfffff000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0xfff98000, 0xfffe8000, 0x00000000 },       /* goal */
            0x00000009,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0xfffff000, 0xfffff000, 0x00000000 },       /* startVel */
            { 0xfff98000, 0x00068000, 0xfff40000, 0xfffe8000 }, /* range */
            { 0xfff98000, 0xfffe8000, 0x00000000 },       /* goal */
            0x00000006,                             /* moveKind */
        },
    },
    {
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00068000, 0xfff40000, 0x00000000 },       /* goal */
            0x00000001,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00000000, 0x00001000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xffff8000, 0x00060000 }, /* range */
            { 0x00068000, 0xfff40000, 0x00000000 },       /* goal */
            0x00000002,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00068000, 0xfff40000, 0x00000000 },       /* goal */
            0x00000001,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00068000, 0xfff40000, 0x00000000 },       /* goal */
            0x00000009,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0xfffff000, 0xfffff000, 0x00000000 },       /* startVel */
            { 0xfff98000, 0x00068000, 0xfff40000, 0xfffe8000 }, /* range */
            { 0x00068000, 0xfff40000, 0x00000000 },       /* goal */
            0x00000006,                             /* moveKind */
        },
    },
    {
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0xfff98000, 0xfff40000, 0x00000000 },       /* goal */
            0x00000001,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00001000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xffff8000, 0x00060000 }, /* range */
            { 0xfff98000, 0xfff40000, 0x00000000 },       /* goal */
            0x00000002,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0xfff98000, 0xfff40000, 0x00000000 },       /* goal */
            0x00000001,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0xfffff000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0xfff98000, 0xfff40000, 0x00000000 },       /* goal */
            0x00000009,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0xfffff000, 0xfffff000, 0x00000000 },       /* startVel */
            { 0xfff98000, 0x00068000, 0xfff40000, 0xfffe8000 }, /* range */
            { 0xfff98000, 0xfff40000, 0x00000000 },       /* goal */
            0x00000007,                             /* moveKind */
        },
    },
    {
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00000000, 0xfff40000, 0x00000000 },       /* goal */
            0x00000001,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00000000, 0xfff70000, 0x00000000 },       /* goal */
            0x00000001,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00000000, 0xfffa0000, 0x00000000 },       /* goal */
            0x00000001,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00002000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00000000, 0xfffa0000, 0x00000000 },       /* goal */
            0x0000000a,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00002000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00000000, 0xfffd0000, 0x00000000 },       /* goal */
            0x0000000a,                             /* moveKind */
        },
    },
};
#pragma section obj end
