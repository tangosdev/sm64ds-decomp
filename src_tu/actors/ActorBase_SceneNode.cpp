//cpp
/* SHADOW translation unit -- arm9 / ActorBase::SceneNode, the NESTED class.
 *
 * NOT ENROLLED, AND NOT CANONICAL. The two one-function sources under src/
 * remain the enrolled owners of 0x0203b4ac..0x0203b4dc.
 *
 * WHY THIS IS A SEPARATE TU FROM ActorBase. These are the only two
 * _ZN9ActorBase* functions in the image that are NOT in the class's main run at
 * 0x02043444..0x02043f4c -- they sit 0x8000 bytes lower, and the linker does not
 * place one object's .text in two places. Their neighbourhood says what they are
 * grouped with: 0x0203b2xx-0x0203b4xx holds the intrusive-list primitives the
 * scene graph is built out of (func_0203b20c, func_0203b244, func_0203b27c,
 * func_0203b3c0 -- every one of them called from ActorBase's own members), and
 * this pair sits at the top of that stretch, immediately below
 * _ZN4cstd5atan2E5Fix12IiES1_.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S: SceneNode() is written first
 * because it is the HIGHER address (0x0203b4c4), Reset() second at 0x0203b4ac.
 *
 * BOTH ARE extern "C" FREE FUNCTIONS RATHER THAN REAL ActorBase::SceneNode::
 * METHODS, and that is a deliberate refusal to risk 1,393 TUs for 0x30 bytes.
 * The mangled names say these are members of a class NESTED inside ActorBase,
 * and include/ActorBase.h currently spells that type at top level as
 * `ActorBase_SceneNode`. Making the real methods possible means nesting it, and
 * the honest version of that also declares a constructor on the nested type --
 * which makes ActorBase's implicit default constructor non-trivial, and
 * ActorBase.h is included by 1,393 enrolled C++ TUs, several of whose classes
 * have compiler-generated constructors that would then have to call it. That is
 * exactly the implicit-constructor hazard notes/mwccarm-codegen.md warns about.
 * The nesting is worth doing; it is a header change with its own blast radius
 * and its own sweep, not a rider on this file.
 *
 * The type used below is ActorBase.h's `ActorBase_SceneNode`, which is the more
 * complete of the tree's two spellings: the auto-generated
 * include/ActorBase__SceneNode.h describes only 0x10 bytes of four words, while
 * ActorBase.h records the fifth field -- the owner back-pointer ActorBase's
 * constructor writes at +0x10 with `str r4, [r5, #0x10]`, which is what makes
 * the object 0x14 bytes. Reset() only clears the first four words; it
 * deliberately leaves the owner alone.
 */
#include "ActorBase.h"

/* Defined below, called by the constructor above it. One declaration, at its
   first occurrence. */
extern "C" void _ZN9ActorBase9SceneNode5ResetEv(ActorBase_SceneNode *self);

/* ------------------------------------------------------------------------- */
/* ROM ordinal 1 -- ActorBase::SceneNode::SceneNode(), 0x0203b4c4, size 0x18  */
/* ------------------------------------------------------------------------- */
/* The C1 constructor: clear the four link words and hand back `this`. It does
 * NOT write the owner field -- ActorBase's own constructor does that
 * immediately afterwards, which is how the field was found. */
extern "C" ActorBase_SceneNode *_ZN9ActorBase9SceneNodeC1Ev(ActorBase_SceneNode *self)
{
    _ZN9ActorBase9SceneNode5ResetEv(self);
    return self;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 0 -- ActorBase::SceneNode::Reset(), 0x0203b4ac, size 0x18      */
/* ------------------------------------------------------------------------- */
/* Four stores, no branch. Called by the constructor above and by
 * AfterCleanupResources' unlink path through the list primitives. */
extern "C" void _ZN9ActorBase9SceneNode5ResetEv(ActorBase_SceneNode *self)
{
    self->unk_000 = 0;
    self->unk_004 = 0;
    self->unk_008 = 0;
    self->unk_00c = 0;
}
