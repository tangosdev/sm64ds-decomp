//cpp
#include "dScene_c.h"
#include "types.h"
struct dFader_c {
    Fix12i currInterp;
    Fix12i speed;
    virtual ~dFader_c();
    virtual int Advance();
    virtual int SetBackwardTime(unsigned frames);
    virtual int SetForwardTime(unsigned frames);
    virtual int IsAtStart();
    virtual int IsAtEnd();
    virtual void Virtual1C();
    virtual void SetToEnd();
    virtual void SetToStart();
};

/* dFdWipe_c: dFader_c(0x0c) + color(2)+unk0e(2) + model(0x50) = 0x60 */
struct dFdWipe_c : dFader_c {
    u16 color;
    u16 unk0e;
    u32 model[0x50/4];
    virtual ~dFdWipe_c();
    virtual int Advance();
    virtual int SetBackwardTime(unsigned frames);
    virtual int SetForwardTime(unsigned frames);
    virtual int IsAtStart();
    virtual int IsAtEnd();
    virtual void Virtual1C();
    virtual void SetToEnd();
    virtual void SetToStart();
};

/* The fader wipe array. dScStage_c::InitResources fills this with
   func_02073470(7, 0x60, 8, &dFdWipe_cC1, &dFdWipe_cD1): seven objects of 0x60,
   which is sizeof(dFdWipe_c). dScStage_c::CleanupResources tears the array down and
   zeroes it. Named data_0209f324 because that is the symbol; every other
   consumer of this address spells it the same way. */
extern dFdWipe_c* data_0209f324;

extern "C" void StartEntranceFaderWipe(int index) {
    dFdWipe_c* f = &data_0209f324[index];
    dScene_c::SetFaders((dFdBrightness_c *)f);
    f->SetToEnd();
}
