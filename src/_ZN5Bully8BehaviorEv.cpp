//cpp
// @symbol _ZN5Bully8BehaviorEv
/* recovered: named members + shared header, real C++ method
 *
 * The bully's whole update is delegated: Enemy::UpdateKillByInvincibleChar
 * decides, from the mesh collision and the animation, whether anything
 * happened this frame. 0 means nothing did and the real behaviour worker
 * runs; 1 means handled; 2 is the death case, and only that arm has a body.
 *
 * On death the bully drops a coin (actor 0x120, subtype 2) 0x136000 above
 * itself, facing its own heading turned 0x8000 (180 degrees) plus a random
 * 10-bit spread, then gives the coin an upward launch: 0xa000 of vertical
 * speed and 0x50000 of terminal velocity, with the two acceleration slots
 * zeroed. Finally it looks up the actor whose unique ID sits in unk_3fc and
 * bumps the byte at +0x3fe of it. The field is left unnamed on purpose: what
 * the bytes establish is that it holds an actor ID, not what that actor is.
 *
 * mAreaId is passed to Spawn as the area, which is what pins the s8 read;
 * mPrevAngleY (not mAngleY) is the heading the coin inherits.
 *
 * THE POSITION TEMPORARIES ARE LOAD-BEARING. Collapsing them into
 * `pos.y = mPosY + 0x136000` and friends costs 9 words of 69: the ROM reads
 * y, then z, then x, and materialises the offset into its own register
 * before the add. Measured, not assumed -- the sibling substitutions in the
 * same sweep (the angle temporary, and the `(long long)` round-trip on the
 * tally) were both free and are gone.
 */
#include "Bully.h"

extern "C" {
extern int _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(
    char *self, WithMeshClsn *wm, ModelAnim *ma, unsigned int flags);
extern char *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
    unsigned int actorId, unsigned int param, const Vector3 *pos,
    const Vector3s *rot, int areaId, int f);
extern char *_ZN5Actor10FindWithIDEj(unsigned int id);
extern int RandomIntInternal(void *seed);
extern int data_0209e650;
extern int func_ov064_02116d1c(char *self);
}

int Bully::Behavior()
{
    int ret = _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(
        (char *)this, &mWithMeshClsn, &mModelAnim, 3);
    if (ret != 0) {
        if (ret == 2) {
            int y = mPosY;
            int yoff = 0x136000;
            int z = mPosZ;
            int x = mPosX;
            int sum = y + yoff;
            Vector3 pos;
            pos.x = x;
            pos.y = sum;
            pos.z = z;
            int r = RandomIntInternal(&data_0209e650);
            int yrot = mPrevAngleY + 0x8000 + (int)(((unsigned)r >> 8) & 0x3ff);
            Vector3s rot;
            rot.x = 0;
            rot.z = 0;
            rot.y = (s16)yrot;

            char *coin = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
                0x120, 2, &pos, &rot, mAreaId, -1);
            if (coin) {
                *(int *)(coin + 0x98) = 0xa000;
                *(int *)(coin + 0xa4) = 0;
                *(int *)(coin + 0xa8) = 0x50000;
                *(int *)(coin + 0xac) = 0;
            }
            char *spawner = _ZN5Actor10FindWithIDEj(unk_3fc);
            if (spawner) {
                (*(u8 *)(spawner + 0x3fe))++;
            }
        }
        return 1;
    }
    return func_ov064_02116d1c((char *)this);
}
