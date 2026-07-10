// NONMATCHING: control flow / switch table shape not yet matched; logic draft from annotated disasm.
extern short data_02082214[];
extern void _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(void *pos, int mag);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned id, void *pos);
extern void func_ov015_0211166c(void *self);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *self);
extern int func_ov015_021114f0(void *self);

int _ZN13PoleBillboard8BehaviorEv(char *self)
{
    unsigned char st = *(unsigned char *)(self + 0x397);
    int pos[3];

    if (st > 3)
        goto tail;

    switch (st) {
    case 0:
    case 3:
        break;

    case 1: {
        /* self+0x390 angle index, +0x394 step, +0x396 scale sign */
        unsigned short idx = *(unsigned short *)(self + 0x390);
        signed char sc = *(signed char *)(self + 0x396);
        short step = *(short *)(self + 0x394);
        short s = *(short *)((char *)data_02082214 + ((idx >> 4) << 2));
        int prod = (int)sc * (int)s;
        int r = (int)(((long long)prod * step + 0x800) >> 12);
        *(short *)(self + 0x8c) = (short)r;
        if (step <= 0) {
            *(short *)(self + 0x8c) = 0;
            *(unsigned short *)(self + 0x390) = 0;
            *(unsigned char *)(self + 0x397) = 0;
        } else {
            short *ps = (short *)(self + 0x394);
            *ps = (short)(*ps - 8);
        }
        {
            short *pa = (short *)(self + 0x390);
            *pa = (short)(*pa + 0x400);
        }
        break;
    }

    case 2: {
        /* dir at 0x396; rolling angle at pooled 0x392; pitch at 0x8c */
        signed char dir = *(signed char *)(self + 0x396);
        short *roll = (short *)(self + 0x392);
        short *pitch = (short *)(self + 0x8c);
        short dstep = *(short *)(self + 0x392); /* same 0x392 used as delta? */
        /* disasm: *roll += (dir << 23) >> 16  i.e. dir * 0x80 as int16 */
        *roll = (short)(*roll + ((int)dir << 7));
        /* then *pitch += *(short*)(self+0x392)  -- re-read after update */
        dstep = *(short *)(self + 0x392);
        *pitch = (short)(*pitch + dstep);

        if (dir == -1) {
            /* swinging negative: hit -0x4000 */
            if (*pitch < -0x4000) {
                *pitch = -0x4000;
                *(short *)(self + 0x392) = 0;
                *(unsigned char *)(self + 0x397) =
                    (unsigned char)(*(unsigned char *)(self + 0x397) + 1);
                pos[0] = *(int *)(self + 0x5c);
                pos[1] = *(int *)(self + 0x60);
                pos[2] = *(int *)(self + 0x64);
                _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(pos, 0x2000000);
                _ZN5Sound9PlayBank3EjRK7Vector3(0x44, self + 0x74);
            }
        } else {
            /* dir != -1: climb 0x38c and y */
            int *p38c = (int *)(self + 0x38c);
            *p38c = *p38c + 0x2000;
            *(int *)(self + 0x60) = *(int *)(self + 0x60) + *p38c;
            {
                int lim = *(int *)(self + 0x388) + 0x46000;
                if (*(int *)(self + 0x60) > lim)
                    *(int *)(self + 0x60) = lim;
            }
            if (*pitch > 0x4000) {
                *pitch = 0x4000;
                *(short *)(self + 0x392) = 0;
                *(unsigned char *)(self + 0x397) =
                    (unsigned char)(*(unsigned char *)(self + 0x397) + 1);
                pos[0] = *(int *)(self + 0x5c);
                pos[1] = *(int *)(self + 0x60);
                pos[2] = *(int *)(self + 0x64);
                _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(pos, 0x2000000);
                _ZN5Sound9PlayBank3EjRK7Vector3(0x44, self + 0x74);
            }
        }
        break;
    }
    }

tail:
    func_ov015_0211166c(self);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(self, 0, 0) != 0)
        _ZN8Platform19UpdateClsnPosAndRotEv(self);
    func_ov015_021114f0(self);
    return 1;
}
