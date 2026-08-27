#!/usr/bin/env python3
"""lvlmod.py -- compile a human-written level edit into the game's .lvlm blob.

The bridge has two ends, the same shape palmod.py has. An editor (or a hand
written JSON) speaks objects; the game's mount layer (port/hal/stage_mods.cpp)
speaks one dumb binary it can parse and fully validate before it writes a word
into the level. This tool is the crossing.

A section REPLACES one sub-table's entry list wholesale, so moving, deleting
and adding are all the same operation: emit the list you want. A sub-table the
file does not name is left exactly as the ROM has it.

JSON in:

    {
      "version": 1,
      "level": 6,
      "sections": [
        { "scope": "misc",        # or 0..7 for an area, or "area 0"
          "type": 0,              # 0 standard, 1 entrance, 5 simple
          "entries": [
            {"raw": 28, "x": -100, "y": 0, "z": 200,
             "rot": [0, 0, 0], "param": 0}
          ]
        }
      ]
    }

.lvlm out (little-endian), matching stage_mods.cpp exactly:

    magic  "LVLM"       4 bytes
    u8     version = 1
    u8     level id     the SM64DS_LEVEL id space
    u16    section count
    then per section:
    u8     scope        0xFF the misc table, 0..7 an area index
    u8     type         the sub-loader index: 0 standard, 1 entrance, 5 simple
    u16    count        entries that follow (255 is the ceiling -- the ROM's
                        own sub-table count field is one byte)
    count * recordsize bytes, in the EXACT ROM record layout:

      type 0 standard  0x10  u16 raw, s16 x, s16 y, s16 z, s16 rot[3], u16 param
      type 1 entrance  0x10  the same sixteen bytes
      type 5 simple    0x08  u16 raw, s16 x, s16 y, s16 z

`raw` is the level data's own index into the actor-id table
(data_ov002_0210cbf4), NOT an actor id -- both ROM loaders spell that out. For
a simple entry the ROM splits it further: raw & 0x1ff indexes the table and
raw >> 9 is the parameter, so a simple entry may give "id" and "param"
separately instead of "raw" and this tool packs them.

SCOPE IS THE AREA, and the star is not addressable. The ROM packs two fields
into a sub-table descriptor's first byte: the low five bits are the sub-loader
(this file's `type`) and the high three are a star filter compared against
data_0209f220. A section names (scope, type) only; the star filter of the
sub-table it lands on is the level's own and is never rewritten. If a level
carries two sub-tables of one type in one area -- which is how a level gives a
mission its own object set -- the game refuses the file rather than guessing
which one an editor meant.

Modes:
    compile  spec.json out.lvlm       the normal path
    dump     in.lvlm                  read a blob back as readable text
    fromdump dump.txt spec.json [lvl] turn a run's [lvldump] lines into a spec,
                                      which is how an editor gets the stock
                                      objects to edit in the first place
"""
import json
import re
import struct
import sys

MAGIC = b"LVLM"
VERSION = 1
SCOPE_MISC = 0xFF

# type -> (record size, name)
TYPES = {0: (0x10, "standard"), 1: (0x10, "entrance"), 5: (0x08, "simple")}


def parse_scope(v):
    """'misc' / 255 / 'area 3' / 3 -> the scope byte."""
    if isinstance(v, int):
        if v == SCOPE_MISC or 0 <= v <= 7:
            return v
        raise ValueError("scope %r is not 0..7 or 255" % (v,))
    s = str(v).strip().lower()
    if s == "misc":
        return SCOPE_MISC
    m = re.fullmatch(r"area\s*([0-7])", s)
    if m:
        return int(m.group(1))
    raise ValueError("scope %r is not 'misc', 'area N' or 0..7" % (v,))


def scope_name(b):
    return "misc" if b == SCOPE_MISC else "area %d" % b


def s16(v, what):
    v = int(v)
    if not -0x8000 <= v <= 0x7FFF:
        raise ValueError("%s = %d does not fit a signed 16-bit field" % (what, v))
    return v


def u16(v, what):
    v = int(v)
    if not 0 <= v <= 0xFFFF:
        raise ValueError("%s = %d does not fit an unsigned 16-bit field" % (what, v))
    return v


def raw_word(e):
    """`raw`, or `id` (+ `param` for a simple entry) packed the ROM's way."""
    if "raw" in e:
        return u16(e["raw"], "raw")
    if "id" not in e:
        raise ValueError("entry has neither 'raw' nor 'id': %r" % (e,))
    ident = int(e["id"])
    if not 0 <= ident <= 0x1FF:
        raise ValueError("id %d is out of the 9-bit range the ROM packs" % ident)
    return ident


