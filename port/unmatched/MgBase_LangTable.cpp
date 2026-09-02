// PORT_HOST_ABI. func_ov004_020b2cb8, dScMgBase_c's per-language file table,
// and A FABRICATED INITIALIZER IN THE DECOMP THAT THE BYTE GATE CANNOT SEE.
// Run link60, lane MG2.
//
// THIS IS WHERE THE FIRST MINIGAME BOOT FAULTED, and the fault is worth stating
// before the fix because it is a clean instance of a class this port keeps
// meeting. Slot 1 of dScMgCurling_c's vtable dispatched into
// func_ov004_020b0930, which calls this function, and the run died:
//
//     FAULT c0000005 at _func_ov004_020b2cb8+0x3ce accessing 0x00000065
//     eax 00000065
//
// 0x65 is 101, and 101 is a literal in src/func_ov004_020b2cb8.c.
//
// ---- WHAT THE SRC SAYS AND WHAT THE ROM SAYS ------------------------------
//
// The src opens with a twenty-nine element initializer:
//
//     Entry entries[29] = { {1,100}, {2,101}, {3,102}, ... {29,128} };
//
// a 1..29 / 100..128 ramp. Then it overwrites twenty-eight of the twenty-nine
// from per-language tables -- entries[0] and entries[2] through entries[28] --
// and ENTRY 1 IS NEVER TOUCHED. Its `ptr` therefore stays 101, and the loop at
// the bottom does `t = entries[i].ptr; t = *(int *)t;`. On the host that
// dereferences 101.
//
// THE RAMP IS NOT THE ROM'S DATA. The ROM builds this array by copying a fixed
// template, and the copy is right there in the instruction stream:
//
//     020b2cc0  ldr   r6, [pc, #0x3e0]     ; -> 0x020bc6e8
//     020b2cc4  add   r5, sp, #0
//     020b2cc8  mov   r4, #0xe
//     020b2ccc  ldm   r6!, {r0, r1, r2, r3}
//     020b2cd0  stm   r5!, {r0, r1, r2, r3}
//     020b2cd4  subs  r4, r4, #1
//     020b2cd8  bne   #0x20b2ccc            ; 14 x 16 = 224 bytes
//     020b2cdc  ldm   r6, {r0, r1}
//     020b2ce0  stm   r5, {r0, r1}          ; + 8 = 232 = 29 x 8
//
// and the template read out of extracted/overlays/overlay_0004.bin at
// (0x020bc6e8 - 0x020ad660) is not a ramp. It is:
//
//     entries[1] = { 320, 0x020b9f54 }
//     every other entry = { 0, 0 }
//
// One real file id and one real ov004 pointer, at exactly the index the src
// never overwrites, with zeros everywhere the src does. So the src's twenty-
// nine pairs are invented, and the one that matters is invented wrong.
//
// WHY NOTHING CAUGHT IT. 0x020bc6e8 is its own config symbol --
// config/arm9/overlays/ov004/symbols.txt line 689, `data_ov004_020bc6e8
// kind:data(any)` -- so the template is DATA the function points at, not bytes
// inside the function. The function's own instructions are a pc-relative load
// and a copy loop and they are identical whatever the template contains, which
// is why this survived as a matched body. The port then hosts the template
// correctly anyway, because tools/ovdata.py emits every config data symbol
// from the ROM image: build/port/host-src/ov004_syms.c carries
// data_ov004_020bc6e8[232] with the ROM's own bytes AND rebases its interior
// pointer, `*(unsigned int *)(data_ov004_020bc6e8 + 12) = (unsigned int)
// (data_ov004_020b9f54 + 0)`, which is entries[1].ptr. THE RIGHT ANSWER WAS
// ALREADY IN THE BINARY; only this TU was not asking for it.
//
// ---- THE HOST COPY --------------------------------------------------------
//
// One change: the initializer list becomes the copy the ROM actually performs,
// 232 bytes out of the mount's template. Everything below it is the src
// verbatim. THIS IS MORE FAITHFUL THAN THE SRC, NOT LESS -- it is the ldm/stm
// run transcribed as a memcpy instead of as invented literals -- and it is the
// reason the mount's rebasing reaches entry 1 the same way it already reached
// entry 0 (which is why the run faulted at i=1 and not at i=0).
//
// NOT FIXED IN src/. The header fix proper is a decomp change: the TU should
// read data_ov004_020bc6e8 rather than spell a template, and whether that
// still builds byte-identically under mwccarm is a question for the byte-gated
// tree and not for this file. Routed, not taken, the way
// unmatched/MgBase_DeclConflict.cpp routes its own five-file header fix.

