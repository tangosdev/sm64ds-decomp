/* func_ov006_020cedf0 at 0x020cedf0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */
extern int data_ov006_02140838;
extern char* data_ov006_0214082c;
extern void func_ov006_020cea2c(char *c);

void func_ov006_020cedf0(void){
  int i=0;
  int off;
  if(data_ov006_02140838>0){
    off=0;
    do{
      char *p = data_ov006_0214082c + off;
      if(*(int*)(p+0x84)!=0){
        func_ov006_020cea2c(p);
      }
      i++;
      off+=0x1d0;
    }while(i<data_ov006_02140838);
  }
}
