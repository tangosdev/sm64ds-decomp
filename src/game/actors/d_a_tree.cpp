//cpp
/* TREE (actor 286) -- ov002/daTree_c.
 *
 * The course trees. Spawn-param bits 4-6 select one of five variants; the
 * first tree of a variant loads that variant's Model, and every tree
 * appends a heap node {render pos, dCcPos_c, next} to its variant's list.
 * Behavior ticks every list's collision, Render billboards each variant's
 * Model at every node with distance-faded opacity, and CleanupResources
 * frees the lists of the variants that loaded.
 *
 * mwccarm emits ordinary function sections in reverse source order, so the
 * factory is written first. InitResources is the key function, so this TU
 * emits the _ZTV/_ZTI/_ZTS group; the inline destructor in daTree_c.h
 * emits the retail D1/D0 pair and no D2.
 *
 * deslop leftovers:
 * - dCcPos_c::Init 6az: two by-value Fix12<int> params; the header member
 *   form homes them to the stack and size-DIFFs, so the TU-local wrapper
 *   keeps scalar ints.
 * - Clipper::Func_02015560 6az: the same wall on its Fix12<int> scale.
 * - dCcPos_c::C1 stays a mangled extern: a language limit, not a codegen
 *   one. C++ has no syntax for a qualified constructor call on storage that
 *   already exists, and the alternative -- placement new -- needs a leaf
 *   `operator new(size_t, void *)` this tree has no precedent for.
 * - _Znwj stays spelled: also a language limit. `::operator new` mangles
 *   _Znwm here (sizeof is unsigned long), which has no ROM home; the
 *   cartridge calls _Znwj.
 * - *(const Vector3 *)&mPosX: dActor_c carries position as three scalars
 *   with no Pos() accessor, so the Init call puns them TU-locally.
 * - data_ov002_02110a48 / data_ov002_0210abb8 / g_profile_TREE: overlay
 *   bss/data owns them (S14); this TU's delinks span is .text-only.
 * - data_0209f43c / data_0209b3ec: arm9 bss, the Clipper singleton and the
 *   live view matrix it clips against (CopyToViewMat's object); read here,
 *   owned elsewhere.
 */

#include "daTree_c.h"
#include "dCcPos_c.h"
#include "Camera.h"

/* One tree instance's list node: the billboard/clip center in
 * matrix-shifted units (>> 3, y lifted by kCanopyLift), its cylinder
 * collision, and the list link. _Znwj allocates 0x4c = sizeof(this). */
struct TreeNode {
    Vector3 pos;      /* 0x00 */
    dCcPos_c clsn;    /* 0x0c */
    TreeNode *next;   /* 0x48 */
};

#ifndef SM64DS_PLATFORM_PC
typedef char TreeNode_size_must_be_0x4c[sizeof(TreeNode) == 0x4c ? 1 : -1];
#endif

enum {
    kNumVariants = 5,
    kMaxVariant = 4,
    /* The node-y lift, the clip scale, and the render restore are one
     * value: nodes float kCanopyLift above the trunk base, and the matrix
     * translation subtracts it back out. */
    kCanopyLift = 0x1e000,
    kClsnRadius = 0x35555,
    kClsnHeight = 0x1f4000,
    /* dCc_c bit table: char-projectile + char-body + player-interact +
     * tree + handstand. */
    kClsnFlags = 0x380000c,
    kClipNear = 0x11000,
    kClipFar = 0x578000,
    kFadeDist = 0x2f000,
    kFadeBase = 0x10000,
    kOpacityFull = 0x1f,
};

/* Shared ABI seams, kept above the first `// @symbol` marker so no member
 * is charged with their mangled spellings. */
extern "C" {
void *_ZN8dCcPos_cC1Ev(void *clsn);
void _ZN8dCcPos_c4InitERK7Vector35Fix12IiES4_jj(
    dCcPos_c *self, const Vector3 &pos, int radius, int height, u32 flags,
    u32 vuln);
int _ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(
    void *clipper, void *matrix, void *pos, int scale, void *result);
void Matrix4x3_FromRotationY(Matrix4x3 *m, short ang);
/* Spelled Vector3 * for the object actually passed. The definition in
 * src/Vec3_AsrInPlace.c says int *, and the tree also carries s32 * and a
 * file-local `struct Vec3 { int x, y, z; }` in src/func_ov060_02117db8.c --
 * four spellings of the same three-word layout, none of them typedefs of each
 * other. The disagreement is nominal, not a contract difference, and int * is
 * the slop spelling this cleanup exists to retire. */
int *Vec3_AsrInPlace(Vector3 *v, int shift);
/* The global scalar operator new. Spelled, not `::operator new`:
 * sizeof is unsigned long here, so the `new` expression mangles _Znwm,
 * which has no ROM home; the cartridge calls _Znwj. */
void *_Znwj(unsigned int size);
/* The shared opacity method preserves the second scalar argument observed at
 * every retail caller. It is unused by this implementation; its source name
 * and unsigned type are reconstruction choices, not recovered ROM spelling.
 * See notes/experiments/applyopacity-api-0920.md. */

extern TreeNode *data_ov002_02110a48[kNumVariants];
extern u16 data_ov002_0210abb8[];
extern int data_0209f43c[];
extern Matrix4x3 data_0209b3ec;
extern Camera *data_0209f318;
}

