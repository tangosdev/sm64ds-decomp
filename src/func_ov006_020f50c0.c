// Call a helper on self, then if the u16 counter at self+0x5322 is nonzero, decrement
// it. The bl target is a relocation wildcard; the check folds to add+ldrh and the
// laundered RMW pool-loads the offset for the predicated tail.
extern void _ZN13dScMgMemory_c11UpdateCardsEv(void *self);
void _ZN13dScMgMemory_c9StateExitEv(char *self)
{
    _ZN13dScMgMemory_c11UpdateCardsEv(self);
    if (*(unsigned short *)(self + 0x5322))
        *(unsigned short *)(self + 0x5322) -= 1;
}
