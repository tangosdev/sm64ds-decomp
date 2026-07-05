typedef short s16;
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern char data_ov036_02114070[];

int func_ov036_02111854(char* c) {
  void* mdl = _ZN5Model8LoadFileER13SharedFilePtr(data_ov036_02114070);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, mdl, 1, -1);
  *(unsigned char*)(c+0x118) = *(unsigned int*)(c+8) & 1;
  *(unsigned char*)(c+0x119) = (*(unsigned int*)(c+8) >> 8) & 1;
  if (*(unsigned char*)(c+0x118) != 0) {
    *(s16 *)(((int)c + 0x8c) & 0xFFFFFFFFFFFFFFFF) += 0x2400;
    *(s16 *)(((int)c + 0x8e) & 0xFFFFFFFFFFFFFFFF) += 0x8000;
    *(s16 *)(((int)c + 0x90) & 0xFFFFFFFFFFFFFFFF) += 0x8000;
  }
  *(s16*)(c+0x110) = *(s16*)(c+0x8c);
  *(s16*)(c+0x112) = *(s16*)(c+0x8e);
  *(s16*)(c+0x114) = *(s16*)(c+0x90);
  *(s16*)(c+0x116) = 0;
  return 1;
}
