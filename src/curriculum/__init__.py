"""Curriculum registry for C Language Review Trainer."""

from __future__ import annotations

from typing import Any

from .mod1_io import MODULE as MOD1
from .mod2_control import MODULE as MOD2
from .mod3_array_str import MODULE as MOD3
from .mod4_func_preproc import MODULE as MOD4
from .mod5_pointer_mem import MODULE as MOD5
from .mod6_struct_list import MODULE as MOD6
from .mod7_bitwise import MODULE as MOD7
from .mod8_file_io import MODULE as MOD8
from .mod9_special import MODULE as MOD9

MODULES: list[dict[str, Any]] = [
    MOD1,
    MOD2,
    MOD3,
    MOD4,
    MOD5,
    MOD6,
    MOD7,
    MOD8,
    MOD9,
]

EXERCISE_MAP: dict[str, dict[str, Any]] = {}
EXERCISE_ORDER: list[str] = []

for _mod in MODULES:
    for _ex in _mod["exercises"]:
        _ex["module_id"] = _mod["id"]
        _ex["module_title"] = _mod["title"]
        EXERCISE_MAP[_ex["id"]] = _ex
        EXERCISE_ORDER.append(_ex["id"])


def get_public_curriculum() -> list[dict[str, Any]]:
    """Return curriculum tree stripped of internal solutions (sent to UI)."""
    public_mods: list[dict[str, Any]] = []
    for mod in MODULES:
        pub_exercises = []
        for ex in mod["exercises"]:
            pub_ex = {
                "id": ex["id"],
                "code": ex["code"],
                "title": ex["title"],
                "kind": ex["kind"],
                "editable_default": ex["editable_default"],
                "summary": ex["summary"],
                "goal": ex["goal"],
                "param_docs": ex.get("param_docs", []),
                "guidance": ex.get("guidance", []),
                "sidebar_refs": ex.get("sidebar_refs", {}),
                "template": ex["template"],
                "sample_io": _extract_sample_io(ex),
                "test_count": len(ex.get("tests", [])),
                "module_id": ex["module_id"],
                "module_title": ex["module_title"],
            }
            pub_exercises.append(pub_ex)
        public_mods.append({
            "id": mod["id"],
            "title": mod["title"],
            "subtitle": mod["subtitle"],
            "badge": mod.get("badge", ""),
            "exercises": pub_exercises,
        })
    return public_mods


def _extract_sample_io(ex: dict[str, Any]) -> dict[str, Any]:
    tests = ex.get("tests", [])
    if not tests:
        return {}
    t0 = tests[0]
    sample: dict[str, Any] = {
        "stdin": t0.get("stdin", ""),
        "expected_stdout": t0.get("expected_stdout", ""),
    }
    if t0.get("input_files"):
        in_preview = {}
        for k, v in t0["input_files"].items():
            if isinstance(v, str):
                in_preview[k] = v
            elif v.get("type") == "text":
                in_preview[k] = v.get("content", "")
            elif v.get("type") == "binary_ints":
                in_preview[k] = f"[二进制 int 数组] {v['values']}"
            else:
                in_preview[k] = f"[二进制文件]"
        sample["input_files"] = in_preview
    if t0.get("expected_files"):
        out_preview = {}
        for k, v in t0["expected_files"].items():
            if isinstance(v, str):
                out_preview[k] = v
            elif v.get("type") == "text":
                out_preview[k] = v.get("content", "")
            elif v.get("type") == "binary_ints":
                out_preview[k] = f"[二进制 int 数组] {v['values']}"
            else:
                out_preview[k] = "[二进制文件]"
        sample["expected_files"] = out_preview
    return sample
