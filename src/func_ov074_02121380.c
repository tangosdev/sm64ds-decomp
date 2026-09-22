// @symbol func_ov074_02121380
/* recovered: daKuriKing_c (Goomboss) rebuilds his collision cylinders and reads
 * the ground under them. Called unconditionally from the boss's Behavior after
 * the state dispatch, so the whole fight's collision runs through it.
 *
 * Four cylinders. Three follow the animated model: for each of bones 1, 8 and
 * 0xa it asks func_ov074_02121270 for that bone's world position, caches it in
 * the bonepos[3] array at c+0x3ac, and writes a radius, a height and a centre
 * into the record at c + (i + 1) * 0x40 + 0x114. The fourth is the boss body
 * itself, written at c+0x114 from his own position. Every radius is the x
 * scale times a constant and every height the y scale times a constant, so the
 * cylinders track him as he grows and shrinks.
 *
 * Then the ground test: bonepos[1].y and bonepos[2].y, each biased by the y
 * scale times -0x4a000, are compared against his own y. Each comparison drives
 * a latch byte (c+0x600, c+0x601) and an edge byte (c+0x5fe, c+0x5ff) that is
 * set only on the frame the foot first goes below, and each latch then flips
 * bit 0x20 of a render word (c+0x1a8, c+0x1e8).
 */
// NONMATCHING: div 11 of 221 words. mwccarm 2004/b56, --module ov074,
// @ 0x02121380 size 0x374. Size and the 0x74 stack frame are exact, the whole
// prologue and the whole tail are byte-exact, and the residue is 11 words in
// the middle of the bone loop.
//
// Two scorers, two numbers, both reported because they measure different
// things. tools/match.py --strict-relocs and tools/wallcrack.py both say **11**,
// comparing word against word at the same offset -- that is the merge gate's
// metric and the authoritative one. tools/nearmiss_db.py says **9**, because it
// scores a difflib alignment over the reloc-wildcarded disassembly, which
// charges a merely REORDERED block once instead of at every offset it moved.
// Both sides wildcard the same two reloc slots (+0x1c8, +0x370), so the gap is
// the alignment, not the wildcarding. Neither is wrong; the gate is the one
// that decides a merge.
//
// Independently re-derived from the ROM listing. A near-miss row for this
// address already existed at div 297 from a `fanout-opus` run; it was not used
// -- structure, externs and frame layout here are a fresh derivation.
//
// LEVER (notes/mwccarm-codegen.md section 6bq): the rank of the loop-invariant
// array base pointers follows the SOURCE ORDER OF THE STORES that consume them.
// The five record stores go to five distinct offsets off one base, so all 120
// orders are semantically identical and the scheduler re-emits them freely --
// but the order the source presents them in decides which array base gets which
// callee-saved register. Only 0x114, 0x144, 0x118, 0x148, 0x14c, paired with
// computing the position temps z, y, x, reproduces the cartridge's colouring
// (yoff->sl, rad->sb, hgt->r8, out->r7); the natural 0x114, 0x118, 0x144, 0x148,
// 0x14c never does, and that order is worth 25 -> 11. Do not "tidy" it.
//
// FLOOR: a two-attractor pin of the 6bn shape. The store order that gives the
// cartridge's REGISTERS emits the 0x144 store three slots early; the store
// order that gives the cartridge's SCHEDULE gives the rotated registers. No
// shape reaches both. Measured closed, on this body, through the gate: all 120
// record-store orders x 6 position-temp orders x 6 bone-store orders (4320
// cells); the full 246-name verified pragma vocabulary at on and off on BOTH
// attractors, then crossed with the whole order space (5040 cells) per 6bo;
// 6bp's named-address lever in both directions (deleting `p`'s name is free and
// changes nothing, deleting `q`'s costs 8 bytes, and naming the three array
// bases costs more); 6bp's statement-RELOCATION neighbourhood climbed to a
// local optimum from both attractors; 24 declaration permutations; 125 element
// type combinations; four spellings of the frame region; four loop forms; three
// address-expression trees; struct-copy spellings of the bone-position traffic;
// a one-variable reuse of the y temp and the record base (which inverts WHICH
// store order colours right, but not the floor); and all 25 archived mwccarm
// builds, of which only 2004/b56 even reaches the right size.
//
// The permuter is the wrong tool for this residue and was measured saying so:
// ~16000 iterations over four runs on two structurally different seeds, and on
// the div-11 seed every candidate it scored BETTER than the base (445 and 575
// against 665) is size-drifted to 0x378/0x37c. That is 6bn addendum 4's hazard
// reproduced -- with an ordering residue the permuter's score and the byte
// oracle point in opposite directions.
//
// AUDIT: differential execution against the cartridge, exhaustive over the
// 2700-state discriminating lattice (three bone heights x below/on/above the
// compare threshold, both ground latches, five x-scales, five y-scales):
// IDENTICAL on every state -- same call sequence with the same arguments, same
// final value of all 216 written bytes, same return. Every one of the 12 branch
// arms is exercised (coverage reported, none unreached). The harness is trusted
// only because it was made to FAIL first: 8 deliberately broken, SIZE-NEUTRAL
// controls -- wrong stored value, wrong store address, wrong load address,
// branch boundary, wrong RMW mask, wrong call argument, wrong latch, wrong
// component -- are all caught, and building them exposed three real harness
// bugs (unimplemented ldm/stm, both sides reading the ROM's data image through
// the pooled address instead of their own, and seeding that never reached the
// >= arm or the exact compare boundary). The harness is not shipped in-tree,
// same as the ov034 one it was adapted from; it is saved with this run's
// artefacts as audit.py.
//
// PLACEHOLDER, NOT RECOVERED SOURCE: `struct Vector3 v[3]` with `&v[2]` passed
// to the bone getter. The cartridge's frame carries 24 bytes between hgt[] and
// the output vector that no instruction touches, plus a word above it. mwccarm
// drops a local nothing references -- checked against unused scalars, unused
// structs, `volatile` unused locals, arrays whose every store is dead, and
// address-taken-but-folded forms, none of which reserve a byte -- so those bytes
// have to belong to an object that IS used, and the only shapes that reproduce
// the frame put them in the same object as the output vector. `Vector3 out[3]`
// with &out[2], a three-member struct with its last member passed, and
// `int buf[9]` with the vector at buf[6] all compile identically. The bytes are
// right; the declaration behind them is a guess and should not be read as
// recovered source.
//
// Counts as decompiled, not matched.
#include "common.h"

