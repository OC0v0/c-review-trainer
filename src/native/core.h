/*
 * Core engine for standalone C Language Review Trainer:
 * - Self-unpacking single-file payload (TCC32 + curriculum_all.json)
 * - Writable directory self-probing (handles Windows Defender / AppData ACLs & Chinese paths)
 * - In-memory TCC C99 runner (-run) with stdin/stdout/stderr pipes, timeout & File I/O sandboxing
 * - Local JSON progress persistence
 */
#ifndef CREV_CORE_H
#define CREV_CORE_H

#define _CRT_SECURE_NO_WARNINGS
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <windows.h>

#ifndef CP_ACP
#define CP_ACP 0
#endif
#ifndef CP_UTF8
#define CP_UTF8 65001
#endif
WINBASEAPI int WINAPI MultiByteToWideChar(UINT, DWORD, LPCSTR, int, LPWSTR, int);
WINBASEAPI int WINAPI WideCharToMultiByte(UINT, DWORD, LPCWSTR, int, LPSTR, int, LPCSTR, LPBOOL);

#include "json.h"

#define APP_VERSION_STR "1.1.0"
#define BUNDLE_MAGIC "CREVBNDL"

typedef struct {
    wchar_t exe_dir[MAX_PATH];
    wchar_t rt_dir[MAX_PATH];
    wchar_t tcc_dir[MAX_PATH];
    wchar_t work_root[MAX_PATH];
    wchar_t appdata_dir[MAX_PATH];
    wchar_t progress_path[MAX_PATH];
    char tcc_short[MAX_PATH];
    int work_probed;
    JVal *modules;     /* J_ARR of modules, each having exercises[] */
    JVal *progress;    /* J_OBJ: passed, drafts, unlocked, settings */
    CRITICAL_SECTION lock;
    volatile LONG case_seq;
} CoreState;

static CoreState g_core;

/* ---------- UTF-8 <-> UTF-16 ---------- */
static void utf8_to_wide(const char *src, wchar_t *dst, int dst_cch) {
    if (!dst || dst_cch <= 0) return;
    if (!src || !src[0]) { dst[0] = L'\0'; return; }
    int n = MultiByteToWideChar(CP_UTF8, 0, src, -1, dst, dst_cch);
    if (n <= 0) dst[0] = L'\0';
}

static wchar_t *utf8_to_wide_alloc(const char *src) {
    if (!src) {
        wchar_t *e = (wchar_t *)malloc(sizeof(wchar_t));
        if (e) e[0] = L'\0';
        return e;
    }
    int n = MultiByteToWideChar(CP_UTF8, 0, src, -1, NULL, 0);
    if (n <= 0) n = 1;
    wchar_t *w = (wchar_t *)malloc((size_t)n * sizeof(wchar_t));
    if (w) {
        if (MultiByteToWideChar(CP_UTF8, 0, src, -1, w, n) <= 0) w[0] = L'\0';
    }
    return w;
}

static char *wide_to_utf8_alloc(const wchar_t *src) {
    if (!src) return xstrdup("");
    int n = WideCharToMultiByte(CP_UTF8, 0, src, -1, NULL, 0, NULL, NULL);
    if (n <= 0) return xstrdup("");
    char *u = (char *)malloc((size_t)n);
    if (u) {
        if (WideCharToMultiByte(CP_UTF8, 0, src, -1, u, n, NULL, NULL) <= 0) u[0] = '\0';
    }
    return u;
}

static void wide_to_short_ascii(const wchar_t *src, char *dst, int dst_bytes) {
    wchar_t shortw[MAX_PATH];
    DWORD r = GetShortPathNameW(src, shortw, MAX_PATH);
    if (r > 0 && r < MAX_PATH) {
        WideCharToMultiByte(CP_ACP, 0, shortw, -1, dst, dst_bytes, NULL, NULL);
    } else {
        WideCharToMultiByte(CP_ACP, 0, src, -1, dst, dst_bytes, NULL, NULL);
    }
}

/* ---------- Filesystem ---------- */
static void ensure_dirs_w(const wchar_t *dir_path) {
    wchar_t tmp[MAX_PATH];
    wcsncpy(tmp, dir_path, MAX_PATH - 1);
    tmp[MAX_PATH - 1] = L'\0';
    size_t len = wcslen(tmp);
    for (size_t i = 0; i < len; i++) {
        if (tmp[i] == L'/') tmp[i] = L'\\';
    }
    for (size_t i = 3; i < len; i++) {
        if (tmp[i] == L'\\') {
            tmp[i] = L'\0';
            CreateDirectoryW(tmp, NULL);
            tmp[i] = L'\\';
        }
    }
    CreateDirectoryW(tmp, NULL);
}

static void ensure_parent_dir_w(const wchar_t *file_path) {
    wchar_t tmp[MAX_PATH];
    wcsncpy(tmp, file_path, MAX_PATH - 1);
    tmp[MAX_PATH - 1] = L'\0';
    wchar_t *slash = wcsrchr(tmp, L'\\');
    wchar_t *fslash = wcsrchr(tmp, L'/');
    if (fslash && (!slash || fslash > slash)) slash = fslash;
    if (slash) {
        *slash = L'\0';
        ensure_dirs_w(tmp);
    }
}

static void rmtree_w(const wchar_t *dir) {
    wchar_t pat[MAX_PATH];
    _snwprintf(pat, MAX_PATH, L"%ls\\*", dir);
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW(pat, &fd);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) continue;
            wchar_t child[MAX_PATH];
            _snwprintf(child, MAX_PATH, L"%ls\\%ls", dir, fd.cFileName);
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                rmtree_w(child);
            } else {
                SetFileAttributesW(child, FILE_ATTRIBUTE_NORMAL);
                DeleteFileW(child);
            }
        } while (FindNextFileW(h, &fd));
        FindClose(h);
    }
    RemoveDirectoryW(dir);
}

