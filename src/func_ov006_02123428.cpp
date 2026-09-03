//cpp
#pragma opt_loop_invariants off

extern "C" {
extern int RandomIntInternal(int *seed);
extern int data_0209e650;
extern int data_ov006_0212f328;

extern int func_ov006_021238d0(int kind);
extern unsigned short func_ov006_0212382c(int column, int side);
extern int func_ov006_021237c8(int kind);
extern int func_ov006_0212373c(int kind);
extern int _Z14ApproachLinearRiii(int *value, int target, int step);
extern void func_0203cc28(int *vector, int angle);
extern char *func_ov006_020cefa4(int resource, int *up, int variant, int face);
}

struct Vec3i {
    int x;
    int y;
    int z;
};

extern "C" void func_ov006_02123428(char *scene)
{
    int occupancy;
    int column;
    int kind;
    int kinds[7];
    short faces[7];
    Vec3i up;
    int i;
    int j;
    int variant;

    occupancy = 0;
    for (i = 0; i < 5; i++) {
        do {
            kind = (int)((((unsigned int)RandomIntInternal(&data_0209e650)
                           & 0x7fffffff) >> 0x13) * 11) >> 12;
        } while ((kind == 7 || kind == 10)
                 && (int)(((unsigned int)RandomIntInternal(&data_0209e650)
                            & 0x7fffffff) >> 0x13) > 0x400);

        for (;;) {
            for (j = 0; j < i; j++) {
                if (kind == kinds[j]) {
                    kind = (int)((((unsigned int)RandomIntInternal(&data_0209e650)
                                   & 0x7fffffff) >> 0x13) * 11) >> 12;
                    j = -1;
                }
            }

            column = func_ov006_021238d0(kind);
            {
                int shift = column * 2;
                int rightBit = 2 << shift;
                int leftBit = 1 << shift;
                int savedOccupancy;

                if ((occupancy & rightBit) != 0 &&
                    (occupancy & leftBit) != 0) {
                    do {
                        if (_Z14ApproachLinearRiii(&kind, 11, 1) != 0)
                            kind = 0;
                    } while ((kind == 7 || kind == 10)
                             && (int)(((unsigned int)RandomIntInternal(&data_0209e650)
                                       & 0x7fffffff) >> 0x13) > 0x800);
                    continue;
                }

                if ((savedOccupancy = (int)(volatile int)occupancy,
                     (savedOccupancy & rightBit) != 0)) {
                    kinds[i] = kind;
                    faces[i] = 8;
                    occupancy |= func_ov006_0212382c(column, 1);
                } else if ((savedOccupancy & leftBit) != 0) {
                    kinds[i] = kind;
                    faces[i] = (short)func_ov006_021237c8(kind);
                    occupancy |= func_ov006_0212382c(column, 0);
                } else {
                    kinds[i] = kind;
                    faces[i] = (short)func_ov006_0212373c(kind);
                    switch (faces[i]) {
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                        occupancy |= func_ov006_0212382c(column, 0);
                        break;
                    default:
                        occupancy |= func_ov006_0212382c(column, 1);
                        break;
                    }
                }
            }
            break;
        }

        up.x = 0;
        up.y = 0x1000;
        up.z = 0;
        if (kind >= 5 ||
            ((kind & 1) != 0 && (faces[i] == 8 || faces[i] == 5))) {
            func_0203cc28((int *)&up, 0x4000);
        }

        variant = 1;
        if (faces[i] == 6)
            variant = 2;
        func_ov006_020cefa4((int)&data_ov006_0212f328 + kinds[i] * 0xc,
                            (int *)&up, variant, faces[i]);
    }
}