def pack_entry(type_id, e):
    if type_id in (0, 1):
        rot = e.get("rot", [0, 0, 0])
        if len(rot) != 3:
            raise ValueError("rot must be three values: %r" % (rot,))
        return struct.pack(
            "<HhhhhhhH",
            raw_word(e),
            s16(e.get("x", 0), "x"), s16(e.get("y", 0), "y"),
            s16(e.get("z", 0), "z"),
            s16(rot[0], "rot[0]"), s16(rot[1], "rot[1]"), s16(rot[2], "rot[2]"),
            u16(e.get("param", 0), "param"),
        )
    # simple: the parameter rides in the top seven bits of raw
    raw = raw_word(e)
    if "raw" not in e:
        raw |= (int(e.get("param", 0)) & 0x7F) << 9
    return struct.pack(
        "<Hhhh", raw,
        s16(e.get("x", 0), "x"), s16(e.get("y", 0), "y"), s16(e.get("z", 0), "z"))


def compile_spec(spec_path, out_path):
    spec = json.load(open(spec_path, encoding="utf-8"))
    if spec.get("version") != VERSION:
        sys.exit("spec version %r is not %d" % (spec.get("version"), VERSION))
    level = int(spec["level"])
    if not 0 <= level <= 255:
        sys.exit("level %d is not 0..255" % level)
    sections = spec["sections"]
    if len(sections) > 0xFFFF:
        sys.exit("%d sections is past the u16 count" % len(sections))

    seen = set()
    body = b""
    for i, sec in enumerate(sections):
        scope = parse_scope(sec["scope"])
        type_id = int(sec["type"])
        if type_id not in TYPES:
            sys.exit("section %d: type %d is not one of %s"
                     % (i, type_id, ", ".join(str(t) for t in sorted(TYPES))))
        key = (scope, type_id)
        if key in seen:
            sys.exit("section %d: %s type %d appears twice, and a section "
                     "replaces a sub-table wholesale"
                     % (i, scope_name(scope), type_id))
        seen.add(key)
        entries = sec["entries"]
        if len(entries) > 255:
            sys.exit("section %d: %d entries, and the ROM's sub-table count "
                     "field is one byte" % (i, len(entries)))
        try:
            packed = b"".join(pack_entry(type_id, e) for e in entries)
        except ValueError as err:
            sys.exit("section %d: %s" % (i, err))
        body += struct.pack("<BBH", scope, type_id, len(entries)) + packed

    blob = MAGIC + struct.pack("<BBH", VERSION, level, len(sections)) + body
    with open(out_path, "wb") as f:
        f.write(blob)
    print("%s: level %d, %d section(s), %d bytes"
          % (out_path, level, len(sections), len(blob)))
    for sec in sections:
        print("  %-7s type %d %-8s %3d entries"
              % (scope_name(parse_scope(sec["scope"])), int(sec["type"]),
                 TYPES[int(sec["type"])][1], len(sec["entries"])))


def dump_blob(path):
    b = open(path, "rb").read()
    if len(b) < 8 or b[:4] != MAGIC:
        sys.exit("%s does not start with LVLM" % path)
    version, level, nsec = struct.unpack_from("<BBH", b, 4)
    print("%s: version %d, level %d, %d section(s), %d bytes"
          % (path, version, level, nsec, len(b)))
    cur = 8
    for i in range(nsec):
        scope, type_id, count = struct.unpack_from("<BBH", b, cur)
        rs = TYPES.get(type_id, (0, "unknown"))[0]
        print("  section %d: %s type %d %s, %d entries"
              % (i, scope_name(scope), type_id,
                 TYPES.get(type_id, (0, "unknown"))[1], count))
        cur += 4
        if not rs:
            sys.exit("    unknown type %d, cannot walk further" % type_id)
        for j in range(count):
            r = b[cur:cur + rs]
            if type_id in (0, 1):
                raw, x, y, z, rx, ry, rz, param = struct.unpack("<HhhhhhhH", r)
                print("    [%3d] raw 0x%04x pos %6d %6d %6d rot %6d %6d %6d "
                      "param 0x%04x" % (j, raw, x, y, z, rx, ry, rz, param))
            else:
                raw, x, y, z = struct.unpack("<Hhhh", r)
                print("    [%3d] raw 0x%04x pos %6d %6d %6d (id %d param %d)"
                      % (j, raw, x, y, z, raw & 0x1FF, raw >> 9))
            cur += rs
        if cur > len(b):
            sys.exit("    section %d runs past the end of the file" % i)
    if cur != len(b):
        print("  WARNING: %d trailing byte(s)" % (len(b) - cur))


