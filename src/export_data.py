"""Export curriculum modules to unified JSON for the standalone native Win32 GUI executable."""

from __future__ import annotations

import json
import re
import struct
import sys
from pathlib import Path
from typing import Any

from curriculum import MODULES, get_public_curriculum


def _spec_to_native(spec: Any) -> dict[str, Any]:
    """Convert Python file spec (text / binary_ints / binary_struct / binary_hex) to text or hex."""
    if isinstance(spec, str):
        return {"type": "text", "content": spec.replace("\r\n", "\n")}
    ftype = spec.get("type", "text")
    if ftype == "text":
        return {"type": "text", "content": spec.get("content", "").replace("\r\n", "\n")}
    if ftype == "binary_hex":
        return {"type": "binary", "hex": spec["hex"].lower()}
    if ftype == "binary_ints":
        fmt = "<" + "i" * len(spec["values"])
        raw = struct.pack(fmt, *spec["values"])
        return {"type": "binary", "hex": raw.hex(), "desc": f"int数组({len(spec['values'])}项): {spec['values']}"}
    if ftype == "binary_struct":
        buf = bytearray()
        fmt = spec["fmt"]
        for row in spec["rows"]:
            packed_row = [v.encode("ascii") if isinstance(v, str) else v for v in row]
            buf.extend(struct.pack(fmt, *packed_row))
        return {"type": "binary", "hex": bytes(buf).hex(), "desc": f"二进制结构体({len(buf)}字节)"}
    raise ValueError(f"Unknown file spec type: {ftype}")


def _code_check_to_tokens(req: dict[str, str]) -> dict[str, Any]:
    pat = req["pattern"]
    msg = req["message"]
    mapping = {
        r"\bprintf\s*\(": ["printf("],
        r"%d": ["%d"],
        r"%\.2f": ["%.2f"],
        r"\bscanf\s*\(": ["scanf("],
        r"\bgetchar\s*\(": ["getchar("],
        r"\bputchar\s*\(": ["putchar("],
        r"\bswitch\s*\(": ["switch"],
        r"\bbreak\s*;": ["break"],
        r"\bcontinue\s*;": ["continue"],
        r"\bfgets\s*\(": ["fgets("],
        r"\bputs\s*\(": ["puts("],
        r"\bstrcmp\s*\(": ["strcmp("],
        r"\bstrcpy\s*\(": ["strcpy("],
        r"\bstrcat\s*\(": ["strcat("],
        r"\bstrlen\s*\(": ["strlen("],
        r"gcd\s*\([^)]*gcd\s*\(|return\s+gcd\s*\(": ["gcd("],
        r"\bstatic\b": ["static"],
        r"#\s*define\s+PI\b": ["#define", "PI"],
        r"#\s*define\s+SQR\s*\(": ["#define", "SQR("],
        r"#\s*define\s+MAX2\s*\(": ["#define", "MAX2("],
        r"int\s*\*\s*\w+": ["*"],
        r"&\s*x": ["&"],
        r"\bmalloc\s*\(": ["malloc("],
        r"\brealloc\s*\(": ["realloc("],
        r"\bfree\s*\(": ["free("],
        r"->": ["->"],
        r"\bfopen\s*\(": ["fopen("],
        r"\bfclose\s*\(": ["fclose("],
        r"\bfscanf\s*\(": ["fscanf("],
        r"\bfprintf\s*\(": ["fprintf("],
        r"\bfgetc\s*\(": ["fgetc("],
        r"\bfputc\s*\(": ["fputc("],
        r"\bfread\s*\(": ["fread("],
        r"\bfwrite\s*\(": ["fwrite("],
        r"\bfseek\s*\(": ["fseek("],
        r"\bftell\s*\(": ["ftell("],
    }
    tokens = mapping.get(pat)
    if tokens is None:
        lit = re.findall(r"[A-Za-z_][A-Za-z0-9_]*", pat)
        tokens = [lit[0]] if lit else []
    return {"tokens": tokens, "message": msg}


def export_all(out_dir: Path) -> None:
    out_dir.mkdir(parents=True, exist_ok=True)
    pub_mods = get_public_curriculum()

    # Build unified curriculum_all.json containing full metadata + tests + solutions
    ex_lookup: dict[str, dict[str, Any]] = {}
    for mod in MODULES:
        for ex in mod["exercises"]:
            ex_lookup[ex["id"]] = ex

    unified_mods: list[dict[str, Any]] = []
    total = 0
    for pmod in pub_mods:
        exs_out = []
        for pex in pmod["exercises"]:
            raw_ex = ex_lookup[pex["id"]]
            tests_out = []
            for idx, tc in enumerate(raw_ex.get("tests", []), start=1):
                in_files = [
                    {"name": fn, **_spec_to_native(sp)}
                    for fn, sp in tc.get("input_files", {}).items()
                ]
                exp_files = [
                    {"name": fn, **_spec_to_native(sp)}
                    for fn, sp in tc.get("expected_files", {}).items()
                ]
                tests_out.append({
                    "name": tc.get("name", f"测试点 {idx}"),
                    "stdin": tc.get("stdin", ""),
                    "has_stdout": "expected_stdout" in tc,
                    "expected_stdout": tc.get("expected_stdout", ""),
                    "timeout_ms": int(float(tc.get("timeout", 2.5)) * 1000),
                    "input_files": in_files,
                    "expected_files": exp_files,
                })
            merged = dict(pex)
            merged["solution"] = raw_ex["solution"]
            merged["code_checks"] = [_code_check_to_tokens(c) for c in raw_ex.get("code_checks", [])]
            merged["tests"] = tests_out
            exs_out.append(merged)
            total += 1
        unified_mods.append({
            "id": pmod["id"],
            "title": pmod["title"],
            "subtitle": pmod["subtitle"],
            "badge": pmod.get("badge", ""),
            "exercises": exs_out,
        })

    (out_dir / "curriculum_all.json").write_text(
        json.dumps(unified_mods, ensure_ascii=False, separators=(",", ":")),
        encoding="utf-8",
    )
    print(f"Exported {len(unified_mods)} modules / {total} exercises to {out_dir}")


if __name__ == "__main__":
    dest = Path(sys.argv[1]) if len(sys.argv) > 1 else Path("data")
    export_all(dest)
