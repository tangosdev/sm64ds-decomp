    extern int Vec3_Dist(void* a, void* b);
    extern int func_ov062_02115f84(char* c);
    extern int Chuckya_ChangeState(void* c, void* p);
    extern void ApproachAngle(void* p, int target, int a, int b, int limit);
    extern char* _ZN5Actor13ClosestPlayerEv(char* self);
    extern int _ZN5Sound8PlayLongEjjjRK7Vector3s(
        unsigned int a, unsigned int b, unsigned int c, void* v, unsigned int e);

    extern char data_ov062_0211df00[];
    extern char data_ov062_0211de70[];
    extern char data_ov062_0211de90[];
    extern char data_ov062_0211dee0[];

#pragma opt_dead_assignments off
    int func_ov062_02116a08(char* c)
    {
        struct { int vd0[3]; int vd1[3]; } L;
        int r;
        char* pl;
        int* pos;
        char* p0;
        char* p1;

        p0 = c + 0x5c;
        p1 = c + 0x3c0;

        L.vd0[0] = 0;
        L.vd0[1] = 0;
        L.vd0[2] = 0;

        Vec3_Dist(p0, p1);

        r = func_ov062_02115f84(c);
        if (r != 0 || *(unsigned char *)(c + 0x3e4) == 1) {
            if (r != 2) {
                Chuckya_ChangeState(c, data_ov062_0211df00);
            } else {
                Chuckya_ChangeState(c, data_ov062_0211de70);
            }
            return 1;
        }

        ApproachAngle(c + 0x94, *(short *)(c + 0x3f4), 0xa, 0x200, 0x100);

        *(int *)(c + 0x98) = 0xa000;
        pl = _ZN5Actor13ClosestPlayerEv(c);

        *(int *)(c + 0x3ec) =
            _ZN5Sound8PlayLongEjjjRK7Vector3s(
                *(unsigned int *)(c + 0x3ec), 3, 0x18a, c + 0x74, 0);

        if (pl != 0 && *(unsigned short *)(c + 0x3e8) == 0) {
            pos = (int *)(((int)pl + 0x5c));
            L.vd1[0] = pos[0];
            L.vd1[1] = pos[1];
            L.vd1[2] = pos[2];

            if (Vec3_Dist(c + 0x3c0, L.vd1) < 0x3e8000) {
                *(int *)(c + 0x98) = 0;
                Chuckya_ChangeState(c, data_ov062_0211de90);
            }
            return 1;
        }

        if (*(unsigned short *)(c + 0x100) == 0)
            Chuckya_ChangeState(c, data_ov062_0211dee0);

        return 1;
    }
