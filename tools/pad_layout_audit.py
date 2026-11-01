#!/usr/bin/env python3
"""Count padding members and header shadows, struct by struct.

Issue #4570 tracks two kinds of layout debt that no byte gate can see, because a
filler array compiles to the same code as the real field it stands in for:

  PAD MEMBERS      array members whose name says "unknown bytes here" --
                   `u8 pad_3ba[0x2];`, `char unk[4];`, `u8 _10[0x10];`. Most are
                   honest: a field nobody has identified yet. They are counted so the
                   size of that unknown is a number, not an impression.
  HEADER SHADOWS   a struct/class defined at file scope in a src/ file under the same
                   name as one defined at file scope in include/. That is a private
                   copy of a class the headers already describe, usually mostly
                   padding, reaching one or two fields at an offset. It shares no code
                   with the real class, never gets the real header's improvements, and
                   `tools/affected_src.py` cannot see that it depends on the class. This
                   is the "fake layout" #821 is about, and it is the number that should
                   only fall (notes/plan-cpp-language-mode.md, Phase 4).

`langmode_audit.py`'s `pad_layout` is a file-level regex (`char\\s+pad\\[0x`) and its
`local_struct_body` counts files that define any struct at all. Neither can say how
many structs carry padding or which of them copy a header class; this tool parses the
bodies to answer that, and langmode_audit ratchets the shadow count from here.

How a file is read:
  * git-tracked .c/.cpp/.h under src/ and include/; include/nitro (the vendor SDK) is
    left out of the pad counts but its names still count as header definitions.
  * src/ files go through msvc_arms.mwcc_side, so a PC-port `_MSC_VER` arm is not
    decomp source. Other `#if` arms are all read, so a struct written twice under
    `#if/#else` counts twice.
  * Comments, string literals and preprocessor lines are blanked before parsing; line
    numbers are kept.
  * A struct is file-scope when every enclosing brace is a namespace or an
    `extern "C"` block. Structs nested in a class or a function are counted for pads
    but never as shadows: their names live in another scope.
  * A pad member is an array of a char or integer type whose name contains pad,
    filler, unk, unused, reserved, rsv, dummy, gap, hole, field_ or unknown, or starts
    with an underscore. Some of these hold real data (a buffer someone named `pad`);
    the count does not try to tell.
  * "Offset comment" means a comment on the member's own line that contains a hex
    number. The array size (`[0x2]`) is not a comment and does not count. "Offset in
    name too" also takes a name ending in a hex offset (`pad_3ba`).
  * A shadow is matched by qualified name only. Most are copies of the header class
    (`Particle::System`, `Matrix4x3`); a generic name (`Obj`) can also be an unrelated
    class that happens to share it, which is still two definitions of one name.

Static analysis only: no compiler, no ROM.

Usage:
    python tools/pad_layout_audit.py                  # summary
    python tools/pad_layout_audit.py --list shadows   # src file:line  name  <- header(s)
    python tools/pad_layout_audit.py --list pads      # every pad member
    python tools/pad_layout_audit.py --list files     # files by pad count, worst first
    python tools/pad_layout_audit.py --json out.json
"""
import argparse
import collections
import json
import pathlib
import re
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))
import msvc_arms  # noqa: E402

VENDOR = "include/nitro/"

STRUCT_HEAD = re.compile(r"\b(struct|class|union)\s*(\w+)?\s*(?::[^{;]*)?\s*$")
ENUM_HEAD = re.compile(r"\benum\b")
# blank() keeps a literal's quotes, so `extern "C"` reads as `extern " "`.
LINKAGE_HEAD = re.compile(r'(?:\bextern\s*"\s*"|\bnamespace(?:\s+(\w+))?)\s*$')
TYPEDEF_HEAD = re.compile(r"\btypedef\b")
ACCESS = re.compile(r"^\s*(?:public|private|protected)\s*:\s*")
CHAR_TYPE = r"(?:unsigned\s+|signed\s+)?char|u8|s8|uint8_t|int8_t"
INT_TYPE = (r"(?:unsigned\s+|signed\s+)?(?:short|int|long)|unsigned|u16|s16|u32|s32|u64|s64"
            r"|uint16_t|int16_t|uint32_t|int32_t|uint64_t|int64_t|fx16|fx32|f32")
MEMBER = re.compile(r"^(?:(?:const|volatile|mutable)\s+)*(?P<type>" + CHAR_TYPE + "|" + INT_TYPE
                    + r")\s+(?P<name>\w+)\s*(?P<dims>(?:\[[^\]]*\]\s*)+)$")
CHAR_ONLY = re.compile(r"^(?:" + CHAR_TYPE + r")$")
PAD_WORDS = ("pad", "filler", "unk", "unused", "reserved", "rsv", "dummy", "gap",
             "hole", "field_", "unknown")
