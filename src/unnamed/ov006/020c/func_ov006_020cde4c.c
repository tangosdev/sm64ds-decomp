void func_ov006_020cde4c(char*c){
int v=*(int*)(c+0xc);
if(v>0x80000){*(int*)(c+0x30)=-0x1000;return;}
if(v<-0x60000)*(int*)(c+0x30)=0x1000;
}
