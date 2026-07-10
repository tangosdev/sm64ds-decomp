//cpp
extern "C" {

struct Sub {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m(int);
};

int _ZN10StarMarker6RenderEv(char *thiz)
{
    unsigned int b = *(unsigned char *)(thiz + 0x1db);
    if ((b << 30) >> 31) {
        ((Sub *)(thiz + 0x114))->m(0);
    }
    return 1;
}

}
