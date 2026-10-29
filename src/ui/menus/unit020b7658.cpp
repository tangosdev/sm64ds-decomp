//cpp
/* unit020b7658 -- ov007 unit 0x020b7658..0x020b797c: 11 functions, .text only.
 *
 * The title menu's particle-effect module. ov007 is the title and
 * file-select menu program that dScDSMT_c hosts; outside dScDSMT_c it has
 * no ROM RTTI or vtable, so no class name is claimed and the file keeps the
 * unit<addr> name.
 *
 * What the code does. func_ov007_020b782c (called from the menu's init
 * dispatcher, func_ov007_020b7138) takes a 0x3c00-byte block from the
 * "Effect" heap, makes a particle manager whose allocator is the bump
 * allocator func_ov007_020b78d0 over that block, clears the two system
 * slots data_ov007_02103450[0..1] and hands the manager menu file 0x17
 * (func_ov007_020b3f20). func_ov007_020b7764 (called from
 * func_ov007_020b6f4c, which tears every menu module down) releases file
 * 0x17 and frees the block.
 * func_ov007_020b77ac seeds the four counters data_ov007_02103434..
 * 02103440 and installs func_ov007_020b7948 / 020b7914 as manager
 * callbacks; each callback returns one counter and moves it by its
 * argument. func_ov007_020b7708 spawns a system into an empty slot,
 * 020b76c4 / 020b76a0 remove one or both, 020b7690 returns a slot and
 * 020b7658 runs the manager.
 *
 * Edges: every function here touches this module's bss block
 * 0x02103434..0x02103458 or calls one that does, and nothing outside the
 * range touches it. The previous function, func_ov007_020b7594, is a
 * helper of func_ov007_020b6d40 and touches none of it; the next one,
 * func_ov007_020b797c, belongs to the module whose bss block starts at
 * 0x02103458.
 *
 * Emission order: mwccarm 2004/b56 emits one .text section per function in
 * the reverse of source order, so the highest-address function is written
 * first here and the functions come out in ROM order. The file-scope
 * opt_common_subs pragma is the one func_ov007_020b78d0 needs; a pragma
 * scoped to that function does not take effect, because mwccarm generates
 * code for the whole unit at the end.
 *
 * The functions keep their ROM names under C linkage.
 */
#include "Particle__Manager.h"
#include "Particle__System.h"

#pragma opt_common_subs off

extern "C" {

extern int func_02049f58(void *);
extern int func_02049ee8(void *, void *);
extern int func_02049d78(void *, void *);
extern void func_0204a0dc(void *, void *);
extern void func_0204a028(void *, void *);
extern char *func_0204a4c8(char *(*alloc)(int), int, int, int, unsigned short);
extern void func_0204a17c(Particle::Manager *, char *);
extern int func_02045d00(void);
extern int func_02045ce0(void);

extern int func_ov007_020c1804(void);
extern int func_ov007_020c17f4(void);
extern int func_ov007_020c3df4(int, void *);
extern void func_ov007_020c3d1c(void *);
extern void func_ov007_020b3edc(int);
extern void *func_ov007_020b3f20(int);

extern char data_ov007_02102dd0[];
extern int data_ov007_02103434;
extern int data_ov007_02103438;
extern int data_ov007_0210343c;
extern int data_ov007_02103440;
extern int data_ov007_02103444;
extern int data_ov007_02103448;
extern void *data_ov007_0210344c;
extern Particle::System *data_ov007_02103450[];

// @symbol func_ov007_020b7948
int func_ov007_020b7948(int x, int cond) {
    int r;
    if (cond) {
        r = data_ov007_02103440;
        data_ov007_02103440 = r + x;
    } else {
        r = data_ov007_0210343c - x;
        data_ov007_0210343c = r;
    }
    return r;
}

// @symbol func_ov007_020b7914
int func_ov007_020b7914(int x, int cond) {
    int r;
    if (cond) {
        r = data_ov007_02103438;
        data_ov007_02103438 = r + x;
    } else {
        r = data_ov007_02103434 - x;
        data_ov007_02103434 = r;
    }
    return r;
}

// @symbol func_ov007_020b78d0
int func_ov007_020b78d0(int x) {
    int r3 = x << 1;
    int r0 = x & ~3;
    int b = data_ov007_02103448;
    int a = data_ov007_02103444;
    int t = r3 - r0;
    int st = a + t;
    int rv = b + a;
    data_ov007_02103444 = st;
    return rv;
}

// @symbol func_ov007_020b782c
void func_ov007_020b782c(void) {
    int i;
    Particle::System *empty;
    data_ov007_02103448 = func_ov007_020c3df4((int)&data_ov007_02102dd0, (void *)0x3c00);
    data_ov007_02103444 = 0;
    data_ov007_0210344c = func_0204a4c8((char *(*)(int))func_ov007_020b78d0, 2, 0xc8, 0x23, 0x25);
    i = 0;
    empty = 0;
    do {
        data_ov007_02103450[i] = empty;
        i++;
    } while (i < 2);
    func_0204a17c(static_cast<Particle::Manager *>(data_ov007_0210344c),
                  static_cast<char *>(func_ov007_020b3f20(0x17)));
}

// @symbol func_ov007_020b77ac
void func_ov007_020b77ac(void) {
    data_ov007_02103440 = func_ov007_020c1804();
    data_ov007_0210343c = func_02045d00();
    data_ov007_02103438 = func_ov007_020c17f4();
    data_ov007_02103434 = func_02045ce0();
    func_0204a0dc(data_ov007_0210344c, (void *)func_ov007_020b7948);
    func_0204a028(data_ov007_0210344c, (void *)func_ov007_020b7914);
}

// @symbol func_ov007_020b7764
void func_ov007_020b7764(void) {
    func_ov007_020b3edc(0x17);
    if (data_ov007_02103448 == 0)
        return;
    func_ov007_020c3d1c((void *)data_ov007_02103448);
    data_ov007_02103448 = 0;
}

// @symbol func_ov007_020b7708
void func_ov007_020b7708(int idx, void *pos) {
    if (data_ov007_02103450[idx] == 0) {
        Particle::System *system;
        data_ov007_02103450[idx] = static_cast<Particle::Manager *>(data_ov007_0210344c)
                                       ->AddSystem(0, *static_cast<Vector3 *>(pos));
        system = data_ov007_02103450[idx];
        system->callbackFlags |= 2;
    }
}

// @symbol func_ov007_020b76c4
void func_ov007_020b76c4(int c) {
    void *r1 = data_ov007_02103450[c];
    if (!r1)
        return;
    func_02049d78(data_ov007_0210344c, r1);
    data_ov007_02103450[c] = 0;
}

// @symbol func_ov007_020b76a0
void func_ov007_020b76a0() {
    int i;
    for (i = 0; i < 2; i++) {
        func_ov007_020b76c4(i);
    }
}

// @symbol func_ov007_020b7690
void *func_ov007_020b7690(int i) {
    return data_ov007_02103450[i];
}

// @symbol func_ov007_020b7658
void func_ov007_020b7658(void *c, int flag) {
    if (flag) {
        func_02049f58(data_ov007_0210344c);
    }
    func_02049ee8(data_ov007_0210344c, c);
}
}
