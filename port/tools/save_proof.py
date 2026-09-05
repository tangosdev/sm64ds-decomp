"""THE SAVE PROOF: the game's own save code, a real medium, and the file it wrote.

Run link100, lane SAVE. Two processes and a hex editor's worth of arithmetic.

WHAT IT PROVES, and every rung is on the SHIPPED binary with the ROM's own
SaveData translation units in it (port/slice_gate215.txt):

  FRESH        with no save file, the game's ReadFileData path answers "blank
               medium" and the marker bytes are absent. The negative control,
               and it runs FIRST so a stale file cannot make a later rung pass.
  NO WRITE     a read-only run does not create a save file. A medium is written
               when the game writes it and at no other time.
  WRITE        SM64DS_SAVE_PROBE=write seeds a ROM-shaped record into slot 0
               through SaveData::SaveFile -> SaveDataToCart -> the card driver
               -> port/ntr/backup.cpp, and a 8192-byte file appears.
  LAYOUT       that file is parsed here, from outside the game, against the ROM's
               own framing read off src/_ZN8SaveData14SaveDataToCartEPcjj.cpp:
                 record at slot<<7 and again at 0x1000 + slot<<7
                 +0  u16 checksum   +2  the 8-byte tag "ds mario"   +10 payload
                 checksum = sum of the tag bytes as a u16, then per payload
                 byte: rotate the u16 left one, xor the byte in
               Both copies are checked, the checksum is RECOMPUTED here rather
               than copied, and every byte of the chip outside the two records
               is still 0xFF -- so there is no host header, no host padding and
               no host checksum anywhere in the file.
  READBACK     a SECOND process reads the same file through ReadFileData and the
               marker bytes come back unchanged, and the payload on disk equals
               what ReadDataFromCart hands the game byte for byte.
  RE-FRESH     delete the file, read again, and the game is back to a blank
               medium. Deleting the save IS buying a new cartridge.
  FILE SELECT  the title route (SM64DS_SKIP_MENU) boots clean on a written chip
               and on a fresh one. The ROM's own integrity check distinguishes
               "blank" (2, install defaults, no dialog) from "damaged" (1, the
               "data is corrupted" dialog that soft-locks the title -- run mg16
               arc 2), and a fresh 0xFF chip must read as blank.

    python port/tools/save_proof.py [--root DIR] [--frames N] [--keep]

Quiet and muted through mp2_proof.env_base: no focus, minimised, volume 0.
"""
import argparse
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mp2_proof as M  # noqa: E402

TAG = b"ds mario"          # arm9 .rodata 0x0208ee50, via func_02042f68
CHIP = 0x2000              # device row 0xd01: a 64 Kbit EEPROM
HALF = CHIP // 2           # SaveDataToCart's mirror offset
REC_HDR = 10               # 2 checksum + 8 tag
FILE_LEN = 0x44            # FileSaveData, slots 0..2
ERASED = 0xFF

WRITE = re.compile(r"^\[savepx\] WRITE slot0 ok=(\d+) path=(\S+) bytes=([0-9a-f]+)", re.M)
READ = re.compile(r"^\[savepx\] READ slot0 rc=(-?\d+) survived=(\d+) path=(\S+) bytes=([0-9a-f]+)", re.M)
OK = [True]
EQ = re.compile(r"^\[savepx\] READ file_payload_equals_savedata=(\d+)", re.M)


def rom_checksum(payload):
    """The ROM's record checksum, transcribed from SaveDataToCart.

        sum = sum of the eight tag bytes, as a u16
        for each payload byte: sum = rotate_left_16(sum, 1) ^ byte
    """
    s = 0
    for b in TAG:
        s = (s + b) & 0xFFFF
    for b in payload:
        s = (((s << 1) & 0xFFFE) | ((s >> 15) & 1)) ^ (b & 0xFF)
        s &= 0xFFFF
    return s


