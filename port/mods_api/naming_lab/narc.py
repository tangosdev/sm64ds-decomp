"""NARC archives (Nitro ARChive): member names and bytes, for the naming lab.

The game asks for a file inside an archive by an id >= 0x8000: each of its 13
archives owns a range (the table at data_0208ecf4, read from the running game),
and id - first is the member index. This reads the extracted .narc so that index
gets its own name. Standard layout: "NARC" header, then BTAF (one start/end pair
per member), BTNF (the directory tree) and GMIF (the data).
"""
import struct


def _chunks(d):
    off = struct.unpack_from('<H', d, 0x0c)[0]
    out = {}
    while off + 8 <= len(d):
        tag, size = d[off:off + 4], struct.unpack_from('<I', d, off + 4)[0]
        out[tag] = (off + 8, size - 8)
        if size < 8:
            break
        off += size
    return out


def members(d):
    """-> [(path, bytes)] in member-index order; path is '' when unnamed"""
    if d[:4] != b'NARC':
        return []
    c = _chunks(d)
    fat, fnt, img = c.get(b'BTAF'), c.get(b'BTNF'), c.get(b'GMIF')
    if not fat or not img:
        return []
    n = struct.unpack_from('<H', d, fat[0])[0]
    spans = [struct.unpack_from('<II', d, fat[0] + 4 + 8 * i) for i in range(n)]
    names = [''] * n
    if fnt:
        base = fnt[0]

        def walk(dir_id, prefix, depth=0):
            if depth > 16:
                return
            o = base + 8 * (dir_id & 0xfff)
            sub, first = struct.unpack_from('<IH', d, o)
            p, fid = base + sub, first
            while p < base + fnt[1]:
                ln = d[p]
                p += 1
                if ln == 0:
                    break
                name = d[p:p + (ln & 0x7f)].decode('latin-1')
                p += ln & 0x7f
                if ln & 0x80:
                    child = struct.unpack_from('<H', d, p)[0]
                    p += 2
                    walk(child, prefix + name + '/', depth + 1)
                else:
                    if fid < n:
                        names[fid] = prefix + name
                    fid += 1
        try:
            walk(0xf000, '')
        except struct.error:
            pass
    return [(names[i], d[img[0] + a:img[0] + b]) for i, (a, b) in enumerate(spans)]
