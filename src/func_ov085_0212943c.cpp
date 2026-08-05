//cpp
struct BCA_File;
struct ModelAnim {
    void SetAnim(BCA_File *, int, int, unsigned int);
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, BCA_File *, int, int, unsigned int);


extern int data_ov085_02130490;

extern "C" void func_ov085_0212943c(void *c) {
    unsigned int flags = 0;
    BCA_File *file = (BCA_File *)(((int *)&data_ov085_02130490)[1]);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((ModelAnim *)((char *)c + 0x108), file, 0, 0x1000, flags);
}
