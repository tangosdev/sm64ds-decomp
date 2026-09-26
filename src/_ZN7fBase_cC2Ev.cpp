//cpp
// @symbol _ZN7fBase_cC2Ev
/* recovered: real C++ constructor -- mwcc emits both C1 and C2 from this
 * definition; this file is enrolled against the base-object C2 variant. */
#include "fBase_c.h"

struct fBaseActorInfo {
    u32 unk_000;
    u16 behaviorPriority;
    u16 renderPriority;
};

extern "C" {
extern u32 data_02099e70;
extern u32 data_020a4b60;
extern u16 data_020a4b54;
extern u8 data_020a4b48;
extern fBase_c::SceneNode *data_020a4b64;
extern fBase_c::SceneNode data_020a4b6c;
extern fBaseActorInfo **data_020a4bb8;

int func_0203b438(fBase_c::SceneNode *root,
                  fBase_c::SceneNode *node,
                  fBase_c::SceneNode *parent);
fBase_c *func_02043810(fBase_c *self);
}

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT C2 NAME, AND IT NEEDS THE ROM'S FIRST STORE.
 * The port's C-named callers -- every ROM factory's constructor chain, and the
 * non-polymorphic ??0fBase_c shadow in hal/link21_rows.cpp -- call
 * _ZN7fBase_cC2Ev, and MSVC's compile of the definition below cannot answer
 * them: its first store writes ??_7fBase_c@@6B@, MSVC's own table (the
 * Itanium D1/D0 pair folded into one slot, so every virtual after the
 * destructor sits a slot early), where arm9 0x02043dec writes the ROM-shaped
 * transient table (ldr r1,[pc,#0x12c]; str r1,[r4]), data_02099edc, which
 * stays the object's live table until the derived class stores its own.
 *
 * So this arm spells the ROM constructor step for step, in the ROM's order:
 * the transient table, SceneNode's constructor on +0x14 and its owner, the
 * behaviour and render list nodes at +0x28 and +0x38, the unique id (post
 * incremented), param, actor id and area byte out of the spawn context, the
 * scene-tree link, both priorities out of the actor's info record, and the
 * parent's pause bits. It is the transcription hal/actor_vtables.cpp carried
 * as the stand-in, moved here statement for statement, including its one
 * host-only line: a refused tree link (func_0203b438 returns 0; the ROM
 * ignores the value) is counted in port_tree_link_refused, which
 * hal/level_change.cpp's teardown trace prints. Nothing here reaches mwccarm:
 * it builds the `#else` arm and emits the ROM bytes it always emitted, and the
 * object is byte-identical either way. */
extern "C" {
void _ZN7fBase_c9SceneNodeC1Ev(void *node);
extern int data_02099edc[];            /* hal/actor_vtables.cpp */
extern int port_tree_link_refused;     /* hal/actor_vtables.cpp */
}
extern "C" void *_ZN7fBase_cC2Ev(char *self)
{
    *(void **)self = data_02099edc;
    _ZN7fBase_c9SceneNodeC1Ev(self + 0x14);
    *(void **)(self + 0x24) = self;             /* sceneNode.owner */
    for (int off = 0x28; off <= 0x38; off += 0x10) {
        *(void **)(self + off) = 0;
        *(void **)(self + off + 4) = 0;
        *(void **)(self + off + 8) = self;
        *(unsigned short *)(self + off + 0xc) = 0;
        *(unsigned short *)(self + off + 0xe) = 0;
    }
    u32 id = data_02099e70;
    *(u32 *)(self + 4) = id;
    data_02099e70 = id + 1;
    *(u32 *)(self + 8) = data_020a4b60;
    *(unsigned short *)(self + 0xc) = data_020a4b54;
    *(unsigned char *)(self + 0x12) = data_020a4b48;
    if (!func_0203b438(&data_020a4b6c, (fBase_c::SceneNode *)(self + 0x14),
                       data_020a4b64))
        ++port_tree_link_refused;
    {
        fBaseActorInfo *info = data_020a4bb8[*(unsigned short *)(self + 0xc)];
        *(unsigned short *)(self + 0x28 + 0xc) = info->behaviorPriority;
        *(unsigned short *)(self + 0x28 + 0xe) = info->behaviorPriority;
        *(unsigned short *)(self + 0x38 + 0xc) = info->renderPriority;
        *(unsigned short *)(self + 0x38 + 0xe) = info->renderPriority;
    }
    {
        char *parent = (char *)func_02043810((fBase_c *)self);
        if (parent) {
            unsigned char pf = *(unsigned char *)(parent + 0x13);
            if (pf & 3)
                *(unsigned char *)(self + 0x13) |= 2;
            if (pf & 0xC)
                *(unsigned char *)(self + 0x13) |= 8;
        }
    }
    return self;
}
#else

fBase_c::fBase_c() : manager(this)
{
    uniqueID = data_02099e70;
    data_02099e70++;
    param1 = data_020a4b60;
    actorID = data_020a4b54;
    unk_012 = data_020a4b48;

    func_0203b438(&data_020a4b6c, &manager.sceneNode, data_020a4b64);

    fBaseActorInfo *info = data_020a4bb8[actorID];
    manager.SetBehaviorPriority(info->behaviorPriority);
    manager.SetRenderPriority(info->renderPriority);

    fBase_c *parent = func_02043810(this);
    if (parent != 0) {
        if ((parent->pauseFlags & 1) || (parent->pauseFlags & 2))
            pauseFlags |= 2;
        if ((parent->pauseFlags & 4) || (parent->pauseFlags & 8))
            pauseFlags |= 8;
    }
}
#endif
