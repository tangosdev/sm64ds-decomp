"""Recover the 2004 mwccarm (core 2.0 build 0056) from public archives.

The compiler that built SM64DS is a 2004 CodeWarrior for NITRO whose binary has
never surfaced. But NITRO was not a fork: the same mwccarm codegen core shipped
in Metrowerks' public, non-NDA "CodeWarrior for ARM ISA Edition" product, and
Metrowerks' own FTP is preserved on archive.org. The October 2004 update from
that line carries mwccarm.exe "Version 2.0 build 56", sixteen builds below the
1.2 release (build 72) that every decomp collection floors at.

This script fetches it and verifies it, downloading ~24 MB rather than the 5.5 GB
the archive actually is: 7z keeps its header at the end, so three range requests
locate and pull only the solid block holding the one file we want. The installer's
payload is then split and its InstallShield volume decompressed in-process, so no
third-party extractor is needed and nothing is executed.

    python tools/recover_cw2004.py            # into tools/mwccarm/2004/b56
    python tools/recover_cw2004.py --dest DIR

Why you want it: build 56 materializes the address of a member read-modify-write
(`add rA,rB,#off / ldr [rA] / orr / str [rA]`) where every 2005+ build folds it to
`[rB,#off]`. That single difference is the long-standing "materialization wall".
It is an ADDITION to the version sweep, not a replacement for 1.2/sp2p3 - see
notes/mwccarm-codegen.md section 6ah for what it wins and what it costs.
"""
import argparse
import os
import shutil
import struct
import subprocess
import sys
import zlib

URL = ("https://archive.org/download/ftp_metrowerks_updates.7z/"
       "ftp_metrowerks_updates.7z")
WANT = "CW_ARM_2.1.1_Update.exe"
# Plain-ASCII marker present in the compiler's own text. The version ("2.0 build
# 0056") is NOT usable here: it lives in the PE version resource as UTF-16, and the
# banner itself is assembled at runtime from a format string.
BANNER = b"Metrowerks C/C++ for Embedded ARM"
EXPECT_BUILD_UTF16 = "2.0 build 0056".encode("utf-16-le")

# Recovered 2026-07-27. mwccarm.exe, PE timestamp 2004-09-16 13:20:24 UTC,
# version resource "2.0 build 0056", copyright 2004.
EXPECT_SIZE = 2248704
EXPECT_SHA256 = "c6aafc210c78e91b123e8e949b8e26faa76849225b9f639780fcc9db6cdff0b7"
EXPECT_SHA1 = "8eb0b9653ea1c9a589c3a4399e37e2780059a818"
REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def fetch(url, start, end, out):
    """Range-fetch [start, end] into `out`, reusing a complete previous download.

    The payload block is ~2 GB, so a re-run after an interruption should not pay
    for it twice. Returns the path; callers read it themselves so the big block
    never has to sit in memory.
    """
    want = end - start + 1
    if os.path.exists(out) and os.path.getsize(out) == want:
        return out
    subprocess.run(["curl", "-sL", "--fail", "-r", f"{start}-{end}", url, "-o", out],
                   check=True, timeout=7200)
    got = os.path.getsize(out)
    if got != want:
        sys.exit(f"short read: got {got} of {want} bytes (archive.org throttling?)")
    return out


def number(buf, pos):
    """7z variable-length integer."""
    first, pos = buf[pos], pos + 1
    mask, value = 0x80, 0
    for i in range(8):
        if not (first & mask):
            return value | ((first & (mask - 1)) << (8 * i)), pos
        value |= buf[pos] << (8 * i)
        pos, mask = pos + 1, mask >> 1
    return value, pos


def pull_installer(work):
    """Range-fetch just the solid block holding WANT, and return its bytes."""
    import py7zr

    head = open(fetch(URL, 0, 31, os.path.join(work, "h0")), "rb").read()
    if head[:6] != b"7z\xbc\xaf\x27\x1c":
        sys.exit("archive.org did not return a 7z (rate limited?)")
    nho, nhs, _ = struct.unpack_from("<QQI", head, 12)
    end_abs = 32 + nho
    end = open(fetch(URL, end_abs, end_abs + nhs - 1, os.path.join(work, "h1")), "rb").read()
    total = end_abs + nhs

    p = 1
    if end[0] != 0x17 or end[p] != 0x06:
        sys.exit("unexpected 7z header layout")
    p += 1
    packpos, p = number(end, p)
    nstreams, p = number(end, p)
    p += 1  # kSize
    sizes = []
    for _ in range(nstreams):
        v, p = number(end, p)
        sizes.append(v)
    hbase = 32 + packpos
    packed = open(fetch(URL, hbase, hbase + sum(sizes) - 1,
                        os.path.join(work, "h2")), "rb").read()

    sparse = os.path.join(work, "remote.7z")
    with open(sparse, "wb") as f:
        f.write(head)
        f.truncate(total)
        f.seek(hbase)
        f.write(packed)
        f.seek(end_abs)
        f.write(end)
    subprocess.run(["fsutil", "sparse", "setflag", sparse], capture_output=True)

    with py7zr.SevenZipFile(sparse, "r") as z:
        names = z.getnames()
        ms = z.header.main_streams
    target = next((n for n in names if n.endswith(WANT)), None)
    if not target:
        sys.exit(f"{WANT} not present in the archive listing")

    base, off, idx, starts = 32 + ms.packinfo.packpos, None, 0, []
    off = base
    for folder in ms.unpackinfo.folders:
        n = getattr(folder, "num_pack_streams", 1)
        span = sum(ms.packinfo.packsizes[idx:idx + n])
        starts.append((off, span))
        off += span
        idx += n
    counts = ms.substreamsinfo.num_unpackstreams_folders or [1] * len(starts)
    fi, k = 0, 0
    for i, c in enumerate(counts):
        for _ in range(c):
            if k < len(names) and names[k] == target:
                fi = i
            k += 1
    s, ln = starts[fi]
    print(f"  fetching solid block {fi}: {ln/1e6:.0f} MB of {total/1e9:.1f} GB")
    blk = fetch(URL, s, s + ln - 1, os.path.join(work, "blk"))
    with open(blk, "rb") as src, open(sparse, "r+b") as f:
        f.seek(s)
        shutil.copyfileobj(src, f, 1 << 22)
    with py7zr.SevenZipFile(sparse, "r") as z:
        z.extract(path=work, targets=[target])
    return os.path.join(work, target.replace("/", os.sep))


