extern int func_0205e89c(unsigned int *ctx);
extern void func_0205e6e4(unsigned int *ctx, const unsigned char *data, int len);
extern int func_0205e7e0(unsigned int *ctx, unsigned char *out);

void func_0205e0f4(unsigned char *out, unsigned char *msg, int msglen, unsigned char *key, int keylen)
{
    unsigned char ipad[64];
    unsigned char opad[64];
    unsigned char digest[20];
    unsigned int ctx[26];
    unsigned char *dg = digest;
    int i;

    if (out == 0) return;
    if (msg == 0) return;
    if (msglen == 0) return;
    if (key == 0) return;
    if (keylen == 0) return;

    if (keylen > 64) {
        func_0205e89c(ctx);
        func_0205e6e4(ctx, key, keylen);
        func_0205e7e0(ctx, dg);
        key = dg;
        keylen = 20;
    }

    for (i = 0; i < keylen; i++) {
        ipad[i] = *key ^ 0x36;
        opad[i] = *key ^ 0x5c;
        key++;
    }
    for (; i < 64; i++) {
        ipad[i] = 0x36;
        opad[i] = 0x5c;
    }

    func_0205e89c(ctx);
    func_0205e6e4(ctx, ipad, 64);
    func_0205e6e4(ctx, msg, msglen);
    func_0205e7e0(ctx, dg);

    func_0205e89c(ctx);
    func_0205e6e4(ctx, opad, 64);
    func_0205e6e4(ctx, dg, 20);
    func_0205e7e0(ctx, out);
}
