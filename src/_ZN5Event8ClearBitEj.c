extern unsigned int EVENT_FIELD;
int _ZN5Event8ClearBitEj(unsigned int bit)
{
    return EVENT_FIELD &= ~(1 << bit);
}
