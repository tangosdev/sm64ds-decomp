extern unsigned int _ZN5Event6GetBitEj(unsigned int bit);
extern void _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern int data_ov002_0210d9e0;

int _ZN12EnemySpawner13InitResourcesEv(char* c){
    *(short*)(c + 0xd4) = 0xce;
    *(unsigned char*)(c + 0xdc) = *(short*)(c + 0x90) & 0x1f;
    *(int*)(c + 0xd8) = _ZN5Event6GetBitEj(*(unsigned char*)(c + 0xdc));
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9e0);
    return 1;
}
