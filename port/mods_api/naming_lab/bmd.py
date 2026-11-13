"""SM64DS BMD model files: the texture and palette tables, decoded to PNG.

Only what the naming lab shows: each texture's own name (a string the ROM
carries, so it is evidence for naming the model's owner), its size and format,
and the pixels. Layout (community-documented BMD header, checked against the
ROM's kuribo_model.bmd):

  header  +0x14 n textures, +0x18 texture table, +0x1c n palettes, +0x20 palette table,
          +0x24 n materials, +0x28 material table
  texture (0x14): name ptr, data ptr, data size, u16 width, u16 height, TEXIMAGE_PARAM
  palette (0x10): name ptr, data ptr, data size, -1
  material (0x30): name ptr, texture index, palette index, ...

Files may be LZ77-compressed ("LZ77" + type 0x10). Reads the extracted ROM tree
only; nothing here is written anywhere but the caller's HTTP response.
"""
import struct
import zlib


def lz77(d):
    if d[:4] == b'LZ77':
        d = d[4:]
    if not d or d[0] != 0x10:
        return d
    size = d[1] | d[2] << 8 | d[3] << 16
    out = bytearray()
    i = 4
    while len(out) < size and i < len(d):
        flags = d[i]
        i += 1
        for bit in range(8):
            if len(out) >= size or i >= len(d):
                break
            if flags & (0x80 >> bit):
                b0, b1 = d[i], d[i + 1]
                i += 2
                n = (b0 >> 4) + 3
                disp = ((b0 & 15) << 8 | b1) + 1
                for _ in range(n):
                    out.append(out[-disp])
            else:
                out.append(d[i])
                i += 1
    return bytes(out)


def _cstr(d, off):
    if off <= 0 or off >= len(d):
        return ''
    e = d.find(b'\0', off)
    return d[off:e if e >= 0 else len(d)].decode('ascii', 'replace')


