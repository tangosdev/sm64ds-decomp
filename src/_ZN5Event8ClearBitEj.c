extern unsigned int data_0209f34c;
int _ZN5Event8ClearBitEj(unsigned int bit)
{
    return data_0209f34c &= ~(1 << bit);
}
