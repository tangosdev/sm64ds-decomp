//cpp
// @symbol _ZN16BowserShockwaves6RenderEv
/* recovered: named members + shared header, real C++ method
 *
 * Two shockwave rings, each with its own model and its own set of three
 * animators, updated in lockstep and then drawn.
 *
 * Every member was already typed in BowserShockwaves.h from the destructor's
 * call list, so the six `Update` calls are member calls now and the two
 * `ModelComponents` arguments -- the placeholder's `c + 0xdc` and `c + 0x17c`
 * -- are each the ring's own `mModelAnimN.data` at +0x08. That is what the
 * six stand-in structs at the top of this file were standing in for.
 *
 * THE TWO DRAW CALLS ARE VIRTUAL HERE, and that is the opposite of ov085.
 * Toad::Render and PrincessPeach::Render call Model::Render through a direct
 * `bl` and need `mModelAnim.Model::Render(0)` to suppress the dispatch; this
 * one genuinely dispatches -- `ldr r,[r]` / `ldr r,[r,#0x14]` / `blx`, slot 5
 * of _ZTV5Model -- so the plain unqualified call is what matches, and
 * qualifying it costs four words.
 *
 * Both placeholders were honest about which: ov085's declared Render
 * non-virtual on a bare `Model`, this one declared a six-slot `Obj` with `m()`
 * at 0x14. The stand-in's SHAPE is evidence, not noise.
 *
 * The second ring is drawn at 0x800 on every axis -- half scale in 20.12 --
 * while the first takes a null scale and renders at its own size.
 */
#include "BowserShockwaves.h"

int BowserShockwaves::Render()
{
    mTextureSequence1.Update(mModelAnim1.data);
    mTextureSequence2.Update(mModelAnim2.data);
    mMaterialChanger1.Update(mModelAnim1.data);
    mMaterialChanger2.Update(mModelAnim2.data);
    mTextureTransformer1.Update(mModelAnim1.data);
    mTextureTransformer2.Update(mModelAnim2.data);
    mModelAnim1.Render(0);
    Vector3 scale;
    scale.x = 0x800;
    scale.y = 0x800;
    scale.z = 0x800;
    mModelAnim2.Render(&scale);
    return 1;
}
