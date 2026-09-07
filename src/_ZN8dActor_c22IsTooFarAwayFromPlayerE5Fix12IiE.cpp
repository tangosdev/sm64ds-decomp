//cpp
typedef int Fix12;

struct dActor_c {
    char pad_b0[0xb0];
    int field_b0;
};

extern "C" int _ZN8dActor_c13DistToCPlayerEv(dActor_c *self);
extern int data_0209fc68;

extern "C" int _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(dActor_c *self, Fix12 threshold)
{
    if (data_0209fc68 == 0) {
        int b = (self->field_b0 & 8) != 0;
        if (b != 0) {
            if (_ZN8dActor_c13DistToCPlayerEv(self) >= threshold)
                return 1;
        }
    }
    return 0;
}
