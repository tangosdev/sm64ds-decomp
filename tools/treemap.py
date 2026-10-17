"""WinDirStat-style treemap of decomp progress.

Every arm-mode function in the game becomes a rectangle sized by its byte count.
Matched functions are green, unmatched are gray. Rectangles are grouped into a
bordered region per module (arm9-main plus every overlay), and each region is
labeled with the module name and its matched byte percentage.

Data comes straight from the existing pipeline helpers so the classification is
identical to the real matcher:
    sweep.funcs(mod)      -> arm-mode (name, addr, size) inside the module image
    ledger.matched_set()  -> set of (module_label, addr) that are matched

Output is a single self-contained HTML file with no external dependencies, so
it opens offline by double-click. The squarified layout is computed here in
Python and emitted as absolutely-positioned SVG rects.

Usage:
    python tools/treemap.py
    python tools/treemap.py --out some/where/treemap.html
"""
import argparse
import html
import pathlib
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
# modules/sweep/ledger need extracted/ ROM files and the local ledger; they are
# imported lazily inside main() so --from-db (the CI path) works without a ROM.

REPO = pathlib.Path(__file__).resolve().parent.parent

# Tasteful palette. Green for matched, muted gray for unmatched.
MATCHED_FILL = "#3fa75a"
MATCHED_STROKE = "#2c7a41"
UNMATCHED_FILL = "#4a4d52"
UNMATCHED_STROKE = "#393b3f"
REGION_BORDER = "#888c92"
REGION_BG = "#1d1f22"
PAGE_BG = "#141517"
TEXT = "#e6e7e9"
SUBTEXT = "#a8abb0"


def gather_from_db(db_path):
    """Same structure as gather(), but sourced from a chaos-db.json produced by
    tools/chaos_db_ci.py - committed data only, so CI can render the treemap."""
    import json
    db = json.loads(pathlib.Path(db_path).read_text(encoding="utf-8"))
    by_mod = {}
    for f in db["functions"]:
        by_mod.setdefault(f["module"], []).append(f)
    mods = []
    for label in sorted(by_mod):
        recs = [{"name": f["name"], "addr": f["addr"], "size": f["size"],
                 "matched": f["matched"]} for f in sorted(by_mod[label], key=lambda f: f["addr"])]
        m_bytes = sum(r["size"] for r in recs)
        done = [r for r in recs if r["matched"]]
        mods.append({"label": label, "recs": recs, "bytes": m_bytes,
                     "done_bytes": sum(r["size"] for r in done),
                     "done_n": len(done), "n": len(recs)})
    return mods


def gather():
    """All records as dicts plus per-module aggregates."""
    import modules as MOD
    import sweep as SW
    mods = []
    for mod in MOD.modules():
        label = "arm9" if mod["name"] == "main" else mod["name"]
        recs = []
        m_bytes = 0
        done_bytes = 0
        done_n = 0
        done = GATHER_DONE
        for name, addr, size in SW.funcs(mod):
            matched = (label, addr) in done
            recs.append({"name": name, "addr": addr, "size": size, "matched": matched})
            m_bytes += size
            if matched:
                done_bytes += size
                done_n += 1
        if recs:
            mods.append({
                "label": label,
                "recs": recs,
                "bytes": m_bytes,
                "done_bytes": done_bytes,
                "done_n": done_n,
                "n": len(recs),
            })
    return mods


