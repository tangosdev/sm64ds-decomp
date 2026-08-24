// @symbol _ZN8dM3dGSphC2Ev
/* recovered: named members + shared header
 *
 * Renamed from func_0203ac70 (2026-08-23). Same single vptr store as
 * _ZN8dM3dGSphC1Ev (0x0203ac60), one page over -- the C2/C1 pair the
 * dtor-variant audit predicted ("the adjacent 0x10-size pair is most likely
 * one class's C2/C1"). The caller scan settles which is which, mirroring
 * section 4/4b: this function's ONLY ROM caller is dBgCh_SphCrr's constructor
 * constructing the dM3dGSph BASE subobject at +0x38 (base-subobject context,
 * hence C2), while C1's only caller constructs a complete-object MEMBER in
 * dBgCh_Lin. Nothing anywhere constructs a standalone dM3dGSph.
 */
extern int _ZTV8dM3dGSph[];
void _ZN8dM3dGSphC2Ev(int *p) { p[0] = (int)_ZTV8dM3dGSph; }
