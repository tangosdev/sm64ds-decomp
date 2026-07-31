//cpp
extern "C" {
extern char* data_0209cef4;
extern unsigned char data_0209ceec;
}

struct ShadowModel {
    void CleanAll(void);
};

void ShadowModel::CleanAll(void)
{
if(data_0209cef4){
    do{
      char* n=data_0209cef4;
      char* nx=*(char**)(n+0x24);
      if(nx) *(int*)(nx+0x20)=0;
      *(int*)(data_0209cef4+0x24)=0;
      data_0209cef4=nx;
    }while(data_0209cef4);
  }
  data_0209ceec=0;

}