def squarify(items, x, y, w, h):
    """Squarified treemap. items: list of dicts with a positive 'value'.

    Returns list of (item, rx, ry, rw, rh). Classic Bruls/Huizing/van Wijk
    algorithm. Values are scaled to the area of the box.
    """
    items = [it for it in items if it["value"] > 0]
    if not items:
        return []
    total = sum(it["value"] for it in items)
    if total <= 0 or w <= 0 or h <= 0:
        return []
    scale = (w * h) / total
    scaled = [(it, it["value"] * scale) for it in items]

    out = []
    rx, ry, rw, rh = x, y, w, h
    i = 0
    n = len(scaled)
    while i < n:
        short = min(rw, rh)
        row = [scaled[i]]
        i += 1
        # Grow the row while it improves the worst aspect ratio.
        while i < n:
            trial = row + [scaled[i]]
            if _worst(trial, short) <= _worst(row, short):
                row = trial
                i += 1
            else:
                break
        out += _layout_row(row, rx, ry, rw, rh)
        row_sum = sum(a for _, a in row)
        if rw <= rh:
            # row was laid horizontally; consume vertical space
            dh = row_sum / rw if rw else 0
            ry += dh
            rh -= dh
        else:
            dw = row_sum / rh if rh else 0
            rx += dw
            rw -= dw
    return out


def _worst(row, short):
    s = sum(a for _, a in row)
    if s <= 0 or short <= 0:
        return float("inf")
    side = s / short
    mx = max(a for _, a in row)
    mn = min(a for _, a in row)
    return max((short * short * mx) / (s * s), (s * s) / (short * short * mn)) if mn else float("inf")


def _layout_row(row, rx, ry, rw, rh):
    s = sum(a for _, a in row)
    out = []
    if s <= 0:
        return out
    if rw <= rh:
        # lay across the top, full width, height = s / rw
        dh = s / rw
        cx = rx
        for it, a in row:
            cw = a / dh if dh else 0
            out.append((it, cx, ry, cw, dh))
            cx += cw
    else:
        # lay down the left, full height, width = s / rh
        dw = s / rh
        cy = ry
        for it, a in row:
            ch = a / dw if dw else 0
            out.append((it, rx, cy, dw, ch))
            cy += ch
    return out


CANVAS_W = 1600
CANVAS_H = 1000


def build_layout(mods):
    """Compute the squarified treemap once. Returns (regions, rects, stats)
    where regions/rects are lists of SVG fragment strings positioned inside a
    CANVAS_W x CANVAS_H box, and stats holds the totals both renderers print."""
    PAD = 4            # gap between module regions
    LABEL_H = 22       # header strip inside each module region
    INNER_PAD = 3      # gap between label strip and the function rects

    total_bytes = sum(m["bytes"] for m in mods)
    total_done_bytes = sum(m["done_bytes"] for m in mods)
    total_n = sum(m["n"] for m in mods)
    total_done_n = sum(m["done_n"] for m in mods)

    mod_items = [{"value": m["bytes"], "mod": m} for m in mods]
    mod_boxes = squarify(mod_items, 0, 0, CANVAS_W, CANVAS_H)

    rects = []      # svg strings for function rects
    regions = []    # svg strings for region borders + labels
    rect_count = 0

    for it, mx, my, mw, mh in mod_boxes:
        m = it["mod"]
        # shrink for the padding gap
        bx, by = mx + PAD / 2, my + PAD / 2
        bw, bh = mw - PAD, mh - PAD
        if bw <= 1 or bh <= 1:
            continue
        pct = 100.0 * m["done_bytes"] / m["bytes"] if m["bytes"] else 0.0

        regions.append(
            f'<rect x="{bx:.2f}" y="{by:.2f}" width="{bw:.2f}" height="{bh:.2f}" '
            f'fill="{REGION_BG}" stroke="{REGION_BORDER}" stroke-width="1" />')

        # label strip (only if region is tall/wide enough to bother)
        show_label = bh > LABEL_H + 8 and bw > 36
        inner_y = by + (LABEL_H if show_label else 0) + INNER_PAD
        inner_h = bh - (LABEL_H if show_label else 0) - INNER_PAD * 2
        inner_x = bx + INNER_PAD
        inner_w = bw - INNER_PAD * 2

        if show_label:
            label_txt = f'{m["label"]}  {pct:.1f}%'
            regions.append(
                f'<text x="{bx + 5:.2f}" y="{by + 15:.2f}" font-size="12" '
                f'font-family="Segoe UI, system-ui, sans-serif" fill="{TEXT}" '
                f'font-weight="600">{html.escape(label_txt)}</text>')

        if inner_w <= 1 or inner_h <= 1:
            continue

        fitems = [{"value": max(r["size"], 1), "rec": r} for r in m["recs"]]
        fboxes = squarify(fitems, inner_x, inner_y, inner_w, inner_h)
        for fit, fx, fy, fw, fh in fboxes:
            r = fit["rec"]
            if fw <= 0.4 or fh <= 0.4:
                continue
            if r["matched"]:
                fill, stroke = MATCHED_FILL, MATCHED_STROKE
            else:
                fill, stroke = UNMATCHED_FILL, UNMATCHED_STROKE
            tip = (f'{r["name"]}\nmodule: {m["label"]}\naddr: 0x{r["addr"]:08x}\n'
                   f'size: {r["size"]} bytes\nmatched: {"yes" if r["matched"] else "no"}')
            rects.append(
                f'<rect x="{fx:.2f}" y="{fy:.2f}" width="{fw:.2f}" height="{fh:.2f}" '
                f'fill="{fill}" stroke="{stroke}" stroke-width="0.4">'
                f'<title>{html.escape(tip)}</title></rect>')
            rect_count += 1

    fn_pct = 100.0 * total_done_n / total_n if total_n else 0.0
    by_pct = 100.0 * total_done_bytes / total_bytes if total_bytes else 0.0
    stats = {"rect_count": rect_count, "total_done_n": total_done_n,
             "total_n": total_n, "total_done_bytes": total_done_bytes,
             "total_bytes": total_bytes, "fn_pct": fn_pct, "by_pct": by_pct,
             "n_mods": len(mods)}
    return regions, rects, stats


