"""consolidate_class.py — Consolidate single-function C++ files into unified class files.

Reads all _ZN<Class>*.c|cpp files for a given class name, extracts external declarations,
types, and function bodies, converts function signatures to native Class::Method syntax,
writes the consolidated src/<Class>.cpp file, verifies all functions against ROM bytes,
and safely removes the old single-function files.

Usage:
    python tools/consolidate_class.py --class Actor
"""
import argparse
import json
import os
import pathlib
import re
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))

import demangle as DM
import nearmiss_db as ND
import match as M

def collect_class_files(class_name):
    src_dir = REPO / "src"
    class_files = []
    prefix = f"_ZN{len(class_name)}{class_name}"

    for path in src_dir.iterdir():
        if path.suffix in (".c", ".cpp") and path.stem.startswith("_Z"):
            d = DM.demangle(path.stem)
            if d and d.get("class") == class_name:
                class_files.append((path, d))

    class_files.sort(key=lambda x: x[0].stem)
    return class_files

def extract_file_components(path, demangled_info):
    text = path.read_text(encoding="utf-8", errors="ignore")
    lines = text.splitlines()

    extern_decls = []
    extern_data = []
    func_body = []
    in_func = False
    func_name = demangled_info["method"]
    stem = path.stem

    for line in lines:
        sline = line.strip()
        if not sline or sline == "//cpp" or sline == "extern \"C\" {" or sline == "}":
            continue
        
        if sline.startswith("extern "):
            if "(" in sline and ")" in sline:
                extern_decls.append(sline)
            else:
                extern_data.append(sline)
        elif stem in sline and "(" in sline and (not in_func):
            in_func = True
            # Convert function signature header
            # Replace extern "C" void* _ZN5ActorC1Ev(char* c) -> void* Actor::Actor(char* c)
            # or void* _ZN5Actor... -> void* Actor::...
            header = line
            if "extern \"C\"" in header:
                header = header.replace("extern \"C\"", "").strip()
            # Replace mangled stem with Class::Method or Class::C1, etc.
            header = re.sub(r"\b" + re.escape(stem) + r"\b", f"{demangled_info['class']}::{demangled_info['method']}", header)
            func_body.append(header)
        elif in_func:
            func_body.append(line)

    return {
        "file": path,
        "stem": stem,
        "demangled": demangled_info,
        "extern_decls": extern_decls,
        "extern_data": extern_data,
        "func_body": "\n".join(func_body)
    }

def consolidate_class(class_name, apply_changes=False):
    files = collect_class_files(class_name)
    if not files:
        print(f"No standalone files found for class {class_name}")
        return

    print(f"Found {len(files)} standalone files for class {class_name}")

    all_extern_decls = set()
    all_extern_data = set()
    parsed_funcs = []

    for path, d in files:
        comp = extract_file_components(path, d)
        parsed_funcs.append(comp)
        all_extern_decls.update(comp["extern_decls"])
        all_extern_data.update(comp["extern_data"])

    # Build consolidated src/<Class>.cpp content
    cpp_lines = [
        "//cpp",
        "/*",
        f" * Consolidated class implementation: {class_name}",
        f" * Total methods: {len(parsed_funcs)}",
        " */",
        "",
        "extern \"C\" {"
    ]

    for decl in sorted(all_extern_decls):
        cpp_lines.append(f"    {decl}")

    cpp_lines.append("}")
    cpp_lines.append("")

    for data in sorted(all_extern_data):
        cpp_lines.append(data)

    cpp_lines.append("")
    cpp_lines.append(f"struct {class_name} {{")
    cpp_lines.append("    // Class member declarations")

    # Add member function declarations inside struct
    declared_methods = set()
    for comp in parsed_funcs:
        m_name = comp["demangled"]["method"]
        if m_name not in declared_methods:
            declared_methods.add(m_name)
            cpp_lines.append(f"    // {m_name}")

    cpp_lines.append("};")
    cpp_lines.append("")

    # Add method implementations
    for comp in parsed_funcs:
        cpp_lines.append(f"// --- {comp['demangled']['qualified']} ---")
        cpp_lines.append(comp["func_body"])
        cpp_lines.append("")

    target_file = REPO / "src" / f"{class_name}.cpp"
    content = "\n".join(cpp_lines)

    if not apply_changes:
        print(f"Preview: would write {len(content)} bytes to {target_file}")
        print("Run with --apply to perform consolidation.")
        return

    print(f"Writing consolidated file {target_file}...")
    target_file.write_text(content, encoding="utf-8")

    # Verify each function in the consolidated file
    print("Verifying consolidated functions against ROM bytes...")
    passed = 0
    failed = 0

    for comp in parsed_funcs:
        sym = comp["stem"]
        m = ND.resolve_name(sym)
        if not m:
            print(f"  [!] Missing metadata for {sym}")
            failed += 1
            continue

        addr, size, mod, _ = m
        addr_int = int(addr, 16) if isinstance(addr, str) else addr

        cmd = [
            sys.executable, str(REPO / "tools" / "match.py"),
            "--c", str(target_file),
            "--func", sym,
            "--addr", f"0x{addr_int:08x}",
            "--size", f"0x{size:x}",
            "--module", mod,
            "--strict-relocs",
            "--brief"
        ]
        r = subprocess.run(cmd, capture_output=True, text=True, cwd=str(REPO))
        if "MATCH" in r.stdout:
            passed += 1
        else:
            print(f"  [X] Failed: {sym}")
            failed += 1

    print(f"\nVerification: {passed}/{len(parsed_funcs)} PASSED, {failed} FAILED")

    if failed == 0 and apply_changes:
        print("All methods verified 100% byte-matched! Cleaning up old single-function files...")
        for path, _ in files:
            if path.exists():
                subprocess.run(["git", "rm", "-f", str(path)], check=True, cwd=str(REPO))
        print("Cleanup complete.")

def main():
    parser = argparse.ArgumentParser(description="Consolidate single-function C++ files into unified class file")
    parser.add_argument("--class", dest="cls_name", required=True, help="Target C++ class name")
    parser.add_argument("--apply", action="store_true", help="Perform consolidation and delete old standalone files")
    args = parser.parse_args()

    consolidate_class(args.cls_name, apply_changes=args.apply)

if __name__ == "__main__":
    main()
