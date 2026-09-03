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
int __fastcall hal_cppd1_PyramidTag(void *self, void *edx);
int __fastcall hal_cppd1_CrazedCrate(void *self, void *edx);
int __fastcall hal_cppd1_BookShotSpawner(void *self, void *edx);
int __fastcall hal_cppd1_WingFeather(void *self, void *edx);
int __fastcall hal_cppd1_InvisibleSecret(void *self, void *edx);
int __fastcall hal_cppd1_EnemySpawner(void *self, void *edx);
int __fastcall hal_cppd1_EnemySwitchTag(void *self, void *edx);
int __fastcall hal_cppd1_HauntedChair(void *self, void *edx);
int __fastcall hal_cppd1_PrincessPeach(void *self, void *edx);
int __fastcall hal_cppd1_Toad(void *self, void *edx);
int __fastcall hal_cppd1_RacingPenguin(void *self, void *edx);
int __fastcall hal_cppd1_IceSlideManager(void *self, void *edx);
int __fastcall hal_cppd1_RollingLogLll(void *self, void *edx);
int __fastcall hal_cppd1_RollingLogTtm(void *self, void *edx);
int __fastcall hal_cppd1_TreasureChest(void *self, void *edx);
int __fastcall hal_cppd1_Clam(void *self, void *edx);
int __fastcall hal_cppd1_CutsceneObject(void *self, void *edx);
int __fastcall hal_cppd1_MegaMushroomCreateTag(void *self, void *edx);
int __fastcall hal_cppd1_ClockPaintingHandShort(void *self, void *edx);
int __fastcall hal_cppd1_SlideDecorationSilverStar(void *self, void *edx);
int __fastcall hal_cppd1_RotatingPlatformRr(void *self, void *edx);
int __fastcall hal_cppd1_OrangeBallBillboard(void *self, void *edx);
int __fastcall hal_cppd1_Pokey(void *self, void *edx);
int __fastcall hal_cppd1_Tornado(void *self, void *edx);
int __fastcall hal_cppd1_Spiny(void *self, void *edx);
int __fastcall hal_cppd1_Lakitu(void *self, void *edx);
int __fastcall hal_cppd1_Moneybag(void *self, void *edx);
int __fastcall hal_cppd1_MugenBgm(void *self, void *edx);
int __fastcall hal_cppd1_PushBlock(void *self, void *edx);
int __fastcall hal_cppd1_PoleLift(void *self, void *edx);
int __fastcall hal_cppd1_BlueFlame(void *self, void *edx);
int __fastcall hal_cppd1_KoopaFlag(void *self, void *edx);
int __fastcall hal_cppd1_BobOmbBuddy(void *self, void *edx);
int __fastcall hal_cppd1_MontyMole(void *self, void *edx);
int __fastcall hal_cppd1_OneUpLogo(void *self, void *edx);
int __fastcall hal_cppd1_SkiLift(void *self, void *edx);
int __fastcall hal_cppd1_Seaweed(void *self, void *edx);
int __fastcall hal_cppd1_BrickBlock(void *self, void *edx);
int __fastcall hal_cppd1_ArrowLift(void *self, void *edx);
int __fastcall hal_cppd1_InvisiblePole(void *self, void *edx);

#endif