# stage_mods.cpp's dump lines, which is where an editor gets the stock objects:
#   [lvldump] level 6 area 0 sub 2: type  0 standard star 0 count  23
#   [lvldump] level 6 area 0 sub 2 rec   0: raw 0x001c pos ... rot ... param ...
SUB_RE = re.compile(
    r"\[lvldump\] level (\d+) (misc|area \d+) sub (\d+): type\s*(\d+)\s+\S+\s+"
    r"star (\d+) count\s*(\d+)")
REC_RE = re.compile(
    r"\[lvldump\] level (\d+) (misc|area \d+) sub (\d+) rec\s*(\d+): "
    r"raw 0x([0-9a-f]+) pos\s*(-?\d+)\s+(-?\d+)\s+(-?\d+)"
    r"(?: rot\s*(-?\d+)\s+(-?\d+)\s+(-?\d+) param 0x([0-9a-f]+))?")


def from_dump(dump_path, spec_path, want_level=None):
    """Turn a stock SM64DS_STAGE_MOD_DUMP=1 run into an editable spec."""
    subs = {}          # (level, scope, sub) -> {"type":, "star":, "entries":[]}
    order = []
    for line in open(dump_path, encoding="utf-8", errors="replace"):
        m = SUB_RE.search(line)
        if m:
            lvl, scope, sub, type_id, star, _count = (
                int(m.group(1)), m.group(2), int(m.group(3)),
                int(m.group(4)), int(m.group(5)), int(m.group(6)))
            key = (lvl, scope, sub)
            if key not in subs:
                subs[key] = {"type": type_id, "star": star, "entries": []}
                order.append(key)
            continue
        m = REC_RE.search(line)
        if not m:
            continue
        lvl, scope, sub = int(m.group(1)), m.group(2), int(m.group(3))
        key = (lvl, scope, sub)
        if key not in subs:
            continue
        e = {"raw": int(m.group(5), 16),
             "x": int(m.group(6)), "y": int(m.group(7)), "z": int(m.group(8))}
        if m.group(9) is not None:
            e["rot"] = [int(m.group(9)), int(m.group(10)), int(m.group(11))]
            e["param"] = int(m.group(12), 16)
        subs[key]["entries"].append(e)

    if not order:
        sys.exit("%s has no [lvldump] sub-table lines -- was the run made "
                 "with SM64DS_STAGE_MOD_DUMP=1?" % dump_path)

    levels = sorted({k[0] for k in order})
    if want_level is None:
        if len(levels) != 1:
            sys.exit("the dump covers levels %s; name one on the command line"
                     % ", ".join(str(x) for x in levels))
        want_level = levels[0]

    # (scope, type) is the section key, so a scope carrying two sub-tables of
    # one type cannot be addressed -- say so here rather than emitting a spec
    # the game will refuse.
    by_key = {}
    for k in order:
        if k[0] != want_level:
            continue
        st = subs[k]
        if st["type"] not in TYPES:
            continue
        by_key.setdefault((k[1], st["type"]), []).append(k)

    sections = []
    for (scope, type_id), keys in sorted(by_key.items()):
        if len(keys) > 1:
            print("  skipping %s type %d: %d sub-tables of that type "
                  "(stars %s) -- the format cannot address one of them"
                  % (scope, type_id, len(keys),
                     ", ".join(str(subs[k]["star"]) for k in keys)),
                  file=sys.stderr)
            continue
        sections.append({"scope": scope, "type": type_id,
                         "entries": subs[keys[0]]["entries"]})

    spec = {"version": VERSION, "level": want_level, "sections": sections}
    json.dump(spec, open(spec_path, "w", encoding="utf-8"), indent=1)
    print("%s: level %d, %d section(s), %d entries"
          % (spec_path, want_level, len(sections),
             sum(len(s["entries"]) for s in sections)))


def main():
    a = sys.argv[1:]
    if len(a) == 3 and a[0] == "compile":
        compile_spec(a[1], a[2])
    elif len(a) == 2 and a[0] == "dump":
        dump_blob(a[1])
    elif a and a[0] == "fromdump" and len(a) in (3, 4):
        from_dump(a[1], a[2], int(a[3]) if len(a) == 4 else None)
    else:
        sys.exit(__doc__)


if __name__ == "__main__":
    main()
