#!/usr/bin/env python3
"""Measure formatting invariance in ignored copies, never in production source.

Compiles every selected complete production source before formatting, with only
its source formatted, and with both sources and selected shared headers formatted.
All phases use the SAME source/include paths, production pin/flags, and no fallback
include directory. Whole raw ELF equality includes data, symbols, relocations and
emission order. This is an experiment, not a new ROM-match or relocation verdict.
"""
import argparse
import concurrent.futures
import hashlib
import json
import os
import pathlib
import re
import subprocess
import sys

import build_pin
import rombuild
import source_style

ROOT = pathlib.Path(__file__).resolve().parent.parent


def digest(raw):
    return hashlib.sha256(raw).hexdigest()


def order_signature(raw):
    text = raw.decode('utf-8')
    return {
        'includes': re.findall(r'^[ \t]*#[ \t]*include[ \t]*([<"][^>"\r\n]+[>"])', text, re.M),
        'pragmas': [re.sub(r'\s+', '', s) for s in
                    re.findall(r'^\s*#\s*pragma\s*(.*)', text, re.M)],
        'symbols': re.findall(r'//\s*@symbol\s+(\S+)', text),
        'cpp_marker': text.startswith('//cpp'),
    }


def compile_input(rel, mirror, output):
    original = ROOT / rel
    version = build_pin.version_for(rel)
    if version is None:
        return {'status': 'control_error', 'error': 'unknown production pin'}
    flags = build_pin.flags_for(original)
    exe = rombuild.MW / version / 'mwccarm.exe'
    target = output / pathlib.Path(rel).with_suffix('.o')
    target.parent.mkdir(parents=True, exist_ok=True)
    cmd = [*rombuild.launcher(), str(exe), *flags.split(), '-i',
           str(mirror / 'include'), '-c', str(mirror / rel), '-o', str(target)]
    try:
        p = subprocess.run(cmd, capture_output=True, cwd=mirror,
                           env=dict(os.environ, LM_LICENSE_FILE=str(rombuild.LICENSE)), timeout=120)
        if p.returncode or not target.is_file():
            return {'status': 'compile_error', 'compiler': version,
                    'error': (p.stdout + p.stderr).decode('utf-8', errors='replace')[:1800]}
        raw = target.read_bytes()
        return {'status': 'compiled', 'compiler': version, 'flags': flags,
                'bytes': len(raw), 'sha256': digest(raw)}
    except (OSError, subprocess.TimeoutExpired) as exc:
        return {'status': 'compile_error', 'compiler': version, 'error': str(exc)}


def format_input(executable, config, rel, raw):
    p = subprocess.run([executable, '--style=file:' + str(config),
                        '--assume-filename=' + rel], input=raw, capture_output=True)
    if p.returncode:
        raise RuntimeError(rel + ': ' + p.stderr.decode('utf-8', errors='replace'))
    return p.stdout


def summarize(rows):
    counts = {}
    for row in rows.values():
        for phase, result in row.items():
            counts.setdefault(phase, {})
            status = result['status']
            counts[phase][status] = counts[phase].get(status, 0) + 1
    return counts


