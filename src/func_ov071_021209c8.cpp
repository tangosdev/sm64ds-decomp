//cpp
extern "C" {
struct BTP_File;
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(char* a, struct BTP_File* f, int b, int c, unsigned int d);
extern void _ZN9Animation8SetFlagsEi(char* a, int f);
extern char* data_ov074_02123038[];
void func_ov071_021209c8(char* c){
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c+0x138, (struct BTP_File*)data_ov074_02123038[1], 0, 0x1000, 0);
  _ZN9Animation8SetFlagsEi(c+0x138, 0x40000000);
  *(int*)(c+0x144)=0x1000;
  *(int*)(c+0x140)=0;
  *(char*)(c+0x214)=0;
}
}
