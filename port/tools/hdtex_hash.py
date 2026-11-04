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

port_name() and port_name2() are the port's own two names, hdtex_name and
hdtex_name2 in port/ntr/hdtex.cpp, over a copy of the texture and palette
slot windows. The loader asks for port_name2 first and port_name on a miss.
port_name's palette part reads fixed runs (four entries per 4x4 block, 32 /
16 / 256 / 4 / 8 for the paletted formats) that can reach past the texture's
own palette into whatever is loaded after it; port_name2 reads only the
entries the decoder reads, so its name does not move with the level. The
selftest builds one 4x4-compressed texture that shows the difference.
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


# ------------------------------------------------------------ the port's names
TEX_VRAM = 0x80000
PLTT_VRAM = 0x20000


def masked_hash_seeded(vram, vram_size, addr, size, seed):
    """hdtex.cpp masked_hash_seeded: masked_hash with a running seed."""
    h = seed
    addr &= vram_size - 1
    while size > 0:
        piece = vram_size - addr if addr + size > vram_size else size
        h = xxh64(bytes(vram[addr:addr + piece]), h)
        addr = (addr + piece) & (vram_size - 1)
        size -= piece
    return h


def _geometry(teximage):
    tp = teximage & ~0xC00F0000 & 0xFFFFFFFF
    fmt = (tp >> 26) & 7
    wl, hl = (tp >> 20) & 7, (tp >> 23) & 7
    return fmt, 8 << wl, 8 << hl, (tp & 0xFFFF) * 8, (tp >> 29) & 1


_TEX_SIZE = {1: lambda w, h: w * h, 2: lambda w, h: w * h // 4,
             3: lambda w, h: w * h // 2, 4: lambda w, h: w * h,
             5: lambda w, h: w * h // 16 * 4, 6: lambda w, h: w * h,
             7: lambda w, h: w * h * 2}
_V1_ENTRIES = {1: 32, 2: 4, 3: 16, 4: 256, 6: 8}


def _head_and_texels(tex, teximage, plttbase):
    fmt, w, h, addr, color0 = _geometry(teximage)
    if fmt not in _TEX_SIZE:
        return None
    hsh = xxh64(struct.pack('<4I', fmt, w, h, color0), 0)
    size = _TEX_SIZE[fmt](w, h)
    hsh = masked_hash_seeded(tex, TEX_VRAM, addr, size, hsh)
    pal_addr = plttbase * 16
    if fmt == 2:
        pal_addr >>= 1
    return fmt, w, h, addr, size, pal_addr, hsh


def _pal_byte(pal, off):
    return pal[off] if off < PLTT_VRAM else 0


