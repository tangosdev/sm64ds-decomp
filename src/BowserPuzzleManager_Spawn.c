extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN5ActorC2Ev(void *);
extern int _ZTV19BowserPuzzleManager[];
int *BowserPuzzleManager_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(216);
    if (p) { _ZN5ActorC2Ev(p); p[0] = (int)_ZTV19BowserPuzzleManager; }
    return p;
}
