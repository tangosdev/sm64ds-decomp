// If the byte flag at self+0x4677 is set, decrement it. The read folds to add+ldrb
// (offset > 0xfff), while the laundered RMW pool-loads the offset, matching the ROM's
// predicated ldrbne/subne/strbne tail.
void func_ov006_02104e80(char *self)
{
    if (*(unsigned char *)(self + 0x4677))
        *(unsigned char *)(((long long)(int)(self + 0x4677)) & 0xFFFFFFFFFFFFFFFFLL) -= 1;
}
