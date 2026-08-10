//cpp
// @symbol _ZN6PlayerD1Ev
/* A REAL `Player::~Player() {}` DOES NOT REPRODUCE THIS FUNCTION, and the way
 * it fails is worth more than the attempt. It is a FAKEMATCH: byte-comparison
 * reports 39 words, 0 mismatches, and the two functions still call different
 * code.
 *
 * The compiler-generated destructor is right about everything structural. All
 * nine destructible members now have real types (see include/Player.h), so it
 * emits the whole sequence by itself, in reverse declaration order, with an
 * empty body -- the same instruction encodings, the same registers, the same
 * three array calls with the 0x14 stride.
 *
 * But it calls __cxa_vec_cleanup where the ROM calls __destroy_arr. Three times
 * each, and zero the other way; read off the compiled object's own relocation
 * table, not guessed. The ROM was built against a runtime whose array-cleanup
 * helper is __destroy_arr (0x0207328c); this compiler configuration emits the
 * Itanium ABI's __cxa_vec_cleanup, which exists nowhere in the ROM.
 *
 * WHY THE BYTE CHECK MISSED IT: those three calls are `bl` instructions, so
 * they are relocated words, and match.compare wildcards every relocated word.
 * tools/fdiff.py and tools/build_pin.py both said match=True. Only the link
 * would have caught it -- and the file was never enrolled, so the link never
 * ran. eligible.py DID catch it, from the other direction, by refusing a file
 * that references a symbol no symbols.txt defines. That refusal was correct and
 * is not an objisolate defect; isolation already drops dropped sections'
 * relocation sections, and this reference is in D1's OWN .text.
 *
 * SO THE STRUCTOR BLOCKER IS A RUNTIME-HELPER MISMATCH, and it is specific to
 * classes with ARRAY members -- which is why objisolate's 69 destructors, none
 * of which have any, were unaffected. Player has three. Until something
 * reconciles __destroy_arr with __cxa_vec_cleanup, this file stays the
 * hand-spelt free function it has always been.
 */
#include "Player.h"
extern "C" {
extern int _ZN12WithMeshClsnD1Ev(void*);
extern int _ZN25MovingCylinderClsnWithPosD1Ev(void*);
extern int _ZN11ShadowModelD1Ev(void*);
extern int __destroy_arr(void*, int, int, void*);
extern int _ZN9ModelAnimD1Ev(void*);
extern int _ZN5ActorD2Ev(void*);
extern void _ZN15TextureSequenceD1Ev(void);
extern void _ZN15MaterialChangerD1Ev(void);
extern void* data_ov002_0210a83c[];
void* _ZN6PlayerD1Ev(struct Player *self) {
  *(void***)((void*)self) = data_ov002_0210a83c;
  _ZN12WithMeshClsnD1Ev((char*)&self->mMeshClsn);
  _ZN25MovingCylinderClsnWithPosD1Ev((char*)&self->mAttackClsn);
  _ZN25MovingCylinderClsnWithPosD1Ev((char*)&self->mMovingCylinderClsnWithPos);
  _ZN11ShadowModelD1Ev((char*)&self->mShadowModel);
  __destroy_arr((char*)((void*)self)+0x254, 2, 0x14, (void*)_ZN15TextureSequenceD1Ev);
  __destroy_arr((char*)((void*)self)+0x22c, 2, 0x14, (void*)_ZN15MaterialChangerD1Ev);
  __destroy_arr((char*)((void*)self)+0x1dc, 4, 0x14, (void*)_ZN15TextureSequenceD1Ev);
  _ZN9ModelAnimD1Ev((char*)&self->mModelAnim4);
  _ZN9ModelAnimD1Ev((char*)&self->mModelAnim3);
  _ZN5ActorD2Ev(((void*)self));
  return ((void*)self);
}
}
