//cpp
struct Player {
    void RegisterEggCoinCount(unsigned int count, bool b2, bool b3);
};

void Player::RegisterEggCoinCount(unsigned int count, bool b2, bool b3)
{
    char* self = (char*)this;
*(unsigned char *)(self + 0x704) = (count & 0xf) << 2;
	if (b3)
		*(unsigned char *)(((long long)(int)(self + 0x704)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x40;
	if (b2)
		*(unsigned char *)(((long long)(int)(self + 0x704)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x80;

}
