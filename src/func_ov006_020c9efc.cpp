//cpp
typedef long long s64;
extern void func_ov006_020c8c78(int a, int b);
extern int ApproachLinear(int &r, int b, int c);
extern void func_ov006_020c9e7c(void* p);
namespace Sound { void PlayBank2_2D(unsigned int); }
extern int data_ov006_02140598;
extern int data_ov006_0213b0f0;

struct Obj {
  virtual void v0();
  virtual void v1();
  virtual void v2();
  virtual void v3();
  virtual void v4();
};

extern "C" void func_ov006_020c9efc(char* c){
  *(short*)(((int)c+0x6c) & 0xFFFFFFFFFFFFFFFFLL) = *(short*)(((int)c+0x6c) & 0xFFFFFFFFFFFFFFFFLL) - 1;
  if(*(short*)(c+0x6c)==0){
    func_ov006_020c8c78(*(short*)(c+0x56),0xc0);
    data_ov006_02140598=data_ov006_02140598-1;
    ApproachLinear(data_ov006_0213b0f0,0,1);
    Sound::PlayBank2_2D(0x130);
    ((Obj*)c)->v4();
    *(int*)(c+0x64)=0;
    func_ov006_020c9e7c(c);
  } else {
    int v=*(int*)(c+0x3c);
    *(int*)(c+0x3c)=(int)(((s64)v*0xc00+0x800)>>12);
    int x=*(int*)(c+0x24);
    if(x<-0x6c000) *(int*)(c+0x24)=-0x6c000;
    else if(x>0x6c000) *(int*)(c+0x24)=0x6c000;
  }
}
