//cpp
extern "C" {
extern int data_ov005_020c2efc[];
int _ZN3OAM9RenderSubEP7OamAttriiii(void*,int,int,int,int);

void func_ov005_020c1030(int a0, int x, int y, int val){
    if(val==0){
        _ZN3OAM9RenderSubEP7OamAttriiii((void*)data_ov005_020c2efc[0], x, y, -1, 0);
        return;
    }
    int n=1;
    {
        int q=val;
        while(q>=10){
            q=q/10;
            n++;
        }
    }
    int xx=(n-1)*8;
    if(val<=0) return;
    do{
        int dig=val%10;
        _ZN3OAM9RenderSubEP7OamAttriiii((void*)data_ov005_020c2efc[dig], x+xx, y, -1, 0);
        val=val/10;
        xx-=8;
    }while(val>0);
}
}
