int func_0205d304(unsigned char* s, int n)
{
    int result = 0;
    if (n <= 3) {
        int i = 0;
        if (n > 0) {
            int shift = 0;
            do {
                unsigned int c = s[i];
                unsigned int t;
                if (c == 0) break;
                t = c - 0x41;
                if (t <= 0x19) t = t + 0x61;
                else t = t + 0x41;
                i++;
                result |= t << shift;
                shift += 8;
            } while (i < n);
        }
    }
    return result;
}