def audit(output, executable, sources=None, headers=None, jobs=16, mode="guarded"):
    # Never overwrite a directory or let caller paths mutate the real checkout.
    output = output.resolve()
    if not output.is_relative_to((ROOT / 'build').resolve()) or output.exists():
        raise ValueError('output must be a NEW directory below this worktree build/')
    if jobs < 1 or mode not in {'guarded', 'raw'}:
        raise ValueError('jobs must be positive and mode must be guarded or raw')
    source_style.formatter_version(executable)
    enrolled = set(rombuild.enrolled())
    sources = sorted(enrolled if sources is None else set(sources))
    if not sources or not set(sources) <= enrolled:
        raise ValueError('select at least one currently enrolled production source')
    tracked = subprocess.check_output(['git', '-C', str(ROOT), 'ls-files', '-z',
                                       '--', 'src', 'include']).decode('utf-8').split('\0')
    tracked = [s for s in tracked if s]
    all_headers = {s for s in tracked if pathlib.Path(s).suffix in {'.h', '.hpp'}}
    headers = sorted(all_headers if headers is None else set(headers))
    if not set(headers) <= all_headers:
        raise ValueError('header selection must name tracked source/include headers')
    output.mkdir(parents=True)
    mirror = output / 'inputs'
    originals = {}
    for rel in tracked:
        raw = (ROOT / rel).read_bytes()
        dest = mirror / rel
        dest.parent.mkdir(parents=True, exist_ok=True)
        dest.write_bytes(raw)
        originals[rel] = raw
    config = output / 'style.yaml'
    config.write_bytes((ROOT / '.clang-format').read_bytes())
    report = {'schema': 1, 'source_commit': subprocess.check_output(
        ['git', '-C', str(ROOT), 'rev-parse', 'HEAD'], text=True).strip(),
        'formatter_version': source_style.FORMAT_VERSION, 'mode': mode,
        'runner_sha256': digest(pathlib.Path(__file__).read_bytes()),
        'implementation_sha256': {name: digest((ROOT / 'tools' / name).read_bytes())
                                  for name in ('source_style.py', 'delaunder.py', 'msvc_arms.py',
                                               'build_pin.py', 'rombuild.py')},
        'config_sha256': digest(config.read_bytes()), 'sources': sources, 'headers': headers,
        'method': 'same paths, production compiler/flags, shadow include only, whole raw ELF comparison',
        'coverage_limit': 'Not a new ROM build, source acceptance, or destination validation. Raw object equality proves invariance only for these exact inputs.',
        'header_changed_count': 0, 'source_changed_count': 0,
        'order_signature_changes': [], 'deferred': {},
        'input_sha256': {s: digest(originals[s]) for s in sorted(set(sources) | set(headers))},
        'results': {s: {} for s in sources}}
    safety = source_style.FormatSafety(ROOT)

    def save():
        report['summary'] = summarize(report['results'])
        (output / 'report.json').write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')

    for phase in ['control', 'formatted_sources', 'formatted_sources_and_headers']:
        if phase != 'control':
            targets = sources if phase == 'formatted_sources' else headers
            with concurrent.futures.ThreadPoolExecutor(max_workers=jobs) as pool:
                def prepare(rel):
                    text = originals[rel].decode('utf-8')
                    raw = format_input(executable, config, rel, originals[rel])
                    if mode == 'guarded' and source_style.tokens(text) != source_style.tokens(raw.decode('utf-8')):
                        return rel, originals[rel], ['preprocessing token spelling/order would change']
                    return rel, raw, []
                # Context analysis is sequential: its include graph/cache is shared.
                eligible, formatted = [], []
                for rel in targets:
                    risks = safety.reasons(rel, originals[rel].decode('utf-8')) if mode == 'guarded' else []
                    if risks:
                        formatted.append((rel, originals[rel], risks))
                    else:
                        eligible.append(rel)
                formatted.extend(pool.map(prepare, eligible))
            for rel, raw, risks in formatted:
                if risks:
                    report['deferred'][rel] = risks
                (mirror / rel).write_bytes(raw)
                if raw != originals[rel]:
                    key = 'source_changed_count' if phase == 'formatted_sources' else 'header_changed_count'
                    report[key] += 1
                if order_signature(raw) != order_signature(originals[rel]):
                    report['order_signature_changes'].append(rel)
        print(f'{phase}: compiling {len(sources)} production inputs', flush=True)
        with concurrent.futures.ThreadPoolExecutor(max_workers=jobs) as pool:
            tasks = {pool.submit(compile_input, rel, mirror, output / phase): rel for rel in sources}
            for index, future in enumerate(concurrent.futures.as_completed(tasks), 1):
                rel = tasks[future]
                result = future.result()
                control = report['results'][rel].get('control', {})
                if phase != 'control' and result['status'] == 'compiled':
                    if control.get('status') != 'compiled':
                        result['status'] = 'unverified_control'
                    else:
                        result['status'] = ('identical' if result['sha256'] == control['sha256']
                                            else 'different')
                report['results'][rel][phase] = result
                if index % 100 == 0:
                    print(f'  {index}/{len(sources)}', flush=True)
        save()
        print(json.dumps(report['summary'][phase]), flush=True)
    report['complete'] = True
    save()
    return report


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--output', required=True, type=pathlib.Path)
    ap.add_argument('--clang-format', required=True)
    ap.add_argument('--sources', nargs='+')
    ap.add_argument('--headers', nargs='*')
    ap.add_argument('-j', type=int, default=16)
    ap.add_argument('--mode', choices=['guarded', 'raw'], default='guarded')
    args = ap.parse_args()
    try:
        r = audit(args.output, args.clang_format, args.sources, args.headers, args.j, args.mode)
    except (ValueError, OSError, source_style.StyleError, RuntimeError) as exc:
        print('format-audit: ERROR:', exc, file=sys.stderr)
        return 2
    print('report:', args.output / 'report.json')
    bad = any(v.get('status') not in {'compiled', 'identical'} for phases in r['results'].values()
              for v in phases.values()) or bool(r['order_signature_changes'])
    return int(bad)


if __name__ == '__main__':
    sys.exit(main())