#include <cstring>

extern "C" {

typedef struct Entry {
    int id;
    int ptr;
} Entry;

int GetGameLanguage(void);
int LoadFile(int handle);

/* THE ROM'S OWN TEMPLATE, hosted and pointer-rebased by the ov004 mount. */
extern unsigned char data_ov004_020bc6e8[232];

extern int data_ov004_020bc42c[];
extern int data_ov004_020bc65c[];
extern int data_ov004_020bc418[];
extern int data_ov004_020bc648[];
extern int data_ov004_020bc3c8[];
extern int data_ov004_020bc5e4[];
extern int data_ov004_020bc4f4[];
extern int data_ov004_020bc300[];
extern int data_ov004_020bc3dc[];
extern int data_ov004_020bc5f8[];
extern int data_ov004_020bc3f0[];
extern int data_ov004_020bc60c[];
extern int data_ov004_020bc404[];
extern int data_ov004_020bc634[];
extern int data_ov004_020bc3b4[];
extern int data_ov004_020bc5bc[];
extern int data_ov004_020bc468[];
extern int data_ov004_020bc6ac[];
extern int data_ov004_020bc454[];
extern int data_ov004_020bc698[];
extern int data_ov004_020bc378[];
extern int data_ov004_020bc594[];
extern int data_ov004_020bc620[];
extern int data_ov004_020bc670[];
extern int data_ov004_020bc440[];
extern int data_ov004_020bc684[];
extern int data_ov004_020bc5d0[];
extern int data_ov004_020bc3a0[];
extern int data_ov004_020bc47c[];
extern int data_ov004_020bc288[];
extern int data_ov004_020bc490[];
extern int data_ov004_020bc29c[];
extern int data_ov004_020bc580[];
extern int data_ov004_020bc350[];
extern int data_ov004_020bc4a4[];
extern int data_ov004_020bc2b0[];
extern int data_ov004_020bc4b8[];
extern int data_ov004_020bc2c4[];
extern int data_ov004_020bc4cc[];
extern int data_ov004_020bc2d8[];
extern int data_ov004_020bc4e0[];
extern int data_ov004_020bc2ec[];
extern int data_ov004_020bc38c[];
extern int data_ov004_020bc508[];
extern int data_ov004_020bc314[];
extern int data_ov004_020bc51c[];
extern int data_ov004_020bc328[];
extern int data_ov004_020bc530[];
extern int data_ov004_020bc6d4[];
extern int data_ov004_020bc544[];
extern int data_ov004_020bc6c0[];
extern int data_ov004_020bc5a8[];
extern int data_ov004_020bc33c[];
extern int data_ov004_020bc558[];
extern int data_ov004_020bc364[];
extern int data_ov004_020bc56c[];
extern int data_ov004_020bf560[];
extern int data_ov004_020bf5d4[];

// PORT_HOST_ABI: src's 29-entry table initializer is fabricated (byte-invisible on ARM); the host copy memcpys the ROM's real template data_ov004_020bc6e8 so entry 1 is not a wild dereference
void func_ov004_020b2cb8(void)
{
    int i;
    Entry entries[29];

    /* the ROM's copy loop, 29 x 8 bytes from 0x020bc6e8. The src spells this
       as an initializer list of invented values; see the header. */
    std::memcpy(entries, data_ov004_020bc6e8, sizeof entries);

    entries[0].id = data_ov004_020bc42c[GetGameLanguage()];
    entries[0].ptr = data_ov004_020bc65c[GetGameLanguage()];
    entries[2].id = data_ov004_020bc418[GetGameLanguage()];
    entries[2].ptr = data_ov004_020bc648[GetGameLanguage()];
    entries[3].id = data_ov004_020bc3c8[GetGameLanguage()];
    entries[3].ptr = data_ov004_020bc5e4[GetGameLanguage()];
    entries[4].id = data_ov004_020bc4f4[GetGameLanguage()];
    entries[4].ptr = data_ov004_020bc300[GetGameLanguage()];
    entries[5].id = data_ov004_020bc3dc[GetGameLanguage()];
    entries[5].ptr = data_ov004_020bc5f8[GetGameLanguage()];
    entries[6].id = data_ov004_020bc3f0[GetGameLanguage()];
    entries[6].ptr = data_ov004_020bc60c[GetGameLanguage()];
    entries[7].id = data_ov004_020bc404[GetGameLanguage()];
    entries[7].ptr = data_ov004_020bc634[GetGameLanguage()];
    entries[8].id = data_ov004_020bc3b4[GetGameLanguage()];
    entries[8].ptr = data_ov004_020bc5bc[GetGameLanguage()];
    entries[9].id = data_ov004_020bc468[GetGameLanguage()];
    entries[9].ptr = data_ov004_020bc6ac[GetGameLanguage()];
    entries[10].id = data_ov004_020bc454[GetGameLanguage()];
    entries[10].ptr = data_ov004_020bc698[GetGameLanguage()];
    entries[11].id = data_ov004_020bc378[GetGameLanguage()];
    entries[11].ptr = data_ov004_020bc594[GetGameLanguage()];
    entries[12].id = data_ov004_020bc620[GetGameLanguage()];
    entries[12].ptr = data_ov004_020bc670[GetGameLanguage()];
    entries[13].id = data_ov004_020bc440[GetGameLanguage()];
    entries[13].ptr = data_ov004_020bc684[GetGameLanguage()];
    entries[14].id = data_ov004_020bc5d0[GetGameLanguage()];
    entries[14].ptr = data_ov004_020bc3a0[GetGameLanguage()];
    entries[15].id = data_ov004_020bc47c[GetGameLanguage()];
    entries[15].ptr = data_ov004_020bc288[GetGameLanguage()];
    entries[16].id = data_ov004_020bc490[GetGameLanguage()];
    entries[16].ptr = data_ov004_020bc29c[GetGameLanguage()];
    entries[17].id = data_ov004_020bc580[GetGameLanguage()];
    entries[17].ptr = data_ov004_020bc350[GetGameLanguage()];
    entries[18].id = data_ov004_020bc4a4[GetGameLanguage()];
    entries[18].ptr = data_ov004_020bc2b0[GetGameLanguage()];
    entries[19].id = data_ov004_020bc4b8[GetGameLanguage()];
    entries[19].ptr = data_ov004_020bc2c4[GetGameLanguage()];
    entries[20].id = data_ov004_020bc4cc[GetGameLanguage()];
    entries[20].ptr = data_ov004_020bc2d8[GetGameLanguage()];
    entries[21].id = data_ov004_020bc4e0[GetGameLanguage()];
    entries[21].ptr = data_ov004_020bc2ec[GetGameLanguage()];
    entries[22].id = data_ov004_020bc38c[GetGameLanguage()];
    entries[22].ptr = data_ov004_020bc508[GetGameLanguage()];
    entries[23].id = data_ov004_020bc314[GetGameLanguage()];
    entries[23].ptr = data_ov004_020bc51c[GetGameLanguage()];
    entries[24].id = data_ov004_020bc328[GetGameLanguage()];
    entries[24].ptr = data_ov004_020bc530[GetGameLanguage()];
    entries[25].id = data_ov004_020bc6d4[GetGameLanguage()];
    entries[25].ptr = data_ov004_020bc544[GetGameLanguage()];
    entries[26].id = data_ov004_020bc6c0[GetGameLanguage()];
    entries[26].ptr = data_ov004_020bc5a8[GetGameLanguage()];
    entries[27].id = data_ov004_020bc33c[GetGameLanguage()];
    entries[27].ptr = data_ov004_020bc558[GetGameLanguage()];
    entries[28].id = data_ov004_020bc364[GetGameLanguage()];
    entries[28].ptr = data_ov004_020bc56c[GetGameLanguage()];

    for (i = 0; i < 29; i++) {
        int t;
        t = LoadFile(entries[i].id);
        ((volatile int *)data_ov004_020bf560)[i] = t;
        t = (int)&entries[i];
        t = *(volatile int *)(t + 4);
        t = *(int *)t;
        data_ov004_020bf5d4[i] = t;
    }
}

}  /* extern "C" */
