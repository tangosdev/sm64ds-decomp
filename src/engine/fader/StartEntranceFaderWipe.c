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

extern FaderWipe* WIPES;
extern "C" void _ZN5Scene9SetFadersEP15FaderBrightness(FaderWipe* f);

extern "C" void StartEntranceFaderWipe(int index) {
    FaderWipe* f = &WIPES[index];
    _ZN5Scene9SetFadersEP15FaderBrightness(f);
    f->SetToEnd();
}
