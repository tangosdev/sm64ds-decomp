//cpp
// NONMATCHING -- this file compiles, but it can never be enrolled as written, and
// it must not be counted as a match.
//
// It reconstructs the thunk's BYTES from a synthetic hierarchy: Base1/Base2/Derived
// below are placeholders chosen to make mwcc emit a this-adjusting thunk of the
// right shape, NOT the ROM's real types. The object therefore defines
// _ZThn16_N7DerivedD0Ev and the Derived structors -- none of which any symbols.txt
// under config/ names -- rather than the single symbol `func_020375b0' this file claims.
// build_pin agrees: "func_020375b0 is not in the object 2004/b56 produced".
//
// Its four bannered D1 twins (src/func_020375c0.c and siblings) say the same thing.
// This one and its three D0 siblings were left unbannered, so progress.py's
// synced_from_src counted them as matched on the strength of the filename alone --
// the PR #1072 shape. Bannered 2026-08-19; see notes/collision-system.md.
//
// WHAT IT REALLY IS, read out of the cartridge:
//   0x020375b0  _ZThn16_N9dBgCh_GndD0Ev
//   ldr ip,[pc,#4] ; add r0,r0,ip ; b 0x020374f0   with the adjustment word 0x10
//   i.e. adjust this by -0x10 -- the dBgPi base of dBgCh_Gnd (dBgCh_Gnd) --
//   and tail-branch to that class's deleting destructor.
//
// Recovering it for real needs the ROM's actual derived class, which is the
// class-renaming workstream (notes/collision-system.md Phase 1), not a byte experiment.
// This-adjusting virtual destructor thunk (adjustment -0x10). Compiler-generated
// as a byproduct of a class with a secondary base of size 0x10 and its own
// out-of-line destructor; the tail-branch target is a relocation wildcard.
struct Base1 {
    char pad[0x10 - 4];
    virtual ~Base1();
    virtual void f1();
};
struct Base2 {
    int x;
    virtual ~Base2();
    virtual void g1();
};
struct Derived : Base1, Base2 {
    virtual ~Derived();
};
Derived::~Derived() {}
