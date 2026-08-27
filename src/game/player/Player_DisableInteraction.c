void Player_DisableInteraction(char *self)
{
    unsigned int tmp;
    char *slot;

    *(unsigned char *)(self + 0x709) = 1;
    slot = (char *)(self + 0x2ec);
    tmp = *(unsigned int *)slot;
    tmp |= 4u;
    *(unsigned int *)slot = tmp;
}