def _rgb555(c, a=255):
    return ((c & 31) * 255 // 31, (c >> 5 & 31) * 255 // 31, (c >> 10 & 31) * 255 // 31, a)


def png(w, h, rgba):
    raw = b''.join(b'\0' + bytes(rgba[y * w * 4:(y + 1) * w * 4]) for y in range(h))

    def chunk(t, b):
        return struct.pack('>I', len(b)) + t + b + struct.pack('>I', zlib.crc32(t + b) & 0xffffffff)
    return (b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>IIBBBBB', w, h, 8, 6, 0, 0, 0))
            + chunk(b'IDAT', zlib.compress(raw, 6)) + chunk(b'IEND', b''))


FORMATS = {1: 'A3I5', 2: '4-colour', 3: '16-colour', 4: '256-colour', 5: '4x4 compressed', 6: 'A5I3', 7: 'direct'}


def decode(fmt, w, h, tex, pal, transp0):
    """-> flat RGBA bytearray, or None when the format or data cannot be read."""
    out = bytearray(w * h * 4)
    colors = [struct.unpack_from('<H', pal, i)[0] for i in range(0, len(pal) - 1, 2)] if pal else []

    def pc(i, a=255):
        return _rgb555(colors[i], a) if i < len(colors) else (255, 0, 255, a)

    def put(x, y, c):
        o = (y * w + x) * 4
        out[o:o + 4] = bytes(c)
    if fmt in (2, 3, 4):
        bpp = {2: 2, 3: 4, 4: 8}[fmt]
        per = 8 // bpp
        for y in range(h):
            for x in range(w):
                n = y * w + x
                if n // per >= len(tex):
                    return out
                v = tex[n // per] >> ((n % per) * bpp) & ((1 << bpp) - 1)
                put(x, y, (0, 0, 0, 0) if transp0 and v == 0 else pc(v))
    elif fmt == 1 or fmt == 6:
        for y in range(h):
            for x in range(w):
                n = y * w + x
                if n >= len(tex):
                    return out
                b = tex[n]
                if fmt == 1:
                    put(x, y, pc(b & 31, (b >> 5) * 255 // 7))
                else:
                    put(x, y, pc(b & 7, (b >> 3) * 255 // 31))
    elif fmt == 7:
        for y in range(h):
            for x in range(w):
                o = (y * w + x) * 2
                if o + 2 > len(tex):
                    return out
                c = struct.unpack_from('<H', tex, o)[0]
                put(x, y, _rgb555(c, 255 if c & 0x8000 else 0))
    elif fmt == 5:
        nblk = (w // 4) * (h // 4)
        texels, index = tex[:nblk * 4], tex[nblk * 4:]
        for b in range(nblk):
            if b * 4 + 4 > len(texels) or b * 2 + 2 > len(index):
                break
            bx, by = b % (w // 4) * 4, b // (w // 4) * 4
            pi = struct.unpack_from('<H', index, b * 2)[0]
            base, mode = (pi & 0x3fff) * 2, pi >> 14
            c0, c1 = pc(base), pc(base + 1)
            if mode == 0:
                cs = [c0, c1, pc(base + 2), (0, 0, 0, 0)]
            elif mode == 1:
                cs = [c0, c1, tuple((c0[k] + c1[k]) // 2 for k in range(3)) + (255,), (0, 0, 0, 0)]
            elif mode == 2:
                cs = [c0, c1, pc(base + 2), pc(base + 3)]
            else:
                cs = [c0, c1, tuple((5 * c0[k] + 3 * c1[k]) // 8 for k in range(3)) + (255,),
                      tuple((3 * c0[k] + 5 * c1[k]) // 8 for k in range(3)) + (255,)]
            for ry in range(4):
                row = texels[b * 4 + ry]
                for rx in range(4):
                    put(bx + rx, by + ry, cs[row >> (rx * 2) & 3])
    else:
        return None
    return out


def textures(data):
    """-> [{name, width, height, format, palette, png}] for every material's texture."""
    d = lz77(data)
    if len(d) < 0x3c:
        return []
    ntex, ttex, npal, tpal, nmat, tmat = struct.unpack_from('<6I', d, 0x14)
    if ntex > 512 or npal > 512 or nmat > 1024 or ttex >= len(d):
        return []
    pals = []
    for i in range(npal):
        name, off, size, _ = struct.unpack_from('<4I', d, tpal + 0x10 * i)
        pals.append((_cstr(d, name), d[off:off + size]))
    # which palette each texture is drawn with: the first material that pairs them
    pair = {}
    for i in range(nmat):
        o = tmat + 0x30 * i
        if o + 12 > len(d):
            break
        t, p = struct.unpack_from('<iI', d, o + 4)
        if 0 <= t < ntex and t not in pair:
            pair[t] = p
    out = []
    for i in range(ntex):
        name, off, size, w, h, param = struct.unpack_from('<IIIHHI', d, ttex + 0x14 * i)
        fmt = param >> 26 & 7
        pi = pair.get(i)
        pname, pal = pals[pi] if pi is not None and pi < len(pals) else ('', b'')
        rec = {'name': _cstr(d, name), 'width': w, 'height': h, 'format': FORMATS.get(fmt, str(fmt)),
               'palette': pname, 'png': None}
        if 0 < w <= 1024 and 0 < h <= 1024:
            # a 4x4-compressed texture's size counts only the texel blocks; the
            # palette-index words (half as many bytes) follow them directly
            data = d[off:off + size + (size // 2 if fmt == 5 else 0)]
            rgba = decode(fmt, w, h, data, pal, bool(param >> 29 & 1))
            if rgba is not None:
                rec['png'] = png(w, h, rgba)
        out.append(rec)
    return out


if __name__ == '__main__':
    import sys
    for t in textures(open(sys.argv[1], 'rb').read()):
        print(t['name'], t['width'], t['height'], t['format'], t['palette'], len(t['png'] or b''))
