#include "types.h"
/* func_0204a5c8 at 0x0204a5c8, size 0x168
 * Matched byte-for-byte with mwccarm 1.2/sp2p3
 * flags: -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 */
extern void func_0204af3c(void* p);
extern void func_0204af38(void* p);
extern void func_0204b244(void* self, void* node);
extern void func_0204be40(void* self, void* node);
extern void func_0204b81c(void* self, void* node);
extern void func_0204b028(void* self, void* node);
extern void func_0204bbd8(void* self, void* node);
extern void func_0204b460(void* self, void* node);

/* File-local C views of include/Particle__Manager.h and Particle__System.h
 * (both C++): the fields this draw pass touches. Names the headers already
 * have are kept; the rest come from what this function and its callers do
 * with them, and the flag bits follow the SPL particle library's resource
 * flags (draw type at bits 4-5, texture animation bit 11, child resource
 * bit 16; the child's draw type at bits 7-8 of its u16 flags). */

/* SystemDefinition::Data, the resource header. */
typedef struct DefinitionDataView {
    u32 pad0 : 4;
    u32 drawType : 2; /* picks the per-particle draw routine below */
    u32 pad1 : 5;
    u32 hasTexAnim : 1; /* rebind each particle's own texture */
    u32 pad2 : 4;
    u32 hasChildResource : 1; /* also draw the child particles */
    u32 pad3 : 15;
    char pad4[0x33 - 4];
    u8 texture; /* 0x33 -- index into ManagerView.textures */
} DefinitionDataView;

/* A particle: ParticleNode's next link, plus the texture its animation is on. */
typedef struct ParticleNodeView {
    struct ParticleNodeView *next; /* 0x00 */
    char pad[0x42 - 4];
    u8 texture; /* 0x42 -- index into ManagerView.textures */
} ParticleNodeView;

/* The child-particle resource SystemDefinition points at from +0x14. */
typedef struct ChildDefinitionView {
    u16 pad0 : 7;
    u16 drawType : 2; /* picks the child draw routine below */
    u16 pad1 : 7;
    char pad2[0xf - 2];
    u8 texture; /* 0x0f -- index into ManagerView.textures */
} ChildDefinitionView;

typedef struct SystemDefinitionView {
    DefinitionDataView *data; /* 0x00 */
    char pad[0x14 - 4];
    ChildDefinitionView *child; /* 0x14 */
} SystemDefinitionView;

typedef struct SystemView {
    char pad0[8];
    ParticleNodeView *particles; /* 0x08 -- System.particles.head */
    char pad1[0x10 - 0xc];
    ParticleNodeView *particles2; /* 0x10 -- System.particles2.head: the children */
    char pad2[0x18 - 0x14];
    SystemDefinitionView *mDefinition; /* 0x18 */
} SystemView;

/* One 0x14-byte texture record: what func_0204af3c (texture, palette, size)
 * binds. */
typedef struct TextureView {
    u32 pad[5];
} TextureView;

/* Particle::Manager. The header still spells 0x20 unk_020 and leaves 0x34 in
 * pad_030: textures is the 0x14-byte texture records func_0204af3c binds,
 * and currentSystem is the system func_02049ee8 stores before each call
 * here. */
typedef struct ManagerView {
    char pad0[0x20];
    TextureView *textures; /* 0x20 */
    char pad1[0x34 - 0x24];
    SystemView *currentSystem; /* 0x34 */
} ManagerView;

void func_0204a5c8(char* self)
{
    ManagerView *mgr = (ManagerView *)self;
    SystemView *system = mgr->currentSystem;
    SystemDefinitionView *def = system->mDefinition;
    ParticleNodeView *n;
    DefinitionDataView *data = def->data;
    void (*fn1)(void*, void*) = 0;
    void (*fn2)(void*);

    func_0204af3c(&mgr->textures[data->texture]);

    switch (data->drawType) {
    case 0: fn1 = func_0204be40; break;
    case 1: fn1 = func_0204b81c; break;
    case 2: fn1 = func_0204b244; break;
    }

    fn2 = data->hasTexAnim ? func_0204af3c : func_0204af38;

    n = system->particles;
    if (n != 0) {
        do {
            fn2(&mgr->textures[n->texture]);
            fn1(self, n);
            n = n->next;
        } while (n != 0);
    }

    if (!data->hasChildResource)
        return;

    func_0204af3c(&mgr->textures[def->child->texture]);

    switch (def->child->drawType) {
    case 0: fn1 = func_0204bbd8; break;
    case 1: fn1 = func_0204b460; break;
    case 2: fn1 = func_0204b028; break;
    }

    n = system->particles2;
    if (n == 0) return;
    do {
        fn1(self, n);
        n = n->next;
    } while (n != 0);
}
