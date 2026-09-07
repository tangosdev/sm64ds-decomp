//cpp
struct BCA_File;
extern "C" {
struct G { int a; struct BCA_File *b; };
extern struct G data_ov065_0211d608;
}
extern "C" int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char*,struct BCA_File*,int,int,unsigned int);
extern "C" short func_ov065_02116588(char *c){
    *(int*)(c+0x3dc)=0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0x300, data_ov065_0211d608.b, 0x40000000, 0x1000, 0);
    *(short*)(c+0x100)=0;
    return 1;
}
