//cpp
struct Base { 
    virtual void method0(); 
    virtual void method1(); 
};
extern "C" void func_ov075_021160dc(char* c) {
    int count = *(unsigned char*)(c + 0x280);
    int i = 0;
    if (count <= 0) return;
    Base *elem = (Base*)(c + 0x70);
    do {
        elem->method1();
        count = *(unsigned char*)(c + 0x280);
        i++;
        elem = (Base*)((char*)elem + 0x24);
    } while (i < count);
}
