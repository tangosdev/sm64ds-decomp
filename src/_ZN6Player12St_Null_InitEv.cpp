//cpp
extern "C" {
extern int data_ov006_021405bc;
extern char* data_ov006_02140554;
void func_ov006_020c8f20(void*);
void func_ov006_020ce46c(void*,int);
void func_ov006_020c8a64(void);
}

struct Player {
    void St_Null_Init(void);
};

void Player::St_Null_Init(void)
{
int i=0;
  if(data_ov006_021405bc>0){
    int off=0;
    do{
      func_ov006_020c8f20(data_ov006_02140554+off);
      func_ov006_020ce46c(data_ov006_02140554+off,i);
      i++;
      off+=0xdc;
    }while(i<data_ov006_021405bc);
  }
  func_ov006_020c8a64();

}
