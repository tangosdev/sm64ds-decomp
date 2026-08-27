extern void func_ov021_02111fcc();
void func_ov021_02111fe4(char* a, char* b, char* d){
  unsigned int x=*(unsigned short*)(d+0xc)==0xbf;
  if(x){
    func_ov021_02111fcc(b, d);
  }
}
