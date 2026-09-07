//cpp
struct dActor_c {
    void UpdatePosWithOnlySpeed(void *);
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(void *, void *);


extern "C" void func_ov015_02111fb8(void *self, int idx);

extern "C" void func_ov015_02111d98(char *c) {
    _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c((dActor_c *)c, 0);
    int r2 = *(int *)(c + 0x320);
    int r0 = *(int *)(c + 0x5c);
    if (r0 < r2) return;
    *(int *)(c + 0x5c) = r2;
    func_ov015_02111fb8(c, 5);
}
