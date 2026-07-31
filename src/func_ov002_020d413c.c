extern int Player_ScaleByCharFactor(void *c, int a);
extern int ApproachAngle(short *angle, int targetAngle, int invFactor, int maxDelta, int minDelta);

void func_ov002_020d413c(char *self, int arg)
{
    int v;
    int diff;
    short s;
    if (*(unsigned char *)(self + 0x703)) return;
    v = Player_ScaleByCharFactor(self, 0x1c000);
    diff = *(int *)(self + 0x98);
    if (diff < v) {
        *(short *)(self + 0x75e) = 0;
        *(short *)(self + 0x760) = *(short *)(self + 0x75e);
        return;
    }
    s = (short)((int)(((long long)((diff - v) / 12) * 0x1c71 + 0x800) >> 12));
    {
        int t = s;
        if (t >= 0x2000) t = 0x2000;
        ApproachAngle((short *)(self + 0x760), t, 4, 0x2000, 0x100);
    }
    {
        int t = (short)((arg - *(short *)(self + 0x94)) * 6);
        if (t < -0x1555) t = -0x1555;
        else if (t > 0x1555) t = 0x1555;
        ApproachAngle((short *)(self + 0x75e), t, 8, 0x100, 0x10);
    }
}