def _blocks(tex, w, h, addr):
    idx_off = 0x20000 + addr // 2
    for b in range(w * h // 16):
        io = (idx_off + b * 2) & (TEX_VRAM - 1)
        yield tex[io] | (tex[(io + 1) & (TEX_VRAM - 1)] << 8)


def port_name(tex, pal, teximage, plttbase):
    """hdtex_name: the first name, fixed palette runs."""
    r = _head_and_texels(tex, teximage, plttbase)
    if r is None:
        return 0
    fmt, w, h, addr, size, pal_addr, hsh = r
    if fmt == 5:
        hsh = masked_hash_seeded(tex, TEX_VRAM, 0x20000 + addr // 2,
                                 w * h // 16 * 2, hsh)
        refs = bytearray()
        for info in _blocks(tex, w, h, addr):
            base = pal_addr + (info & 0x3FFF) * 4
            refs += bytes(_pal_byte(pal, base + k) for k in range(8))
        hsh = xxh64(bytes(refs), hsh)
    elif fmt in _V1_ENTRIES:
        hsh = masked_hash_seeded(pal, PLTT_VRAM, pal_addr & 0x1FFFF,
                                 _V1_ENTRIES[fmt] * 2, hsh)
    return hsh


# Entries each 4x4 block mode reads: mode 0 three, 1 and 3 two, 2 four.
_MODE_ENTRIES = (3, 2, 4, 2)


def _top_index(tex, fmt, addr, size):
    top = 0
    for i in range(size):
        v = tex[(addr + i) & (TEX_VRAM - 1)]
        if fmt == 1:
            m = v & 0x1F
        elif fmt == 6:
            m = v & 7
        elif fmt == 2:
            m = max(v & 3, (v >> 2) & 3, (v >> 4) & 3, v >> 6)
        elif fmt == 3:
            m = max(v & 15, v >> 4)
        else:
            m = v
        top = max(top, m)
    return top


def port_name2(tex, pal, teximage, plttbase):
    """hdtex_name2: only the palette entries the decoder reads."""
    r = _head_and_texels(tex, teximage, plttbase)
    if r is None:
        return 0
    fmt, w, h, addr, size, pal_addr, hsh = r
    if fmt == 7:
        return port_name(tex, pal, teximage, plttbase)
    if fmt == 5:
        hsh = masked_hash_seeded(tex, TEX_VRAM, 0x20000 + addr // 2,
                                 w * h // 16 * 2, hsh)
        refs = bytearray()
        for info in _blocks(tex, w, h, addr):
            base = pal_addr + (info & 0x3FFF) * 4
            n = _MODE_ENTRIES[info >> 14] * 2
            refs += bytes(_pal_byte(pal, base + k) for k in range(n))
        return xxh64(bytes(refs), hsh)
    top = _top_index(tex, fmt, addr, size)
    return masked_hash_seeded(pal, PLTT_VRAM, pal_addr & 0x1FFFF,
                              (top + 1) * 2, hsh)


def _fmt5_case():
    """One 8x8 format-5 texture (four blocks). Its palette is eight entries
    long, and block 3 is mode 1 with its offset on the LAST two of them; the
    entries after those belong to the next model. Returns (tex, pal,
    teximage, plttbase, past), past being the byte offset of the next
    model's entries."""
    tex = bytearray(TEX_VRAM)
    pal = bytearray(PLTT_VRAM)
    addr = 0x1000
    teximage = (5 << 26) | (addr // 8)
    plttbase = 0x10                    # the palette at byte 0x100
    rnd = random.Random(55)
    for i in range(16):                # four blocks of 2-bit texels
        tex[addr + i] = rnd.getrandbits(8)
    idx = 0x20000 + addr // 2
    # (mode, offset in two-colour units): mode 0 on entries 0..2, mode 3 on
    # 2..3, mode 2 on 4..7, mode 1 on 6..7, the last two.
    for b, (mode, off) in enumerate(((0, 0), (3, 1), (2, 2), (1, 3))):
        info = (mode << 14) | off
        tex[idx + b * 2] = info & 0xFF
        tex[idx + b * 2 + 1] = info >> 8
    base = plttbase * 16
    for i in range(16):
        pal[base + i] = rnd.getrandbits(8)
    return tex, pal, teximage, plttbase, base + 16


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
    # The port's two names on one hand-built texture.
    tex, pal, ti, pb, past = _fmt5_case()
    v1a, v2a = port_name(tex, pal, ti, pb), port_name2(tex, pal, ti, pb)
    for i in range(8):                 # another model's palette moves in
        pal[past + i] ^= 0x5A
    v1b, v2b = port_name(tex, pal, ti, pb), port_name2(tex, pal, ti, pb)
    print("fmt5, mode-1 block on the last two entries, next palette changed:")
    print("  v1 %016x -> %016x  %s (expect changed)"
          % (v1a, v1b, "changed" if v1a != v1b else "same"))
    print("  v2 %016x -> %016x  %s (expect same)"
          % (v2a, v2b, "changed" if v2a != v2b else "same"))
    ok &= (v1a != v1b) and (v2a == v2b)
    pal[past - 1] ^= 0x01              # the texture's own last entry
    v2c = port_name2(tex, pal, ti, pb)
    print("  v2 after the texture's own last entry changed: %s (expect changed)"
          % ("changed" if v2c != v2a else "same"))
    ok &= (v2c != v2a)
    print("SELFTEST", "PASS" if ok else "FAIL")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(selftest())
