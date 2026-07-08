// Call a helper on self, then if the u16 counter at self+0x5322 is nonzero, decrement
// it. The bl target is a relocation wildcard; the check folds to add+ldrh and the
// laundered RMW pool-loads the offset for the predicated tail.
extern void func_ov006_020f50c0_cb(void *self);
void func_ov006_020f50c0(char *self)
{
    func_ov006_020f50c0_cb(self);
    if (*(unsigned short *)(self + 0x5322))
        *(unsigned short *)(((long long)(int)(self + 0x5322)) & 0xFFFFFFFFFFFFFFFFLL) -= 1;
}
