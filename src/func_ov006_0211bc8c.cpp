//cpp
// @symbol func_ov006_0211bc8c
/* recovered: dScMgSound_c per-note animation tick, ov006 0x0211bc8c (696
 * bytes). One note slot per call. The slot's frame counter at +0x50f0 is
 * advanced and compared against the per-pattern length in
 * data_ov006_0212ef3c; below it the call returns. At the wrap the counter is
 * cleared and the slot's pattern index at +0x50f7 steps on, and while that
 * index is still 0..6 the call returns as well. Past 6 the index is pinned at
 * 6, the slot's state byte at +0x50f4 goes to 2, the note id at +0x50f8 (+1)
 * is appended to the playback queue at +0x5610 with the slot number packed
 * into the high byte, the queue length at +0x5625 grows, the queue timer at
 * +0x5614 is armed, and the note is handed to func_ov006_0211b654 before
 * being played. Which player it goes to depends on dScMgBase_c::Virtual8C
 * (the scene's own spawn-parameter predicate) and on the instrument select at
 * +0x5627 -- the two-player answer folds instruments 0/2 together and reads
 * data_ov006_0213f794 to pick between func_02012790 and the +0x560c bank
 * handle, instrument 1 quantises the note to the start of its group of four
 * (the repeated-subtraction divide of id - 0x1a by 4), and everything else
 * goes to Sound::PlayBank2_2D; the one-player answer is the same set of
 * destinations wired differently. Finally, when the mode byte at +0x5624 is
 * 1, the whole helper object at +0x4f38 is retriggered.
 *
 * Every offset here is inside mTable, the single 0x6f4-byte member
 * include/dScMgSound_c.h describes, so they stay raw casts off the scene, the
 * same way src/func_ov006_0211ba88.cpp and src/func_ov006_0211bbe0.c reach them.
 *
 * TWO SPELLINGS ARE LOAD-BEARING, both measured. First,
 * func_ov006_0211b654's SECOND argument: the callee is (scene, slot) --
 * src/func_ov006_0211b654.c's own recovered signature -- and passing `idx`
 * costs zero bytes here because it is already in r1, but it keeps idx's web
 * alive across the queue-append block. Dropping it (a one-argument call)
 * leaves the whole function schedule-identical and rotates ten registers in
 * that block, because the `orr` then coalesces into idx's dead register
 * instead of the loaded halfword's (div 10, and no spelling, pragma or
 * declaration order moves it -- see notes/mwccarm-codegen.md 6ab). Second,
 * the queue slot at +0x5610 and the length at +0x5625 are written through the
 * plain member forms and mwccarm picks the two address shapes itself: the
 * plain store folds to `this + n*2` with an `add #0x5600`/`[#0x10]` pair
 * while the read-modify-write takes the pool-loaded base with the scaled
 * index. Forcing either shape breaks the other. */
#include "types.h"
#include "dScMgSound_c.h"

extern "C" {
void func_ov006_0211b654(dScMgSound_c *self, int idx);
void func_ov006_020c2300(char *p);
void func_02012174(u32 bank, u32 id);
void func_02012790(u32 id);
extern u16 data_ov006_0212ef3c[];
extern u16 data_ov006_0213f794[];
extern u16 data_ov006_0213f7e8[];
}

namespace Sound { void PlayBank2_2D(unsigned int id); }

extern "C" void func_ov006_0211bc8c(dScMgSound_c *self, int idx)
{
    char *c = (char *)self;
    int o = idx * 0x14;
    u8 id;
    u8 v;

    *(u16 *)(c + 0x50f0 + o) = *(u16 *)(c + 0x50f0 + o) + 1;
    if (*(u16 *)(c + 0x50f0 + o) < data_ov006_0212ef3c[((u8 *)(c + 0x50f7))[o]]) return;
    *(u16 *)(c + 0x50f0 + o) = 0;
    ((u8 *)(c + 0x50f7))[o] = ((u8 *)(c + 0x50f7))[o] + 1;
    if (((u8 *)(c + 0x50f7))[o] <= 6) return;
    ((u8 *)(c + 0x50f7))[o] = 6;
    ((u8 *)(c + o))[0x50f4] = 2;

    *(u16 *)(c + *(u8 *)(c + 0x5625) * 2 + 0x5610) = ((u8 *)(c + 0x50f8))[o] + 1;
    ((u16 *)(c + 0x5610))[*(u8 *)(c + 0x5625)] |= idx << 8;
    (*(u8 *)(c + 0x5625))++;
    *(u16 *)(c + 0x5614) = 0x20;

    func_ov006_0211b654(self, idx);
    id = ((u8 *)(c + 0x50f8))[o];

    if (self->Virtual8C()) {
        v = *(u8 *)(c + 0x5627);
        if (v == 0 || v == 2) {
            if (data_ov006_0213f794[id] == 2) {
                func_02012790(data_ov006_0213f7e8[id]);
            } else {
                func_02012174(*(u32 *)(c + 0x560c), data_ov006_0213f7e8[id]);
            }
        } else if (v == 1) {
            int n = id - 0x1a;
            int k = 0;
            while (n >= 4) { n -= 4; k++; }
            func_02012174(n, data_ov006_0213f7e8[0x1a + k * 4]);
        } else {
            Sound::PlayBank2_2D(data_ov006_0213f7e8[id]);
        }
    } else {
        v = *(u8 *)(c + 0x5627);
        if (v == 0 || v == 2) {
            func_02012790(data_ov006_0213f7e8[id]);
        } else if (v == 1) {
            func_02012174(*(u32 *)(c + 0x560c), data_ov006_0213f7e8[id]);
        } else if (v == 4) {
            if (data_ov006_0213f794[id] == 2) {
                Sound::PlayBank2_2D(data_ov006_0213f7e8[id]);
            } else {
                func_02012174(*(u32 *)(c + 0x560c), data_ov006_0213f7e8[id]);
            }
        } else {
            Sound::PlayBank2_2D(data_ov006_0213f7e8[id]);
        }
    }

    if (*(u8 *)(c + 0x5624) == 1) func_ov006_020c2300((char *)self->mTable);
}
