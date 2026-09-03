extern void _ZN13dScMgMemory_c15SetupDifficultyEv(void* c);
extern void _ZN13dScMgMemory_c12ShuffleCardsEv(void* c);
extern void FreeGfxSlotsById(int x);
extern void func_ov006_020c1764(void* p);

void _ZN13dScMgMemory_c10StateSetupEv(char* c){
    _ZN13dScMgMemory_c15SetupDifficultyEv(c);
    _ZN13dScMgMemory_c12ShuffleCardsEv(c);
    FreeGfxSlotsById(0x1d);
    func_ov006_020c1764(c + 0x4f38);
    *(int*)(c + 0x5314) = 1;
}
