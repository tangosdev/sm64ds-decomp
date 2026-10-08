extern unsigned char* data_0209fd0c;
extern unsigned char NumStars(void);
extern int func_020138dc(void);
extern unsigned char data_02092824[];
extern unsigned short data_02092808[];
extern void func_02031b84(int x);
extern void func_02031028(int x);
extern unsigned char data_0209fca0[];
extern unsigned char data_02092818[];

void func_0203128c(void){
  unsigned char* g;
  int mode;
  int sub;
  int num;
  int seen;
  int i;
  int q;
  int cnt;
  int n2;
  g = data_0209fd0c;
  mode = (unsigned short)(g[4] | (g[3]<<8));
  switch(mode){
  case 0:
    sub = g[5];
    num = 0;
    seen = num;
    cnt = g[6];
    switch(sub){
    case 0:
      num = (cnt - NumStars()) & 0xff;
      break;
    case 1:
      num = func_020138dc() & 0xff;
      break;
    }
    for(i = 0; i < 3; i++){
      q = num / data_02092808[i];
      if(q != 0 || seen != 0 || i == 2){
        func_02031b84(data_02092824[q]);
        seen = 1;
      }
      num = (num % data_02092808[i]) & 0xff;
    }
    break;
  case 1:
    func_02031028(-1);
    n2 = 0;
    if((int)data_0209fca0[0] > 0){
      unsigned char* pp = data_02092818;
      do {
        func_02031b84(*pp);
        n2++;
        pp++;
      } while(n2 < (int)data_0209fca0[0]);
    }
    break;
  }
}
