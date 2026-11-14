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
 * MenuText_ClearMainPlane, belongs to the module whose bss block starts at
 * 0x02103458.
 *
 * Emission order: mwccarm 2004/b56 emits one .text section per function in
 * the reverse of source order, so the highest-address function is written
 * first here and the functions come out in ROM order.
 *
 */
#include "Particle__Manager.h"
#include "Particle__System.h"

extern "C" void func_02049f58(Particle::Manager *);
void func_02049ee8(Particle::Manager *, Matrix4x3 *);
void func_02049d78(Particle::Manager *, Particle::System *);
extern "C" int func_0204a0dc(Particle::Manager *, u32 (*allocTexVram)(u32, u32));
extern "C" int func_0204a028(Particle::Manager *, u32 (*allocPalVram)(u32, u32));
extern "C" char *func_0204a4c8(char *(*alloc)(int), int, int, int, unsigned short);
extern "C" void func_0204a17c(Particle::Manager *, char *);
int func_02045d00(void);
extern "C" int func_02045ce0(void);

extern int func_ov007_020c1804(void);
extern int func_ov007_020c17f4(void);
extern "C" void *func_ov007_020c3df4(const char *name, int size);
extern "C" void func_ov007_020c3d1c(void *);
extern "C" void func_ov007_020b3edc(int);
extern "C" void *func_ov007_020b3f20(int);

extern char data_ov007_02102dd0[];
extern int data_ov007_02103434;
extern int data_ov007_02103438;
extern int data_ov007_0210343c;
extern int data_ov007_02103440;
extern int data_ov007_02103444;
extern char *data_ov007_02103448;
extern Particle::Manager *data_ov007_0210344c;
extern Particle::System *data_ov007_02103450[];

u32 func_ov007_020b7948(u32 size, u32 isTexel4x4) {
    int r;
    if (isTexel4x4) {
        r = data_ov007_02103440;
        data_ov007_02103440 = r + size;
    } else {
        r = data_ov007_0210343c - size;
        data_ov007_0210343c = r;
    }
    return r;
}

u32 func_ov007_020b7914(u32 size, u32 fromLowAddress) {
    int r;
    if (fromLowAddress) {
        r = data_ov007_02103438;
        data_ov007_02103438 = r + size;
    } else {
        r = data_ov007_02103434 - size;
        data_ov007_02103434 = r;
    }
    return r;
}

char *func_ov007_020b78d0(int x) {
    char *rv = data_ov007_02103448 + data_ov007_02103444;
    data_ov007_02103444 += (x << 1) - (x & ~3);
    return rv;
}

void func_ov007_020b782c(void) {
    int i;
    Particle::System *empty;
    data_ov007_02103448 = (char *)func_ov007_020c3df4(data_ov007_02102dd0, 0x3c00);
    data_ov007_02103444 = 0;
    data_ov007_0210344c =
        (Particle::Manager *)func_0204a4c8(func_ov007_020b78d0, 2, 0xc8, 0x23, 0x25);
    i = 0;
    empty = 0;
    do {
        data_ov007_02103450[i] = empty;
        i++;
    } while (i < 2);
    func_0204a17c(data_ov007_0210344c, static_cast<char *>(func_ov007_020b3f20(0x17)));
}

extern "C" void func_ov007_020b77ac(void) {
    data_ov007_02103440 = func_ov007_020c1804();
    data_ov007_0210343c = func_02045d00();
    data_ov007_02103438 = func_ov007_020c17f4();
    data_ov007_02103434 = func_02045ce0();
    func_0204a0dc(data_ov007_0210344c, func_ov007_020b7948);
    func_0204a028(data_ov007_0210344c, func_ov007_020b7914);
}

void func_ov007_020b7764(void) {
    func_ov007_020b3edc(0x17);
    if (data_ov007_02103448 == 0)
        return;
    func_ov007_020c3d1c(data_ov007_02103448);
    data_ov007_02103448 = 0;
}

void func_ov007_020b7708(int idx, void *pos) {
    if (data_ov007_02103450[idx] == 0) {
        Particle::System *system;
        data_ov007_02103450[idx] = data_ov007_0210344c->AddSystem(0, *static_cast<Vector3 *>(pos));
        system = data_ov007_02103450[idx];
        system->callbackFlags |= 2;
    }
}

void func_ov007_020b76c4(int c) {
    Particle::System *system = data_ov007_02103450[c];
    if (!system)
        return;
    func_02049d78(data_ov007_0210344c, system);
    data_ov007_02103450[c] = 0;
}

extern "C" void func_ov007_020b76a0() {
    int i;
    for (i = 0; i < 2; i++) {
        func_ov007_020b76c4(i);
    }
}

void *func_ov007_020b7690(int i) {
    return data_ov007_02103450[i];
}

void func_ov007_020b7658(Matrix4x3 *viewMatrix, int update) {
    if (update) {
        func_02049f58(data_ov007_0210344c);
    }
    func_02049ee8(data_ov007_0210344c, viewMatrix);
}
