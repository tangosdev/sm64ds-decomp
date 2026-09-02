// func_ov007_020b6eb4, the title screen's per-frame display sync, displaced
// from src/ for ONE ARM REGISTER.
//
// Run link60 Stage 5 lane TS1. Same class and same treatment as
// unmatched/Ov007_RideThrough_020be980.cpp (lane RT1) and
// unmatched/Ov007_ImplicitR0_020add3c.cpp (lane AE1): the matched TU is
// correct on ARM and loses information on MSVC, so the port compiles a copy
// that spells the argument and the matched body stays matched.
//
// THE DEFECT. src/func_ov007_020b6eb4.c declares its callee as
//
//     extern void func_ov007_020bd648(void);
//
// and calls it with nothing, while src/func_ov007_020bd648.c defines it as
// `(int mode)` and switches on that mode TWICE -- once for the BGxCNT and
// bank setup and once for DISPCNT. On ARM the value rides r0: the guard the
// call sits under is `if (*(int *)(g + 0x1c) != 0)`, so the compiler has
// already loaded that word into r0 to test it and there is nothing to emit at
// the branch. On MSVC the caller pushes zero arguments and the callee reads a
// stack slot nobody wrote.
//
// port/ov007_seat.txt section 5d screened this row as a CANDIDATE and named
// the frame correctly: "bd648 <- b6eb4. ONE OF TWO CALLERS IS CORRECT ...
// src/func_ov007_020b7138.c declares `extern void func_ov007_020bd648(int
// mode)` and pushes one (00430E4F). src/func_ov007_020b6eb4.c declares it
// `(void)` and pushes none (00430C11). Displacing bd648 would be displacing
// the blameless end." So b6eb4 is the frame to displace, and that ruling was
// made before this lane existed.
//
// IT IS NOT A CANDIDATE ANY MORE, IT IS MEASURED. Lane TS1 dispatched the
// graphics-block beat for the first time and read the two words either side
// of the call on frame 0 of scene 1:
//
//     +18 0->0  +1c 1->0  DISPCNT 00010008->00010008  SUB 00010000->00010000
//
// The pending mode is CONSUMED, so the guard passed and the call happened; and
// DISPCNT does not move, so the callee took neither switch. With the argument
// spelled, the same frame reads
//
//     +18 0->0  +1c 1->0  DISPCNT 00010008->00011b08  SUB 00010000->00011c00
//
// which is func_ov007_020bd648's own `case 1` in its second switch:
// (DISPCNT & ~0x1f00) | 0x1b00 on engine A, | 0x1c00 on engine B. Those are the
// title screen's layer enables and nothing else in ov007 writes them; every
// other DISPCNT statement in the overlay CLEARS bits 8-12.
//
// WHAT IS COPIED, AND WHAT IS NOT. Everything else in this body is
// src/func_ov007_020b6eb4.c verbatim, including the ARMMathSaveState /
// ARMMathLoadState bracket and the order of the four calls. The ONLY
// difference is that the bd648 call passes the word its own guard just read.
// The matched TU is not edited and stays matched; port/CMakeLists.txt cuts it
// out of the ov007 slice, in the same loop and the same shape as the two
// displacements above it, and sets its own DISPLACED flag so the cut and
// anything that reads it cannot drift.
//
// WHAT THIS DOES NOT CLAIM. That the rest of the title screen's display sync
// is right; only that the argument arrives. The other three calls in this body
// were not audited by this lane beyond reading them.

typedef struct ARMMathState { int pad[7]; } ARMMathState;

extern "C" {
void ARMMathSaveState(ARMMathState *s);
void ARMMathLoadState(ARMMathState *s);
void func_ov007_020bf690(void);
void func_ov007_020bd4e8(void);
void func_ov007_020bd648(int mode);
void func_ov007_020b72a0(void);
extern char *data_ov007_02103430;
extern char *data_ov007_0210342c;

// PORT_HOST_ABI: register ride-through displaced from src for one ARM argument; src/func_ov007_020b6eb4.c declares the bd648 callee (void) and lets the mode word ride r0, which MSVC cannot reproduce (the caller pushes nothing). This copy spells the argument so the matched TU stays matched.
int func_ov007_020b6eb4(void)
{
    ARMMathState s;
    if (data_ov007_02103430 != 0) {
        ARMMathSaveState(&s);
        func_ov007_020bf690();
        if (*(int *)(data_ov007_0210342c + 0x18) != 0) {
            func_ov007_020bd4e8();
            *(int *)(data_ov007_0210342c + 0x18) = 0;
        }
        {
            /* THE ONE LINE THAT DIFFERS. The matched TU reads this word to
               test it and lets ARM carry it into the callee; here it is named
               so MSVC pushes it. */
            const int mode = *(int *)(data_ov007_0210342c + 0x1c);
            if (mode != 0) {
                func_ov007_020bd648(mode);
                *(int *)(data_ov007_0210342c + 0x1c) = 0;
            }
        }
        func_ov007_020b72a0();
        ARMMathLoadState(&s);
    }
    return 1;
}
}
