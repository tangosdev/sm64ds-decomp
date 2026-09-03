extern void _ZN13dScMgMemory_c11UpdateCardsEv(void* c);
extern void _ZN13dScMgMemory_c9JudgePairEv(void* c);
extern void _ZN13dScMgMemory_c13CheckFinishedEv(void* c);
void _ZN13dScMgMemory_c10StateJudgeEv(void* c) {
    _ZN13dScMgMemory_c11UpdateCardsEv(c);
    _ZN13dScMgMemory_c9JudgePairEv(c);
    _ZN13dScMgMemory_c13CheckFinishedEv(c);
}
