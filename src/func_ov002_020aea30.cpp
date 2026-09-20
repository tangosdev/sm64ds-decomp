//cpp
// @symbol func_ov002_020aea30
/* Start the selected enemy death sequence. This reconstructed interface uses
 * receiver, attacker and nullable collision pointers, matching all callers.
 * The handler table forwards the latter two registers; current handlers use
 * the attacker and ignore collision. The exact original prototype is unknown.
 * Incoming r3 is overwritten before use. Prior fourth-argument claims confused
 * death-state stores with argument setup; three-argument compiler probes match.
 * ov004 has a different function at this address and remains separately named.
 */
struct C;
struct dActor_c;
struct dBgCh_Actr;
typedef void (C::*PMF)(dActor_c*, dBgCh_Actr*);
extern PMF data_ov002_0210db80[];
struct C {
  char pad0[0x9c];
  int f9c;
  char pad1[0x10];
  int fb0;
  char pad2[0x4e];
  short f102;
  char pad3[8];
  int f10c;
};
extern "C" void func_ov002_020aea30(C* c, dActor_c* a, dBgCh_Actr* b) {
  if (c->f10c == 0) return;
  (*(unsigned int*)((char*)c + 0xb0)) &= ~0x10000000;
  c->f102 = 0;
  (c->*data_ov002_0210db80[c->f10c - 1])(a, b);
  c->f9c = -0x2000;
  (*(unsigned int*)((char*)c + 0xb0)) &= ~0x10000000;
}
