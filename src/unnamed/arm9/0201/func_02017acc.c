/* recovered: the fileID RIDES THROUGH r1. The body is push {r4, lr} / mov r4, r0 /
 * bl func_02017e48 / mov r0, r4 -- nothing touches r1, so the fileID the caller
 * left there is what func_02017e0c finally reads. Naming one parameter was
 * byte-exact and wrong; the callers already pass two. Returns self, which is what
 * the closing mov r0, r4 says. */
extern void *func_02017e48(void *self, unsigned int fileID);
void *func_02017acc(void *self, unsigned int fileID) { func_02017e48(self, fileID); return self; }