def render(mods, out_path):
    regions, rects, st = build_layout(mods)
    rect_count = st["rect_count"]
    total_done_n, total_n = st["total_done_n"], st["total_n"]
    total_done_bytes, total_bytes = st["total_done_bytes"], st["total_bytes"]
    fn_pct, by_pct = st["fn_pct"], st["by_pct"]

    doc = f"""<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8" />
<meta name="viewport" content="width=device-width, initial-scale=1" />
<title>SM64DS decomp progress treemap</title>
<style>
  html, body {{ margin: 0; background: {PAGE_BG}; color: {TEXT};
    font-family: "Segoe UI", system-ui, sans-serif; }}
  .wrap {{ max-width: {CANVAS_W + 40}px; margin: 0 auto; padding: 20px; }}
  nav {{ display: flex; gap: 20px; margin-bottom: 20px; font-size: 14px; }}
  nav a {{ color: #91c5ff; }}
  h1 {{ font-size: 20px; font-weight: 600; margin: 0 0 4px; }}
  .stats {{ display: flex; gap: 28px; flex-wrap: wrap; margin: 10px 0 16px;
    font-size: 14px; color: {SUBTEXT}; }}
  .stats b {{ color: {TEXT}; font-weight: 600; }}
  .legend {{ display: flex; gap: 18px; align-items: center; font-size: 13px;
    color: {SUBTEXT}; margin-bottom: 14px; }}
  .sw {{ display: inline-block; width: 13px; height: 13px; border-radius: 2px;
    margin-right: 6px; vertical-align: -2px; }}
  svg rect:hover {{ stroke: #fff; stroke-width: 1; }}
  .note {{ font-size: 12px; color: {SUBTEXT}; margin-top: 14px; line-height: 1.5; }}
</style>
</head>
<body>
<div class="wrap">
  <nav aria-label="Progress views"><strong aria-current="page">Function treemap</strong><a href="tu-treemap.html">TU treemap</a></nav>
  <h1>SM64DS decompilation progress</h1>
  <div class="stats">
    <div>Functions matched: <b>{total_done_n:,}</b> / {total_n:,}
      (<b>{fn_pct:.2f}%</b>)</div>
    <div>Code bytes matched: <b>{total_done_bytes:,}</b> / {total_bytes:,}
      (<b>{by_pct:.2f}%</b>)</div>
    <div>Modules: <b>{len(mods)}</b></div>
    <div>Rectangles: <b>{rect_count:,}</b></div>
  </div>
  <div class="legend">
    <span><span class="sw" style="background:{MATCHED_FILL}"></span>matched</span>
    <span><span class="sw" style="background:{UNMATCHED_FILL}"></span>unmatched</span>
    <span>each rectangle is one function, sized by byte count</span>
  </div>
  <svg viewBox="0 0 {CANVAS_W} {CANVAS_H}" width="100%"
       xmlns="http://www.w3.org/2000/svg" shape-rendering="crispEdges">
    {''.join(regions)}
    {''.join(rects)}
  </svg>
  <div class="note">
    Rectangles cover the arm-mode functions that live inside each module image,
    the same universe the matcher sweeps. Hover any rectangle for its name,
    module, address, size, and match status. Modules are sized by total byte
    mass, so the largest overlays dominate. Regenerate with
    <code>python tools/treemap.py</code>.
  </div>
</div>
</body>
</html>
"""
    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text(doc, encoding="utf-8")
    return rect_count, total_done_n, total_n, total_done_bytes, total_bytes, fn_pct, by_pct


