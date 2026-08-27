// Call a helper on self, then if the u16 counter at self+0x53e2 is nonzero, decrement
// it. The bl target is a relocation wildcard; the check folds to add+ldrh and the
// laundered RMW pool-loads the offset for the predicated tail.
extern void func_ov006_020f5c40(void *self);
void func_ov006_020f7190(char *self)
{
    func_ov006_020f5c40(self);
    if (*(unsigned short *)(self + 0x53e2))
        *(unsigned short *)(self + 0x53e2) -= 1;
}
