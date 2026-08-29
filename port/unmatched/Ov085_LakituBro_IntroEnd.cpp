/* HOST COPY of src/func_ov085_0212d5dc.cpp -- LakituBro's LAST OPENING STATE,
 * the one that ends the new-file cutscene.
 *
 * THE r0-PASSTHROUGH SEAM, the same one as Ov085_LakituBro_ClosestPlayer.cpp
 * (func_ov085_0212d9b8/dd10/e310/e4a4), Ov085_Rabbit_b8dc.cpp and
 * Actor_ClosestPlayerWrappers.cpp. This body is a fifth instance and it is in a
 * file of its own only because those four are a set another lane owns.
 *
 * THE MISMATCH. The matched source ends with:
 *
 *     if (_ZN5Actor13ClosestPlayerEv(c) != 0) {
 *         func_ov002_020c3e8c();          <-- declared (void) in that TU
 *         data_0209caa0[2] |= 0x80;
 *     }
 *
 * but func_ov002_020c3e8c is DEFINED as `void func_ov002_020c3e8c(void* player)`
 * and does Player::ChangeState(player, data_ov002_0211013c) -- the state that
 * hands control back to the player. On ARM the player pointer is still live in
 * r0 from ClosestPlayer's return, so the call works. On x86 __cdecl the callee
 * reads a stack slot nobody wrote and changes the state of a garbage pointer.
 *
 * THE TREE ALREADY KNEW: the pair is frozen in
 * port/tools/aritycheck_plainfunc_baseline.txt as
 *     func_ov002_020c3e8c|src/func_ov085_0212d5dc.cpp
 * It stayed latent because with the intro bit force-set in hal/level_boot.cpp
 * this state was unreachable -- LakituBro::InitResources only selects the
 * opening state chain when that bit is CLEAR. Opening the gate reaches it.
 *
 * WHY IT MATTERS MORE THAN A CRASH: these are the three lines that END the
 * opening. func_ov002_020c3e8c gives the player back control, and the line after
 * it is the ROM's own `data_0209caa0[2] |= 0x80` -- the write that records "the
 * intro has been seen". That single write is the state a completed opening
 * leaves behind, and it is what the port must never fabricate.
 *
 * Nothing else changes. The body below is the matched source's control flow line
 * for line; only the ClosestPlayer result is kept in a local and handed to
 * func_ov002_020c3e8c as its real argument.
 *
 * PORT_HOST_ABI: implicit-register-arg (func_ov002_020c3e8c's player rode r0
 * from ClosestPlayer's return; the host passes it). See the header.
 */
#include <cstdio>
#include <cstdlib>

#include "common.h"

struct Range { int a, b, c, d, e, f; };

extern "C" {
void _ZN6Camera9SetFlag_3Ev(void* self);
void _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(void* dst, const void* src, int step);
void _ZN6Camera9SetLookAtERK7Vector3(void* self, const void* v);
void _ZN6Camera6SetPosERK7Vector3(void* self, const void* v);
int Vec3_Dist(const void* a, const void* b);
int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b,
                                    unsigned int c, int d, bool e);
void* _ZN5Actor13ClosestPlayerEv(void* self);
void func_ov002_020c3e8c(void* player);   /* real one-arg (the player) shape */
void func_ov085_0212e728(void* c, void* p);
}
extern void* data_0209f318;
extern int data_0209caa0[];
extern int data_ov085_021307e0;

#define AT(p, off) ((void*)(int)(((long long)(int)((char*)(p) + (off)))))

/* ENTRY AND EXIT CONDITIONS, because this function is where the opening's last
   unmet assertion lives. flags2 bit 7 is written on exactly one line below, and
   two things gate it: a frame counter at +0x2c8 that has to pass 0x64, and
   Sound::PlaySub returning non-zero. A run where the bit never moves cannot
   distinguish "the state is never entered" from "the counter never gets there"
   from "PlaySub always answers 0", and those are three different problems.
   First entry, then every 60th frame, then loudly on the branch.
   Inert unless SM64DS_INTRO_WATCH. */
static int lb_watch_on(void)
{
    static int on = -1;
    if (on < 0)
        on = std::getenv("SM64DS_INTRO_WATCH") ? 1 : 0;
    return on;
}

extern "C" int func_ov085_0212d5dc(char* c) {
  Range r;
  void* cam = *(void**)&data_0209f318;
  if (lb_watch_on()) {
      static int seen;
      if (!seen) {
          seen = 1;
          std::fprintf(stderr, "  [lakitu] intro-end state ENTERED, counter %d\n",
                       *(int*)(c + 0x2c8));
      }
  }
  _ZN6Camera9SetFlag_3Ev(cam);
  r.a = -0x4b0000;
  r.b = 0x19f000;
  r.c = 0x1a90000;
  r.d = -0x4b0000;
  r.e = 0x250000;
  r.f = 0x1d4c000;
  _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(c+0x2b0, &r, 0x70000);
  _ZN6Camera9SetLookAtERK7Vector3(cam, c+0x2b0);
  _ZN6Camera6SetPosERK7Vector3(cam, c+0x2bc);
  Vec3_Dist(c+0x2b0, &r);
  (*(int*)AT(c, 0x2c8))++;
  if (lb_watch_on() && (*(int*)(c+0x2c8) % 60) == 0)
      std::fprintf(stderr, "  [lakitu] intro-end counter %d (needs > 0x64)\n",
                   *(int*)(c + 0x2c8));
  if (*(int*)(c+0x2c8) > 0x64) {
    const int played = _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x4b, 0x7f, 0, 0x7222,
                                                       false);
    if (lb_watch_on()) {
        static int last = -2;
        if (played != last) {
            std::fprintf(stderr, "  [lakitu] counter past 0x64; Sound::PlaySub "
                         "returned %d (non-zero is what ends the opening)\n",
                         played);
            last = played;
        }
    }
    if (played != 0) {
      *(int*)AT(cam, 0x154) &= ~8;
      *(int*)(c+0x98) = 0;
      *(int*)(c+0x2c8) = 0;
      *(int*)(c+0x2cc) = 0;
      *(int*)(c+0xa4) = 0;
      *(int*)(c+0xa8) = 0;
      *(int*)(c+0xac) = 0;
      *(short*)(c+0x8c) = 0;
      {
        void* pl = _ZN5Actor13ClosestPlayerEv(c);   /* <-- the ROM's r0 */
        if (lb_watch_on())
            std::fprintf(stderr, "  [lakitu] handing control back: "
                         "ClosestPlayer %p\n", pl);
        if (pl != 0) {
          func_ov002_020c3e8c(pl);
          data_0209caa0[2] |= 0x80;   /* the ROM's own "intro seen" write */
        }
      }
      func_ov085_0212e728(c, &data_ov085_021307e0);
    }
  }
  return 1;
}
