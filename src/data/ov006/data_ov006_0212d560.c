//cpp
/* Actor path table, 45 HeyhoPath entries. */
#include "types.h"
#include "nitro/fx/fx.h"
#include "Heyho.h"

#pragma define_section obj ".rodata.before_coin_tables" ".rodata.before_coin_tables" ".rodata.before_coin_tables" far_data
#pragma section obj begin
struct HeyhoPath sHeyhoLayoutFlyInPaths[15][3] = {
    {
        {
            0x00001998,                             /* baseSize */
            { 0x00000000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0xfffc0000, 0xfff35000, 0x00000000 },       /* goal */
            0x00000000,                             /* moveKind */
        },
        {
            0x00001998,                             /* baseSize */
            { 0x00000000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00000000, 0xfff35000, 0x00000000 },       /* goal */
            0x00000000,                             /* moveKind */
        },
        {
            0x00001998,                             /* baseSize */
            { 0x00000000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00040000, 0xfff35000, 0x00000000 },       /* goal */
            0x00000000,                             /* moveKind */
        },
    },
    {
        {
            0x00001998,                             /* baseSize */
            { 0x00000000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0xfffa0000, 0xfff35000, 0x00000000 },       /* goal */
            0x00000000,                             /* moveKind */
        },
        {
            0x00001998,                             /* baseSize */
            { 0x00000000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0xfffc0000, 0xfff35000, 0x00000000 },       /* goal */
            0x00000000,                             /* moveKind */
        },
        {
            0x00001998,                             /* baseSize */
            { 0x00000000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00060000, 0xfff35000, 0x00000000 },       /* goal */
            0x00000000,                             /* moveKind */
        },
    },
    {
        {
            0x00001998,                             /* baseSize */
            { 0x00000000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0xfffc0000, 0xfff35000, 0x00000000 },       /* goal */
            0x00000000,                             /* moveKind */
        },
        {
            0x00001998,                             /* baseSize */
            { 0x00001000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00000000, 0xfff70000, 0x00000000 },       /* goal */
            0x00000001,                             /* moveKind */
        },
        {
            0x00001998,                             /* baseSize */
            { 0x00000000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00040000, 0xfff35000, 0x00000000 },       /* goal */
            0x00000000,                             /* moveKind */
        },
    },
    {
        {
            0x00001998,                             /* baseSize */
            { 0x00001000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0xfffc0000, 0xfff35000, 0x00000000 },       /* goal */
            0x00000001,                             /* moveKind */
        },
        {
            0x00001998,                             /* baseSize */
            { 0x00001000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0xfffe0000, 0xfff65000, 0x00000000 },       /* goal */
            0x00000001,                             /* moveKind */
        },
        {
            0x00001998,                             /* baseSize */
            { 0x00001000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00000000, 0xfff95000, 0x00000000 },       /* goal */
            0x00000001,                             /* moveKind */
        },
    },
    {
        {
            0x00001998,                             /* baseSize */
            { 0x00000000, 0x00001000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfffa0000, 0x00060000 }, /* range */
            { 0xfffa0000, 0xfff90000, 0x00000000 },       /* goal */
            0x00000002,                             /* moveKind */
        },
        {
            0x00001998,                             /* baseSize */
            { 0x00001000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00000000, 0xfff35000, 0x00000000 },       /* goal */
            0x00000001,                             /* moveKind */
        },
        {
            0x00001998,                             /* baseSize */
            { 0x00000000, 0x00001000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfffa0000, 0x00060000 }, /* range */
            { 0x00060000, 0xfff90000, 0x00000000 },       /* goal */
            0x00000002,                             /* moveKind */
        },
    },
    {
        {
            0x00001000,                             /* baseSize */
            { 0x00000000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0xfffc0000, 0xfff35000, 0x00000000 },       /* goal */
            0x00000000,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00000000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00000000, 0xfff35000, 0x00000000 },       /* goal */
            0x00000000,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00000000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00040000, 0xfff35000, 0x00000000 },       /* goal */
            0x00000000,                             /* moveKind */
        },
    },
    {
        {
            0x00001000,                             /* baseSize */
            { 0x00000000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0xfffa0000, 0xfff35000, 0x00000000 },       /* goal */
            0x00000000,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00000000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0xfffc0000, 0xfff35000, 0x00000000 },       /* goal */
            0x00000000,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00000000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00060000, 0xfff35000, 0x00000000 },       /* goal */
            0x00000000,                             /* moveKind */
        },
    },
    {
        {
            0x00001000,                             /* baseSize */
            { 0x00000000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0xfffc0000, 0xfff35000, 0x00000000 },       /* goal */
            0x00000000,                             /* moveKind */
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
            { 0x00000000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00040000, 0xfff35000, 0x00000000 },       /* goal */
            0x00000000,                             /* moveKind */
        },
    },
    {
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0xfffc0000, 0xfff35000, 0x00000000 },       /* goal */
            0x00000001,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0xfffe0000, 0xfff65000, 0x00000000 },       /* goal */
            0x00000001,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00000000, 0xfff95000, 0x00000000 },       /* goal */
            0x00000001,                             /* moveKind */
        },
    },
    {
        {
            0x00001000,                             /* baseSize */
            { 0x00000000, 0x00001000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfffa0000, 0x00060000 }, /* range */
            { 0xfffa0000, 0xfff90000, 0x00000000 },       /* goal */
            0x00000002,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00000000, 0xfff35000, 0x00000000 },       /* goal */
            0x00000001,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00000000, 0x00001000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfffa0000, 0x00060000 }, /* range */
            { 0x00060000, 0xfff90000, 0x00000000 },       /* goal */
            0x00000002,                             /* moveKind */
        },
    },
    {
        {
            0x00001998,                             /* baseSize */
            { 0x00000000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0xfffa0000, 0x00068000, 0x00000000 },       /* goal */
            0x00000000,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00000800, 0x00000400, 0x00000000 },       /* startVel */
            { 0xfffe0000, 0x00020000, 0x00000000, 0x00080000 }, /* range */
            { 0x00000000, 0xfff35000, 0x00000000 },       /* goal */
            0x00000005,                             /* moveKind */
        },
        {
            0x00001998,                             /* baseSize */
            { 0x00000000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00060000, 0x00068000, 0x00000000 },       /* goal */
            0x00000000,                             /* moveKind */
        },
    },
    {
        {
            0x00001998,                             /* baseSize */
            { 0x00000000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0xfffa0000, 0xfff30000, 0x00000000 },       /* goal */
            0x00000000,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00001000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00000000, 0xfffe0000, 0x00000000 },       /* goal */
            0x00000003,                             /* moveKind */
        },
        {
            0x00001998,                             /* baseSize */
            { 0x00000000, 0x00000000, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00060000, 0xfff30000, 0x00000000 },       /* goal */
            0x00000000,                             /* moveKind */
        },
    },
    {
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0xfffff000, 0x00000000 },       /* startVel */
            { 0xfffa0000, 0x00060000, 0xfff40000, 0xfffe8000 }, /* range */
            { 0xfffa0000, 0xfffe8000, 0x00000000 },       /* goal */
            0x00000006,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00000000, 0x00001000, 0x00000000 },       /* startVel */
            { 0x000c0000, 0x000c0000, 0xfffa0000, 0x00060000 }, /* range */
            { 0x00060000, 0x00068000, 0x00000000 },       /* goal */
            0x00000002,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0xfffff000, 0xfffff000, 0x00000000 },       /* startVel */
            { 0xfffa0000, 0x00060000, 0xfff40000, 0xfffe8000 }, /* range */
            { 0x00060000, 0xfffe8000, 0x00000000 },       /* goal */
            0x00000007,                             /* moveKind */
        },
    },
    {
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00000800, 0x00000000 },       /* startVel */
            { 0xfffe0000, 0x00020000, 0x00000000, 0x00100000 }, /* range */
            { 0xfffc0000, 0xfff35000, 0x00000000 },       /* goal */
            0x00000005,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00000800, 0x00000000 },       /* startVel */
            { 0xfffe0000, 0x00020000, 0x00000000, 0x00100000 }, /* range */
            { 0x00000000, 0xfff35000, 0x00000000 },       /* goal */
            0x00000005,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00001000, 0x00000800, 0x00000000 },       /* startVel */
            { 0xfffe0000, 0x00020000, 0x00000000, 0x00100000 }, /* range */
            { 0x00040000, 0xfff35000, 0x00000000 },       /* goal */
            0x00000005,                             /* moveKind */
        },
    },
    {
        {
            0x00001000,                             /* baseSize */
            { 0x00000000, 0x00001400, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0xfff98000, 0x00078000, 0x00000000 },       /* goal */
            0x00000008,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00000000, 0xffffec00, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0xfff98000, 0xfff98000, 0x00000000 },       /* goal */
            0x00000009,                             /* moveKind */
        },
        {
            0x00001000,                             /* baseSize */
            { 0x00000000, 0x00001400, 0x00000000 },       /* startVel */
            { 0xfff40000, 0x000c0000, 0xfff40000, 0x000c0000 }, /* range */
            { 0x00068000, 0xfff98000, 0x00000000 },       /* goal */
            0x00000009,                             /* moveKind */
        },
    },
};
#pragma section obj end