HEX_IN_COMMENT = re.compile(r"(?://|/\*).*?0x[0-9a-fA-F]+")
# `pad_3ba`, `unk_0x40`, `_10`: the name itself says where the member sits.
HEX_IN_NAME = re.compile(r"(?:^|_)(?:0x)?[0-9a-fA-F]{2,}$")
TRAILING_NAME = re.compile(r"\s*(\w+)")


def pad_kind(name):
    """The pad family a member name belongs to, or None for a real name."""
    low = name.lower()
    for word in PAD_WORDS:
        if word in low:
            return word
    return "_leading" if low.startswith("_") else None


def blank(text):
    """Comments, literals and directives replaced by spaces; newlines kept."""
    out, i, n = [], 0, len(text)
    while i < n:
        if text.startswith("//", i):
            j = text.find("\n", i)
            j = n if j < 0 else j
            out.append(" " * (j - i))
            i = j
        elif text.startswith("/*", i):
            j = text.find("*/", i + 2)
            j = n if j < 0 else j + 2
            out.append(re.sub(r"[^\n]", " ", text[i:j]))
            i = j
        elif text[i] in "\"'":
            quote, j = text[i], i + 1
            while j < n and text[j] != quote and text[j] != "\n":
                j += 2 if text[j] == "\\" else 1
            j = min(j + 1, n)
            out.append(quote + " " * max(0, j - i - 2) + (quote if j - i > 1 else ""))
            i = j
        else:
            out.append(text[i])
            i += 1
    flat = "".join(out)
    return re.sub(r"(?m)^[ \t]*#(?:[^\n]*\\\n)*[^\n]*",
                  lambda m: re.sub(r"[^\n]", " ", m.group(0)), flat)


def parse(path, text):
    """Every struct/class/union body in `text`, with its pad members.

    Returns dicts: file, line, name, kind, file_scope, pads (list of member dicts).
    A file-scope name is qualified by its enclosing namespaces (`Particle::System`).
    """
    t = blank(text)
    raw_lines = text.split("\n")
    structs = []
    # Each frame: kind is top / linkage / struct / block. `head` is the text since the
    # last `;`, `{` or `}` at this level; `start` is where that text begins.
    frames = [{"kind": "top", "head": [], "start": None}]
    i, n = 0, len(t)
    while i < n:
        c = t[i]
        f = frames[-1]
        if c == "{":
            head = "".join(f["head"])
            m = STRUCT_HEAD.search(head)
            if m and not ENUM_HEAD.search(head):
                file_scope = all(fr["kind"] in ("top", "linkage") for fr in frames)
                start = f["start"] if f["start"] is not None else i
                frames.append({"kind": "struct", "head": [], "start": None,
                               "name": m.group(2), "keyword": m.group(1),
                               "typedef": bool(TYPEDEF_HEAD.search(head)),
                               "scope": [fr["ns"] for fr in frames if fr.get("ns")],
                               "file_scope": file_scope,
                               "line": t.count("\n", 0, start) + 1, "pads": []})
            elif LINKAGE_HEAD.search(head):
                frames.append({"kind": "linkage", "head": [], "start": None,
                               "ns": LINKAGE_HEAD.search(head).group(1)})
            else:
                frames.append({"kind": "block", "head": [], "start": None})
            f["head"], f["start"] = [], None
        elif c == "}":
            if len(frames) > 1:
                fr = frames.pop()
                if fr["kind"] == "struct":
                    name = fr["name"]
                    if name is None and fr["typedef"]:
                        m = TRAILING_NAME.match(t, i + 1)
                        name = m.group(1) if m else None
                    if name is not None and fr["file_scope"]:
                        name = "::".join(fr["scope"] + [name])
                    structs.append({"file": path, "line": fr["line"],
                                    "name": name or "<anonymous>", "kind": fr["keyword"],
                                    "file_scope": fr["file_scope"] and name is not None,
                                    "pads": fr["pads"]})
            frames[-1]["head"], frames[-1]["start"] = [], None
        elif c == ";":
            if f["kind"] == "struct" and f["start"] is not None:
                stmt = ACCESS.sub("", " ".join("".join(f["head"]).split()))
                m = MEMBER.match(stmt)
                if m and pad_kind(m.group("name")):
                    line = t.count("\n", 0, f["start"]) + 1
                    raw = raw_lines[line - 1] if line <= len(raw_lines) else ""
                    f["pads"].append({
                        "line": line, "type": " ".join(m.group("type").split()),
                        "name": m.group("name"), "kind": pad_kind(m.group("name")),
                        "char": bool(CHAR_ONLY.match(" ".join(m.group("type").split()))),
                        "offset_comment": bool(HEX_IN_COMMENT.search(raw)),
                        "offset_named": bool(HEX_IN_NAME.search(m.group("name"))),
                    })
            f["head"], f["start"] = [], None
        else:
            if f["start"] is None and not c.isspace():
                f["start"] = i
            f["head"].append(c)
            if c == ":" and f["kind"] == "struct" and ACCESS.fullmatch("".join(f["head"])):
                # `public:` is not part of the next member; drop it so the member's
                # line is its own.
                f["head"], f["start"] = [], None
        i += 1
    return structs


