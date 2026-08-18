//cpp
struct ActorBase {
    int v0();
};

typedef int (ActorBase::*PMF1)();
typedef int (ActorBase::*PMF2)();
typedef void (ActorBase::*PMF3)(int);

extern "C" int _ZN9ActorBase7ProcessEMS_FivEMS_FbvEMS_FvjE(
    ActorBase *self, PMF2 b, PMF1 a, PMF3 c)
{
    int r = (self->*a)();
    int code;
    if (r != 0) {
        r = (self->*b)();
        if (r == -1) code = 3;
        else if (r == 1) code = 2;
        else code = 1;
    } else {
        code = 0;
    }
    (self->*c)(code);
    return r;
}
