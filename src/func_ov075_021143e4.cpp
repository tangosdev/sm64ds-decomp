//cpp
extern "C" {
extern int RandomIntInternal(void*);
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void*,void*,int,int,int,unsigned short);
extern void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void*,void*);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void*,void*,int,int,unsigned int);
extern int _ZNK9Animation13GetFrameCountEv(void*);
extern long long func_01ffadf0(unsigned int,int);
extern int data_ov075_0211d380[];
extern int RNG_STATE[];
extern unsigned char data_ov075_0211b524[];
extern unsigned char data_0209b2f0[];
extern int* data_ov075_0211d3ac[];
extern int* data_ov075_0211d40c[];
extern int* data_ov075_0211c688[];
extern int* data_ov075_0211d3dc[];
extern int* data_ov075_0211d3c4[];
void func_ov075_021143e4(char*c){
  void*r4;
  if(data_ov075_0211d380[0]<0){
    data_ov075_0211d380[0]=data_ov075_0211b524[((unsigned int)RandomIntInternal(RNG_STATE)>>0x10)%6];
  }
  if(data_0209b2f0[*(unsigned char*)(c+0x152)]!=0){
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c,(void*)data_ov075_0211d3ac[1],0,0,0x1000,0);
    r4=(void*)data_ov075_0211d40c[1];
    *(int*)(c+0x110)=8;
  } else {
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c,(void*)((int*)data_ov075_0211c688[data_ov075_0211d380[0]&3])[1],0,0,0x1000,0);
    r4=(void*)data_ov075_0211d3dc[1];
    *(int*)(c+0x110)=9;
    data_ov075_0211d380[0]>>=2;
  }
  _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File((void*)data_ov075_0211d3c4[1],r4);
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c+0xd4,r4,0,0x1000,0);
  {
    unsigned int rv=(unsigned int)RandomIntInternal(RNG_STATE);
    int fc=_ZNK9Animation13GetFrameCountEv(c+0x50);
    long long v=func_01ffadf0(rv>>0x10,fc);
    *(int*)(c+0xdc)=((unsigned)((int)(v>>32)<<0x10))>>4;
  }
}
}
