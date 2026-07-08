//cpp
typedef int Fix12;

struct Actor {
    char pad_b0[0xb0];
    int field_b0;
};

extern "C" int _ZN5Actor13DistToCPlayerEv(Actor *self);
extern int DP_STATE;

extern "C" int _ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(Actor *self, Fix12 threshold)
{
    if (DP_STATE == 0) {
        int b = (self->field_b0 & 8) != 0;
        if (b != 0) {
            if (_ZN5Actor13DistToCPlayerEv(self) >= threshold)
                return 1;
        }
    }
    return 0;
}