namespace Memory {
void operator_delete2(void *p);
}

// @symbol daTree_c_classInit
extern "C" daTree_c *daTree_c_classInit()
{
    return new daTree_c();
}

// @symbol _ZN8daTree_c13InitResourcesEv
/* First tree of a variant loads that variant's Model; every tree appends
 * a node to its variant's list. Returns 1 when this tree was first. */
int daTree_c::InitResources()
{
    int variant = (param1 >> 4) & 7;
    TreeNode **slot;
    TreeNode *node;

    if (variant >= kMaxVariant)
        variant = kMaxVariant;
    slot = &data_ov002_02110a48[variant];
    if (*slot == 0)
        mModel[variant].LoadAndSetFile(data_ov002_0210abb8[variant], 1, 1);
    node = (TreeNode *)_Znwj(sizeof(TreeNode));
    if (node != 0)
        _ZN8dCcPos_cC1Ev(&node->clsn);
    node->pos.x = mPosX;
    node->pos.y = mPosY;
    node->pos.z = mPosZ;
    Vec3_AsrInPlace(&node->pos, 3);
    node->pos.y += kCanopyLift;
    _ZN8dCcPos_c4InitERK7Vector35Fix12IiES4_jj(
        &node->clsn, *(const Vector3 *)&mPosX, kClsnRadius, kClsnHeight,
        kClsnFlags, 0);
    node->next = *slot;
    *slot = node;
    if (node->next != 0) {
        return 0;
    } else {
        return 1;
    }
}

// @symbol _ZN8daTree_c8BehaviorEv
int daTree_c::Behavior()
{
    TreeNode **slot = data_ov002_02110a48;
    int i;

    for (i = 0; i < kNumVariants; i++) {
        TreeNode *node = *slot;
        while (node != 0) {
            node->clsn.Clear();
            node->clsn.Update();
            node = node->next;
        }
        slot++;
    }
    return 1;
}

// @symbol _ZN8daTree_c6RenderEv
int daTree_c::Render()
{
    Camera *cam = data_0209f318;
    TreeNode **slot = data_ov002_02110a48;
    Model *model = mModel;
    int i;

    for (i = 0; i < kNumVariants; i++) {
        /* node before mat: the order selects retail's r8/sb assignment. */
        TreeNode *node;
        Matrix4x3 *mat = &model->mat4x3;

        Matrix4x3_FromRotationY(mat, cam->mAngleY);
        node = *slot;
        while (node != 0) {
            /* Leftover: out is int[3], not Vector3. This is an ownership
             * dependency, not a codegen wall -- Render reproduces
             * byte-for-byte either way. A stack Vector3 odr-uses the type, so
             * mwccarm re-emits its trivial vague-linkage Vector3D1 (4 bytes)
             * beside the licensed text, and production _isolate refuses it.
             * This TU's manifest already carries compiler_only_output rows
             * (the RTTI group); what it lacks is a deadstrip-duplicate row for
             * that one symbol, exactly as ov002/da1up_c already licenses it.
             * Deferred with completion: partial; see issue #2748. */
            int out[3];
            int dist =
                _ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(
                    data_0209f43c, &data_0209b3ec, &node->pos, kCanopyLift,
                    (Vector3 *)out);
            if (dist > kClipNear && dist < kClipFar) {
                int opacity = kOpacityFull;
                if (dist < kFadeDist)
                    opacity = ((dist - kFadeBase) >> 12) & 0xff;
                model->ApplyOpacity(opacity, 0);
                mat->t.x = node->pos.x;
                mat->t.y = node->pos.y - kCanopyLift;
                mat->t.z = node->pos.z;
                model->Render(0);
            }
            node = node->next;
        }
        model++;
        slot++;
    }
    return 1;
}

// @symbol _ZN8daTree_c16OnPendingDestroyEv
/* Empty in the ROM: the override exists only to occupy vtable slot 12. */
void daTree_c::OnPendingDestroy()
{
}

// @symbol _ZN8daTree_c16CleanupResourcesEv
int daTree_c::CleanupResources()
{
    Model *model = mModel;
    TreeNode **slot = data_ov002_02110a48;
    int i;

    for (i = 0; i < kNumVariants; i++) {
        /* transformsBuf is allocated by DoSetFile, so it doubles as the
         * "this variant loaded" flag guarding its list. */
        if (model->transformsBuf != 0) {
            while (*slot != 0) {
                TreeNode *node = *slot;
                *slot = node->next;
                /* Redundant against the while, and retail keeps it: the
                 * node test and the link store both survive in the ROM. */
                if (node != 0) {
                    node->clsn.dCcPos_c::~dCcPos_c();
                    Memory::operator_delete2(node);
                }
            }
        }
        model++;
        slot++;
    }
    return 1;
}
