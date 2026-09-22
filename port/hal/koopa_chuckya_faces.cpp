// GATE 182's one real-name method face, in its own TU on purpose: the face
// spells class WithMeshClsn with just the one method so the mangle comes out
// ?IsOnGround@dBgCh_Actr@@QBE_NXZ (const, bool) -- the spelling
// func_ov062_02118004.cpp's own shadow uses. It CANNOT live in a TU that
// includes include/WithMeshClsn.h (the real class definition, no such method)
// or bob_enemy_bridges.cpp (which includes it): one class, two definitions.
// The body forwards to the linked C-named matched TU.
extern "C" int _ZNK10dBgCh_Actr10IsOnGroundEv(const void *self);

struct WithMeshClsn { bool IsOnGround() const; };
bool WithMeshClsn::IsOnGround() const
{ return _ZNK10dBgCh_Actr10IsOnGroundEv((const void *)this) != 0; }
