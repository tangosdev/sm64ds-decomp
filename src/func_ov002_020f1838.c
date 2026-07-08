extern unsigned int _ZN5Event6GetBitEj(unsigned int bit);
extern void _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern int BOB_OMB_MODEL_PTR;

int func_ov002_020f1838(char* c){
    *(short*)(c + 0xd4) = 0xce;
    *(unsigned char*)(c + 0xdc) = *(short*)(c + 0x90) & 0x1f;
    *(int*)(c + 0xd8) = _ZN5Event6GetBitEj(*(unsigned char*)(c + 0xdc));
    _ZN5Model8LoadFileER13SharedFilePtr(&BOB_OMB_MODEL_PTR);
    return 1;
}
