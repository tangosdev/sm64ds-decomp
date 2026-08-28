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
          "star": 0,              # the sub-table's star filter, 0..7
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
    then per section (SIX bytes):
    u8     scope        0xFF the misc table, 0..7 an area index
    u8     type         the sub-loader index: 0 standard, 1 entrance, 5 simple
    u8     star         the sub-table's star filter, 0..7, verbatim from the
                        high three bits of the ROM's own descriptor byte
    u8     reserved     must be 0; anything else refuses the whole file
    u16    count        entries that follow (255 is the ceiling -- the ROM's
                        own sub-table count field is one byte)
    count * recordsize bytes, in the EXACT ROM record layout:

      type 0 standard  0x10  u16 raw, s16 x, s16 y, s16 z, s16 rot[3], u16 param
      type 1 entrance  0x10  the same sixteen bytes
      type 5 simple    0x08  u16 raw, s16 x, s16 y, s16 z
      type 240 props   0x04  u8 field, u8 value, u16 pad = 0

STAGE PROPERTIES (type 240 = 0xF0) do not name a sub-table. They carry level
wide settings, so their scope is always 255 and their star always 0, and the
records are field/value pairs rather than objects:

    { "scope": "misc", "type": "properties",
      "properties": {"skybox": 3} }

Field 0 is the skybox: 0 for none, 1..11 for the game's own vrbox skies.
Stage::LoadSkybox indexes data_02075620 at skyboxID - 1 and that table holds
eleven real file handles, which is where the ceiling comes from -- the ROM's
own mask is five bits wide and would read straight off the end of the table.
An UNKNOWN FIELD makes the game refuse the whole file rather than apply the
part it understands, so this tool refuses to write one; the field space is how
future settings arrive, never a new section type.

`raw` is the level data's own index into the actor-id table
(data_ov002_0210cbf4), NOT an actor id -- both ROM loaders spell that out. For
a simple entry the ROM splits it further: raw & 0x1ff indexes the table and
raw >> 9 is the parameter, so a simple entry may give "id" and "param"
separately instead of "raw" and this tool packs them.

