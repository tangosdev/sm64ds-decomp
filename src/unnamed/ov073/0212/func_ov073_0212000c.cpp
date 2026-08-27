//cpp
struct BCA_File;
extern "C" {
struct G { int a; struct BCA_File *b; };
extern struct G data_ov073_02123280;
}
extern "C" int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(char*,struct BCA_File&,int,int,int,unsigned short);
extern "C" short func_ov073_0212000c(char *c){
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x30c, *data_ov073_02123280.b, 4, 0, 0x1000, 0);
    *(short*)(c+0x100)=0x32;
    return 1;
}
