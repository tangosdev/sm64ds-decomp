//cpp
extern "C" {
extern int EndKuppaScript(void);
}

struct Player {
    int St_NoControl_Cleanup();
};

int Player::St_NoControl_Cleanup()
{
    char* c = (char*)this;
int v=0x1000;
  if(*(unsigned char*)(c+0x703)) v=0x3000;
  *(int*)(c+0x80)=v;
  *(int*)(c+0x84)=v;
  *(int*)(c+0x88)=v;
  if(*(unsigned char*)(c+0x70a)==0x11) EndKuppaScript();
  return 1;

}
