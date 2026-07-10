extern void* _ZN5Actor10FindWithIDEj(unsigned int id);
extern void DeathTable_ClearBit(int id);

void _ZN10StarMarker16OnPendingDestroyEv(char* c){
    char* a = (char*)_ZN5Actor10FindWithIDEj(*(unsigned int*)(c + 0x1cc));
    if (a == 0) return;
    if (*(short*)(a + 0xce) >= 0) return;
    DeathTable_ClearBit(*(short*)(c + 0x1d6));
}