def split_payload(exe):
    """The appended payload is <name>\\0<path>\\0<version>\\0<size>\\0<bytes>..."""
    import pefile

    d = open(exe, "rb").read()
    pe = pefile.PE(exe, fast_load=True)
    pos = max(s.PointerToRawData + s.SizeOfRawData for s in pe.sections)
    parts = {}
    while pos < len(d) - 4:
        fields = []
        for _ in range(4):
            e = d.find(b"\0", pos)
            if e < 0 or e - pos > 260:
                return parts
            fields.append(d[pos:e].decode("ascii", "replace"))
            pos = e + 1
        if not fields[3].isdigit():
            return parts
        n = int(fields[3])
        parts[os.path.basename(fields[1] or fields[0])] = d[pos:pos + n]
        pos += n
    return parts


def carve_compiler(volume):
    """Walk the InstallShield volume's <u16 len><raw deflate> chains."""
    pos, best = 0, None
    while pos < len(volume) - 4:
        out, p = bytearray(), pos
        while p + 2 <= len(volume):
            n = struct.unpack_from("<H", volume, p)[0]
            if n == 0 or p + 2 + n > len(volume):
                break
            try:
                piece = zlib.decompressobj(-15).decompress(volume[p + 2:p + 2 + n])
            except zlib.error:
                break
            if not piece:
                break
            out += piece
            p += 2 + n
            if len(piece) < 0x2000:
                break
        if len(out) >= 4096:
            blob = bytes(out)
            if blob[:2] == b"MZ" and BANNER in blob and EXPECT_BUILD_UTF16 in blob:
                return blob
            pos = p
        else:
            pos += 1
    return best


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--dest", default=os.path.join(REPO, "tools", "mwccarm", "2004", "b56"))
    ap.add_argument("--work", default=os.path.join(REPO, ".cw2004work"))
    ap.add_argument("--keep", action="store_true", help="keep the work directory")
    a = ap.parse_args()

    for mod in ("py7zr", "pefile"):
        try:
            __import__(mod)
        except ImportError:
            sys.exit(f"pip install {mod}")

    os.makedirs(a.work, exist_ok=True)
    print("1/4 locating the installer inside the remote 7z")
    exe = pull_installer(a.work)
    print(f"      {os.path.basename(exe)}  {os.path.getsize(exe)/1e6:.1f} MB")

    print("2/4 splitting the self-extractor payload")
    parts = split_payload(exe)
    vol = parts.get("data2.cab")
    if not vol:
        sys.exit("data2.cab not found in payload")
    print(f"      data2.cab  {len(vol)/1e6:.1f} MB")

    print("3/4 decompressing to find mwccarm.exe")
    cc = carve_compiler(vol)
    if not cc:
        sys.exit("could not recover a build-0056 mwccarm.exe")

    import hashlib

    got256 = hashlib.sha256(cc).hexdigest()
    got1 = hashlib.sha1(cc).hexdigest()
    if len(cc) != EXPECT_SIZE or got256 != EXPECT_SHA256:
        sys.exit(f"recovered bytes do not match the known artifact\n"
                 f"  size   {len(cc)} (expected {EXPECT_SIZE})\n"
                 f"  sha256 {got256}\n  expected {EXPECT_SHA256}")
    print(f"      sha1   {got1}\n      sha256 {got256}")

    os.makedirs(a.dest, exist_ok=True)
    out = os.path.join(a.dest, "mwccarm.exe")
    open(out, "wb").write(cc)
    # the runtime DLLs are identical across builds; borrow the 1.2 set
    src = os.path.join(REPO, "tools", "mwccarm", "1.2", "sp2p3")
    for dll in ("ELFIO.dll", "MSL_All-DLL80_x86.dll", "lmgr8c.dll"):
        p = os.path.join(src, dll)
        if os.path.exists(p):
            shutil.copy(p, a.dest)

    print(f"4/4 verifying {out}")
    env = dict(os.environ,
               LM_LICENSE_FILE=os.path.join(REPO, "tools", "mwccarm", "license.dat"))
    r = subprocess.run([out, "-version"], capture_output=True, timeout=120, env=env)
    banner = r.stdout.decode("ascii", "replace").strip()
    print("      " + "\n      ".join(banner.splitlines()[:5]))
    if "build 56" not in banner:
        sys.exit("banner check failed")

    if not a.keep:
        shutil.rmtree(a.work, ignore_errors=True)
    print(f"\nOK. Use it as `--version 2004/b56` with tools/match.py.")


if __name__ == "__main__":
    main()
