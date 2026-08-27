//cpp
extern "C" {
struct BCA{int pad; void* f; };
extern BCA* data_ov034_02113860[];
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
void func_ov034_02111a0c(char* c){
  int i;
  char* p=c+0x110;
  for(i=0;i<5;i++,p+=0x64){
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(p, data_ov034_02113860[i]->f, 0, 0x1000, 0);
  }
}
}
