"""Tiny read-only dashboard over the local agent-lock Redis instance. Zero
abstraction on purpose: each request queries Redis directly and renders plain HTML
strings -- no framework, no templating engine, no JS, no build step. Auto-refreshes
via <meta http-equiv="refresh">.

Two reports:
  1. Active locks  - every held lock (files and ranges), flat, TTL-sorted.
  2. Ranges by module - address-range locks grouped and sorted by start address,
     so an overlap or a gap is visible at a glance.

Usage:
    python tools/agentlock_web.py [--port 8787]
Then open http://127.0.0.1:8787/
"""
import argparse
import html
import json
import time
from collections import defaultdict
from http.server import BaseHTTPRequestHandler, HTTPServer

import agentlock  # sibling module: reused only for the Redis connection helper


def _gather(c):
    files, ranges = [], []
    for k in c.scan_iter("lock:*"):
        v = c.get(k)
        if v is None:
            continue
        ttl_ms = c.pttl(k)
        ttl_s = ttl_ms / 1000 if ttl_ms and ttl_ms > 0 else None
        try:
            data = json.loads(v)
        except ValueError:
            data = {}
        parts = k.split(":", 3)
        holder = data.get("holder", "?")
        note = data.get("note", "")
        acquired_at = data.get("acquired_at", "")
        if parts[1] == "file":
            files.append({"resource": parts[2] if len(parts) > 2 else k, "holder": holder,
                          "note": note, "ttl": ttl_s, "acquired_at": acquired_at})
        elif parts[1] == "addr":
            module = parts[2]
            span = parts[3].rsplit(":", 1)[0]  # drop the trailing random id
            s, e = span.split("-")
            ranges.append({"module": module, "start": int(s), "end": int(e),
                           "holder": holder, "note": note, "ttl": ttl_s,
                           "acquired_at": acquired_at})
    return files, ranges


def _esc(s):
    return html.escape(str(s), quote=True)


def _ttl_cell(ttl_s):
    if ttl_s is None:
        return "<td>?</td>"
    css = ' class="soon"' if ttl_s < 60 else ""
    return f"<td{css}>{ttl_s:.0f}s</td>"


def _report_active(files, ranges):
    rows = []
    for f in sorted(files, key=lambda r: r["ttl"] if r["ttl"] is not None else 1e18):
        rows.append("<tr><td>file</td><td colspan=2>{}</td><td>{}</td>{}<td>{}</td></tr>".format(
            _esc(f["resource"]), _esc(f["holder"]), _ttl_cell(f["ttl"]), _esc(f["note"])))
    for r in sorted(ranges, key=lambda r: r["ttl"] if r["ttl"] is not None else 1e18):
        span = f"0x{r['start']:08x}-0x{r['end']:08x}"
        rows.append("<tr><td>range</td><td>{}</td><td>{}</td><td>{}</td>{}<td>{}</td></tr>".format(
            _esc(r["module"]), span, _esc(r["holder"]), _ttl_cell(r["ttl"]), _esc(r["note"])))
    body = "\n".join(rows) or '<tr><td colspan="6"><em>nothing held</em></td></tr>'
    return f"""
<h2>Active locks ({len(files)} files, {len(ranges)} ranges)</h2>
<table>
  <tr><th>kind</th><th>module</th><th>resource</th><th>holder</th><th>ttl</th><th>note</th></tr>
  {body}
</table>
"""


def _report_by_module(ranges):
    by_module = defaultdict(list)
    for r in ranges:
        by_module[r["module"]].append(r)
    if not by_module:
        return "<h2>Ranges by module</h2><p><em>no address-range locks held</em></p>"
    sections = []
    for module in sorted(by_module):
        rows = sorted(by_module[module], key=lambda r: r["start"])
        trs = "\n".join(
            "<tr><td>0x{:08x}</td><td>0x{:08x}</td><td>{}</td>{}<td>{}</td></tr>".format(
                r["start"], r["end"], _esc(r["holder"]), _ttl_cell(r["ttl"]), _esc(r["note"]))
            for r in rows)
        sections.append(f"""
<h3>{_esc(module)}</h3>
<table>
  <tr><th>start</th><th>end</th><th>holder</th><th>ttl</th><th>note</th></tr>
  {trs}
</table>
""")
    return "<h2>Ranges by module</h2>" + "\n".join(sections)


PAGE = """<!doctype html>
<html><head>
<meta http-equiv="refresh" content="3">
<title>agent-lock</title>
<style>
  body {{ font-family: ui-monospace, monospace; margin: 2rem; background: #111; color: #ddd; }}
  h1 {{ font-size: 1.1rem; color: #888; }}
  h2 {{ margin-top: 2rem; }}
  h3 {{ color: #9cf; margin-bottom: 0.3rem; }}
  table {{ border-collapse: collapse; width: 100%; margin-bottom: 1rem; }}
  th, td {{ border: 1px solid #333; padding: 0.3rem 0.6rem; text-align: left; font-size: 0.9rem; }}
  th {{ background: #1c1c1c; }}
  td.soon {{ color: #f66; font-weight: bold; }}
  .meta {{ color: #666; font-size: 0.8rem; }}
</style>
</head><body>
<h1>sm64ds-decomp local agent-lock -- {holder_count} holder(s) active</h1>
<p class="meta">source: {redis_url} -- refreshes every 3s -- read-only</p>
{report1}
{report2}
</body></html>
"""


class Handler(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path != "/":
            self.send_response(404)
            self.end_headers()
            return
        try:
            c = agentlock._client()
            files, ranges = _gather(c)
            holders = {r["holder"] for r in files} | {r["holder"] for r in ranges}
            page = PAGE.format(
                holder_count=len(holders),
                redis_url=_esc(agentlock.REDIS_URL),
                report1=_report_active(files, ranges),
                report2=_report_by_module(ranges),
            )
            status = 200
        except agentlock.LockError as e:
            page = f"<!doctype html><pre>can't reach redis: {_esc(e)}</pre>"
            status = 503
        body = page.encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "text/html; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def log_message(self, fmt, *args):
        pass  # quiet; this is a local dev dashboard, not a service to audit


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--port", type=int, default=8787)
    args = ap.parse_args()
    srv = HTTPServer(("127.0.0.1", args.port), Handler)
    print(f"agent-lock dashboard: http://127.0.0.1:{args.port}/  (ctrl-c to stop)")
    try:
        srv.serve_forever()
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    main()
