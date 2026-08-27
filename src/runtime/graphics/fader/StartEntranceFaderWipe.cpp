//cpp
#include "types.h"
struct Fader {
    Fix12i currInterp;
    Fix12i speed;
    virtual ~Fader();
    virtual int Advance();
    virtual int SetBackwardTime(unsigned frames);
    virtual int SetForwardTime(unsigned frames);
    virtual int IsAtStart();
    virtual int IsAtEnd();
    virtual void Virtual1C();
    virtual void SetToEnd();
    virtual void SetToStart();
};

/* FaderWipe: Fader(0x0c) + color(2)+unk0e(2) + model(0x50) = 0x60 */
struct FaderWipe : Fader {
    u16 color;
    u16 unk0e;
    u32 model[0x50/4];
    virtual ~FaderWipe();
    virtual int Advance();
    virtual int SetBackwardTime(unsigned frames);
    virtual int SetForwardTime(unsigned frames);
    virtual int IsAtStart();
    virtual int IsAtEnd();
    virtual void Virtual1C();
    virtual void SetToEnd();
    virtual void SetToStart();
};

/* The fader wipe array. Stage::InitResources fills this with
   func_02073470(7, 0x60, 8, &FaderWipeC1, &FaderWipeD1): seven objects of 0x60,
   which is sizeof(FaderWipe). Stage::CleanupResources tears the array down and
   zeroes it. Named data_0209f324 because that is the symbol; every other
   consumer of this address spells it the same way. */
extern FaderWipe* data_0209f324;
extern "C" void _ZN8dScene_c9SetFadersEP15FaderBrightness(FaderWipe* f);

extern "C" void StartEntranceFaderWipe(int index) {
    FaderWipe* f = &data_0209f324[index];
    _ZN8dScene_c9SetFadersEP15FaderBrightness(f);
    f->SetToEnd();
}
