#!/usr/bin/env python3
"""The HD texture pack's content hash, in python.

This is the same arithmetic port/ntr/hdtex.cpp computes in the game, kept
here so a tool can name a texture without running anything, and so the C++
copy has something to be checked against. Run this file directly for its
selftest: a pure-python XXH64 against the canonical empty-string vector and,
when the xxhash module is installed, against 400 random buffers.

  python port/tools/hdtex_hash.py

The hash is the parked tangOS-SM64DS edition's, reproduced bit for bit so its
206 shipped PNGs load in the port unrenamed. Definition:
recomp/vendor/melonds/GPU3D_Texcache.h, Texcache::GetTexture.
"""
import random
import struct
import sys

M = (1 << 64) - 1
P1 = 0x9E3779B185EBCA87
P2 = 0xC2B2AE3D27D4EB4F
P3 = 0x165667B19E3779F9
P4 = 0x85EBCA77C2B2AE63
P5 = 0x27D4EB2F165667C5


def rotl(x, r):
    x &= M
    return ((x << r) | (x >> (64 - r))) & M


def _round(acc, inp):
    acc = (acc + (inp * P2)) & M
    acc = rotl(acc, 31)
    return (acc * P1) & M


def _merge(acc, val):
    val = _round(0, val)
    acc ^= val
    return ((acc * P1) + P4) & M


def xxh64(data, seed=0):
    n = len(data)
    p = 0
    if n >= 32:
        v1 = (seed + P1 + P2) & M
        v2 = (seed + P2) & M
        v3 = seed & M
        v4 = (seed - P1) & M
        while n - p >= 32:
            v1 = _round(v1, struct.unpack_from('<Q', data, p)[0]); p += 8
            v2 = _round(v2, struct.unpack_from('<Q', data, p)[0]); p += 8
            v3 = _round(v3, struct.unpack_from('<Q', data, p)[0]); p += 8
            v4 = _round(v4, struct.unpack_from('<Q', data, p)[0]); p += 8
        h = (rotl(v1, 1) + rotl(v2, 7) + rotl(v3, 12) + rotl(v4, 18)) & M
        h = _merge(h, v1)
        h = _merge(h, v2)
        h = _merge(h, v3)
        h = _merge(h, v4)
    else:
        h = (seed + P5) & M
    h = (h + n) & M
    while n - p >= 8:
        k1 = _round(0, struct.unpack_from('<Q', data, p)[0])
        p += 8
        h ^= k1
        h = ((rotl(h, 27) * P1) + P4) & M
    if n - p >= 4:
        h ^= (struct.unpack_from('<I', data, p)[0] * P1) & M
        p += 4
        h = ((rotl(h, 23) * P2) + P3) & M
    while n - p >= 1:
        h ^= (data[p] * P5) & M
        p += 1
        h = (rotl(h, 11) * P1) & M
    h ^= h >> 33
    h = (h * P2) & M
    h ^= h >> 29
    h = (h * P3) & M
    h ^= h >> 32
    return h


# ---------------------------------------------------------------- tangOS hash
def masked_hash(vram, vram_size, addr, size):
    """melonDS Texcache::MaskedHash, verbatim."""
    h = 0
    while size > 0:
        if addr + size > vram_size:
            piece = vram_size - addr
        else:
            piece = size
        h = xxh64(vram[addr:addr + piece], h)
        addr = (addr + piece) & (vram_size - 1)
        size -= piece
    return h


def tangos_texname(tex_h0, tex_h1, pal_h, fmt, wlog2, hlog2):
    """GPU3D_Texcache.h GetTexture, the `u64 texname = ...` expression."""
    return (tex_h0
            ^ (((tex_h1 << 17) | (tex_h1 >> 47)) & M)
            ^ (((pal_h << 31) | (pal_h >> 33)) & M)
            ^ ((fmt & 0xFF) << 56)
            ^ ((wlog2 & 0xFF) << 48)
            ^ ((hlog2 & 0xFF) << 40)) & M


def selftest():
    try:
        import xxhash as _x
    except ImportError:
        print("no xxhash module; only the empty-string vector is checked")
        _x = None
    ok = True
    # Canonical vector
    e = xxh64(b"")
    print("XXH64('')            = 0x%016X  (expect 0xEF46DB3751D8E999)" % e)
    ok &= (e == 0xEF46DB3751D8E999)
    if _x is not None:
        rnd = random.Random(1234)
        bad = 0
        for i in range(400):
            n = rnd.choice([0, 1, 3, 4, 7, 8, 15, 16, 31, 32, 33, 63, 64, 65,
                            127, 128, 200, 511, 512, 1024, 4096, 0x10000])
            b = bytes(rnd.getrandbits(8) for _ in range(min(n, 4096)))
            if n > 4096:
                b = b * (n // 4096) + b[:n % 4096]
            seed = rnd.choice([0, 1, 0xDEADBEEF, 0xFFFFFFFFFFFFFFFF])
            mine = xxh64(b, seed)
            theirs = _x.xxh64_intdigest(b, seed=seed)
            if mine != theirs:
                bad += 1
                if bad < 4:
                    print("  MISMATCH len=%d seed=%#x mine=%016x theirs=%016x"
                          % (len(b), seed, mine, theirs))
        print("cross-check vs xxhash module: %d/400 mismatches" % bad)
        ok &= (bad == 0)
    print("SELFTEST", "PASS" if ok else "FAIL")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(selftest())
