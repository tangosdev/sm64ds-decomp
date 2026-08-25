// PORT_HOST_ABI.  The trampoline RECORD READOUT.  Run mg12, lane TRM.
//
// WHY THIS EXISTS AND WHY IT IS NOT A COUNTER
//
// Until run mg12 the only instrument either trampoline scene had for "did a
// stylus stroke get judged" was the WANT COUNT of the trap standing in for
// func_ov006_020d0c38: the body had no decompilation, the trap returned the
// miss arm, and counting how often it was asked was the honest measurement.
// That body is decompiled and seated now (src/func_ov006_020d0c38.c, an honest
// NONMATCHING seat with its divergences in its banner), so the trap is gone and
// its counter can only read zero.  A zero that used to mean "no stroke was ever
// judged" would now mean nothing at all, which is exactly the shape run mg11
// banked as a vacuous reading, so the instrument is replaced rather than left
// to rot.
//
// WHAT IT READS, AND WHY THESE FOUR ADDRESSES
//
// The hit test's success tail writes three of these in the same breath as the
// record install:
//
//     *(short *)&data_ov006_02140cb4[i * 0x32c] = 1;      record i goes LIVE
//     *(short *)&data_ov006_02140cb6[i * 0x32c] = 0;
//     *(short *)&data_ov006_02140cae[i * 0x32c] = i;
//
// AND IT IS NOT THE ONLY WRITER, WHICH IS WHY THIS READOUT SPLITS THE FOUR.
// The first version of this file said "func_ov006_020d0c38 is the only writer
// of that word" and the first run disproved it: record 3 came up live before a
// stylus had touched anything.  src/func_ov006_020d0b78.c installs the scene's
// OWN starting trampoline into RECORD 3 at setup -- it writes
// data_ov006_02141590[0x54] = 3, [0x55] = 0 and [0x51] = 3, which resolve to
// 0x02140990 + 3*0x32c + 0x324, +0x326 and +0x31e, the same three fields --
// and it is the one live caller of the 0x800 installer, which is where the
// "020d01e0 wanted 1 time" both censuses have always printed comes from.
//
// So the split is: RECORD 3 belongs to the scene's setup and a nonzero there
// says nothing about the stylus.  RECORDS 0..2 are the only ones the hit test
// can take -- its own loop is `for (i = 0; i < 3; i++)` -- and it writes 1
// there, never 3.  A live 0..2 record is a stroke this port ACCEPTED.
//
// The record base itself,
// data_ov006_02140990 (stride 0x32c, four records), carries the two stroke
// endpoints at +0x50 and +0x54, and the RENDER GATE at +0x328 which decides
// whether func_ov006_020cf2fc and func_ov006_020cf758 ever look at the record
// (src/func_ov006_020d09e0.c and src/func_ov006_020d0ac0.c are the two loops
// that read it).
//
// A CORRECTION, MEASURED. What stood here said the endpoints were written by
// the TEST, "before it called the installer". They are not: the test PASSES
// them (func_ov006_020d01e0(g, a, b), the call at +0x344 of the seat) and the
// installer is what stores them. Run mg12 lane OVERLAY wired slot 23's missing
// beat and drew a scripted line on 384; record 0 came up live=1, +0x326=0,
// index=0 -- the seat's own three-store success tail at +0x34c..+0x36c, exactly
// as its banner describes -- with the endpoints still (0,0)->(0,0) because the
// installer is still trapped. So +0x50/+0x54 belong on the SAME side of the
// wall as the render gate, and a reader must not take zeros there as evidence
// that no stroke was accepted. The live flag is the evidence; these two are
// the floor's.
//
// READ THE GATE HONESTLY.  +0x328 is NOT written by the hit test.  It is
// written inside func_ov006_020d01e0, the 0x800 installer, which is still a
// trapped floor -- so on this build the gate stays 0, the two render loops skip
// every record, and a LIVE record with a zeroed gate is the exact, expected
// picture of "the stroke was accepted and the trampoline was not built yet".
// That is a partial result and this readout is worded so it cannot be mistaken
// for a whole one.

#include <cstdio>

extern "C" {

/* the ov006 mount's own C names; src/func_ov006_020d0ac0.c reaches the record
   base the same way */
extern unsigned char data_ov006_02140990[];
extern unsigned char data_ov006_02140cae[];
extern unsigned char data_ov006_02140cb4[];
extern unsigned char data_ov006_02140cb6[];

void port_mg_trampoline_record_report(void);

}

#define REC_STRIDE 0x32c

void port_mg_trampoline_record_report(void)
{
    int i;
    int strokes = 0;            /* records 0..2 only, and only value 1 */
    int gated = 0;
    int setup = 0;              /* record 3, written by func_ov006_020d0b78 */

    for (i = 0; i < 4; i++) {
        const unsigned char *rec = &data_ov006_02140990[i * REC_STRIDE];
        const short flag = *(const short *)&data_ov006_02140cb4[i * REC_STRIDE];
        const short f326 = *(const short *)&data_ov006_02140cb6[i * REC_STRIDE];
        const short idx  = *(const short *)&data_ov006_02140cae[i * REC_STRIDE];
        const short *p0 = (const short *)(rec + 0x50);
        const short *p1 = (const short *)(rec + 0x54);
        const unsigned char gate = rec[0x328];

        if (gate != 0) ++gated;
        if (i == 3) {
            if (flag != 0) ++setup;
        } else if (flag == 1) {
            ++strokes;
        }

        std::printf("[scene] trampoline record %d (%s): live=%d (+0x324), "
                    "+0x326=%d, index=%d (+0x31e), stroke (%d,%d)->(%d,%d) "
                    "(+0x50/+0x54), render gate +0x328=%u\n",
                    i, i == 3 ? "SETUP, func_ov006_020d0b78's"
                              : "stylus, func_ov006_020d0c38's",
                    (int)flag, (int)f326, (int)idx,
                    (int)p0[0], (int)p0[1], (int)p1[0], (int)p1[1],
                    (unsigned)gate);
    }

    std::printf("[scene] trampoline records: %d of 3 stylus records (0..2) "
                "carry an ACCEPTED stroke, %d setup record (3, the scene's own "
                "starting trampoline from func_ov006_020d0b78) is set, %d of 4 "
                "render-gated. Record 3 being live is a boot fact and NOT "
                "stylus evidence. The gate at +0x328 is written inside "
                "func_ov006_020d01e0, which is STILL A TRAPPED FLOOR, so "
                "gated==0 is the expected shape of this build: a stroke can be "
                "judged and accepted, and the trampoline is not built yet\n",
                strokes, setup, gated);
}
