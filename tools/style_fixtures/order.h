#ifndef FORMAT_ORDER_H
#define FORMAT_ORDER_H
#pragma defer_codegen off
class OrderProbe {
public:
    OrderProbe():mSecond(2),mFirst(1){}
    virtual ~OrderProbe();
    virtual int Zeta();
    virtual int Alpha();
private:
    int mSecond;
    short mFirst;
};
#endif
