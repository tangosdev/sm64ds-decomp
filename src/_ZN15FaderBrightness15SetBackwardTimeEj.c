#include "types.h"
extern "C" Fix12i _ZN4cstd4fdivEii(Fix12i a, Fix12i b);

struct Fader {
    Fix12i currInterp;
    Fix12i speed;
    virtual ~Fader();
    virtual int Advance();
    virtual int SetBackwardTime(unsigned frames);
    virtual int SetForwardTime(unsigned frames);
    virtual int IsAtStart();
    virtual int IsAtEnd();
};

struct FaderBrightness : Fader {
    virtual ~FaderBrightness();
    virtual int Advance();
    virtual int SetBackwardTime(unsigned frames);
    virtual int SetForwardTime(unsigned frames);
    virtual int IsAtStart();
    virtual int IsAtEnd();
};

int FaderBrightness::SetBackwardTime(unsigned frames)
{
    if (frames == 0) {
        speed = -(Fix12i)0x1000;
    } else {
        speed = _ZN4cstd4fdivEii(-(Fix12i)0x1000, (Fix12i)(frames << 12));
    }
    return IsAtStart();
}
