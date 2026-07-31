//cpp
struct Actor {
    int GetSubtraction(short a, short b);
};

int Actor::GetSubtraction(short a, short b)
{
    void* self = (void*)this;
int d=(short)(b-a);
if(d==-0x8000) d=-0x7fff;
if(d<0) d=(short)(-d);
return d;

}
