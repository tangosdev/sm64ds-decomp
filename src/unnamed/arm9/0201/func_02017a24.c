/* recovered: the fileID RIDES THROUGH r1. The body is push {r4, lr} / mov r4, r0 /
 * bl func_02017ae4 / mov r0, r4 -- nothing touches r1, so the fileID the caller
 * left there is what the chain finally reads. Byte-identical with both spelled. */
extern void *func_02017ae4(void *self, unsigned int fileID);
void *func_02017a24(void *self, unsigned int fileID) { func_02017ae4(self, fileID); return self; }
