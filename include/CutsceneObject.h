/* The cutscene-only actor family, vtable _ZTV14CutsceneObject.
 *
 * The factory allocates 0x104 bytes, constructs dActor_c, and installs this
 * vtable. The destructor performs no class-local teardown: it changes the
 * vptr, runs dActor_c's destruction, and releases the actor allocation. That
 * is exactly the code generated for an empty destructor on this inheritance
 * graph; none of those operations belongs in the source body.
 *
 * The two owned render objects are selected by param1. InitResources writes a
 * Model pointer at 0xdc for the static variants and a ModelAnim pointer at
 * 0xe0 for the animated variants; CleanupResources destroys whichever exists.
 */
#ifndef CUTSCENEOBJECT_H
#define CUTSCENEOBJECT_H

#include "dActor_c.h"

struct Model;
struct ModelAnim;

struct CutsceneObject : dActor_c {
    u8 pad_0d0[0xc];       /* 0x0d0 */
    Model *mModel;          /* 0x0dc */
    ModelAnim *mModelAnim;  /* 0x0e0 */
    u8 pad_0e4[0x1e];      /* 0x0e4 */
    u8 mOpacity;            /* 0x102 */
    u8 unk_103;             /* 0x103 */

    /* Keep the destructor first: it is the class's key function and remains
       the translation-unit owner selected by the ROM's lifecycle symbols. */
    virtual ~CutsceneObject();
    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();
    virtual void OnPendingDestroy();
};

typedef char CutsceneObject_size_must_be_0x104[
    sizeof(CutsceneObject) == 0x104 ? 1 : -1];

#endif
