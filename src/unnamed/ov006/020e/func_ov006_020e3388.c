extern void func_ov004_020adb1c(int self);

#pragma opt_strength_reduction off
void func_ov006_020e3388(int self){
  int i;
  char* c=(char*)self;
  char* r=c;
  for(i=0;i<5;i++){
    *(int*)(r+0x4660)=0;
    *(int*)(r+0x4664)=0;
    *(int*)(r+0x4668)=0;
    *(int*)(r+0x466c)=0;
    *(int*)(r+0x4670)=0;
    *(int*)(r+0x467c)=0;
    *(short*)(r+0x4682)=0;
    *(short*)(r+0x4684)=0;
    *(short*)(r+0x4686)=0;
    *(unsigned char*)(r+0x4688)=0;
    *(unsigned char*)(r+0x4689)=0;
    *(unsigned char*)(r+0x468a)=0;
    r+=0x2c;
  }
  for(i=0;i<5;i++){
    char* q=c+i*0x10;
    *(int*)(q+0x473c)=0;
    *(int*)(q+0x4740)=0;
    *(short*)(q+0x4744)=0;
    *(short*)(q+0x4746)=0;
    *(unsigned char*)(q+0x4748)=0;
    *(unsigned char*)(q+0x4749)=0;
  }
  *(short*)(c+0x4ee0)=0;
  *(unsigned char*)(c+0x4ee6)=0;
  *(int*)(c+0x4eb0)=0;
  *(int*)(c+0x4eb4)=0;
  *(int*)(c+0x4ec0)=0;
  *(int*)(c+0x4ec4)=0;
  *(int*)(c+0x4ec8)=0;
  *(short*)(c+0x4ede)=0;
  *(unsigned char*)(c+0x4ee4)=0;
  *(unsigned char*)(c+0x4ee5)=0;
  *(unsigned char*)(c+0x4ee7)=0;
  *(unsigned char*)(c+0x4ee8)=0;
  *(short*)(c+0x4ee2)=0;
  *(short*)(c+0x4edc)=0;
  *(unsigned char*)(c+0x4ee9)=0;
  func_ov004_020adb1c(0);
}
