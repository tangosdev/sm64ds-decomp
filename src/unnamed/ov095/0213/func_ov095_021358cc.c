#pragma opt_common_subs off
int func_ov095_021358cc(int a, short* pos, short* vel, int target, short thresh, int accel, short mult)
{
    short old = pos[0];
    pos[0] = old + vel[0];
    short now = pos[0];
    if (now == target
        || ((now - target) * (old - target) < 0
            && vel[0] > -thresh && vel[0] < thresh)) {
        pos[0] = target;
        vel[0] = 0;
        return 1;
    }
    if (now >= target)
        accel = (short)-accel;
    if ((short)vel[0] * (short)accel < 0)
        accel = (short)accel * (short)mult;
    vel[0] = vel[0] + accel;
    return 0;
}
