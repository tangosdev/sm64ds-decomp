/* recovered: r1 rides through. The body is push {r4, lr} / mov r4, r0 /
 * bl func_02017e0c / mov r0, r4, so the fileID its callers leave in r1 reaches
 * func_02017e0c(self, fileID) untouched; the one-argument call was byte-exact
 * and wrong. Returns self, which is what makes SharedFilePtr::Construct's
 * returned reference free. */
extern void func_02017e0c(void *self, unsigned int fileID);
void *func_02017e48(void *self, unsigned int fileID) { func_02017e0c(self, fileID); return self; }
