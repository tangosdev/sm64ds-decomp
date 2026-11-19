#include "SHA1Context.h"

/* RFC 3174 SHA1Reset, SHA1Input and SHA1Result. */
extern int SHA1Reset(SHA1Context *context);
extern int SHA1Input(SHA1Context *context, const unsigned char *data, int len);
extern int SHA1Result(SHA1Context *context, unsigned char *out);

void DGT_Hash2CalcHmac(unsigned char *out, unsigned char *msg, int msglen, unsigned char *key,
                       int keylen) {
    unsigned char ipad[64];
    unsigned char opad[64];
    unsigned char digest[20];
    SHA1Context ctx;
    unsigned char *dg = digest;
    int i;

    if (out == 0) return;
    if (msg == 0) return;
    if (msglen == 0) return;
    if (key == 0) return;
    if (keylen == 0) return;

    if (keylen > 64) {
        SHA1Reset(&ctx);
        SHA1Input(&ctx, key, keylen);
        SHA1Result(&ctx, dg);
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

    SHA1Reset(&ctx);
    SHA1Input(&ctx, ipad, 64);
    SHA1Input(&ctx, msg, msglen);
    SHA1Result(&ctx, dg);

    SHA1Reset(&ctx);
    SHA1Input(&ctx, opad, 64);
    SHA1Input(&ctx, dg, 20);
    SHA1Result(&ctx, out);
}