SCOPE IS THE AREA AND STAR IS THE MISSION, and a section needs both. The ROM
packs two fields into a sub-table descriptor's first byte: the low five bits
are the sub-loader (this file's `type`) and the high three are a star filter
compared against data_0209f220. A level gives each mission its own object list,
so one area routinely carries eight standard sub-tables that differ ONLY in
that star value -- Bob-omb Battlefield's area 0 has exactly that. Naming a
sub-table therefore takes the whole triple (scope, star, type), which is the
ROM's own key, and measured over the forty-six mounted levels it names all 366
editable sub-tables with no collisions.

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

# The stage-properties section. 0xF0 is outside the ROM's five-bit sub-table
# kind, so it can share the section header without ever colliding with a real
# sub-loader index.
TYPE_PROPS = 0xF0
PROPS_RECSIZE = 4

# field name -> (field id, smallest legal value, largest legal value)
FIELDS = {"skybox": (0, 0, 11)}
FIELD_BY_ID = {v[0]: k for k, v in FIELDS.items()}


def parse_type(v):
    """0 / 1 / 5 / 240, or 'standard' / 'entrance' / 'simple' / 'properties'."""
    if isinstance(v, str):
        s = v.strip().lower()
        if s in ("properties", "props"):
            return TYPE_PROPS
        for t, (_rs, name) in TYPES.items():
            if s == name:
                return t
        raise ValueError("type %r is not a known sub-table type name" % (v,))
    return int(v)


def type_name(type_id):
    if type_id == TYPE_PROPS:
        return "properties"
    return TYPES.get(type_id, (0, "unknown"))[1]


def pack_properties(sec, index, seen_fields):
    """A properties section's records, with the game's own refusals applied here.

    The writer refuses exactly what stage_mods.cpp refuses, so a file that
    compiles is a file the game will take. Producing a blob the port then
    rejects would just move the error somewhere harder to read.
    """
    props = sec.get("properties")
    entries = sec.get("entries")
    pairs = []
    if props is not None and entries is not None:
        raise ValueError("give either 'properties' or 'entries', not both")
    if props is not None:
        if not isinstance(props, dict):
            raise ValueError("'properties' must be an object of name: value")
        for name, value in props.items():
            key = str(name).strip().lower()
            if key not in FIELDS:
                raise ValueError(
                    "'%s' is not a stage property this build writes; it knows %s"
                    % (name, ", ".join(sorted(FIELDS))))
            pairs.append((FIELDS[key][0], int(value)))
    elif entries is not None:
        for e in entries:
            if "field" not in e or "value" not in e:
                raise ValueError("a properties entry needs 'field' and 'value'")
            pairs.append((int(e["field"]), int(e["value"])))
    else:
        raise ValueError("a properties section needs 'properties' or 'entries'")

    out = b""
    for field, value in pairs:
        if field not in FIELD_BY_ID:
            raise ValueError(
                "field %d is not one this build writes, and the game refuses "
                "the whole file on an unknown field rather than applying the "
                "rest" % field)
        name = FIELD_BY_ID[field]
        _id, lo, hi = FIELDS[name]
        if not lo <= value <= hi:
            raise ValueError("%s = %d is outside %d..%d" % (name, value, lo, hi))
        if field in seen_fields:
            raise ValueError("%s is set more than once, and a stage property "
                             "has one value" % name)
        seen_fields.add(field)
        out += struct.pack("<BBH", field, value, 0)
    return len(pairs), out


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


def star_value(v):
    v = int(v)
    if not 0 <= v <= 7:
        raise ValueError("star %d is not 0..7" % v)
    return v


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
    seen_fields = set()
    body = b""
    counts = []            # per section, for the summary below
    for i, sec in enumerate(sections):
        try:
            type_id = parse_type(sec["type"])
        except ValueError as err:
            sys.exit("section %d: %s" % (i, err))
        if type_id != TYPE_PROPS and type_id not in TYPES:
            sys.exit("section %d: type %d is not one of %s or %d properties"
                     % (i, type_id, ", ".join(str(t) for t in sorted(TYPES)),
                        TYPE_PROPS))
        # An object section must say which sub-table it replaces; a properties
        # section has only one legal scope, so it may leave it out.
        if type_id == TYPE_PROPS:
            scope = parse_scope(sec.get("scope", "misc"))
        else:
            scope = parse_scope(sec["scope"])
        try:
            star = star_value(sec.get("star", 0))
        except ValueError as err:
            sys.exit("section %d: %s" % (i, err))

        if type_id == TYPE_PROPS:
            # Stage properties belong to the level, so the two bytes that name
            # a sub-table have one legal spelling and the game refuses the
            # rest. Say so here rather than shipping a blob it will reject.
            if scope != SCOPE_MISC:
                sys.exit("section %d: stage properties belong to the level, so "
                         "their scope is 'misc', not %s"
                         % (i, scope_name(scope)))
            if star != 0:
                sys.exit("section %d: stage properties are not per-mission, so "
                         "their star is 0, not %d" % (i, star))
            try:
                n, packed = pack_properties(sec, i, seen_fields)
            except ValueError as err:
                sys.exit("section %d: %s" % (i, err))
        else:
            entries = sec["entries"]
            if len(entries) > 255:
                sys.exit("section %d: %d entries, and the ROM's sub-table count "
                         "field is one byte" % (i, len(entries)))
            try:
                packed = b"".join(pack_entry(type_id, e) for e in entries)
            except ValueError as err:
                sys.exit("section %d: %s" % (i, err))
            n = len(entries)

        key = (scope, star, type_id)
        if key in seen:
            sys.exit("section %d: %s star %d type %d appears twice, and a "
                     "section replaces a sub-table wholesale"
                     % (i, scope_name(scope), star, type_id))
        seen.add(key)
        counts.append((scope, star, type_id, n))
        body += struct.pack("<BBBBH", scope, type_id, star, 0, n) + packed

    blob = MAGIC + struct.pack("<BBH", VERSION, level, len(sections)) + body
    with open(out_path, "wb") as f:
        f.write(blob)
    print("%s: level %d, %d section(s), %d bytes"
          % (out_path, level, len(sections), len(blob)))
    for scope, star, type_id, n in counts:
        print("  %-7s star %d type %3d %-10s %3d %s"
              % (scope_name(scope), star, type_id, type_name(type_id), n,
                 "propert%s" % ("y" if n == 1 else "ies")
                 if type_id == TYPE_PROPS else "entries"))


def dump_blob(path):
    b = open(path, "rb").read()
    if len(b) < 8 or b[:4] != MAGIC:
        sys.exit("%s does not start with LVLM" % path)
    version, level, nsec = struct.unpack_from("<BBH", b, 4)
    print("%s: version %d, level %d, %d section(s), %d bytes"
          % (path, version, level, nsec, len(b)))
    cur = 8
    for i in range(nsec):
        scope, type_id, star, reserved, count = struct.unpack_from("<BBBBH",
                                                                   b, cur)
        rs = (PROPS_RECSIZE if type_id == TYPE_PROPS
              else TYPES.get(type_id, (0, "unknown"))[0])
        print("  section %d: %s star %d type %d %s, %d %s%s"
              % (i, scope_name(scope), star, type_id, type_name(type_id),
                 count, "record(s)" if type_id == TYPE_PROPS else "entries",
                 "" if reserved == 0 else
                 "  RESERVED=%d (the game refuses this)" % reserved))
        cur += 6
        if not rs:
            sys.exit("    unknown type %d, cannot walk further" % type_id)
        if type_id == TYPE_PROPS:
            for j in range(count):
                field, value, pad = struct.unpack("<BBH", b[cur:cur + rs])
                name = FIELD_BY_ID.get(field)
                print("    [%3d] field %d %-8s value %d%s%s"
                      % (j, field, name if name else "UNKNOWN", value,
                         "" if name else
                         "  (the game refuses the whole file on this)",
                         "" if pad == 0 else
                         "  PAD=%d (the game refuses this)" % pad))
                cur += rs
            if cur > len(b):
                sys.exit("    section %d runs past the end of the file" % i)
            continue
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
# The stage-properties line the same dump emits, one per mounted level:
#   [lvldump] level 6 properties: skybox 1
PROP_RE = re.compile(r"\[lvldump\] level (\d+) properties: skybox (\d+)")
REC_RE = re.compile(
    r"\[lvldump\] level (\d+) (misc|area \d+) sub (\d+) rec\s*(\d+): "
    r"raw 0x([0-9a-f]+) pos\s*(-?\d+)\s+(-?\d+)\s+(-?\d+)"
    r"(?: rot\s*(-?\d+)\s+(-?\d+)\s+(-?\d+) param 0x([0-9a-f]+))?")


def from_dump(dump_path, spec_path, want_level=None):
    """Turn a stock SM64DS_STAGE_MOD_DUMP=1 run into an editable spec."""
    subs = {}          # (level, scope, sub) -> {"type":, "star":, "entries":[]}
    order = []
    props = {}         # level -> {"skybox": value}
    for line in open(dump_path, encoding="utf-8", errors="replace"):
        m = PROP_RE.search(line)
        if m:
            props[int(m.group(1))] = {"skybox": int(m.group(2))}
            continue
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

    # (scope, star, type) is the section key and it is the ROM's own key for a
    # sub-table, so every editable sub-table in the dump is addressable. A
    # collision here would mean the ROM itself carries two sub-tables with the
    # same descriptor byte in one table, which no mounted level does -- it is
    # reported rather than resolved, because inventing a tie-break would be
    # inventing a format.
    by_key = {}
    for k in order:
        if k[0] != want_level:
            continue
        st = subs[k]
        if st["type"] not in TYPES:
            continue
        by_key.setdefault((k[1], st["star"], st["type"]), []).append(k)

    sections = []
    # The stage properties come first, so a spec reads the way a level does:
    # what the place IS, then what is standing in it. A dump made by a build
    # older than the properties line simply has none, and the spec is then the
    # objects-only file it always was.
    if want_level in props:
        sky = props[want_level]["skybox"]
        lo, hi = FIELDS["skybox"][1], FIELDS["skybox"][2]
        if lo <= sky <= hi:
            sections.append({"scope": "misc", "star": 0, "type": "properties",
                             "properties": props[want_level]})
        else:
            # A HANDFUL OF ROWS READ OUT OF RANGE AND THEY ARE NOT AN ERROR
            # HERE. Read off the ROM across all 46 mounted levels, 44 carry a
            # settings word of the shape 0x000000Xf and a skybox in 0..11. The
            # exceptions are levels 30 and 46 (16 and 20), and they are two of
            # the exact five rows port/hal/level_boot.cpp itself calls "not
            # stages" -- 0, 30, 32, 34, 46 -- whose whole settings word does
            # not fit the shape either. The ROM's mask is five bits, so the
            # number is real; it just does not name one of the eleven vrbox
            # handles data_02075620 holds.
            #
            # Emitting it would produce a spec that this tool then refuses to
            # compile, which is a trap. Leaving it out keeps the spec a working
            # round trip of everything that IS editable, and says so.
            print("  note: level %d dumps skybox %d, which is outside %d..%d, "
                  "so no properties section was written. The level keeps its "
                  "own sky; every other edit in this spec still applies."
                  % (want_level, sky, lo, hi))
    for (scope, star, type_id), keys in sorted(by_key.items()):
        if len(keys) > 1:
            sys.exit("%s star %d type %d names %d sub-tables in the dump -- "
                     "the (scope, star, type) key does not identify one, and "
                     "this tool will not guess"
                     % (scope, star, type_id, len(keys)))
        sections.append({"scope": scope, "star": star, "type": type_id,
                         "entries": subs[keys[0]]["entries"]})

    spec = {"version": VERSION, "level": want_level, "sections": sections}
    json.dump(spec, open(spec_path, "w", encoding="utf-8"), indent=1)
    print("%s: level %d, %d section(s), %d entries%s"
          % (spec_path, want_level, len(sections),
             sum(len(s.get("entries", ())) for s in sections),
             "" if want_level not in props
             else ", skybox %d" % props[want_level]["skybox"]))


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
