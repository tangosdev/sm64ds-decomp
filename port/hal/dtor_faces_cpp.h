/* Lane DTOR-FACES-CPP: the slot-16 adapters for the real-C++ D1 TUs.
 *
 * Each adapter is the vtable-law thunk shape (host __fastcall, `this` in
 * ecx, the edx word unused, returns `this` like the ROM body) and its body is
 * one qualified call into the matched src/ destructor, which MSVC emits under
 * its own name (??1<Class>@@UAE@XZ). A fill writes `vt[16] = hal_cppd1_X`
 * and nothing else changes. Definitions, and the shadow-class faces the
 * destructor bodies unwind through, are in hal/dtor_faces_cpp.cpp. */
#ifndef PORT_HAL_DTOR_FACES_CPP_H
#define PORT_HAL_DTOR_FACES_CPP_H

int __fastcall hal_cppd1_Coin(void *self, void *edx);
int __fastcall hal_cppd1_StarMarker(void *self, void *edx);
int __fastcall hal_cppd1_Bird(void *self, void *edx);
int __fastcall hal_cppd1_Butterfly(void *self, void *edx);
int __fastcall hal_cppd1_Fish(void *self, void *edx);
int __fastcall hal_cppd1_PeachPainting(void *self, void *edx);
int __fastcall hal_cppd1_Flag(void *self, void *edx);
int __fastcall hal_cppd1_Cannon(void *self, void *edx);
int __fastcall hal_cppd1_VirtualDoor(void *self, void *edx);
int __fastcall hal_cppd1_PoppingLavaBubbles(void *self, void *edx);
int __fastcall hal_cppd1_AmbientSoundEffects(void *self, void *edx);
int __fastcall hal_cppd1_Door(void *self, void *edx);
int __fastcall hal_cppd1_BowserTail(void *self, void *edx);
int __fastcall hal_cppd1_Bowser(void *self, void *edx);
int __fastcall hal_cppd1_BowserSkyPlatform(void *self, void *edx);
int __fastcall hal_cppd1_Amp(void *self, void *edx);
int __fastcall hal_cppd1_FlameChomp(void *self, void *edx);
int __fastcall hal_cppd1_FlameChompFire(void *self, void *edx);
int __fastcall hal_cppd1_MrI(void *self, void *edx);
int __fastcall hal_cppd1_MrI_Projectile(void *self, void *edx);
int __fastcall hal_cppd1_Scuttlebug(void *self, void *edx);
int __fastcall hal_cppd1_BabyPenguin(void *self, void *edx);
int __fastcall hal_cppd1_SnowmanBody(void *self, void *edx);
int __fastcall hal_cppd1_SnowmanHead(void *self, void *edx);

#endif
