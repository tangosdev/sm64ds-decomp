"""Build the ARM7 binary from arm7/src and check it against arm7/arm7.sha1.

    python arm7/build.py [--base path/to/arm7.bin]

Every row of arm7/swap.txt is a function built from source and linked at its storage
address: .c files with mwccarm 2004/b56, .s files with mwasmarm 1.2/sp2p3. Bytes not
covered by a row (unenrolled NONMATCHING drafts, padding) are copied from the retail
arm7.bin extracted by dsd, the same way the ROM build fills gaps from retail bytes. The
retail image is read from extracted/ and never written into the tree.
"""
import argparse
import hashlib
import os
import shutil
import subprocess
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parent
BUILD = REPO / "build" / "arm7"

# name: (file offset, length, storage base, link origin)
REGIONS = {
    "crt0": (0x0, 0x168, 0x02380000, 0x02380000),
    "wram": (0x168, 0xDF20, 0x02380168, 0x037F8000),
    "main": (0xE088, 0x16A84, 0x0238E088, 0x027C0000),
    "tail": (0x24B0C, 0x18, 0x023A4B0C, 0x023A4B0C),
}
IMAGE_LEN = 0x24B24

CFLAGS = [
    "-O4,s", "-proc", "arm7tdmi", "-interworking", "-fp", "soft",
    "-enum", "int", "-lang", "c99", "-char", "signed",
]


def tools_root():
    env = os.environ.get("ARM7_MW")
    candidates = [Path(env)] if env else []
    candidates.append(REPO / "tools" / "mwccarm")
    for path in candidates:
        if (path / "license.dat").is_file():
            return path
    raise SystemExit("mwccarm tree not found; put it in tools/mwccarm or set ARM7_MW")


def base_image(arg):
    candidates = [Path(arg)] if arg else []
    candidates += [REPO / "extracted" / "dsd" / "arm7" / "arm7.bin", REPO / "extracted" / "arm7.bin"]
    for path in candidates:
        if path.is_file():
            blob = path.read_bytes()
            if len(blob) != IMAGE_LEN:
                raise SystemExit("%s is 0x%X bytes, expected 0x%X" % (path, len(blob), IMAGE_LEN))
            return blob
    raise SystemExit("retail arm7.bin not found; run the dsd extract first or pass --base")


