// @symbol _ZN8dM3dGSphC1Ev
/* recovered: named members + shared header
 *
 * Renamed from func_0203ac60 (2026-08-23). RTTI on the vtable it stores
 * (data_020994cc) reads typeinfo "8dM3dGSph", and its only ROM caller is
 * dBgCh_Lin's constructor constructing the bound-sphere MEMBER at +0x64 --
 * a complete-object context, hence C1 rather than C2. Body is a single
 * vptr store; centre and radius are left alone (SetObjAndLine fills them).
 */
extern int data_020994cc[];
void _ZN8dM3dGSphC1Ev(int *p) { p[0] = (int)data_020994cc; }
