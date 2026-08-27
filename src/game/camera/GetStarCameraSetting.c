extern unsigned int data_02092134;
unsigned char GetStarCameraSetting(int idx)
{
    return (data_02092134 >> (idx * 4)) & 0xf;
}