def run(cmd, cwd):
    launcher = os.environ.get("MWCCARM_LAUNCHER") or ("" if os.name == "nt" else "wine")
    if launcher and str(cmd[0]).lower().endswith(".exe"):
        cmd = launcher.split() + list(cmd)
    result = subprocess.run([str(c) for c in cmd], cwd=str(cwd), text=True,
                            stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    if result.returncode != 0:
        print(result.stdout)
        raise SystemExit("command failed (%s): %s" % (result.returncode, " ".join(str(c) for c in cmd)))


def write_words(path, data, label=None):
    if len(data) % 4:
        raise SystemExit("%s length 0x%X is not a multiple of 4" % (path, len(data)))
    lines = [".section .text"]
    if label:
        lines += [".global %s" % label, "%s:" % label]
    for i in range(0, len(data), 4):
        lines.append("\t.word 0x%08X" % int.from_bytes(data[i:i + 4], "little"))
    path.write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")


def load_swap():
    rows = []
    for line in (HERE / "swap.txt").read_text(encoding="utf-8").splitlines():
        line = line.split("#", 1)[0].strip()
        if not line:
            continue
        storage, size, symbol, source, region = line.split()
        rows.append({"storage": int(storage, 16), "size": int(size, 16), "symbol": symbol,
                     "source": HERE / source, "region": region})
    return rows


def load_syms(defined):
    """Absolute symbols, one `name = 0xADDR` per line; duplicates must agree."""
    syms = {}
    for line in (HERE / "syms.txt").read_text(encoding="utf-8").splitlines():
        line = line.split("#", 1)[0].strip().rstrip(";")
        if not line:
            continue
        name, value = [x.strip() for x in line.split("=")]
        value = int(value, 0)
        if syms.get(name, value) != value:
            raise SystemExit("symbol %s defined twice with different values" % name)
        syms[name] = value
    return {k: v for k, v in sorted(syms.items()) if k not in defined}


def region_parts(data, base, rows):
    cuts = sorted((row["storage"] - base, row) for row in rows)
    parts, cursor = [], 0
    for off, row in cuts:
        if off < cursor or off + row["size"] > len(data):
            raise SystemExit("overlap or out of range at %s" % row["symbol"])
        parts.append(("gap", data[cursor:off]))
        parts.append(("src", row))
        cursor = off + row["size"]
    parts.append(("gap", data[cursor:]))
    return [p for p in parts if not (p[0] == "gap" and len(p[1]) == 0)]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--base", metavar="ARM7_BIN")
    parser.add_argument("--swap", action="store_true", help="accepted for compatibility; always on")
    args = parser.parse_args()

    blob = base_image(args.base)
    expect = (HERE / "arm7.sha1").read_text(encoding="utf-8").strip()
    if hashlib.sha1(blob).hexdigest() != expect:
        raise SystemExit("retail arm7.bin does not hash to arm7/arm7.sha1")

    mw = tools_root()
    os.environ["LM_LICENSE_FILE"] = str(mw / "license.dat")
    os.environ.setdefault("MWCIncludes", str(REPO / "include"))
    cc = mw / "2004" / "b56" / "mwccarm.exe"
    asm = mw / "1.2" / "sp2p3" / "mwasmarm.exe"
    ld = mw / "1.2" / "sp2p3" / "mwldarm.exe"
    for tool in (cc, asm, ld):
        if not tool.is_file():
            raise SystemExit("missing " + str(tool))

    if BUILD.exists():
        shutil.rmtree(BUILD)
    BUILD.mkdir(parents=True)

    rows = load_swap()
    objects, lcf_objs = [], {name: [] for name in REGIONS}
    for region, (off, length, base, _origin) in REGIONS.items():
        data = blob[off:off + length]
        parts = region_parts(data, base, [r for r in rows if r["region"] == region])
        for index, (kind, payload) in enumerate(parts):
            if kind == "gap":
                name = "%s_gap_%d" % (region, index)
                label = "_start" if region == "crt0" and index == 0 else None
                write_words(BUILD / (name + ".s"), payload, label)
                run([asm, "-proc", "arm4t", "-o", BUILD / (name + ".o"), BUILD / (name + ".s")], HERE)
            else:
                name = payload["symbol"]
                src = payload["source"]
                if src.suffix == ".s":
                    run([asm, "-proc", "arm4t", "-o", BUILD / (name + ".o"), src], HERE)
                else:
                    run([cc, *CFLAGS, "-c", "-o", BUILD / (name + ".o"), src], HERE)
            objects.append(name + ".o")
            lcf_objs[region].append(name + ".o")

    defined = {r["symbol"] for r in rows if r["source"].suffix == ".c"}
    lines = ["MEMORY {"]
    for region, (_off, length, _base, origin) in REGIONS.items():
        lines.append("    %s : ORIGIN = 0x%08X, LENGTH = 0x%X > %s.bin" % (region, origin, length, region))
    lines += ["}", "", "SECTIONS {"]
    lines += ["    %s = 0x%08X;" % kv for kv in load_syms(defined).items()]
    lines.append("")
    for region in REGIONS:
        lines.append("    .%s : {" % region)
        lines += ["        %s(.text)" % name for name in lcf_objs[region]]
        lines += ["    } > %s" % region, ""]
    lines.append("}")
    (BUILD / "arm7.lcf").write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")

    run([ld, "-proc", "v4t", "-nostdlib", "-interworking", "-m", "_start", "-nodead",
         "-msgstyle", "gcc", "-o", "arm7.elf", "arm7.lcf", *objects], BUILD)

    image = b""
    for region, (_off, length, _base, _origin) in REGIONS.items():
        part = (BUILD / (region + ".bin")).read_bytes()
        if len(part) != length:
            raise SystemExit("%s.bin is 0x%X, expected 0x%X" % (region, len(part), length))
        image += part
    (BUILD / "arm7.bin").write_bytes(image)
    digest = hashlib.sha1(image).hexdigest()
    built = sum(r["size"] for r in rows)
    print("rows %d, 0x%X of 0x%X bytes from source" % (len(rows), built, IMAGE_LEN))
    print("sha1", digest)
    if digest != expect:
        raise SystemExit("SHA-1 mismatch: got %s expected %s" % (digest, expect))
    print("SHA-1 matches", expect)


if __name__ == "__main__":
    main()
