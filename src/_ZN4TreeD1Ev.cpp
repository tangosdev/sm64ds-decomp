//cpp
extern "C" {
extern int func_0207328c(void*, int, int, void*);
extern void _ZN5ActorD1Ev(void*);
extern void *_ZTV4Tree;
extern int _ZN5ModelD1Ev();
int _ZN4TreeD1Ev(char* c){
    *(void**)c = (void*)&_ZTV4Tree;
    func_0207328c((char*)c+0xd4, 5, 0x50, (void*)&_ZN5ModelD1Ev);
    _ZN5ActorD1Ev(c);
    return (int)c;
}
}
