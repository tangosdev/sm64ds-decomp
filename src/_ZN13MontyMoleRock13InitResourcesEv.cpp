//cpp
extern "C" {
int _ZN5Model8LoadFileER13SharedFilePtr(void*);
int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*,int,int,int);
int _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void*,void*,int,int,unsigned,unsigned);
int _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void*,void*,int,int,int,int);
extern int data_ov080_021283c8[];
int _ZN13MontyMoleRock13InitResourcesEv(char* c){
  int m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov080_021283c8);
  if(_ZN9ModelBase7SetFileEP8BMD_Fileii(c+0x110, m, 1, -1) == 0) return 0;
  _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c+0x160, c, 0x1e000, 0x1e000, 0x200004, 0);
  *(unsigned char*)(c+0x350) = *(int*)(c+8) & 1;
  _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c+0x194, c, 0x1e000, 0x1e000, 0, 0);
  *(int*)(c+0x9c) = -0x2000;
  *(int*)(c+0xa0) = -0x3c000;
  if(*(unsigned char*)(c+0x350) == 0){
    *(int*)(c+0x80) = 0x1000;
    *(int*)(c+0x84) = 0x1000;
    *(int*)(c+0x88) = 0x1000;
  } else {
    *(int*)(c+0x80) = 0x800;
    *(int*)(c+0x84) = 0x800;
    *(int*)(c+0x88) = 0x800;
  }
  return 1;
}
}
