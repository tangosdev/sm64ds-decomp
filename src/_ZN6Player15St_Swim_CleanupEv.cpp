//cpp
extern "C" {
extern int func_ov002_020bd8c0(void*,int);
}

struct Player {
    int St_Swim_Cleanup();
};

int Player::St_Swim_Cleanup()
{
    char* c = (char*)this;
if(*(unsigned char*)(c+0x6f7)!=0){
    *(char*)(c+0x6f7)=0;
    func_ov002_020bd8c0(c,0x33);
  }
  return 1;

}