def run(exe, root, rundir, name, mode, frames, extra=None):
    d = os.path.join(rundir, name)
    os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
    log = os.path.join(d, "run.log")
    e = M.env_base(root, d, name)
    e.pop("SM64DS_LEVEL", None)
    e["SM64DS_SCENE"] = "1"                       # the title, where the probe ticks
    e["SM64DS_WINDOW_SELFTEST"] = str(frames)
    e["SM64DS_SAVE_PATH"] = os.path.join(rundir, "sm64ds.sav")
    if mode:
        e["SM64DS_SAVE_PROBE"] = mode
        e["SM64DS_SAVE_PROBE_FRAME"] = "60"
    if extra:
        e.update(extra)
    rc = M.finish(M.spawn(exe, d, e, log), 600)
    return rc, M.text(log), log


def parse_record(img, off):
    """-> (ok, checksum_stored, checksum_computed, tag_ok, payload)"""
    crc = struct.unpack_from("<H", img, off)[0]
    tag = img[off + 2:off + 10]
    payload = img[off + REC_HDR:off + REC_HDR + FILE_LEN]
    return (tag == TAG, crc, rom_checksum(payload), tag, payload)


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    ap = argparse.ArgumentParser()
    ap.add_argument("--root", default=os.path.abspath(os.path.join(here, "..", "..")))
    ap.add_argument("--frames", default="140")
    ap.add_argument("--keep", action="store_true",
                    help="leave the proof's save file behind for inspection")
    args = ap.parse_args()

    root = os.path.abspath(args.root)
    exe = os.path.join(root, "build", "port", "walk_window.exe")
    rundir = os.path.join(root, "build", "save_proof")
    sav = os.path.join(rundir, "sm64ds.sav")
    os.makedirs(rundir, exist_ok=True)
    if os.path.exists(sav):
        os.remove(sav)

    if not os.path.exists(exe):
        print("save_proof: no %s -- build first" % exe)
        return 2
    print("save_proof: exe=%s\n            save=%s" % (exe, sav), flush=True)

    # ---- rung 1: FRESH, and NO WRITE ---------------------------------------
    rc, t, log = run(exe, root, rundir, "fresh", "read", args.frames)
    m = READ.search(t)
    OK[0] &= M.verdict(rc == 0, "FRESH  run exits 0 (rc=%d, %s)" % (rc, log))
    OK[0] &= M.verdict(m is not None, "FRESH  the read probe reported")
    if m:
        OK[0] &= M.verdict(int(m.group(2)) == 0,
                  "FRESH  markers absent on a blank medium (survived=%s)" % m.group(2))
        OK[0] &= M.verdict(int(m.group(1)) == 1,
                  "FRESH  ReadFileData says blank-and-defaulted, not damaged "
                  "(rc=%s; 0 would be the corrupted-save answer)" % m.group(1))
    OK[0] &= M.verdict(not os.path.exists(sav),
              "NOWRITE a read-only run created no save file")

    # ---- rung 2: WRITE ------------------------------------------------------
    rc, t, log = run(exe, root, rundir, "write", "write", args.frames)
    w = WRITE.search(t)
    OK[0] &= M.verdict(rc == 0, "WRITE  run exits 0 (rc=%d, %s)" % (rc, log))
    OK[0] &= M.verdict(w is not None, "WRITE  the write probe reported")
    seeded = bytes.fromhex(w.group(3)) if w else b""
    if w:
        OK[0] &= M.verdict(int(w.group(1)) == 1,
                  "WRITE  SaveData::SaveFile succeeded (ok=%s)" % w.group(1))
    OK[0] &= M.verdict(os.path.exists(sav), "WRITE  the save file exists")
    img = open(sav, "rb").read() if os.path.exists(sav) else b""
    OK[0] &= M.verdict(len(img) == CHIP,
              "WRITE  the file is exactly %d bytes, a bare cartridge image "
              "with no host header (got %d)" % (CHIP, len(img)))

    # ---- rung 3: LAYOUT, checked from outside the game ----------------------
    if len(img) == CHIP:
        for label, off in (("primary", 0), ("mirror", HALF)):
            tag_ok, crc, want, tag, payload = parse_record(img, off)
            OK[0] &= M.verdict(tag_ok, "LAYOUT %s record tag at 0x%04x is %r (got %r)"
                      % (label, off + 2, TAG, tag))
            OK[0] &= M.verdict(crc == want,
                      "LAYOUT %s record checksum at 0x%04x verifies "
                      "(stored 0x%04x, recomputed 0x%04x)" % (label, off, crc, want))
            OK[0] &= M.verdict(payload == seeded,
                      "LAYOUT %s payload at 0x%04x is the SaveData the game "
                      "handed the save path" % (label, off + REC_HDR))
        OK[0] &= M.verdict(img[0:REC_HDR + FILE_LEN] == img[HALF:HALF + REC_HDR + FILE_LEN],
                  "LAYOUT the ROM wrote both copies and they are identical")
        # Everything the ROM did not write is still an erased cell. This is the
        # rung that would catch a host header, a host trailer or host padding.
        holes = []
        for a in range(CHIP):
            in_rec = (a < REC_HDR + FILE_LEN) or (HALF <= a < HALF + REC_HDR + FILE_LEN)
            if not in_rec and img[a] != ERASED:
                holes.append(a)
        OK[0] &= M.verdict(not holes,
                  "LAYOUT every byte outside the two records is still 0xFF "
                  "(%d exceptions%s)" % (len(holes),
                                         "" if not holes else ", first 0x%04x" % holes[0]))

    # ---- rung 4: READBACK in a second process -------------------------------
    rc, t, log = run(exe, root, rundir, "read", "read", args.frames)
    m = READ.search(t)
    eq = EQ.search(t)
    OK[0] &= M.verdict(rc == 0, "READ   run exits 0 (rc=%d, %s)" % (rc, log))
    OK[0] &= M.verdict(m is not None, "READ   the read probe reported")
    if m:
        OK[0] &= M.verdict(int(m.group(2)) == 1,
                  "READ   the marker bytes survived a process boundary "
                  "(survived=%s)" % m.group(2))
        OK[0] &= M.verdict(m.group(4) == (w.group(3) if w else None),
                  "READ   ReadFileData returned the same 0x44 bytes SaveFile wrote")
    OK[0] &= M.verdict(eq is not None and int(eq.group(1)) == 1,
              "READ   the payload on disk equals what ReadDataFromCart hands "
              "the game, byte for byte")

    # ---- rung 5: FILE SELECT, on a written chip and then a fresh one --------
    rc, t, log = run(exe, root, rundir, "select_saved", None, args.frames,
                     {"SM64DS_SKIP_MENU": "1"})
    OK[0] &= M.verdict(rc == 0,
              "SELECT the file select boots clean on a written chip "
              "(rc=%d, %s)" % (rc, log))

    # ---- rung 6: RE-FRESH ---------------------------------------------------
    if os.path.exists(sav):
        os.remove(sav)
    rc, t, log = run(exe, root, rundir, "refresh", "read", args.frames)
    m = READ.search(t)
    OK[0] &= M.verdict(rc == 0, "REFRESH run exits 0 (rc=%d, %s)" % (rc, log))
    OK[0] &= M.verdict(m is not None and int(m.group(2)) == 0,
              "REFRESH deleting the file is a fresh cartridge: the markers are "
              "gone again")
    rc, t, log = run(exe, root, rundir, "select_fresh", None, args.frames,
                     {"SM64DS_SKIP_MENU": "1"})
    OK[0] &= M.verdict(rc == 0,
              "SELECT the file select boots clean on a FRESH chip too -- the "
              "0xFF medium reads as blank, not as damaged (rc=%d, %s)" % (rc, log))

    if not args.keep and os.path.exists(sav):
        os.remove(sav)
    print("")
    print("VERDICT:", "ALL GREEN" if OK[0] else "FAILED")
    return 0 if OK[0] else 1


if __name__ == "__main__":
    sys.exit(main())
