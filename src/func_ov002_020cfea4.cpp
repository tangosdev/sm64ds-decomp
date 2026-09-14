//cpp
// @symbol func_ov002_020cfea4
/* recovered: Player ground/edge probe used while carrying or standing on
 * another actor.
 *
 * With no timer at self+0x6a6 and bit 1 of self+0x6e9 set, the player first
 * asks its dBgCh_Actr (self+0x380) whether it stands on, or presses against, a
 * collider that belongs to another actor: standing on one that faces more
 * than 90 degrees away, or touching one as a wall, ends the probe with 1.
 * Otherwise a dBgCh_Gnd is built 32 units ahead of the player along its
 * facing angle (sin/cos from data_02082214, 1.19.12 units shifted up by 5)
 * and 0x14000 above it; ground within 0x50000 below the player's own height
 * ends the probe with 1. The remaining two checks hand func_ov002_020cfd84 a
 * pair of points 0xa000 below that probe: the point itself, and the point
 * moved 60 units to the player's right (angle + 0x4000), then 60 units to
 * its left. pts[] holds the eight Vector3 slots of the ROM's frame: pts[1]
 * is the ground probe, pts[2]/pts[3] the base pair, pts[4]/pts[5] and
 * pts[6]/pts[7] the copies handed to each call.
 *
 * Two spellings are load-bearing for the register colouring of the tail
 * (the 6bs/6ct receiver lever): the sideways offsets are kept in `dx`/`dz`,
 * the locals that already cross the first func_ov002_020cfd84 guard, as the
 * finished products tbl[t] * 60 (the ROM keeps exactly those products live
 * across the call); and the lowered y is computed before pts[1].x/.z are
 * reloaded, with the second lowered y computed after the two products. With
 * both in place 2004/b56 gives pts[1].x ip, pts[1].z r3, the lowered y lr,
 * the two sums r8/r7 and the second lowered y sb, which is the ROM's tail.
 */
struct Vector3 { int x, y, z; };
extern "C" {
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *self);
extern void *_ZNK10dBgCh_Actr14GetFloorResultEv(void *self);
extern int _ZNK5dBgPi9GetClsnIDEv(void *self);
extern void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
extern int AngleDiff(int a, int b);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void *self);
extern void *_ZNK10dBgCh_Actr13GetWallResultEv(void *self);
extern void _ZN9dBgCh_GndC1Ev(void *self);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(void *self, const Vector3 *pos, void *actor);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(void *self);
extern void _ZN9dBgCh_GndD1Ev(void *self);
extern int func_ov002_020cfd84(void *actor, const Vector3 *a, const Vector3 *b);
extern short data_02082214[];
}
extern "C" int func_ov002_020cfea4(char *self)
{
    Vector3 pts[8];
    char rg[0x54];
    void *fr;
    void *other;
    int idx, dx, dz;
    if (*(unsigned short *)(self + 0x6a6) == 0 && (*(unsigned char *)(self + 0x6e9) & 2) != 0) {
        if (_ZNK10dBgCh_Actr10IsOnGroundEv(self + 0x380) != 0) {
            fr = _ZNK10dBgCh_Actr14GetFloorResultEv(self + 0x380);
            if (_ZNK5dBgPi9GetClsnIDEv(fr) != -1) {
                other = _ZN8dActor_c10FindWithIDEj((unsigned int)_ZNK5dBgPi9GetClsnIDEv(fr));
                if (other != 0) {
                    if (AngleDiff(*(short *)(self + 0x8e), *(short *)((char *)other + 0x8e)) >= 0x4000)
                        return 1;
                }
            }
        }
        if (_ZNK10dBgCh_Actr8IsOnWallEv(self + 0x380) != 0) {
            fr = _ZNK10dBgCh_Actr13GetWallResultEv(self + 0x380);
            if (_ZNK5dBgPi9GetClsnIDEv(fr) != -1) {
                other = _ZN8dActor_c10FindWithIDEj((unsigned int)_ZNK5dBgPi9GetClsnIDEv(fr));
                if (other != 0)
                    return 1;
            }
        }
    }
    _ZN9dBgCh_GndC1Ev(rg);
    {
        int y;
        short *tbl;
        int ang = *(short *)(self + 0x8e);
        tbl = data_02082214;
        y = *(int *)(self + 0x60);
        idx = (unsigned short)(short)(ang + 0x8000) >> 4;
        dz = tbl[(idx << 1) + 1];
        dx = tbl[idx << 1];
        {
            int z = *(int *)(self + 0x64);
            int x = *(int *)(self + 0x5c);
            int pz = z + (dz << 5);
            int px = x + (dx << 5);
            int py = y + 0x14000;
            pts[1].x = px;
            pts[1].y = py;
            pts[1].z = pz;
        }
    }
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(rg, &pts[1], self);
    if (_ZN9dBgCh_Gnd10DetectClsnEv(rg) != 0) {
        if (*(int *)(rg + 0x44) >= *(int *)(self + 0x60) - 0x50000) {
            _ZN9dBgCh_GndD1Ev(rg);
            return 1;
        }
    }
    {
        int y = *(int *)(self + 0x60);
        int yl = y - 0xa000;
        int px = pts[1].x;
        int pz = pts[1].z;
        pts[2].x = px;
        pts[2].y = yl;
        pts[2].z = pz;
        {
            short *tbl;
            int y2;
            int ang = *(short *)(self + 0x8e);
            y2 = *(int *)(self + 0x60);
            tbl = data_02082214;
            idx = (unsigned short)(short)(ang + 0x4000) >> 4;
            {
                int t = idx << 1;
                int yl2;
                dx = tbl[t] * 60;
                dz = tbl[t + 1] * 60;
                yl2 = y2 - 0xa000;
                pts[3].x = px + dx;
                pts[3].y = yl2;
                pts[3].z = pz + dz;
            }
        }
    }
    pts[4] = pts[2];
    pts[5] = pts[3];
    if (func_ov002_020cfd84(self, &pts[4], &pts[5]) != 0) {
        _ZN9dBgCh_GndD1Ev(rg);
        return 1;
    }
    pts[3].x = pts[1].x - dx;
    pts[3].z = pts[1].z - dz;
    pts[6] = pts[2];
    pts[7] = pts[3];
    if (func_ov002_020cfd84(self, &pts[6], &pts[7]) != 0) {
        _ZN9dBgCh_GndD1Ev(rg);
        return 1;
    }
    _ZN9dBgCh_GndD1Ev(rg);
    return 0;
}