static char *read_file_all_w(const wchar_t *path, size_t *out_len) {
    FILE *fp = _wfopen(path, L"rb");
    if (!fp) return NULL;
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    if (sz < 0) { fclose(fp); return NULL; }
    rewind(fp);
    char *buf = (char *)malloc((size_t)sz + 1);
    if (!buf) { fclose(fp); return NULL; }
    size_t rd = sz > 0 ? fread(buf, 1, (size_t)sz, fp) : 0;
    fclose(fp);
    buf[rd] = '\0';
    if (out_len) *out_len = rd;
    return buf;
}

static int write_file_all_w(const wchar_t *path, const void *data, size_t len) {
    ensure_parent_dir_w(path);
    FILE *fp = _wfopen(path, L"wb");
    if (!fp) return 0;
    size_t wr = len > 0 ? fwrite(data, 1, len, fp) : 0;
    fclose(fp);
    return wr == len;
}

static int probe_dir_writable_w(const wchar_t *dir) {
    ensure_dirs_w(dir);
    wchar_t probe[MAX_PATH];
    _snwprintf(probe, MAX_PATH, L"%ls\\_probe_%lu.tmp", dir, (unsigned long)GetCurrentProcessId());
    if (!write_file_all_w(probe, "OK", 2)) return 0;
    DeleteFileW(probe);
    return 1;
}