extern void func_ov074_02121270(struct Vector3* out, char* c, int i);

#define FX(a, b) (int)(((long long)(a) * (long long)(b) + 0x800) >> 12)

void func_ov074_02121380(char* c) {
    int bone[3] = {1, 8, 0xa};
    int yoff[3], rad[3], hgt[3];
    struct Vector3 v[3];
    int i;
    int h0, r0, y0;
    int d, a, b;
    char* q;
    int tx, ty, tz;

    y0 = FX(*(int*)(c + 0x84), 0x40000);
    rad[1] = FX(*(int*)(c + 0x80), 0x50000);
    hgt[1] = FX(*(int*)(c + 0x84), 0x40000);
    yoff[1] = FX(*(int*)(c + 0x84), -0x10000);
    rad[2] = FX(*(int*)(c + 0x80), 0x50000);
    hgt[2] = FX(*(int*)(c + 0x84), 0x40000);
    yoff[2] = FX(*(int*)(c + 0x84), -0x10000);
    h0 = FX(*(int*)(c + 0x84), 0x96000);
    r0 = FX(*(int*)(c + 0x80), 0x5a000);
    yoff[0] = FX(*(int*)(c + 0x84), 0x14000);
    rad[0] = FX(*(int*)(c + 0x80), 0xb4000);
    hgt[0] = FX(*(int*)(c + 0x84), 0x104000);
    q = c;
    for (i = 0; i < 3; i++) {
        char* p;
        int ax, ay, az;
        func_ov074_02121270(&v[2], c, bone[i]);
        *(int*)(q + 0x3ac) = v[2].x;
        *(int*)(q + 0x3b0) = v[2].y;
        *(int*)(q + 0x3b4) = v[2].z;
        az = *(int*)(q + 0x3b4);
        ay = *(int*)(q + 0x3b0) + yoff[i];
        ax = *(int*)(q + 0x3ac);
        p = c + (i + 1) * 0x40;
        *(int*)(p + 0x114) = rad[i];
        *(int*)(p + 0x144) = ax;
        *(int*)(p + 0x118) = hgt[i];
        *(int*)(p + 0x148) = ay;
        *(int*)(p + 0x14c) = az;
        q += 0xc;
    }

    tx = *(int*)(c + 0x5c);
    ty = *(int*)(c + 0x60) + y0;
    tz = *(int*)(c + 0x64);
    *(int*)(c + 0x114) = r0;
    *(int*)(c + 0x118) = h0;
    *(int*)(c + 0x144) = tx;
    *(int*)(c + 0x148) = ty;
    *(int*)(c + 0x14c) = tz;

    d = FX(*(int*)(c + 0x84), -0x4a000);
    a = *(int*)(c + 0x3c8) + d;
    b = *(int*)(c + 0x3bc) + d;
    if (a < *(int*)(c + 0x60)) {
        if (*(unsigned char*)(c + 0x600) == 0) {
            *(unsigned char*)(c + 0x5fe) = 1;
        } else {
            *(unsigned char*)(c + 0x5fe) = 0;
        }
        *(unsigned char*)(c + 0x600) = 1;
    } else {
        *(unsigned char*)(c + 0x600) = 0;
        *(unsigned char*)(c + 0x5fe) = 0;
    }
    if (b < *(int*)(c + 0x60)) {
        if (*(unsigned char*)(c + 0x601) == 0) {
            *(unsigned char*)(c + 0x5ff) = 1;
        } else {
            *(unsigned char*)(c + 0x5ff) = 0;
        }
        *(unsigned char*)(c + 0x601) = 1;
    } else {
        *(unsigned char*)(c + 0x601) = 0;
        *(unsigned char*)(c + 0x5ff) = 0;
    }
    if (*(unsigned char*)(c + 0x601) == 0) {
        *(int*)(c + 0x1a8) |= 0x20;
    } else {
        *(int*)(c + 0x1a8) &= ~0x20;
    }
    if (*(unsigned char*)(c + 0x600) == 0) {
        *(int*)(c + 0x1e8) |= 0x20;
    } else {
        *(int*)(c + 0x1e8) &= ~0x20;
    }
}