def render_svg(mods, out_path):
    """Emit a standalone static .svg with a stats header baked in, suitable for
    embedding directly in the GitHub README as an image (GitHub renders SVG but
    runs no script, so this is a fixed snapshot with no hover tooltips)."""
    regions, rects, st = build_layout(mods)
    HEADER_H = 92
    total_h = CANVAS_H + HEADER_H
    title = "SM64DS decompilation progress"
    line2 = (f'Functions matched: {st["total_done_n"]:,} / {st["total_n"]:,} '
             f'({st["fn_pct"]:.2f}%)   |   '
             f'Code bytes matched: {st["total_done_bytes"]:,} / '
             f'{st["total_bytes"]:,} ({st["by_pct"]:.2f}%)   |   '
             f'Modules: {st["n_mods"]}')
    line3 = "Each rectangle is one function, sized by byte count. Green = matched, gray = unmatched."

    doc = f"""<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {CANVAS_W} {total_h}" \
width="100%" font-family="Segoe UI, system-ui, sans-serif" shape-rendering="crispEdges">
  <rect x="0" y="0" width="{CANVAS_W}" height="{total_h}" fill="{PAGE_BG}" />
  <text x="6" y="30" font-size="24" font-weight="600" fill="{TEXT}">{html.escape(title)}</text>
  <text x="6" y="58" font-size="15" fill="{TEXT}">{html.escape(line2)}</text>
  <text x="6" y="80" font-size="13" fill="{SUBTEXT}">{html.escape(line3)}</text>
  <g transform="translate(0,{HEADER_H})">
    {''.join(regions)}
    {''.join(rects)}
  </g>
</svg>
"""
    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text(doc, encoding="utf-8")
    return st


GATHER_DONE = None


def main():
    global GATHER_DONE
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", default=str(REPO / "progress" / "treemap.html"),
                    help="interactive HTML output path")
    ap.add_argument("--svg", default=None,
                    help="also emit a standalone static SVG (for the README image)")
    ap.add_argument("--from-db", default=None,
                    help="render from a chaos-db.json (no ROM or ledger needed; the CI path)")
    args = ap.parse_args()

    if args.from_db:
        mods = gather_from_db(args.from_db)
    else:
        import ledger as L
        GATHER_DONE = L.matched_set()    # green = byte-exact matches ONLY
        mods = gather()
    out_path = pathlib.Path(args.out)
    rc, dn, tn, db, tb, fp, bp = render(mods, out_path)
    print(f"wrote {out_path}")
    print(f"  rectangles : {rc:,}")
    print(f"  functions  : {dn:,} / {tn:,}  ({fp:.2f}%)")
    print(f"  code bytes : {db:,} / {tb:,}  ({bp:.2f}%)")
    print(f"  modules    : {len(mods)}")
    if args.svg:
        svg_path = pathlib.Path(args.svg)
        render_svg(mods, svg_path)
        print(f"wrote {svg_path}")


if __name__ == "__main__":
    main()
