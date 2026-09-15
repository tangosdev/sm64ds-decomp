//cpp
/* Camera-tag position marker -- ov002/daCamTag_c.
 *
 * ROM RTTI names daCamTag_c; overlay_actors.md CAMERA_TAG(333). Ugly RTTI
 * name is final. mwccarm emits ordinary function sections in reverse source
 * order; keep the factory first. The inline destructor in daCamTag_c.h emits
 * retail D1 then D0 and no D2.
 *
 * All five overrides are stubs -- InitResources, Behavior, Render and
 * CleanupResources return 1, OnPendingDestroy is empty, and none reads
 * `this`. The class exists to BE somewhere: a dActor_c and nothing more,
 * marking a camera-trigger position other code queries. (N64 kept camera
 * triggers as static per-level structs; here they are instanced actors --
 * notes/archive/n64-decomp-cross-reference.md.)
 *
 * daCamTag_c_classInit is reconstructed (RTTI daCamTag_c, CAMERA_TAG
 * registry). Retail does not store that spelling. Historical alias
 * daCamTag_c_Spawn.
 *
 * deslop
 * Leftover: g_profile_CAMERA_TAG (ov002 0x02108518) stays outside this TU
 *   (S14) -- defining it here would emit .data this entry is not licensed
 *   to own; the ROM supplies it.
 * Leftover: pad_0d0 keeps the sibling spelling (daChRoom_c, daChScene_c,
 *   daBar_c all pad_0d0); no enrolled body touches those bytes.
 */

#include "daCamTag_c.h"

// @symbol daCamTag_c_classInit
extern "C" daCamTag_c *daCamTag_c_classInit()
{
    return new daCamTag_c();
}

// @symbol _ZN10daCamTag_c13InitResourcesEv
int daCamTag_c::InitResources()
{
    return 1;
}

// @symbol _ZN10daCamTag_c8BehaviorEv
int daCamTag_c::Behavior()
{
    return 1;
}

// @symbol _ZN10daCamTag_c6RenderEv
int daCamTag_c::Render()
{
    return 1;
}

// @symbol _ZN10daCamTag_c16OnPendingDestroyEv
void daCamTag_c::OnPendingDestroy()
{
}

// @symbol _ZN10daCamTag_c16CleanupResourcesEv
int daCamTag_c::CleanupResources()
{
    return 1;
}
