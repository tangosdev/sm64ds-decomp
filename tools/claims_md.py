"""Read active claims so schedulers stop handing out taken work.

WHY THIS EXISTS
---------------
Claims are how contributors avoid grinding the same function twice, and they were the
only coordination the schedulers did NOT consult. `worklist.py` and `coddog.py` built
batches straight from the unmatched pool, so a function another contributor had marked
**active** went out to an agent anyway. That is not theoretical: a single ov002 batch
shipped five functions held by someone else, three of which came back as duplicate
matches of work already done.

TWO REGISTERS, MERGED
---------------------
The authoritative register is the claims service (tangos.json `data.claimsApi`,
scoped to this repo's board via `data.projectId`): agents try-lock through it and the
website writes to it. CLAIMS.md rows are the fallback register and are still honored,
so a hand-written row keeps protecting its range. Reading only the file was the gap
where an API lock never reached a scheduler and the same function went out twice.

The API is fetched best-effort with a short timeout: offline or with the service down,
the file rows still apply and a batch still builds. Set CLAIMS_NO_API=1 to skip the
network deliberately (offline batch generation without the timeout wait).

CLAIMS.md matching is by SYMBOL NAME and by ADDRESS, because rows are written by hand
and spell targets inconsistently:

    | ov098 func_ov098_0213b9d8 (0x0213b9d8, size 0x144) | ... | **active** | ...
    | arm9 Stage::InitResources (_ZN5Stage13InitResourcesEv @ 0x0202cc0c, ...) | ... | **active** |

Both the demangled name and the mangled symbol can appear, so an address hit is the
reliable signal and the name is the fallback. API claims are proper half-open
[start, end) ranges scoped to a module, so those match by module + address-in-range.

Only rows whose status is **active** or **partial** are treated as held. done,
released, merged and closed are free.

Usage:
    import claims_md
    held = claims_md.held_targets()                       # {"names", "addrs", "ranges", "rows"}
    if claims_md.is_held(held, name, addr, module=mod):   # skip this target
"""
import json
import os
import pathlib
import re
import urllib.request

REPO = pathlib.Path(__file__).resolve().parent.parent
CLAIMS = REPO / "CLAIMS.md"
DESCRIPTOR = REPO / "tangos.json"
API_TIMEOUT_S = 5

# A row is held only while it is active/partial; done/released/merged/closed are free.
HELD_STATUS = re.compile(r"\*\*(active|partial)\*\*", re.I)
# Any 0x-prefixed 8-hex-digit address anywhere in the row.
ADDR = re.compile(r"0x([0-9a-fA-F]{6,8})")
# Symbol-shaped tokens: mangled (_Z...), func_<addr>, __sinit_..., or a Class::Method pair.
SYM = re.compile(r"(_Z[A-Za-z0-9_]+|__sinit_[A-Za-z0-9_]+|func_(?:ov\d+_)?[0-9a-fA-F]{6,8}|[A-Za-z_]\w*::[A-Za-z_]\w*)")


def _api_ranges():
    """Active locks from the claims service as (module, start, end) tuples.

    Best-effort by design: any failure (offline, service down, malformed descriptor)
    returns [] so the schedulers keep working from CLAIMS.md alone. The descriptor is
    the single source for where the board lives - no second copy of the URL here.
    """
    if os.environ.get("CLAIMS_NO_API"):
        return []
    try:
        data = json.loads(DESCRIPTOR.read_text(encoding="utf-8")).get("data", {})
        api = data.get("claimsApi")
        if not api:
            return []
        project = data.get("projectId")
        url = api + (("&" if "?" in api else "?") + "project=" + project if project else "")
        with urllib.request.urlopen(url, timeout=API_TIMEOUT_S) as r:
            body = json.loads(r.read().decode("utf-8"))
        ranges = []
        for c in body.get("claims", []):
            try:
                ranges.append((str(c["module"]).lower(), int(str(c["start"]), 0), int(str(c["end"]), 0)))
            except (KeyError, TypeError, ValueError):
                continue
        return ranges
    except Exception:
        return []


def held_targets(path=None):
    """All live holds: the claims service's board merged with CLAIMS.md rows.

    Returns {"names": set[str], "addrs": set[int], "ranges": list[(module, start, end)],
    "rows": int}. rows counts both registers.
    """
    p = pathlib.Path(path) if path else CLAIMS
    names, addrs, rows = set(), set(), 0
    if p.exists():
        for line in p.read_text(encoding="utf-8", errors="ignore").splitlines():
            s = line.strip()
            if not s.startswith("|") or "---" in s:
                continue
            if not HELD_STATUS.search(s):
                continue
            rows += 1
            for m in ADDR.finditer(s):
                addrs.add(int(m.group(1), 16))
            for m in SYM.finditer(s):
                names.add(m.group(1))
    ranges = _api_ranges()
    return {"names": names, "addrs": addrs, "ranges": ranges, "rows": rows + len(ranges)}


def is_held(held, name=None, addr=None, module=None):
    """True if this target is claimed by someone. Address is the reliable signal.

    `module` scopes the API-range check (overlays reuse addresses, so a bare address
    can collide across modules). Without it, a range in ANY module matches - the
    conservative direction for a scheduler: skip a target rather than duplicate work.
    """
    a = None
    if addr is not None:
        try:
            a = addr if isinstance(addr, int) else int(str(addr), 0)
        except (TypeError, ValueError):
            a = None
    if a is not None:
        if a in held["addrs"]:
            return True
        mod = str(module).lower() if module else None
        for r_module, start, end in held.get("ranges", ()):
            if mod is not None and r_module != mod:
                continue
            if start <= a < end:
                return True
    if name and name in held["names"]:
        return True
    return False


if __name__ == "__main__":
    h = held_targets()
    print(f"claims: {h['rows']} held ({len(h['ranges'])} API locks, "
          f"{len(h['addrs'])} file addresses, {len(h['names'])} file names)")
    for mod, start, end in sorted(h["ranges"])[:20]:
        print(f"   {mod} 0x{start:08x}-0x{end:08x}")
    for a in sorted(h["addrs"])[:20]:
        print(f"   0x{a:08x}")
