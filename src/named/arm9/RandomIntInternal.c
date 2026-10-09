int RandomIntInternal(int* seed) {
    *seed = *seed * 0x19660d + 0x3c6ef35f;
    return *seed;
}
