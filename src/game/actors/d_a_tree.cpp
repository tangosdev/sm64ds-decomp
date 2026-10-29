//cpp
/* TREE (actor 286) -- ov002/daTree_c.
 *
 * The course trees. Spawn-param bits 4-6 select one of five variants; the
 * first tree of a variant loads that variant's dExtSimpleModel_c, and every tree
 * appends a heap node {render pos, dCcPos_c, next} to its variant's list.
 * Behavior ticks every list's collision, Render billboards each variant's
 * dExtSimpleModel_c at every node with distance-faded opacity, and CleanupResources
 * frees the lists of the variants that loaded.
 *
 * mwccarm emits ordinary function sections in reverse source order, so the
 * factory is written first. InitResources is the key function, so this TU
 * emits the _ZTV/_ZTI/_ZTS group; the inline destructor in daTree_c.h
 * emits the retail D1/D0 pair and no D2.
 *
 * NOTES:
 * - InitResources allocates each node with a plain `new TreeNode`: the
 *   null test and dCcPos_c's C1 call the ROM makes are that expression's
 *   own, so the node's collision cylinder needs no explicit construction.
 * - data_ov002_02110a48 / data_ov002_0210abb8 / g_profile_TREE: overlay
 *   bss/data owns them (S14); this TU's delinks span is .text-only.
 * - data_0209f43c / data_0209b3ec: arm9 bss, the dClipper singleton and the
 *   live view matrix it clips against (CopyToViewMat's object); read here,
 *   owned elsewhere.
 */

#include "daTree_c.h"
#include "dCcPos_c.h"
#include "dCamera_c.h"
#include "dClipper.h"
#include "Memory.h"

/* One tree instance's list node: the billboard/clip center in
 * matrix-shifted units (>> 3, y lifted by kCanopyLift), its cylinder
 * collision, and the list link. operator new allocates 0x4c = sizeof(this). */
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
 * is charged with their mangled spellings. local extern: no shared header
 * declares either helper. */
extern "C" {
void Matrix4x3_FromRotationY(Matrix4x3 *m, short ang);
/* Spelled Vector3 * for the object actually passed. The definition in
 * src/Vec3_AsrInPlace.c says int *, and the tree also carries s32 * and a
 * file-local `struct Vec3 { int x, y, z; }` in src/game/actors/d_a_kpa3_bg.cpp --
 * four spellings of the same three-word layout, none of them typedefs of each
 * other. The disagreement is nominal, not a contract difference, and int * is
 * the slop spelling this cleanup exists to retire. */
int *Vec3_AsrInPlace(Vector3 *v, int shift);
}

extern TreeNode *data_ov002_02110a48[kNumVariants];
extern u16 data_ov002_0210abb8[];
extern dClipper data_0209f43c;
extern Matrix4x3 data_0209b3ec;
extern dCamera_c *data_0209f318;

// @symbol daTree_c_classInit
extern "C" daTree_c *daTree_c_classInit()
{
    return new daTree_c();
}

// @symbol _ZN8daTree_c13InitResourcesEv
/* First tree of a variant loads that variant's dExtSimpleModel_c; every tree appends
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
    node = new TreeNode;
    node->pos.x = mPosX;
    node->pos.y = mPosY;
    node->pos.z = mPosZ;
    Vec3_AsrInPlace(&node->pos, 3);
    node->pos.y += kCanopyLift;
    node->clsn.Init(mPos, kClsnRadius, kClsnHeight, kClsnFlags, 0);
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
    dCamera_c *cam = data_0209f318;
    TreeNode **slot = data_ov002_02110a48;
    dExtSimpleModel_c *model = mModel;
    int i;

    for (i = 0; i < kNumVariants; i++) {
        /* node before mat: the order selects retail's r8/sb assignment. */
        TreeNode *node;
        Matrix4x3 *mat = &model->mat4x3;

        Matrix4x3_FromRotationY(mat, cam->mAngleY);
        node = *slot;
        while (node != 0) {
            Vector3 out;
            int dist = data_0209f43c.TransformAndClip(data_0209b3ec, node->pos, kCanopyLift, out);
            if (dist > kClipNear && dist < kClipFar) {
                int opacity = kOpacityFull;
                if (dist < kFadeDist)
                    opacity = ((dist - kFadeBase) >> 12) & 0xff;
                /* The second argument is the one every retail caller passes;
                   see notes/experiments/applyopacity-api-0920.md. */
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
    dExtSimpleModel_c *model = mModel;
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
