//cpp
extern "C" {
extern void func_ov007_020c43bc(void*, int);
extern char* data_ov007_0210342c;
extern unsigned char data_ov007_020d7dc8[];
extern unsigned char data_ov007_020d7dd0[];
extern unsigned char data_ov007_020d7dd1[];
extern unsigned char data_ov007_020d7ddc[];
extern unsigned char data_ov007_020d7ddd[];
extern unsigned char data_ov007_020d7dc0[];
extern short data_ov007_020d7de8[];
extern short data_ov007_020d7dea[];
void func_ov007_020b3d30(char* thiz, int idx){
  *( *(int**)(thiz+0x5c) )=*(int*)((char*)data_ov007_0210342c + 0x78 + data_ov007_020d7dc8[idx]*4);
  *(short*)(thiz+0x70)=data_ov007_020d7dd0[idx*2];
  *(short*)(thiz+0x72)=data_ov007_020d7dd1[idx*2];
  *(short*)(thiz+0xc)=data_ov007_020d7ddc[idx*2];
  *(short*)(thiz+0xe)=data_ov007_020d7ddd[idx*2];
  *(char*)(thiz+0x7d)=data_ov007_020d7dc0[idx];
  *(short*)(thiz+0x7e)=*(short*)((char*)data_ov007_020d7de8 + idx*4);
  *(short*)(thiz+0x80)=*(short*)((char*)data_ov007_020d7dea + idx*4);
  if(idx==1) *(int*)(thiz+0x10)=-0x10c0;
  else *(int*)(thiz+0x10)=(int)0xffffef38;
  func_ov007_020c43bc(thiz, *(int*)((char*)data_ov007_0210342c+0x34));
}
}
