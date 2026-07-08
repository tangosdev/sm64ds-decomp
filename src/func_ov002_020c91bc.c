extern unsigned char PLAYER_TALKING;
extern int NEXT_ACTOR_UPDATE_FLAGS;
extern int _ZN6Player12FinishedAnimEv(char* c);
extern void _ZN6Player11ChangeStateERNS_5StateE(char* c, void* st);
extern int _ZN6Player7ST_SWIME;
extern int _ZN6Player7ST_WALKE;

int func_ov002_020c91bc(char* c)
{
    if (PLAYER_TALKING != 0) return 1;
    *(int *)((int)(c + 0xb0) & 0xFFFFFFFFFFFFFFFF) &= ~0x800000;
    NEXT_ACTOR_UPDATE_FLAGS &= ~0x800000;
    if (_ZN6Player12FinishedAnimEv(c)) {
        if (*(unsigned char*)(c + 0x706) != 0)
            _ZN6Player11ChangeStateERNS_5StateE(c, &_ZN6Player7ST_SWIME);
        else
            _ZN6Player11ChangeStateERNS_5StateE(c, &_ZN6Player7ST_WALKE);
    }
    return 0;
}
