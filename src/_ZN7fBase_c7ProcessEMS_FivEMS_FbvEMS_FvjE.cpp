//cpp
struct fBase_c {
    int v0();
};

typedef int (fBase_c::*PMF1)();
typedef int (fBase_c::*PMF2)();
typedef void (fBase_c::*PMF3)(int);

extern "C" int _ZN7fBase_c7ProcessEMS_FivEMS_FbvEMS_FvjE(
    fBase_c *self, PMF2 b, PMF1 a, PMF3 c)
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
