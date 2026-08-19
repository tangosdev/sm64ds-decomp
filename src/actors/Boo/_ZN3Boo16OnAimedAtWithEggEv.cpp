//cpp
// @symbol _ZN3Boo16OnAimedAtWithEggEv
/* Boo::OnAimedAtWithEgg (slot 29, include/dActor_c.h) - name recovered from
   the vtable slot it fills. The body is a decompilation verified against the
   ROM, not an inferred stub. Reads a word 8 bytes into
   mMovingCylinderClsnWithPos (include/Boo.h, +0x184), i.e. inside that
   member's own MovingCylinderClsn base -- not a separate Boo field. */
extern "C" int _ZN3Boo16OnAimedAtWithEggEv(void* c) {
    return *(int *)((char *)c + 0x18c) / 2;
}