def tracked(root):
    out = subprocess.run(["git", "-C", str(root), "ls-files", "src", "include"],
                         capture_output=True, text=True, check=True).stdout.split()
    return [p for p in out if p.endswith((".c", ".cpp", ".h"))]


def read(root, path):
    text = (root / path).read_text(encoding="utf-8", errors="replace")
    return msvc_arms.mwcc_side(text) if path.startswith("src/") else text


def audit(root=REPO, paths=None):
    paths = tracked(root) if paths is None else paths
    structs = []
    for p in paths:
        try:
            structs.extend(parse(p, read(root, p)))
        except OSError:
            continue
    header_defs = collections.defaultdict(set)
    for s in structs:
        if s["file"].startswith("include/") and s["file_scope"]:
            header_defs[s["name"]].add(s["file"])

    counted = [s for s in structs if not s["file"].startswith(VENDOR)]
    pads = [dict(p, file=s["file"], struct=s["name"]) for s in counted for p in s["pads"]]
    padded = [s for s in counted if s["pads"]]
    shadows = sorted(
        ({"file": s["file"], "line": s["line"], "name": s["name"], "pads": len(s["pads"]),
          "headers": sorted(header_defs[s["name"]])}
         for s in structs
         if s["file"].startswith("src/") and s["file_scope"] and s["name"] in header_defs),
        key=lambda r: (r["file"], r["line"]))
    where = lambda f: "include" if f.startswith("include/") else "src"
    return {
        "pad_members": len(pads),
        "pad_members_by_dir": dict(collections.Counter(where(p["file"]) for p in pads)),
        "pad_members_char": sum(p["char"] for p in pads),
        "pad_members_offset_comment": sum(p["offset_comment"] for p in pads),
        "pad_members_offset_located": sum(p["offset_comment"] or p["offset_named"] for p in pads),
        "pad_members_by_kind": dict(collections.Counter(p["kind"] for p in pads).most_common()),
        "padded_structs": len(padded),
        "padded_structs_by_dir": dict(collections.Counter(where(s["file"]) for s in padded)),
        "padded_struct_files": len({s["file"] for s in padded}),
        "header_shadows": len(shadows),
        "header_shadows_padded": sum(1 for s in shadows if s["pads"]),
        "header_shadow_files": len({s["file"] for s in shadows}),
        "header_shadow_classes": len({s["name"] for s in shadows}),
        "_shadows": shadows,
        "_pads": pads,
    }


def summary(r):
    pct = lambda a, b: f"{100.0 * a / b:.0f}%" if b else "n/a"
    by_dir = lambda d: ", ".join(f"{d.get(k, 0)} {k}/" for k in ("src", "include"))
    return "\n".join([
        "pad layout audit (src/ and include/, include/nitro excluded)",
        f"  pad members          {r['pad_members']:6d}  ({by_dir(r['pad_members_by_dir'])})",
        f"    char/u8            {r['pad_members_char']:6d}",
        f"    offset comment     {r['pad_members_offset_comment']:6d}  "
        f"({pct(r['pad_members_offset_comment'], r['pad_members'])})",
        f"    offset in name too {r['pad_members_offset_located']:6d}  "
        f"({pct(r['pad_members_offset_located'], r['pad_members'])})",
        f"  padded structs       {r['padded_structs']:6d}  ({by_dir(r['padded_structs_by_dir'])}),"
        f" in {r['padded_struct_files']} files",
        f"  header shadows       {r['header_shadows']:6d}  "
        f"({r['header_shadows_padded']} padded; {r['header_shadow_classes']} classes in "
        f"{r['header_shadow_files']} files)",
        "    a file-scope src/ struct with the name of a file-scope include/ struct;",
        "    `--list shadows` for the work list.",
    ])


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--list", choices=["shadows", "pads", "files"],
                    help="print a work list and nothing else")
    ap.add_argument("--json", metavar="PATH", help="write the report, lists included")
    args = ap.parse_args(argv)
    r = audit()
    if args.list == "shadows":
        for s in r["_shadows"]:
            print(f"{s['file']}:{s['line']}  {s['name']}  pads={s['pads']}  <- "
                  + ", ".join(s["headers"]))
        return 0
    if args.list == "pads":
        for p in r["_pads"]:
            print(f"{p['file']}:{p['line']}  {p['struct']}  {p['type']} {p['name']}")
        return 0
    if args.list == "files":
        for f, k in collections.Counter(p["file"] for p in r["_pads"]).most_common():
            print(f"{k:5d}  {f}")
        return 0
    if args.json:
        out = {k.lstrip("_"): v for k, v in r.items()}
        pathlib.Path(args.json).write_text(json.dumps(out, indent=2) + "\n")
        print(f"wrote {args.json}")
    print(summary(r))
    return 0


if __name__ == "__main__":
    sys.exit(main())
