"""Coordinate decomp work via the tangos.dev claims lock service so multiple bots
do not grind the same address ranges. Programmatic sibling of CLAIMS.md.

Lock flow: try-lock a (module, start, end) span -> renew while working -> release when
done. The handle (display label) comes from CLAIMS_HANDLE / tools/claims_handle.txt /
the OS username -- the API key is what proves identity. Endpoints:
  POST /api/claims/try-lock     {module,start,end,handle,note} -> {id,...}
  POST /api/claims/{id}/renew   {handle}
  POST /api/claims/{id}/release {handle}
  GET  /api/claims/check?module=&start=&end=   (read-only preview)

The API key is NOT in this file: it comes from the CLAIMS_API_KEY env var or the
gitignored tools/claims_key.txt. Without a key, check() still works read-only;
lock/renew/release will 401 (coordinate via CLAIMS.md instead).

CLI:
  python tools/claims.py check    --module ov006 --start 0x020f0000 --end 0x020f0100
  python tools/claims.py lock     --module ov006 --start 0x020f0000 --end 0x020f0100 --note "matching"
  python tools/claims.py renew    --id <claim_id>
  python tools/claims.py release  --id <claim_id>
  python tools/claims.py lock-worklist  progress/wl.jsonl     # per-module spans, prints id map
  python tools/claims.py release-ids    id1 id2 ...
"""
import json
import os
import sys
import urllib.request
import urllib.parse

BASE = "https://tangos.dev"


def _load_local(env, fname):
    """env var, else the gitignored tools/<fname> sibling."""
    v = os.environ.get(env)
    if v:
        return v.strip()
    import pathlib
    p = pathlib.Path(__file__).resolve().parent / fname
    if p.is_file():
        return p.read_text(encoding="utf-8").strip()
    return None


def _default_handle():
    """The handle is display-only (the key proves identity), but it must identify
    THIS contributor: CLAIMS_HANDLE env / tools/claims_handle.txt, else OS user."""
    h = _load_local("CLAIMS_HANDLE", "claims_handle.txt")
    if h:
        return h
    import getpass
    try:
        return getpass.getuser()
    except Exception:
        return "anonymous"


HANDLE = _default_handle()
API_KEY = _load_local("CLAIMS_API_KEY", "claims_key.txt")


def _req(path, payload=None, method="GET"):
    data = json.dumps(payload).encode() if payload is not None else None
    headers = {"Content-Type": "application/json"} if data else {}
    if API_KEY:
        headers["X-Api-Key"] = API_KEY
    req = urllib.request.Request(BASE + path, data=data, headers=headers, method=method)
    try:
        with urllib.request.urlopen(req, timeout=25) as r:
            body = r.read().decode()
            return r.status, (json.loads(body) if body.strip() else {})
    except urllib.error.HTTPError as e:
        return e.code, {"error": e.read().decode()[:300]}
    except Exception as e:
        return None, {"error": str(e)}


def try_lock(module, start, end, handle=HANDLE, note=""):
    return _req("/api/claims/try-lock", {"module": module, "start": start, "end": end,
                                         "handle": handle, "note": note}, "POST")


def renew(claim_id, handle=HANDLE):
    return _req(f"/api/claims/{claim_id}/renew", {"handle": handle}, "POST")


def release(claim_id, handle=HANDLE):
    return _req(f"/api/claims/{claim_id}/release", {"handle": handle}, "POST")


def check(module, start, end):
    q = urllib.parse.urlencode({"module": module, "start": start, "end": end})
    return _req(f"/api/claims/check?{q}")


def _claim_id(res):
    """The claim id lives at res['claim']['id'] (try-lock 201 response)."""
    return res.get("claim", {}).get("id") if isinstance(res, dict) else None


def lock_worklist(path, handle=None, note=None):
    """Try-lock each worklist row's exact [addr, addr+size) range; rows whose range
    conflicts with someone else's active claim are DROPPED from the worklist file in
    place, so the caller only dispatches functions it holds. Returns
    {module: [claim_id, ...]}.

    Per-function granularity, not module spans: two agents running the same
    deterministic scheduler produce overlapping spans constantly, and a span skip
    throws away every function in the module for one conflict (a live 20-candidate
    batch locked 0/20 that way). try-lock is the atomic acquire; no check() first."""
    handle = handle or HANDLE
    import pathlib
    wl = pathlib.Path(path)
    rows = [json.loads(l) for l in wl.read_text(encoding="utf-8").splitlines() if l.strip()]
    held, kept, dropped = {}, [], []
    for r in rows:
        a = int(r["addr"], 16) if isinstance(r["addr"], str) else r["addr"]
        sz = int(r["size"], 16) if isinstance(r["size"], str) else r["size"]
        st, res = try_lock(r["module"], f"0x{a:08x}", f"0x{a + sz:08x}", handle,
                           note or f"crack-loop: {r['name'][:60]}")
        cid = _claim_id(res)
        if cid:
            held.setdefault(r["module"], []).append(cid)
            kept.append(r)
        else:
            who = "?"
            if isinstance(res, dict):
                cs = res.get("conflicts") or ([res["claim"]] if res.get("claim") else [])
                if cs:
                    who = cs[0].get("handle", "?")
            dropped.append((r["name"], who))
    if dropped:
        wl.write_text("".join(json.dumps(r) + "\n" for r in kept), encoding="utf-8")
        for name, who in dropped:
            print(f"  conflict: {name} (held by {who}) -- dropped from worklist")
    print(f"locked {sum(len(v) for v in held.values())}/{len(rows)} function ranges "
          f"across {len(held)} modules (handle {handle})")
    return held


def main():
    a = sys.argv
    if len(a) < 2:
        print(__doc__); return
    cmd = a[1]

    def opt(name, default=None):
        return a[a.index(name) + 1] if name in a else default

    if cmd == "check":
        print(json.dumps(check(opt("--module"), opt("--start"), opt("--end")), indent=1))
    elif cmd == "lock":
        print(json.dumps(try_lock(opt("--module"), opt("--start"), opt("--end"),
                                  opt("--handle", HANDLE), opt("--note", "")), indent=1))
    elif cmd == "renew":
        print(json.dumps(renew(opt("--id"), opt("--handle", HANDLE)), indent=1))
    elif cmd == "release":
        print(json.dumps(release(opt("--id"), opt("--handle", HANDLE)), indent=1))
    elif cmd == "lock-worklist":
        import pathlib
        ids = lock_worklist(a[2])
        pathlib.Path("progress/claims_active.json").write_text(json.dumps(ids))
        print("CLAIM_IDS " + json.dumps(ids))
    elif cmd == "release-active":
        import pathlib
        p = pathlib.Path("progress/claims_active.json")
        ids = json.loads(p.read_text()) if p.exists() else {}
        for mod, v in ids.items():   # per-function locks store a list per module
            for cid in (v if isinstance(v, list) else [v]):
                print(f"  release {mod} {cid}: {release(cid)}")
        p.unlink(missing_ok=True)
    elif cmd == "release-ids":
        for cid in a[2:]:
            print(cid, release(cid))
    else:
        print(__doc__)


if __name__ == "__main__":
    main()
