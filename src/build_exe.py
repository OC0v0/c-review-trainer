"""Build pure-native Win32 GUI single-file executable (c-review-trainer.exe) with embedded TCC C99."""

from __future__ import annotations

import argparse
import ctypes
import math
import os
import shutil
import struct
import subprocess
import sys
from pathlib import Path

from export_data import export_all

BUNDLE_MAGIC = b"CREVBNDL"


def _render_c99_dib(size: int) -> bytes:
    """Render a crisp anti-aliased 32-bit BGRA DIB icon (rounded blue badge with C99 glyph)."""
    ss = 4
    w = size
    GLYPHS = {
        "C": [
            ".###.",
            "#...#",
            "#....",
            "#....",
            "#....",
            "#...#",
            ".###.",
        ],
        "9": [
            ".###.",
            "#...#",
            "#...#",
            ".####",
            "....#",
            "#...#",
            ".###.",
        ],
    }
    if size <= 20:
        grid = GLYPHS["C"]
        gw, gh = 5, 7
        gx0, gy0, gx1, gy1 = 0.24, 0.20, 0.76, 0.80
    else:
        grid = [
            f"{GLYPHS['C'][r]}.{GLYPHS['9'][r]}.{GLYPHS['9'][r]}"
            for r in range(7)
        ]
        gw, gh = 17, 7
        gx0, gy0, gx1, gy1 = 0.12, 0.30, 0.88, 0.70

    radius = 0.22
    pixels_bgra = bytearray(w * w * 4)

    for py in range(w):
        for px in range(w):
            ar = ag = ab = aa = 0.0
            for sy in range(ss):
                for sx in range(ss):
                    u = (px + (sx + 0.5) / ss) / w
                    v = (py + (sy + 0.5) / ss) / w
                    dx = max(abs(u - 0.5) - (0.46 - radius), 0.0)
                    dy = max(abs(v - 0.5) - (0.46 - radius), 0.0)
                    dist = math.hypot(dx, dy) - radius
                    if dist <= 0.0:
                        r_c = 59.0 * (1.0 - v) + 29.0 * v
                        g_c = 130.0 * (1.0 - v) + 78.0 * v
                        b_c = 246.0 * (1.0 - v) + 216.0 * v
                        if dist > -0.045:
                            r_c = min(255.0, r_c * 0.86)
                            g_c = min(255.0, g_c * 0.86)
                            b_c = min(255.0, b_c * 0.86)
                        if gx0 <= u < gx1 and gy0 <= v < gy1:
                            col = int((u - gx0) / (gx1 - gx0) * gw)
                            row = int((v - gy0) / (gy1 - gy0) * gh)
                            if 0 <= row < gh and 0 <= col < gw and grid[row][col] == "#":
                                r_c, g_c, b_c = 255.0, 255.0, 255.0
                        ar += r_c
                        ag += g_c
                        ab += b_c
                        aa += 255.0
            n = ss * ss
            a = int(round(aa / n))
            if aa > 0:
                r = int(round(ar / (aa / 255.0)))
                g = int(round(ag / (aa / 255.0)))
                b = int(round(ab / (aa / 255.0)))
            else:
                r = g = b = 0
            dib_y = (w - 1) - py
            off = (dib_y * w + px) * 4
            pixels_bgra[off] = min(255, max(0, b))
            pixels_bgra[off + 1] = min(255, max(0, g))
            pixels_bgra[off + 2] = min(255, max(0, r))
            pixels_bgra[off + 3] = min(255, max(0, a))

    and_stride = ((w + 31) // 32) * 4
    and_mask = bytearray(and_stride * w)
    for dib_y in range(w):
        for px in range(w):
            alpha = pixels_bgra[(dib_y * w + px) * 4 + 3]
            if alpha < 128:
                and_mask[dib_y * and_stride + (px // 8)] |= 0x80 >> (px % 8)

    bih = struct.pack(
        "<IiiHHIIiiII",
        40,
        w,
        w * 2,
        1,
        32,
        0,
        len(pixels_bgra) + len(and_mask),
        0,
        0,
        0,
        0,
    )
    return bih + bytes(pixels_bgra) + bytes(and_mask)


def embed_app_icon(exe_path: Path) -> None:
    """Embed multi-size C99 icon (RT_ICON + RT_GROUP_ICON ID=1) into Win32 PE executable."""
    sizes = [16, 32, 48]
    dibs = [(sz, _render_c99_dib(sz)) for sz in sizes]

    k32 = ctypes.windll.kernel32
    k32.BeginUpdateResourceW.argtypes = [ctypes.c_wchar_p, ctypes.c_int]
    k32.BeginUpdateResourceW.restype = ctypes.c_void_p
    k32.UpdateResourceW.argtypes = [
        ctypes.c_void_p,
        ctypes.c_void_p,
        ctypes.c_void_p,
        ctypes.c_uint16,
        ctypes.c_void_p,
        ctypes.c_uint32,
    ]
    k32.UpdateResourceW.restype = ctypes.c_int
    k32.EndUpdateResourceW.argtypes = [ctypes.c_void_p, ctypes.c_int]
    k32.EndUpdateResourceW.restype = ctypes.c_int

    hupd = k32.BeginUpdateResourceW(str(exe_path), 0)
    if not hupd:
        print("Warning: BeginUpdateResourceW failed; skipping icon embed.")
        return

    RT_ICON = 3
    RT_GROUP_ICON = 14
    LANG_NEUTRAL = 0

    grp = bytearray(struct.pack("<HHH", 0, 1, len(dibs)))
    ok_all = True
    for idx, (sz, dib) in enumerate(dibs, start=1):
        buf = ctypes.create_string_buffer(dib, len(dib))
        if not k32.UpdateResourceW(hupd, ctypes.c_void_p(RT_ICON), ctypes.c_void_p(idx), LANG_NEUTRAL, buf, len(dib)):
            ok_all = False
            break
        grp.extend(struct.pack("<BBBBHHIH", sz, sz, 0, 0, 1, 32, len(dib), idx))

    if ok_all:
        gbuf = ctypes.create_string_buffer(bytes(grp), len(grp))
        if not k32.UpdateResourceW(hupd, ctypes.c_void_p(RT_GROUP_ICON), ctypes.c_void_p(1), LANG_NEUTRAL, gbuf, len(grp)):
            ok_all = False

    if not k32.EndUpdateResourceW(hupd, 0 if ok_all else 1) or not ok_all:
        print("Warning: EndUpdateResourceW failed; icon may not be embedded.")


def pack_bundle(entries: list[tuple[str, Path]]) -> bytes:
    buf = bytearray()
    buf.extend(struct.pack("<I", len(entries)))
    for rel_path, file_path in entries:
        rel_bytes = rel_path.replace("\\", "/").encode("utf-8")
        data = file_path.read_bytes()
        buf.extend(struct.pack("<I", len(rel_bytes)))
        buf.extend(rel_bytes)
        buf.extend(struct.pack("<I", len(data)))
        buf.extend(data)
    return bytes(buf)


def collect_dir(root: Path, prefix: str) -> list[tuple[str, Path]]:
    out: list[tuple[str, Path]] = []
    for p in sorted(root.rglob("*")):
        if p.is_file():
            rel = f"{prefix}/{p.relative_to(root).as_posix()}"
            out.append((rel, p))
    return out


def build(tcc_dir: Path, sci_dir: Path, out_exe: Path, build_dir: Path) -> None:
    src_dir = Path(__file__).resolve().parent
    data_dir = build_dir / "exported_data"
    stub_exe = build_dir / "c-review-trainer-gui-stub.exe"

    if not (tcc_dir / "tcc.exe").is_file():
        raise FileNotFoundError(f"TCC compiler not found in {tcc_dir}")
    if not (sci_dir / "Scintilla.dll").is_file() or not (sci_dir / "Lexilla.dll").is_file():
        raise FileNotFoundError(f"Scintilla.dll / Lexilla.dll not found in {sci_dir}")

    build_dir.mkdir(parents=True, exist_ok=True)
    if data_dir.exists():
        shutil.rmtree(data_dir)
    export_all(data_dir)

    gui_c = src_dir / "native" / "gui_app.c"
    cmd = [
        str(tcc_dir / "tcc.exe"),
        f"-B{tcc_dir}",
        f"-I{tcc_dir / 'include'}",
        f"-I{tcc_dir / 'include' / 'winapi'}",
        f"-I{src_dir / 'native'}",
        f"-L{tcc_dir / 'lib'}",
        "-Wl,-subsystem=gui",
        "-luser32",
        "-lgdi32",
        "-o",
        str(stub_exe),
        str(gui_c),
    ]
    print("Compiling pure native Win32 GUI application...")
    proc = subprocess.run(cmd, capture_output=True, text=True)
    if proc.returncode != 0:
        print("STDOUT:", proc.stdout)
        print("STDERR:", proc.stderr)
        sys.exit(proc.returncode)

    embed_app_icon(stub_exe)

    entries: list[tuple[str, Path]] = []
    entries.extend(collect_dir(tcc_dir, "tcc"))
    entries.append(("sci/Scintilla.dll", sci_dir / "Scintilla.dll"))
    entries.append(("sci/Lexilla.dll", sci_dir / "Lexilla.dll"))
    entries.extend(collect_dir(data_dir, "data"))

    bundle = pack_bundle(entries)
    stub_bytes = stub_exe.read_bytes()
    bundle_offset = len(stub_bytes)

    out_exe.parent.mkdir(parents=True, exist_ok=True)
    with out_exe.open("wb") as f:
        f.write(stub_bytes)
        f.write(bundle)
        f.write(struct.pack("<Q", bundle_offset))
        f.write(BUNDLE_MAGIC)

    size_mb = out_exe.stat().st_size / (1024 * 1024)
    print(f"Built {out_exe} ({size_mb:.2f} MB, {len(entries)} bundled files, gui_stub={bundle_offset} bytes)")


def main() -> None:
    repo_root = Path(__file__).resolve().parent.parent
    parser = argparse.ArgumentParser(description="Build standalone c-review-trainer.exe")
    parser.add_argument("--tcc-dir", type=Path, default=Path(os.environ.get("TCC_DIR", str(repo_root / "vendor" / "tcc"))))
    parser.add_argument("--sci-dir", type=Path, default=Path(os.environ.get("SCI_DIR", str(repo_root / "vendor" / "scintilla"))))
    parser.add_argument("--out", type=Path, default=repo_root / "output" / "c-review-trainer.exe")
    parser.add_argument("--build-dir", type=Path, default=repo_root / "build")
    args = parser.parse_args()
    build(args.tcc_dir, args.sci_dir, args.out, args.build_dir)


if __name__ == "__main__":
    main()