/* ---------- Hex & Text Helpers ---------- */
static int hex_nibble(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static unsigned char *hex_to_bytes(const char *hex, size_t *out_len) {
    size_t n = strlen(hex) / 2;
    unsigned char *buf = (unsigned char *)malloc(n ? n : 1);
    for (size_t i = 0; i < n; i++) {
        int hi = hex_nibble(hex[i * 2]);
        int lo = hex_nibble(hex[i * 2 + 1]);
        buf[i] = (unsigned char)(((hi < 0 ? 0 : hi) << 4) | (lo < 0 ? 0 : lo));
    }
    if (out_len) *out_len = n;
    return buf;
}

static char *bytes_to_hex(const unsigned char *buf, size_t len) {
    static const char H[] = "0123456789abcdef";
    char *out = (char *)malloc(len * 2 + 1);
    for (size_t i = 0; i < len; i++) {
        out[i * 2] = H[(buf[i] >> 4) & 0xF];
        out[i * 2 + 1] = H[buf[i] & 0xF];
    }
    out[len * 2] = '\0';
    return out;
}

static char *ensure_utf8_text(const char *raw, size_t len) {
    int valid = 1;
    for (size_t i = 0; i < len; ) {
        unsigned char c = (unsigned char)raw[i];
        if (c < 0x80) i++;
        else if ((c & 0xE0) == 0xC0 && i + 1 < len && (((unsigned char)raw[i + 1]) & 0xC0) == 0x80) i += 2;
        else if ((c & 0xF0) == 0xE0 && i + 2 < len && (((unsigned char)raw[i + 1]) & 0xC0) == 0x80 && (((unsigned char)raw[i + 2]) & 0xC0) == 0x80) i += 3;
        else if ((c & 0xF8) == 0xF0 && i + 3 < len) i += 4;
        else { valid = 0; break; }
    }
    if (valid) {
        char *r = (char *)malloc(len + 1);
        memcpy(r, raw, len);
        r[len] = '\0';
        return r;
    }
    int wlen = MultiByteToWideChar(CP_ACP, 0, raw, (int)len, NULL, 0);
    if (wlen <= 0) return xstrdup("");
    wchar_t *wbuf = (wchar_t *)malloc((size_t)(wlen + 1) * sizeof(wchar_t));
    MultiByteToWideChar(CP_ACP, 0, raw, (int)len, wbuf, wlen);
    wbuf[wlen] = L'\0';
    int ulen = WideCharToMultiByte(CP_UTF8, 0, wbuf, wlen, NULL, 0, NULL, NULL);
    char *ubuf = (char *)malloc((size_t)(ulen + 1));
    WideCharToMultiByte(CP_UTF8, 0, wbuf, wlen, ubuf, ulen, NULL, NULL);
    ubuf[ulen] = '\0';
    free(wbuf);
    return ubuf;
}

static char *normalize_text(const char *src) {
    if (!src) return xstrdup("");
    StrBuf sb;
    sb_init(&sb);
    size_t len = strlen(src);
    size_t i = 0;
    while (i < len) {
        size_t line_start = i;
        while (i < len && src[i] != '\n' && src[i] != '\r') i++;
        size_t line_end = i;
        while (line_end > line_start && (src[line_end - 1] == ' ' || src[line_end - 1] == '\t')) line_end--;
        sb_append_mem(&sb, src + line_start, line_end - line_start);
        if (i < len) {
            if (src[i] == '\r' && i + 1 < len && src[i + 1] == '\n') i += 2;
            else i++;
            sb_append_char(&sb, '\n');
        }
    }
    while (sb.len > 0 && sb.data[sb.len - 1] == '\n') sb.data[--sb.len] = '\0';
    return sb.data;
}

static char *strip_c_comments(const char *src) {
    StrBuf sb;
    sb_init(&sb);
    size_t n = strlen(src);
    for (size_t i = 0; i < n; ) {
        if (src[i] == '/' && i + 1 < n && src[i + 1] == '/') {
            i += 2;
            while (i < n && src[i] != '\n') i++;
            sb_append_char(&sb, ' ');
        } else if (src[i] == '/' && i + 1 < n && src[i + 1] == '*') {
            i += 2;
            while (i + 1 < n && !(src[i] == '*' && src[i + 1] == '/')) i++;
            if (i + 1 < n) i += 2;
            sb_append_char(&sb, ' ');
        } else {
            sb_append_char(&sb, src[i++]);
        }
    }
    return sb.data;
}

/* ---------- TCC Subprocess Execution ---------- */
typedef struct {
    HANDLE hRead;
    StrBuf buf;
} PipeReaderArgs;

static DWORD WINAPI core_pipe_reader(LPVOID param) {
    PipeReaderArgs *a = (PipeReaderArgs *)param;
    char tmp[4096];
    DWORD got = 0;
    while (ReadFile(a->hRead, tmp, sizeof(tmp), &got, NULL) && got > 0) {
        if (a->buf.len < 512 * 1024) sb_append_mem(&a->buf, tmp, got);
    }
    return 0;
}

typedef struct {
    int ok_launch;
    int timed_out;
    DWORD exit_code;
    DWORD run_ms;
    char *stdout_utf8;
    char *stderr_utf8;
} ProcResult;

static void proc_result_free(ProcResult *r) {
    free(r->stdout_utf8);
    free(r->stderr_utf8);
    r->stdout_utf8 = r->stderr_utf8 = NULL;
}

static ProcResult run_tcc_in_dir(const wchar_t *src_path, const wchar_t *cwd_dir, const char *stdin_data, DWORD timeout_ms) {
    ProcResult res;
    memset(&res, 0, sizeof(res));

    char src_short[MAX_PATH];
    wide_to_short_ascii(src_path, src_short, sizeof(src_short));

    wchar_t tcc_exe[MAX_PATH], tcc_b[MAX_PATH], src_w[MAX_PATH];
    _snwprintf(tcc_exe, MAX_PATH, L"%ls\\tcc.exe", g_core.tcc_dir);
    MultiByteToWideChar(CP_ACP, 0, g_core.tcc_short, -1, tcc_b, MAX_PATH);
    MultiByteToWideChar(CP_ACP, 0, src_short, -1, src_w, MAX_PATH);

    wchar_t cmdline[2048];
    _snwprintf(
        cmdline, 2048,
        L"\"%ls\" \"-B%ls\" \"-I%ls\\include\" \"-I%ls\\include\\winapi\" \"-L%ls\\lib\" -run \"%ls\"",
        tcc_exe, tcc_b, tcc_b, tcc_b, tcc_b, src_w
    );

    SECURITY_ATTRIBUTES sa;
    memset(&sa, 0, sizeof(sa));
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;

    HANDLE in_rd = NULL, in_wr = NULL, out_rd = NULL, out_wr = NULL, err_rd = NULL, err_wr = NULL;
    if (!CreatePipe(&in_rd, &in_wr, &sa, 0) ||
        !CreatePipe(&out_rd, &out_wr, &sa, 0) ||
        !CreatePipe(&err_rd, &err_wr, &sa, 0)) {
        return res;
    }
    SetHandleInformation(in_wr, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(out_rd, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(err_rd, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOW si;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = in_rd;
    si.hStdOutput = out_wr;
    si.hStdError = err_wr;

    PROCESS_INFORMATION pi;
    memset(&pi, 0, sizeof(pi));

    DWORD t0 = GetTickCount();
    BOOL created = CreateProcessW(NULL, cmdline, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, cwd_dir, &si, &pi);
    CloseHandle(in_rd);
    CloseHandle(out_wr);
    CloseHandle(err_wr);

    if (!created) {
        CloseHandle(in_wr); CloseHandle(out_rd); CloseHandle(err_rd);
        return res;
    }

    res.ok_launch = 1;
    PipeReaderArgs out_arg, err_arg;
    out_arg.hRead = out_rd; sb_init(&out_arg.buf);
    err_arg.hRead = err_rd; sb_init(&err_arg.buf);

    HANDLE th_out = CreateThread(NULL, 0, core_pipe_reader, &out_arg, 0, NULL);
    HANDLE th_err = CreateThread(NULL, 0, core_pipe_reader, &err_arg, 0, NULL);

    if (stdin_data && stdin_data[0] != '\0') {
        DWORD total = (DWORD)strlen(stdin_data);
        DWORD written_total = 0;
        while (written_total < total) {
            DWORD w = 0;
            if (!WriteFile(in_wr, stdin_data + written_total, total - written_total, &w, NULL) || w == 0) break;
            written_total += w;
        }
    }
    CloseHandle(in_wr);

    DWORD wait_r = WaitForSingleObject(pi.hProcess, timeout_ms);
    res.run_ms = GetTickCount() - t0;
    if (wait_r == WAIT_TIMEOUT) {
        res.timed_out = 1;
        TerminateProcess(pi.hProcess, 1);
        WaitForSingleObject(pi.hProcess, 1000);
        res.exit_code = (DWORD)-1;
    } else {
        GetExitCodeProcess(pi.hProcess, &res.exit_code);
    }

    if (th_out) { WaitForSingleObject(th_out, 1500); CloseHandle(th_out); }
    if (th_err) { WaitForSingleObject(th_err, 1500); CloseHandle(th_err); }
    CloseHandle(out_rd); CloseHandle(err_rd);
    CloseHandle(pi.hThread); CloseHandle(pi.hProcess);

    res.stdout_utf8 = ensure_utf8_text(out_arg.buf.data ? out_arg.buf.data : "", out_arg.buf.len);
    res.stderr_utf8 = ensure_utf8_text(err_arg.buf.data ? err_arg.buf.data : "", err_arg.buf.len);
    sb_free(&out_arg.buf);
    sb_free(&err_arg.buf);
    return res;
}

static int is_tcc_compile_error(DWORD exit_code, const char *stderr_text) {
    if (exit_code == 0 || !stderr_text || !stderr_text[0]) return 0;
    if (strstr(stderr_text, "tcc: error:") != NULL) return 1;
    if (strstr(stderr_text, "main.c:") != NULL && strstr(stderr_text, "error:") != NULL) return 1;
    if (strstr(stderr_text, "error:") != NULL && strstr(stderr_text, "include") != NULL) return 1;
    return 0;
}

static int probe_tcc_writable_dir(const wchar_t *dir) {
    wchar_t pdir[MAX_PATH], src[MAX_PATH], outf[MAX_PATH];
    _snwprintf(pdir, MAX_PATH, L"%ls\\probe_%lu", dir, (unsigned long)GetTickCount());
    ensure_dirs_w(pdir);
    _snwprintf(src, MAX_PATH, L"%ls\\main.c", pdir);
    const char *code = "#include <stdio.h>\nint main(void){FILE *f=fopen(\"ok.tmp\",\"w\");if(!f)return 1;fputc(65,f);fclose(f);return 0;}\n";
    if (!write_file_all_w(src, code, strlen(code))) { rmtree_w(pdir); return 0; }
    ProcResult r = run_tcc_in_dir(src, pdir, "", 4000);
    _snwprintf(outf, MAX_PATH, L"%ls\\ok.tmp", pdir);
    int ok = r.ok_launch && !r.timed_out && r.exit_code == 0 && (GetFileAttributesW(outf) != INVALID_FILE_ATTRIBUTES);
    proc_result_free(&r);
    rmtree_w(pdir);
    return ok;
}

/* ---------- Progress Store ---------- */
static JVal *core_make_default_progress(void) {
    JVal *root = j_new(J_OBJ);
    j_obj_set(root, "passed", j_new(J_OBJ));
    j_obj_set(root, "drafts", j_new(J_OBJ));
    j_obj_set(root, "unlocked", j_new(J_OBJ));
    JVal *settings = j_new(J_OBJ);
    j_obj_set(settings, "quest_mode", j_bool(0));
    j_obj_set(root, "settings", settings);
    return root;
}

static void core_save_progress(void) {
    EnterCriticalSection(&g_core.lock);
    StrBuf sb;
    sb_init(&sb);
    j_serialize_into(g_core.progress, &sb);
    write_file_all_w(g_core.progress_path, sb.data, sb.len);
    sb_free(&sb);
    LeaveCriticalSection(&g_core.lock);
}

static void core_load_progress(void) {
    g_core.progress = core_make_default_progress();
    size_t len = 0;
    char *raw = read_file_all_w(g_core.progress_path, &len);
    if (!raw) return;
    JVal *loaded = j_parse(raw, len);
    free(raw);
    if (!loaded || loaded->type != J_OBJ) { j_free(loaded); return; }
    const char *keys[3] = {"passed", "drafts", "unlocked"};
    for (int i = 0; i < 3; i++) {
        JVal *sub = j_obj_get(loaded, keys[i]);
        if (sub && sub->type == J_OBJ) {
            StrBuf sb; sb_init(&sb);
            j_serialize_into(sub, &sb);
            JVal *cp = j_parse(sb.data, sb.len);
            sb_free(&sb);
            if (cp) j_obj_set(g_core.progress, keys[i], cp);
        }
    }
    JVal *s = j_obj_get(loaded, "settings");
    if (s && s->type == J_OBJ) {
        JVal *gs = j_obj_get(g_core.progress, "settings");
        for (size_t i = 0; i < s->obj_len; i++) {
            const char *k = s->pairs[i].key;
            JVal *v = s->pairs[i].val;
            if (!k || !v) continue;
            if (v->type == J_BOOL) j_obj_set(gs, k, j_bool(v->bval));
            else if (v->type == J_NUM) j_obj_set(gs, k, j_num(v->nval));
            else if (v->type == J_STR) j_obj_set(gs, k, j_str(v->sval));
        }
    }
    j_free(loaded);
}

/* ---------- Bundle Unpacker & Init ---------- */
static int unpack_bundle_from_file(const wchar_t *bundle_exe_path, const wchar_t *dest_root) {
    FILE *fp = _wfopen(bundle_exe_path, L"rb");
    if (!fp) return 0;
    fseek(fp, 0, SEEK_END);
    long total_sz = ftell(fp);
    if (total_sz < 16) { fclose(fp); return 0; }
    fseek(fp, total_sz - 16, SEEK_SET);
    uint64_t offset = 0;
    char magic[8] = {0};
    if (fread(&offset, sizeof(uint64_t), 1, fp) != 1 || fread(magic, 1, 8, fp) != 8) {
        fclose(fp); return 0;
    }
    if (memcmp(magic, BUNDLE_MAGIC, 8) != 0 || offset >= (uint64_t)total_sz) {
        fclose(fp); return 0;
    }
    fseek(fp, (long)offset, SEEK_SET);
    uint32_t count = 0;
    if (fread(&count, sizeof(uint32_t), 1, fp) != 1) { fclose(fp); return 0; }

    for (uint32_t i = 0; i < count; i++) {
        uint32_t plen = 0, dlen = 0;
        if (fread(&plen, sizeof(uint32_t), 1, fp) != 1 || plen == 0 || plen >= 512) { fclose(fp); return 0; }
        char rel[512];
        if (fread(rel, 1, plen, fp) != plen) { fclose(fp); return 0; }
        rel[plen] = '\0';
        for (uint32_t k = 0; k < plen; k++) if (rel[k] == '/') rel[k] = '\\';
        if (fread(&dlen, sizeof(uint32_t), 1, fp) != 1) { fclose(fp); return 0; }
        char *buf = (char *)malloc(dlen ? dlen : 1);
        if (!buf) { fclose(fp); return 0; }
        if (dlen > 0 && fread(buf, 1, dlen, fp) != dlen) { free(buf); fclose(fp); return 0; }
        wchar_t wrel[MAX_PATH], out_path[MAX_PATH];
        utf8_to_wide(rel, wrel, MAX_PATH);
        _snwprintf(out_path, MAX_PATH, L"%ls\\%ls", dest_root, wrel);
        int ok = write_file_all_w(out_path, buf, dlen);
        free(buf);
        if (!ok) { fclose(fp); return 0; }
    }
    fclose(fp);
    return 1;
}

static int core_init(const wchar_t *override_bundle_exe) {
    memset(&g_core, 0, sizeof(g_core));
    InitializeCriticalSection(&g_core.lock);

    wchar_t exe_path[MAX_PATH], exe_dir[MAX_PATH];
    if (override_bundle_exe && override_bundle_exe[0]) {
        wcsncpy(exe_path, override_bundle_exe, MAX_PATH - 1);
        exe_path[MAX_PATH - 1] = L'\0';
    } else {
        GetModuleFileNameW(NULL, exe_path, MAX_PATH);
    }
    wcsncpy(exe_dir, exe_path, MAX_PATH - 1);
    exe_dir[MAX_PATH - 1] = L'\0';
    wchar_t *sl = wcsrchr(exe_dir, L'\\');
    if (sl) *sl = L'\0';

    DWORD pid = GetCurrentProcessId();
    wchar_t tmp[MAX_PATH] = {0}, lad[MAX_PATH] = {0};
    GetTempPathW(MAX_PATH, tmp);
    size_t tl = wcslen(tmp);
    if (tl > 0 && tmp[tl - 1] == L'\\') tmp[tl - 1] = L'\0';
    GetEnvironmentVariableW(L"LOCALAPPDATA", lad, MAX_PATH);

    wchar_t cand[3][MAX_PATH];
    int ncand = 0;
    _snwprintf(cand[ncand++], MAX_PATH, L"%ls\\.crev_rt_%lu", exe_dir, (unsigned long)pid);
    if (tmp[0]) _snwprintf(cand[ncand++], MAX_PATH, L"%ls\\crev_rt_%lu", tmp, (unsigned long)pid);
    if (lad[0]) _snwprintf(cand[ncand++], MAX_PATH, L"%ls\\CReviewTrainer\\rt_%lu", lad, (unsigned long)pid);

    int unpacked = 0;
    for (int i = 0; i < ncand; i++) {
        ensure_dirs_w(cand[i]);
        if (unpack_bundle_from_file(exe_path, cand[i])) {
            wcsncpy(g_core.rt_dir, cand[i], MAX_PATH - 1);
            SetFileAttributesW(g_core.rt_dir, FILE_ATTRIBUTE_HIDDEN);
            unpacked = 1;
            break;
        }
        rmtree_w(cand[i]);
    }
    if (!unpacked) return 0;

    wcsncpy(g_core.exe_dir, exe_dir, MAX_PATH - 1);
    g_core.exe_dir[MAX_PATH - 1] = L'\0';
    _snwprintf(g_core.tcc_dir, MAX_PATH, L"%ls\\tcc", g_core.rt_dir);
    wide_to_short_ascii(g_core.tcc_dir, g_core.tcc_short, sizeof(g_core.tcc_short));
    g_core.work_probed = 0;

    /* Probe writable persistent progress directory */
    wchar_t acands[3][MAX_PATH];
    int napp = 0;
    _snwprintf(acands[napp++], MAX_PATH, L"%ls\\.crev_data", exe_dir);
    if (lad[0]) _snwprintf(acands[napp++], MAX_PATH, L"%ls\\CReviewTrainer", lad);
    if (tmp[0]) _snwprintf(acands[napp++], MAX_PATH, L"%ls\\CReviewTrainer", tmp);
    for (int i = 0; i < napp; i++) {
        if (probe_dir_writable_w(acands[i])) {
            wcsncpy(g_core.appdata_dir, acands[i], MAX_PATH - 1);
            if (wcsstr(g_core.appdata_dir, L"\\.crev_data")) {
                SetFileAttributesW(g_core.appdata_dir, FILE_ATTRIBUTE_HIDDEN);
            }
            break;
        }
    }
    if (!g_core.appdata_dir[0]) wcsncpy(g_core.appdata_dir, g_core.rt_dir, MAX_PATH - 1);
    _snwprintf(g_core.progress_path, MAX_PATH, L"%ls\\progress.json", g_core.appdata_dir);

    /* Load unified curriculum JSON */
    wchar_t cpath[MAX_PATH];
    _snwprintf(cpath, MAX_PATH, L"%ls\\data\\curriculum_all.json", g_core.rt_dir);
    size_t clen = 0;
    char *craw = read_file_all_w(cpath, &clen);
    if (!craw) return 0;
    g_core.modules = j_parse(craw, clen);
    free(craw);
    if (!g_core.modules || g_core.modules->type != J_ARR) return 0;

    core_load_progress();
    return 1;
}

static void core_shutdown(void) {
    if (g_core.rt_dir[0]) rmtree_w(g_core.rt_dir);
    j_free(g_core.modules);
    j_free(g_core.progress);
    DeleteCriticalSection(&g_core.lock);
}

static void core_ensure_work_root(void) {
    EnterCriticalSection(&g_core.lock);
    if (!g_core.work_probed) {
        DWORD pid = GetCurrentProcessId();
        wchar_t tmp[MAX_PATH] = {0};
        GetTempPathW(MAX_PATH, tmp);
        size_t tl = wcslen(tmp);
        if (tl > 0 && tmp[tl - 1] == L'\\') tmp[tl - 1] = L'\0';

        wchar_t wcands[2][MAX_PATH];
        _snwprintf(wcands[0], MAX_PATH, L"%ls\\work", g_core.rt_dir);
        _snwprintf(wcands[1], MAX_PATH, L"%ls\\crev_work_%lu", tmp[0] ? tmp : g_core.exe_dir, (unsigned long)pid);
        int fw = 0;
        for (int i = 0; i < 2; i++) {
            ensure_dirs_w(wcands[i]);
            if (probe_tcc_writable_dir(wcands[i])) {
                wcsncpy(g_core.work_root, wcands[i], MAX_PATH - 1);
                fw = 1;
                break;
            }
        }
        if (!fw) {
            wcsncpy(g_core.work_root, wcands[0], MAX_PATH - 1);
            ensure_dirs_w(g_core.work_root);
        }
        g_core.work_probed = 1;
    }
    LeaveCriticalSection(&g_core.lock);
}

/* ---------- Exercise Grading Result (Human-Readable Report + Status) ---------- */
typedef struct {
    int compiled;
    int passed;         /* 1=passed, 0=failed, -1=run_only */
    int passed_cases;
    int total_cases;
    int timed_out;
    int error_lines[16];
    int error_line_count;
    char *report_utf8;  /* Formatted multi-line report for console panel */
} GradeReport;

static void parse_tcc_error_lines(const char *stderr_text, GradeReport *gr) {
    if (!stderr_text || !gr) return;
    const char *p = stderr_text;
    while (*p && gr->error_line_count < 16) {
        const char *m = strstr(p, "main.c:");
        if (!m) break;
        const char *q = m + 7;
        int ln = 0;
        while (*q >= '0' && *q <= '9') { ln = ln * 10 + (*q - '0'); q++; }
        if (ln > 0 && *q == ':') {
            int dup = 0;
            for (int i = 0; i < gr->error_line_count; i++) {
                if (gr->error_lines[i] == ln) { dup = 1; break; }
            }
            if (!dup) gr->error_lines[gr->error_line_count++] = ln;
        }
        p = q;
    }
}

static void describe_char_utf8(char c, int is_eof, char *out, size_t cap) {
    if (is_eof) _snprintf(out, cap, "输出结束");
    else if (c == '\n') _snprintf(out, cap, "换行符(\\n)");
    else if (c == ' ') _snprintf(out, cap, "空格(' ')");
    else if (c == '\t') _snprintf(out, cap, "制表符(\\t)");
    else if ((unsigned char)c >= 32 && (unsigned char)c < 127) _snprintf(out, cap, "'%c'", c);
    else _snprintf(out, cap, "字节(0x%02X)", (unsigned char)c);
}

static void append_first_diff_hint(StrBuf *sb, const char *exp, const char *act) {
    if (!exp) exp = "";
    if (!act) act = "";
    int line = 1, col = 1;
    size_t i = 0;
    while (exp[i] && act[i] && exp[i] == act[i]) {
        if (exp[i] == '\n') { line++; col = 1; }
        else col++;
        i++;
    }
    if (exp[i] == act[i]) return;
    char de[32], da[32];
    describe_char_utf8(exp[i], exp[i] == '\0', de, sizeof(de));
    describe_char_utf8(act[i], act[i] == '\0', da, sizeof(da));
    sb_append(sb, "    [差异定位] 第 ");
    sb_append_int(sb, line);
    sb_append(sb, " 行第 ");
    sb_append_int(sb, col);
    sb_append(sb, " 列：期望 ");
    sb_append(sb, de);
    sb_append(sb, "，实际为 ");
    sb_append(sb, da);
    sb_append_char(sb, '\n');
}

static void append_indented_lines(StrBuf *sb, const char *text) {
    if (!text || !text[0]) {
        sb_append(sb, "      (空)\n");
        return;
    }
    const char *p = text;
    while (*p) {
        const char *nl = strchr(p, '\n');
        size_t len = nl ? (size_t)(nl - p) : strlen(p);
        sb_append(sb, "      ");
        sb_append_mem(sb, p, len);
        sb_append_char(sb, '\n');
        if (!nl) break;
        p = nl + 1;
    }
}

static void grade_report_free(GradeReport *r) {
    free(r->report_utf8);
    r->report_utf8 = NULL;
}

static void core_write_input_files(const wchar_t *case_dir, const JVal *input_files) {
    if (!input_files || input_files->type != J_ARR) return;
    for (size_t i = 0; i < input_files->arr_len; i++) {
        JVal *f = input_files->items[i];
        const char *name = j_get_str(f, "name", "");
        const char *type = j_get_str(f, "type", "text");
        wchar_t wname[MAX_PATH], fpath[MAX_PATH];
        utf8_to_wide(name, wname, MAX_PATH);
        _snwprintf(fpath, MAX_PATH, L"%ls\\%ls", case_dir, wname);
        if (strcmp(type, "binary") == 0) {
            size_t blen = 0;
            unsigned char *bytes = hex_to_bytes(j_get_str(f, "hex", ""), &blen);
            write_file_all_w(fpath, bytes, blen);
            free(bytes);
        } else {
            const char *c = j_get_str(f, "content", "");
            write_file_all_w(fpath, c, strlen(c));
        }
    }
}

static GradeReport core_grade_or_run(const JVal *ex, const char *code, int run_only, const char *custom_stdin) {
    core_ensure_work_root();
    GradeReport gr;
    memset(&gr, 0, sizeof(gr));

    LONG seq = InterlockedIncrement(&g_core.case_seq);
    wchar_t work_dir[MAX_PATH], src_path[MAX_PATH];
    _snwprintf(work_dir, MAX_PATH, L"%ls\\run_%ld_%lu", g_core.work_root, (long)seq, (unsigned long)GetTickCount());
    ensure_dirs_w(work_dir);
    _snwprintf(src_path, MAX_PATH, L"%ls\\main.c", work_dir);

    StrBuf clean;
    sb_init(&clean);
    for (const char *p = code; *p; p++) {
        if (*p == '\r' && p[1] == '\n') continue;
        if (*p == '\r') { sb_append_char(&clean, '\n'); continue; }
        sb_append_char(&clean, *p);
    }
    write_file_all_w(src_path, clean.data, clean.len);
    sb_free(&clean);

    JVal *tests = j_obj_get(ex, "tests");
    StrBuf rep;
    sb_init(&rep);

    if (run_only) {
        gr.passed = -1;
        JVal *sample = (tests && tests->type == J_ARR && tests->arr_len > 0) ? tests->items[0] : NULL;
        const char *stdin_text = (custom_stdin && custom_stdin[0]) ? custom_stdin : (sample ? j_get_str(sample, "stdin", "") : "");
        wchar_t case_dir[MAX_PATH];
        _snwprintf(case_dir, MAX_PATH, L"%ls\\run_case", work_dir);
        ensure_dirs_w(case_dir);
        if (sample) core_write_input_files(case_dir, j_obj_get(sample, "input_files"));

        ProcResult r = run_tcc_in_dir(src_path, case_dir, stdin_text, 3000);
        if (!r.ok_launch) {
            sb_append(&rep, "[错误] 无法启动内置 TCC 编译器。\n");
        } else if (!r.timed_out && is_tcc_compile_error(r.exit_code, r.stderr_utf8)) {
            gr.compiled = 0;
            parse_tcc_error_lines(r.stderr_utf8, &gr);
            sb_append(&rep, "[编译失败]\n");
            sb_append(&rep, r.stderr_utf8);
        } else {
            gr.compiled = 1;
            gr.timed_out = r.timed_out;
            if (r.timed_out) {
                sb_append(&rep, "[运行超时] 程序运行超过 3 秒，已自动中止（请检查是否存在死循环或 scanf 等待输入）。\n");
            } else {
                sb_append(&rep, "[试运行完成] 退出码: ");
                sb_append_int(&rep, (int)r.exit_code);
                sb_append(&rep, "  耗时: ");
                sb_append_int(&rep, r.run_ms);
                sb_append(&rep, " ms\n");
                sb_append(&rep, "---- 标准输出 (stdout) ----\n");
                sb_append(&rep, (r.stdout_utf8 && r.stdout_utf8[0]) ? r.stdout_utf8 : "(无输出)\n");
                if (r.stdout_utf8 && r.stdout_utf8[0] && r.stdout_utf8[strlen(r.stdout_utf8) - 1] != '\n') sb_append_char(&rep, '\n');
                if (r.stderr_utf8 && r.stderr_utf8[0]) {
                    sb_append(&rep, "---- 错误输出 (stderr) ----\n");
                    sb_append(&rep, r.stderr_utf8);
                    sb_append_char(&rep, '\n');
                }
                JVal *exp_files = sample ? j_obj_get(sample, "expected_files") : NULL;
                if (exp_files && exp_files->type == J_ARR) {
                    for (size_t i = 0; i < exp_files->arr_len; i++) {
                        JVal *ef = exp_files->items[i];
                        const char *fn = j_get_str(ef, "name", "");
                        const char *ft = j_get_str(ef, "type", "text");
                        wchar_t wfn[MAX_PATH], fp[MAX_PATH];
                        utf8_to_wide(fn, wfn, MAX_PATH);
                        _snwprintf(fp, MAX_PATH, L"%ls\\%ls", case_dir, wfn);
                        size_t flen = 0;
                        char *raw = read_file_all_w(fp, &flen);
                        sb_append(&rep, "---- 生成文件: ");
                        sb_append(&rep, fn);
                        sb_append(&rep, " ----\n");
                        if (!raw) {
                            sb_append(&rep, "(未生成该文件)\n");
                        } else if (strcmp(ft, "binary") == 0) {
                            char *hex = bytes_to_hex((unsigned char *)raw, flen);
                            sb_append(&rep, "[二进制 ");
                            sb_append_int(&rep, (long long)flen);
                            sb_append(&rep, " 字节] ");
                            sb_append(&rep, hex);
                            sb_append_char(&rep, '\n');
                            free(hex);
                            free(raw);
                        } else {
                            char *u = ensure_utf8_text(raw, flen);
                            sb_append(&rep, u);
                            if (u[0] && u[strlen(u) - 1] != '\n') sb_append_char(&rep, '\n');
                            free(u);
                            free(raw);
                        }
                    }
                }
            }
        }
        proc_result_free(&r);
        rmtree_w(work_dir);
        gr.report_utf8 = rep.data;
        return gr;
    }

    /* Full grading */
    char *stripped = strip_c_comments(code);
    JVal *checks = j_obj_get(ex, "code_checks");
    StrBuf warn_sb;
    sb_init(&warn_sb);
    int warn_cnt = 0;
    if (checks && checks->type == J_ARR) {
        for (size_t i = 0; i < checks->arr_len; i++) {
            JVal *ck = checks->items[i];
            JVal *toks = j_obj_get(ck, "tokens");
            int ok_tok = 1;
            if (toks && toks->type == J_ARR) {
                for (size_t k = 0; k < toks->arr_len; k++) {
                    const char *t = (toks->items[k]->type == J_STR) ? toks->items[k]->sval : "";
                    if (t[0] && strstr(stripped, t) == NULL) { ok_tok = 0; break; }
                }
            }
            if (!ok_tok) {
                warn_cnt++;
                sb_append(&warn_sb, "  ! ");
                sb_append(&warn_sb, j_get_str(ck, "message", "未满足题目写法要求"));
                sb_append_char(&warn_sb, '\n');
            }
        }
    }
    free(stripped);

    size_t ntests = (tests && tests->type == J_ARR) ? tests->arr_len : 0;
    gr.total_cases = (int)ntests;
    gr.compiled = 1;
    int passed_cases = 0;
    StrBuf cases_sb;
    sb_init(&cases_sb);

    for (size_t i = 0; i < ntests; i++) {
        JVal *tc = tests->items[i];
        const char *tc_name = j_get_str(tc, "name", "测试点");
        const char *stdin_text = j_get_str(tc, "stdin", "");
        DWORD timeout_ms = (DWORD)j_get_int(tc, "timeout_ms", 2500);

        wchar_t case_dir[MAX_PATH];
        _snwprintf(case_dir, MAX_PATH, L"%ls\\case_%u", work_dir, (unsigned)(i + 1));
        ensure_dirs_w(case_dir);
        core_write_input_files(case_dir, j_obj_get(tc, "input_files"));

        ProcResult r = run_tcc_in_dir(src_path, case_dir, stdin_text, timeout_ms);
        if (!r.ok_launch) {
            gr.compiled = 0;
            sb_append(&rep, "[错误] 无法启动内置 TCC 编译器。\n");
            proc_result_free(&r);
            break;
        }
        if (!r.timed_out && is_tcc_compile_error(r.exit_code, r.stderr_utf8)) {
            gr.compiled = 0;
            parse_tcc_error_lines(r.stderr_utf8, &gr);
            sb_append(&rep, "[编译失败]\n");
            sb_append(&rep, r.stderr_utf8);
            proc_result_free(&r);
            break;
        }

        int case_ok = 1;
        StrBuf detail;
        sb_init(&detail);

        if (r.timed_out) {
            case_ok = 0;
            gr.timed_out = 1;
            sb_append(&detail, "    运行超时（可能存在死循环或等待输入）\n");
        } else if (r.exit_code != 0) {
            case_ok = 0;
            sb_append(&detail, "    运行异常 (退出码 ");
            sb_append_int(&detail, (int)r.exit_code);
            sb_append(&detail, ")，请检查指针越界、空指针、除零或文件打开失败\n");
            if (r.stderr_utf8 && r.stderr_utf8[0]) {
                sb_append(&detail, "    stderr: ");
                sb_append(&detail, r.stderr_utf8);
                sb_append_char(&detail, '\n');
            }
        } else {
            if (j_get_bool(tc, "has_stdout", 0)) {
                char *na = normalize_text(r.stdout_utf8);
                char *ne = normalize_text(j_get_str(tc, "expected_stdout", ""));
                if (strcmp(na, ne) != 0) {
                    case_ok = 0;
                    sb_append(&detail, "    标准输出不匹配\n");
                    append_first_diff_hint(&detail, ne, na);
                    sb_append(&detail, "    [期望输出]\n");
                    append_indented_lines(&detail, ne);
                    sb_append(&detail, "    [实际输出]\n");
                    append_indented_lines(&detail, na);
                }
                free(na); free(ne);
            }
            JVal *exp_files = j_obj_get(tc, "expected_files");
            if (exp_files && exp_files->type == J_ARR) {
                for (size_t k = 0; k < exp_files->arr_len; k++) {
                    JVal *ef = exp_files->items[k];
                    const char *fn = j_get_str(ef, "name", "");
                    const char *ft = j_get_str(ef, "type", "text");
                    wchar_t wfn[MAX_PATH], fp[MAX_PATH];
                    utf8_to_wide(fn, wfn, MAX_PATH);
                    _snwprintf(fp, MAX_PATH, L"%ls\\%ls", case_dir, wfn);
                    size_t flen = 0;
                    char *raw = read_file_all_w(fp, &flen);
                    if (!raw) {
                        case_ok = 0;
                        sb_append(&detail, "    未生成目标文件: ");
                        sb_append(&detail, fn);
                        sb_append_char(&detail, '\n');
                    } else if (strcmp(ft, "binary") == 0) {
                        const char *exp_hex = j_get_str(ef, "hex", "");
                        char *act_hex = bytes_to_hex((unsigned char *)raw, flen);
                        if (strcmp(act_hex, exp_hex) != 0) {
                            case_ok = 0;
                            sb_append(&detail, "    二进制文件 ");
                            sb_append(&detail, fn);
                            sb_append(&detail, " 不匹配 (期望: ");
                            sb_append(&detail, j_get_str(ef, "desc", exp_hex));
                            sb_append(&detail, ", 实际 ");
                            sb_append_int(&detail, (long long)flen);
                            sb_append(&detail, " 字节)\n");
                        }
                        free(act_hex); free(raw);
                    } else {
                        char *u = ensure_utf8_text(raw, flen);
                        char *na = normalize_text(u);
                        char *ne = normalize_text(j_get_str(ef, "content", ""));
                        if (strcmp(na, ne) != 0) {
                            case_ok = 0;
                            sb_append(&detail, "    文件 ");
                            sb_append(&detail, fn);
                            sb_append(&detail, " 内容不匹配\n");
                            append_first_diff_hint(&detail, ne, na);
                            sb_append(&detail, "    [期望输出]\n");
                            append_indented_lines(&detail, ne);
                            sb_append(&detail, "    [实际输出]\n");
                            append_indented_lines(&detail, na);
                        }
                        free(na); free(ne); free(u); free(raw);
                    }
                }
            }
        }

        if (case_ok) passed_cases++;
        sb_append(&cases_sb, case_ok ? "  [通过] " : "  [未过] ");
        sb_append(&cases_sb, tc_name);
        sb_append(&cases_sb, " (");
        sb_append_int(&cases_sb, r.run_ms);
        sb_append(&cases_sb, " ms)\n");
        if (detail.len > 0) sb_append_mem(&cases_sb, detail.data, detail.len);

        sb_free(&detail);
        proc_result_free(&r);
    }

    if (gr.compiled) {
        gr.passed_cases = passed_cases;
        gr.passed = (passed_cases == (int)ntests && warn_cnt == 0) ? 1 : 0;
        if (gr.passed) {
            sb_append(&rep, "[全部通过] 测试点 ");
            sb_append_int(&rep, passed_cases);
            sb_append(&rep, "/");
            sb_append_int(&rep, (long long)ntests);
            sb_append(&rep, " 全部正确！（可点击上方「查看参考写法」对照）\n");
        } else {
            sb_append(&rep, "[未全部通过] 通过测试点 ");
            sb_append_int(&rep, passed_cases);
            sb_append(&rep, "/");
            sb_append_int(&rep, (long long)ntests);
            sb_append_char(&rep, '\n');
        }
        if (warn_cnt > 0) {
            sb_append(&rep, "代码要求提示：\n");
            sb_append_mem(&rep, warn_sb.data, warn_sb.len);
        }
        sb_append_mem(&rep, cases_sb.data, cases_sb.len);
    }

    sb_free(&warn_sb);
    sb_free(&cases_sb);
    rmtree_w(work_dir);
    gr.report_utf8 = rep.data;
    return gr;
}

#endif
