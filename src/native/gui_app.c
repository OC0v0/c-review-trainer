/*
 * C Language Review Trainer — Modern Pure Native Win32 Desktop GUI Application
 * - Standalone single-file .exe with bundled TCC C99 compiler + Scintilla/Lexilla C/C++ IDE engine
 * - Zero browser / zero HTTP / zero console flash / DPI-aware Light & Dark theme
 * - Word-wrapping collapsible TreeList + live draggable column splitters
 * - Scintilla C99 editor: line numbers, native C lexer, matching braces, multi-level Undo/Redo,
 *   plain-text paste, Tab/Shift+Tab block indent, auto-pair, smart Enter/outdent,
 *   native static C function-name autocomplete, and airtight partial lock/unlock mode
 */

#define _CRT_SECURE_NO_WARNINGS
#include "core.h"
#include "c_intellisense.h"

#ifndef WS_EX_NOACTIVATE
#define WS_EX_NOACTIVATE 0x08000000L
#endif
#ifndef CS_DROPSHADOW
#define CS_DROPSHADOW 0x00020000
#endif
#ifndef MA_NOACTIVATE
#define MA_NOACTIVATE 3
#endif

typedef BOOL (WINAPI *pfn_SetProcessDPIAware)(void);
typedef void *(WINAPI *pfn_CreateLexer)(const char *);

/* ---------- RichEdit 5.0 (for read-only problem, reference, console panels) ---------- */
#define EM_EXGETSEL         (WM_USER + 52)
#define EM_EXSETSEL         (WM_USER + 55)
#define EM_SETCHARFORMAT    (WM_USER + 68)
#define EM_SETBKGNDCOLOR    (WM_USER + 67)
#define EM_REPLACESEL       0x00C2
#define EM_LINESCROLL       0x00B6
#define EM_SETMARGINS       0x00D3
#define EC_LEFTMARGIN       0x0001
#define EC_RIGHTMARGIN      0x0002
#define SCF_SELECTION       0x0001
#define CFM_BOLD            0x00000001
#define CFM_BACKCOLOR       0x04000000
#define CFM_SIZE            0x80000000
#define CFM_COLOR           0x40000000
#define CFM_FACE            0x20000000
#define CFE_BOLD            0x0001
#define CFE_AUTOBACKCOLOR   0x04000000

typedef struct { LONG cpMin; LONG cpMax; } MY_CHARRANGE;

typedef struct {
    UINT cbSize;
    DWORD dwMask;
    DWORD dwEffects;
    LONG yHeight;
    LONG yOffset;
    COLORREF crTextColor;
    BYTE bCharSet;
    BYTE bPitchAndFamily;
    WCHAR szFaceName[32];
    WORD wWeight;
    SHORT sSpacing;
    COLORREF crBackColor;
    LCID lcid;
    DWORD dwReserved;
    SHORT sStyle;
    WORD wKerning;
    BYTE bUnderlineType;
    BYTE bAnimation;
    BYTE bRevAuthor;
    BYTE bReserved1;
} MY_CHARFORMAT2W;

typedef struct {
    HWND hwndFrom;
    UINT_PTR idFrom;
    UINT code;
} MY_NMHDR;

/* ---------- Scintilla 4.4.6 ABI Constants & Structs (x86) ---------- */
#define SCI_INSERTTEXT              2003
#define SCI_GETCHARAT               2007
#define SCI_GETCURRENTPOS           2008
#define SCI_GETSTYLEAT              2010
#define SCI_GOTOLINE                2024
#define SCI_GOTOPOS                 2025
#define SCI_SETANCHOR               2026
#define SCI_SETEOLMODE              2031
#define SCI_SETTABWIDTH             2036
#define SCI_SETCODEPAGE             2037
#define SCI_MARKERDEFINE            2040
#define SCI_MARKERSETFORE           2041
#define SCI_MARKERSETBACK           2042
#define SCI_MARKERADD               2043
#define SCI_MARKERDELETEALL         2045
#define SCI_STYLECLEARALL           2050
#define SCI_STYLESETFORE            2051
#define SCI_STYLESETBACK            2052
#define SCI_STYLESETBOLD            2053
#define SCI_STYLESETITALIC          2054
#define SCI_STYLESETSIZE            2055
#define SCI_STYLESETFONT            2056
#define SCI_SETSELBACK              2068
#define SCI_SETCARETFORE            2069
#define SCI_BEGINUNDOACTION         2078
#define SCI_ENDUNDOACTION           2079
#define SCI_SETCARETLINEVISIBLE     2096
#define SCI_SETCARETLINEBACK        2098
#define SCI_AUTOCSHOW               2100
#define SCI_AUTOCCANCEL             2101
#define SCI_AUTOCACTIVE             2102
#define SCI_AUTOCSETCHOOSESINGLE    2113
#define SCI_AUTOCSETIGNORECASE      2115
#define SCI_SETINDENT               2122
#define SCI_SETUSETABS              2124
#define SCI_SETLINEINDENTATION      2126
#define SCI_GETLINEINDENTATION      2127
#define SCI_SETINDENTATIONGUIDES    2132
#define SCI_GETLINEENDPOSITION      2136
#define SCI_SETCURRENTPOS           2141
#define SCI_GETSELECTIONSTART       2143
#define SCI_GETSELECTIONEND         2145
#define SCI_GETLINECOUNT            2154
#define SCI_SETMARGINLEFT           2155
#define SCI_SETSEL                  2160
#define SCI_POINTXFROMPOSITION      2164
#define SCI_POINTYFROMPOSITION      2165
#define SCI_LINEFROMPOSITION        2166
#define SCI_POSITIONFROMLINE        2167
#define SCI_SCROLLCARET             2169
#define SCI_REPLACESEL              2170
#define SCI_SETREADONLY             2171
#define SCI_EMPTYUNDOBUFFER         2175
#define SCI_SETTEXT                 2181
#define SCI_GETTEXT                 2182
#define SCI_GETTEXTLENGTH           2183
#define SCI_CALLTIPSHOW             2200
#define SCI_CALLTIPCANCEL           2201
#define SCI_CALLTIPACTIVE           2202
#define SCI_CALLTIPSETHLT           2204
#define SCI_CALLTIPSETBACK          2205
#define SCI_CALLTIPSETFORE          2206
#define SCI_CALLTIPSETFOREHLT       2207
#define SCI_CALLTIPUSESTYLE         2212
#define SCI_SETMARGINTYPEN          2240
#define SCI_SETMARGINWIDTHN         2242
#define SCI_SETMARGINMASKN          2244
#define SCI_SETTABINDENTS           2260
#define SCI_SETBACKSPACEUNINDENTS   2262
#define SCI_TEXTHEIGHT              2279
#define SCI_BRACEHIGHLIGHT          2351
#define SCI_BRACEBADLIGHT           2352
#define SCI_BRACEMATCH              2353
#define SCI_SETZOOM                 2373
#define SCI_GETZOOM                 2374
#define SCI_DELETERANGE             2645
#define SCI_SETKEYWORDS             4005
#define SCI_SETILEXER               4033

#define SC_CP_UTF8                  65001
#define SC_EOL_LF                   2
#define SC_MARGIN_SYMBOL            0
#define SC_MARGIN_NUMBER            1
#define SC_MARK_BACKGROUND          22
#define SC_MARK_FULLRECT            26
#define SC_IV_LOOKBOTH              3
#define SC_ORDER_PERFORMSORT        1
#define SC_ORDER_CUSTOM             2
#define SC_MOD_INSERTTEXT           0x1
#define SC_MOD_DELETETEXT           0x2

#define STYLE_DEFAULT               32
#define STYLE_LINENUMBER            33
#define STYLE_BRACELIGHT            34
#define STYLE_BRACEBAD              35
#define STYLE_CALLTIP               38

/* C/C++ Lexer styles */
#define SCE_C_DEFAULT               0
#define SCE_C_COMMENT               1
#define SCE_C_COMMENTLINE           2
#define SCE_C_COMMENTDOC            3
#define SCE_C_NUMBER                4
#define SCE_C_WORD                  5
#define SCE_C_STRING                6
#define SCE_C_CHARACTER             7
#define SCE_C_PREPROCESSOR          9
#define SCE_C_OPERATOR              10
#define SCE_C_IDENTIFIER            11
#define SCE_C_WORD2                 16

/* Scintilla notifications */
#define SCN_CHARADDED               2001
#define SCN_UPDATEUI                2007
#define SCN_MODIFIED                2008
#define SCN_AUTOCCOMPLETED          2030

typedef struct {
    MY_NMHDR nmhdr;
    int position;
    int ch;
    int modifiers;
    int modificationType;
    const char *text;
    int length;
    int linesAdded;
    int message;
    WPARAM wParam;
    LPARAM lParam;
    int line;
    int foldLevelNow;
    int foldLevelPrev;
    int margin;
    int listType;
    int x;
    int y;
    int token;
    int annotationLinesAdded;
    int updated;
    int listCompletionMethod;
    int characterSource;
} MY_SCNotification;

/* ---------- Control IDs & Messages ---------- */
#define ID_TREE             101
#define ID_BTN_PREV         102
#define ID_BTN_NEXT         103
#define ID_CHK_QUEST        104
#define ID_BTN_RESET_ALL    105
#define ID_BTN_UNLOCK       106
#define ID_BTN_RESET_CODE   107
#define ID_BTN_SOLUTION     108
#define ID_BTN_RUN          109
#define ID_BTN_SUBMIT       110
#define ID_VERSION_LBL      111
#define ID_EDITOR           112
#define ID_PROBLEM_BOX      113
#define ID_REF_BOX          114
#define ID_CONSOLE_BOX      115
#define ID_STDIN_BOX        116
#define ID_BTN_THEME        118
#define ID_SAMPLE_BOX       119
#define ID_BTN_TOGGLE_REF   120
#define ID_BTN_TOGGLE_NAV   121
#define ID_BTN_FILL_STDIN   122
#define ID_FLT_ALL          123
#define ID_FLT_TODO         124
#define ID_FLT_HARD         125
#define ID_BTN_FONT_SIZE    126

#define WM_APP_GRADE_DONE   (WM_APP + 10)
#define IDT_AUTOSAVE        9001

/* ---------- Theme Palette ---------- */
typedef struct {
    COLORREF bg;
    COLORREF surface;
    COLORREF surface_alt;
    COLORREF header_bg;
    COLORREF border;
    COLORREF border_strong;
    COLORREF text;
    COLORREF text_muted;
    COLORREF accent;
    COLORREF accent_hover;
    COLORREF accent_soft;
    COLORREF pass;
    COLORREF pass_soft;
    COLORREF fail;
    COLORREF fail_soft;
    COLORREF warn;
    COLORREF locked_bg;
    COLORREF edit_bg;
    COLORREF caret_line;
    COLORREF sel_bg;
    COLORREF gutter_bg;
    COLORREF gutter_fg;
    COLORREF syn_kw;
    COLORREF syn_type;
    COLORREF syn_pre;
    COLORREF syn_str;
    COLORREF syn_num;
    COLORREF syn_com;
    COLORREF syn_op;
} Theme;

static const Theme THEME_LIGHT = {
    .bg            = RGB(241, 245, 249),
    .surface       = RGB(255, 255, 255),
    .surface_alt   = RGB(248, 250, 252),
    .header_bg     = RGB(255, 255, 255),
    .border        = RGB(226, 232, 240),
    .border_strong = RGB(203, 213, 225),
    .text          = RGB(30, 41, 59),
    .text_muted    = RGB(100, 116, 139),
    .accent        = RGB(37, 99, 235),
    .accent_hover  = RGB(29, 78, 216),
    .accent_soft   = RGB(239, 246, 255),
    .pass          = RGB(5, 150, 105),
    .pass_soft     = RGB(236, 253, 245),
    .fail          = RGB(220, 38, 38),
    .fail_soft     = RGB(254, 226, 226),
    .warn          = RGB(217, 119, 6),
    .locked_bg     = RGB(241, 245, 249),
    .edit_bg       = RGB(255, 255, 255),
    .caret_line    = RGB(248, 250, 252),
    .sel_bg        = RGB(191, 219, 254),
    .gutter_bg     = RGB(248, 250, 252),
    .gutter_fg     = RGB(148, 163, 184),
    .syn_kw        = RGB(0, 92, 197),
    .syn_type      = RGB(109, 40, 217),
    .syn_pre       = RGB(124, 58, 237),
    .syn_str       = RGB(180, 83, 9),
    .syn_num       = RGB(3, 105, 161),
    .syn_com       = RGB(100, 116, 139),
    .syn_op        = RGB(51, 65, 85),
};

static const Theme THEME_DARK = {
    .bg            = RGB(15, 23, 42),
    .surface       = RGB(30, 41, 59),
    .surface_alt   = RGB(22, 32, 50),
    .header_bg     = RGB(22, 32, 50),
    .border        = RGB(51, 65, 85),
    .border_strong = RGB(71, 85, 105),
    .text          = RGB(226, 232, 240),
    .text_muted    = RGB(148, 163, 184),
    .accent        = RGB(59, 130, 246),
    .accent_hover  = RGB(96, 165, 250),
    .accent_soft   = RGB(30, 58, 95),
    .pass          = RGB(16, 185, 129),
    .pass_soft     = RGB(6, 78, 59),
    .fail          = RGB(248, 113, 113),
    .fail_soft     = RGB(82, 26, 30),
    .warn          = RGB(245, 158, 11),
    .locked_bg     = RGB(22, 31, 48),
    .edit_bg       = RGB(11, 18, 32),
    .caret_line    = RGB(20, 30, 50),
    .sel_bg        = RGB(38, 79, 120),
    .gutter_bg     = RGB(15, 23, 42),
    .gutter_fg     = RGB(100, 116, 139),
    .syn_kw        = RGB(96, 165, 250),
    .syn_type      = RGB(167, 139, 250),
    .syn_pre       = RGB(192, 132, 252),
    .syn_str       = RGB(251, 191, 36),
    .syn_num       = RGB(56, 189, 248),
    .syn_com       = RGB(100, 116, 139),
    .syn_op        = RGB(203, 213, 225),
};

static int g_dark_mode = 0;
static const Theme *T(void) { return g_dark_mode ? &THEME_DARK : &THEME_LIGHT; }

static int g_dpi = 96;
static int g_font_level = 1; /* 0=小(90%), 1=默认(108%), 2=大(125%), 3=超大(145%) */
static int S(int px) { return (px * g_dpi + 48) / 96; }
static int clamp_i(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

static int font_pct(void) {
    static const int PCTS[4] = { 90, 108, 125, 145 };
    return PCTS[clamp_i(g_font_level, 0, 3)];
}
static int FP(int half_pt) { return (half_pt * 10 * font_pct() + 50) / 100; }
static int sci_base_pt(void) {
    static const int PTS[4] = { 10, 11, 13, 15 };
    return PTS[clamp_i(g_font_level, 0, 3)];
}
static const wchar_t *font_level_label(void) {
    static const wchar_t *LBLS[4] = { L"字号:小", L"字号:默认", L"字号:大", L"字号:超大" };
    return LBLS[clamp_i(g_font_level, 0, 3)];
}

/* ---------- Static C99 Lexer Keyword Lists (Pure C Only) ---------- */
static const char *C_KW_LIST =
    "auto break case const continue default do else enum extern for goto if "
    "inline register restrict return sizeof static struct switch typedef union volatile while";

static const char *C_WORD2_LIST =
    "char double float int long short signed unsigned void _Bool bool size_t ptrdiff_t FILE NULL EOF "
    "true false SEEK_SET SEEK_CUR SEEK_END stdin stdout stderr EXIT_SUCCESS EXIT_FAILURE RAND_MAX "
    "INT_MAX INT_MIN LONG_MAX LONG_MIN LLONG_MAX LLONG_MIN M_PI time_t clock_t "
    "int8_t uint8_t int16_t uint16_t int32_t uint32_t int64_t uint64_t "
    "printf scanf sprintf snprintf sscanf fprintf fscanf fopen freopen fclose fflush "
    "getchar putchar fgetc getc fputc putc ungetc fgets fputs puts fread fwrite fseek ftell rewind "
    "feof ferror clearerr perror remove rename "
    "malloc calloc realloc free atoi atol atoll atof strtol strtoul strtoll strtod rand srand "
    "abs labs llabs qsort bsearch exit abort system "
    "strlen strcpy strncpy strcat strncat strcmp strncmp strchr strrchr strstr strtok "
    "memcpy memmove memset memcmp memchr "
    "sqrt cbrt pow exp log log10 log2 fabs fmod ceil floor round trunc fmax fmin hypot "
    "sin cos tan asin acos atan atan2 sinh cosh tanh "
    "isalpha isdigit isalnum isspace islower isupper isxdigit ispunct isprint tolower toupper "
    "time clock difftime localtime gmtime mktime strftime assert";

/* Custom RedPanda-C++ style floating completion popup state */
static HWND g_hwnd_ac_popup = NULL;
static CIResult g_ac_res;
static int g_ac_visible = 0;
static int g_ac_sel = 0;
static int g_ac_top = 0;
static int g_ac_hover = -1;
#define AC_MAX_VISIBLE 9

/* ---------- Flat Exercise Table & Custom Collapsible Word-Wrap Tree ---------- */
typedef struct {
    int mod_idx;
    int ex_idx;
    JVal *mod;
    JVal *ex;
    const char *id;
} FlatEx;

typedef struct {
    int is_mod;
    int mod_idx;
    int flat_idx;
    int y_top;
    int height;
} TreeRow;

static FlatEx g_flat[256];
static int g_flat_count = 0;
static int g_mod_collapsed[32] = {0};
static TreeRow g_tree_rows[300];
static int g_tree_row_count = 0;
static int g_tree_total_h = 0;
static int g_tree_scroll_y = 0;
static int g_tree_hover_row = -1;
static int g_tree_wheel_num = 0;
static int g_cur_flat = 0;
static int g_done_count = 0;

/* Draggable column widths, row heights & panel toggles */
static int g_nav_w = 0;
static int g_ref_w = 0;
static int g_prob_h = 0;
static int g_cons_h = 0;
static int g_nav_hidden = 0;
static int g_ref_hidden = 0;
static int g_tree_filter = 0; /* 0=all, 1=todo, 2=stage+special */
static int g_editor_zoom = 0;
static int g_drag_split = 0;  /* 1=left, 2=right, 3=top-h, 4=bot-h */
static int g_hover_split = 0;
static RECT g_rc_split_left = {0};
static RECT g_rc_split_right = {0};
static RECT g_rc_split_top = {0};
static RECT g_rc_split_bot = {0};

static HMODULE g_hScintilla = NULL;
static HMODULE g_hLexilla = NULL;
static pfn_CreateLexer g_CreateLexer = NULL;

static HWND g_hwnd_main = NULL;
static HWND g_hwnd_sol = NULL;
static HWND g_hwnd_sol_sci = NULL;
static HWND g_hwnd_sol_copy = NULL;
static HWND g_hwnd_sol_close = NULL;
static HWND g_hwnd_tree = NULL;
static HWND g_hwnd_flt_all = NULL;
static HWND g_hwnd_flt_todo = NULL;
static HWND g_hwnd_flt_hard = NULL;
static HWND g_hwnd_problem = NULL;
static HWND g_hwnd_sample = NULL;
static HWND g_hwnd_lbl_sample = NULL;
static HWND g_hwnd_sci = NULL; /* Scintilla IDE control */
static HWND g_hwnd_ref = NULL;
static HWND g_hwnd_console = NULL;
static HWND g_hwnd_stdin = NULL;
static HWND g_hwnd_btn_fill_stdin = NULL;
static HWND g_hwnd_mode_lbl = NULL;
static HWND g_hwnd_status = NULL;
static HWND g_hwnd_version = NULL;
static HWND g_hwnd_chk_quest = NULL;
static HWND g_hwnd_btn_prev = NULL;
static HWND g_hwnd_btn_next = NULL;
static HWND g_hwnd_btn_toggle_nav = NULL;
static HWND g_hwnd_btn_toggle_ref = NULL;
static HWND g_hwnd_btn_font = NULL;
static HWND g_hwnd_btn_theme = NULL;
static HWND g_hwnd_btn_reset_all = NULL;
static HWND g_hwnd_btn_unlock = NULL;
static HWND g_hwnd_btn_reset_code = NULL;
static HWND g_hwnd_btn_solution = NULL;
static HWND g_hwnd_btn_run = NULL;
static HWND g_hwnd_btn_submit = NULL;
static HWND g_hwnd_lbl_nav = NULL;
static HWND g_hwnd_lbl_ref = NULL;
static HWND g_hwnd_lbl_stdin = NULL;
static HWND g_hwnd_lbl_console = NULL;

static RECT g_rc_header = {0};
static RECT g_rc_progbar = {0};
static RECT g_rc_card_nav = {0};
static RECT g_rc_card_prob = {0};
static RECT g_rc_card_sample = {0};
static RECT g_rc_card_ed = {0};
static RECT g_rc_card_cons = {0};
static RECT g_rc_card_ref = {0};
static RECT g_rc_card_stdin = {0};

static HFONT g_font_ui = NULL;
static HFONT g_font_ui_sm = NULL;
static HFONT g_font_ui_bold = NULL;
static HFONT g_font_title = NULL;
static HFONT g_font_code = NULL;
static HFONT g_font_tree = NULL;
static HFONT g_font_tree_bold = NULL;
static HBRUSH g_br_bg = NULL;
static HBRUSH g_br_surface = NULL;
static HBRUSH g_br_surface_alt = NULL;
static HBRUSH g_br_header = NULL;

static char *g_last_console_utf8 = NULL;
static int g_last_console_compiled = 1;
static int g_last_console_passed = -1;

static WNDPROC g_orig_sci_proc = NULL;
static WNDPROC g_orig_btn_proc = NULL;
static WNDPROC g_orig_stdin_proc = NULL;
static HWND g_hover_btn = NULL;

static const wchar_t *g_richedit_class = L"RICHEDIT50W";

/* Exact byte-length lock invariant: editable range is [g_lock_prefix_len, doc_len - g_lock_suffix_len] */
static int g_has_partial = 0;
static int g_unlocked = 0;
static int g_lock_prefix_len = 0;
static int g_lock_suffix_len = 0;
static int g_loading_code = 0;
static int g_busy = 0;
static int g_version_clicks = 0;

typedef struct {
    int flat_idx;
    int run_only;
    char *code_utf8;
    char *stdin_utf8;
    GradeReport report;
} GradeJob;

/* Forward declarations */
static void load_exercise(int flat_idx, int sync_tree);
static void trigger_grade_or_run(int run_only);
static void apply_theme_visuals(void);
static void render_problem_panel(const JVal *ex);
static void render_reference_panel(const JVal *ex);
static void layout_controls(int W, int H);
static void rebuild_tree_rows(int preserve_scroll);
static void recalc_tree_row_heights(void);

static LRESULT sci(UINT msg, WPARAM wp, LPARAM lp) {
    return SendMessageW(g_hwnd_sci, msg, wp, lp);
}

/* ---------- RichEdit Helpers (Read-only panels) ---------- */
static void re_reset_margins(HWND hwnd) {
    SendMessageW(hwnd, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(0, 0));
}

static void re_append(HWND hwnd, const wchar_t *text, COLORREF color, int bold, int half_pt, const wchar_t *face) {
    if (!text || !text[0]) return;
    size_t slen = wcslen(text);
    wchar_t *norm = (wchar_t *)malloc((slen * 2 + 2) * sizeof(wchar_t));
    if (!norm) return;
    size_t j = 0;
    for (size_t i = 0; i < slen; i++) {
        if (text[i] == L'\r' && text[i + 1] == L'\n') { norm[j++] = L'\r'; norm[j++] = L'\n'; i++; }
        else if (text[i] == L'\n' || text[i] == L'\r') { norm[j++] = L'\r'; norm[j++] = L'\n'; }
        else norm[j++] = text[i];
    }
    norm[j] = L'\0';

    MY_CHARRANGE cr = { -1, -1 };
    SendMessageW(hwnd, EM_EXSETSEL, 0, (LPARAM)&cr);
    MY_CHARFORMAT2W cf;
    memset(&cf, 0, sizeof(cf));
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_COLOR | CFM_BACKCOLOR | CFM_BOLD | CFM_SIZE | CFM_FACE;
    cf.crTextColor = color;
    cf.dwEffects = (bold ? CFE_BOLD : 0) | CFE_AUTOBACKCOLOR;
    cf.yHeight = FP(half_pt);
    wcsncpy(cf.szFaceName, face ? face : L"Microsoft YaHei UI", 31);
    SendMessageW(hwnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
    SendMessageW(hwnd, EM_REPLACESEL, FALSE, (LPARAM)norm);
    free(norm);
}

static void re_append_utf8(HWND hwnd, const char *utf8, COLORREF color, int bold, int half_pt, const wchar_t *face) {
    if (!utf8 || !utf8[0]) return;
    wchar_t *w = utf8_to_wide_alloc(utf8);
    if (w) { re_append(hwnd, w, color, bold, half_pt, face); free(w); }
}

static void re_append_code_block_utf8(HWND hwnd, const char *utf8, COLORREF fg, COLORREF bg) {
    if (!utf8 || !utf8[0]) return;
    wchar_t *text = utf8_to_wide_alloc(utf8);
    if (!text) return;
    size_t slen = wcslen(text);
    /* Trim trailing newline(s) so the shaded block stays crisp and tight */
    while (slen > 0 && (text[slen - 1] == L'\n' || text[slen - 1] == L'\r')) slen--;
    wchar_t *norm = (wchar_t *)malloc((slen * 2 + 4) * sizeof(wchar_t));
    if (!norm) { free(text); return; }
    size_t j = 0;
    for (size_t i = 0; i < slen; i++) {
        if (text[i] == L'\r' && i + 1 < slen && text[i + 1] == L'\n') { norm[j++] = L'\r'; norm[j++] = L'\n'; i++; }
        else if (text[i] == L'\n' || text[i] == L'\r') { norm[j++] = L'\r'; norm[j++] = L'\n'; }
        else norm[j++] = text[i];
    }
    norm[j] = L'\0';
    free(text);

    MY_CHARRANGE cr = { -1, -1 };
    SendMessageW(hwnd, EM_EXSETSEL, 0, (LPARAM)&cr);
    MY_CHARFORMAT2W cf;
    memset(&cf, 0, sizeof(cf));
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_COLOR | CFM_BACKCOLOR | CFM_BOLD | CFM_SIZE | CFM_FACE;
    cf.crTextColor = fg;
    cf.crBackColor = bg;
    cf.dwEffects = 0;
    cf.yHeight = FP(19); /* scaled Consolas */
    wcsncpy(cf.szFaceName, L"Consolas", 31);
    SendMessageW(hwnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
    SendMessageW(hwnd, EM_REPLACESEL, FALSE, (LPARAM)norm);
    free(norm);

    /* Reset background color on trailing newline */
    MY_CHARRANGE cr2 = { -1, -1 };
    SendMessageW(hwnd, EM_EXSETSEL, 0, (LPARAM)&cr2);
    cf.dwEffects = CFE_AUTOBACKCOLOR;
    SendMessageW(hwnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
    SendMessageW(hwnd, EM_REPLACESEL, FALSE, (LPARAM)L"\r\n");
}

static void re_clear(HWND hwnd) { SetWindowTextW(hwnd, L""); }

static void re_scroll_top(HWND hwnd) {
    MY_CHARRANGE cr = { 0, 0 };
    SendMessageW(hwnd, EM_EXSETSEL, 0, (LPARAM)&cr);
    SendMessageW(hwnd, EM_LINESCROLL, 0, -10000);
}

static wchar_t *get_window_text_alloc(HWND hwnd) {
    int len = GetWindowTextLengthW(hwnd);
    wchar_t *buf = (wchar_t *)malloc((size_t)(len + 2) * sizeof(wchar_t));
    if (!buf) return NULL;
    GetWindowTextW(hwnd, buf, len + 1);
    buf[len] = L'\0';
    return buf;
}

/* ---------- Scintilla Setup, Theme, Text & Airtight Lock Regions ---------- */
static void apply_scintilla_styles_to(HWND hSci) {
    if (!hSci) return;
    const Theme *th = T();

    SendMessageW(hSci, SCI_SETCODEPAGE, SC_CP_UTF8, 0);
    SendMessageW(hSci, SCI_SETEOLMODE, SC_EOL_LF, 0);

    if (g_CreateLexer) {
        void *lex = g_CreateLexer("cpp");
        if (lex) SendMessageW(hSci, SCI_SETILEXER, 0, (LPARAM)lex);
    }
    SendMessageW(hSci, SCI_SETKEYWORDS, 0, (LPARAM)C_KW_LIST);
    SendMessageW(hSci, SCI_SETKEYWORDS, 1, (LPARAM)C_WORD2_LIST);

    SendMessageW(hSci, SCI_STYLESETFONT, STYLE_DEFAULT, (LPARAM)"Consolas");
    SendMessageW(hSci, SCI_STYLESETSIZE, STYLE_DEFAULT, sci_base_pt());
    SendMessageW(hSci, SCI_STYLESETFORE, STYLE_DEFAULT, th->text);
    SendMessageW(hSci, SCI_STYLESETBACK, STYLE_DEFAULT, th->edit_bg);
    SendMessageW(hSci, SCI_STYLECLEARALL, 0, 0);

    SendMessageW(hSci, SCI_STYLESETFORE, SCE_C_DEFAULT, th->text);
    SendMessageW(hSci, SCI_STYLESETFORE, SCE_C_IDENTIFIER, th->text);
    SendMessageW(hSci, SCI_STYLESETFORE, SCE_C_OPERATOR, th->syn_op);
    SendMessageW(hSci, SCI_STYLESETFORE, SCE_C_NUMBER, th->syn_num);
    SendMessageW(hSci, SCI_STYLESETFORE, SCE_C_STRING, th->syn_str);
    SendMessageW(hSci, SCI_STYLESETFORE, SCE_C_CHARACTER, th->syn_str);
    SendMessageW(hSci, SCI_STYLESETFORE, SCE_C_PREPROCESSOR, th->syn_pre);
    SendMessageW(hSci, SCI_STYLESETFORE, SCE_C_COMMENT, th->syn_com);
    SendMessageW(hSci, SCI_STYLESETITALIC, SCE_C_COMMENT, 1);
    SendMessageW(hSci, SCI_STYLESETFORE, SCE_C_COMMENTLINE, th->syn_com);
    SendMessageW(hSci, SCI_STYLESETITALIC, SCE_C_COMMENTLINE, 1);
    SendMessageW(hSci, SCI_STYLESETFORE, SCE_C_COMMENTDOC, th->syn_com);
    SendMessageW(hSci, SCI_STYLESETITALIC, SCE_C_COMMENTDOC, 1);
    SendMessageW(hSci, SCI_STYLESETFORE, SCE_C_WORD, th->syn_kw);
    SendMessageW(hSci, SCI_STYLESETBOLD, SCE_C_WORD, 1);
    SendMessageW(hSci, SCI_STYLESETFORE, SCE_C_WORD2, th->syn_type);
    SendMessageW(hSci, SCI_STYLESETBOLD, SCE_C_WORD2, 1);

    SendMessageW(hSci, SCI_SETMARGINTYPEN, 0, SC_MARGIN_NUMBER);
    SendMessageW(hSci, SCI_SETMARGINWIDTHN, 0, S((42 * font_pct() + 50) / 100));
    SendMessageW(hSci, SCI_SETMARGINTYPEN, 1, SC_MARGIN_SYMBOL);
    SendMessageW(hSci, SCI_SETMARGINWIDTHN, 1, S(5));
    SendMessageW(hSci, SCI_SETMARGINMASKN, 1, (1 << 2) | (1 << 3));
    SendMessageW(hSci, SCI_SETMARGINLEFT, 0, S(6));
    SendMessageW(hSci, SCI_STYLESETFORE, STYLE_LINENUMBER, th->gutter_fg);
    SendMessageW(hSci, SCI_STYLESETBACK, STYLE_LINENUMBER, th->gutter_bg);

    SendMessageW(hSci, SCI_STYLESETFORE, STYLE_BRACELIGHT, th->accent);
    SendMessageW(hSci, SCI_STYLESETBACK, STYLE_BRACELIGHT, th->accent_soft);
    SendMessageW(hSci, SCI_STYLESETBOLD, STYLE_BRACELIGHT, 1);
    SendMessageW(hSci, SCI_STYLESETFORE, STYLE_BRACEBAD, th->fail);
    SendMessageW(hSci, SCI_STYLESETBOLD, STYLE_BRACEBAD, 1);

    SendMessageW(hSci, SCI_SETCARETFORE, th->text, 0);
    SendMessageW(hSci, SCI_SETCARETLINEVISIBLE, 1, 0);
    SendMessageW(hSci, SCI_SETCARETLINEBACK, th->caret_line, 0);
    SendMessageW(hSci, SCI_SETSELBACK, 1, th->sel_bg);
    SendMessageW(hSci, SCI_SETUSETABS, 0, 0);
    SendMessageW(hSci, SCI_SETTABWIDTH, 4, 0);
    SendMessageW(hSci, SCI_SETINDENT, 4, 0);
    SendMessageW(hSci, SCI_SETTABINDENTS, 1, 0);
    SendMessageW(hSci, SCI_SETBACKSPACEUNINDENTS, 1, 0);
    SendMessageW(hSci, SCI_SETINDENTATIONGUIDES, SC_IV_LOOKBOTH, 0);

    SendMessageW(hSci, SCI_CALLTIPUSESTYLE, 10, 0);
    SendMessageW(hSci, SCI_STYLESETFONT, STYLE_CALLTIP, (LPARAM)"Consolas");
    SendMessageW(hSci, SCI_STYLESETSIZE, STYLE_CALLTIP, max(9, sci_base_pt() - 1));
    SendMessageW(hSci, SCI_CALLTIPSETBACK, th->surface, 0);
    SendMessageW(hSci, SCI_CALLTIPSETFORE, th->text, 0);
    SendMessageW(hSci, SCI_CALLTIPSETFOREHLT, th->accent, 0);

    /* Marker 0: locked template bg; Marker 1: compile error bg;
       Marker 2: editable region accent stripe; Marker 3: compile error red stripe */
    SendMessageW(hSci, SCI_MARKERDEFINE, 0, SC_MARK_BACKGROUND);
    SendMessageW(hSci, SCI_MARKERSETBACK, 0, th->locked_bg);
    SendMessageW(hSci, SCI_MARKERDEFINE, 1, SC_MARK_BACKGROUND);
    SendMessageW(hSci, SCI_MARKERSETBACK, 1, th->fail_soft);
    SendMessageW(hSci, SCI_MARKERDEFINE, 2, SC_MARK_FULLRECT);
    SendMessageW(hSci, SCI_MARKERSETFORE, 2, th->accent);
    SendMessageW(hSci, SCI_MARKERSETBACK, 2, th->accent);
    SendMessageW(hSci, SCI_MARKERDEFINE, 3, SC_MARK_FULLRECT);
    SendMessageW(hSci, SCI_MARKERSETFORE, 3, th->fail);
    SendMessageW(hSci, SCI_MARKERSETBACK, 3, th->fail);
    SendMessageW(hSci, SCI_SETZOOM, (WPARAM)g_editor_zoom, 0);
}

static void setup_scintilla_styles(void) {
    apply_scintilla_styles_to(g_hwnd_sci);
    if (g_hwnd_sol_sci) apply_scintilla_styles_to(g_hwnd_sol_sci);
}

static void clear_error_markers(void) {
    if (!g_hwnd_sci) return;
    sci(SCI_MARKERDELETEALL, 1, 0);
    sci(SCI_MARKERDELETEALL, 3, 0);
}

static void mark_compile_error_lines(const int *lines, int count) {
    clear_error_markers();
    if (!g_hwnd_sci || !lines || count <= 0) return;
    int total_lines = (int)sci(SCI_GETLINECOUNT, 0, 0);
    int first_ln = -1;
    for (int i = 0; i < count; i++) {
        int ln = lines[i] - 1;
        if (ln >= 0 && ln < total_lines) {
            sci(SCI_MARKERADD, (WPARAM)ln, 1);
            sci(SCI_MARKERADD, (WPARAM)ln, 3);
            if (first_ln < 0) first_ln = ln;
        }
    }
    if (first_ln >= 0) {
        sci(SCI_GOTOLINE, (WPARAM)first_ln, 0);
        sci(SCI_SCROLLCARET, 0, 0);
    }
}

static char *get_editor_utf8(void) {
    int len = (int)sci(SCI_GETTEXTLENGTH, 0, 0);
    char *buf = (char *)malloc((size_t)len + 2);
    if (!buf) return xstrdup("");
    sci(SCI_GETTEXT, (WPARAM)(len + 1), (LPARAM)buf);
    buf[len] = '\0';
    return buf;
}

static void refresh_locked_line_shading(void) {
    if (!g_hwnd_sci) return;
    sci(SCI_MARKERDELETEALL, 0, 0);
    sci(SCI_MARKERDELETEALL, 2, 0);
    if (g_unlocked || !g_has_partial) return;

    int total_len = (int)sci(SCI_GETTEXTLENGTH, 0, 0);
    int total_lines = (int)sci(SCI_GETLINECOUNT, 0, 0);
    int edit_start = g_lock_prefix_len;
    int edit_end = total_len - g_lock_suffix_len;
    if (edit_end < edit_start) edit_end = edit_start;

    int first_edit_line = (int)sci(SCI_LINEFROMPOSITION, (WPARAM)edit_start, 0);
    int first_suffix_line = total_lines;
    if (g_lock_suffix_len > 0) {
        int end_line = (int)sci(SCI_LINEFROMPOSITION, (WPARAM)edit_end, 0);
        int end_line_start = (int)sci(SCI_POSITIONFROMLINE, (WPARAM)end_line, 0);
        first_suffix_line = (edit_end == end_line_start) ? end_line : (end_line + 1);
    }

    for (int ln = 0; ln < total_lines; ln++) {
        if (ln < first_edit_line || ln >= first_suffix_line) {
            sci(SCI_MARKERADD, (WPARAM)ln, 0);
        } else {
            sci(SCI_MARKERADD, (WPARAM)ln, 2);
        }
    }
}

static const char *MARK_START = "// <<<EDIT_START>>>";
static const char *MARK_END = "// <<<EDIT_END>>>";

static int template_has_markers(const char *tpl) {
    return tpl && strstr(tpl, MARK_START) != NULL && strstr(tpl, MARK_END) != NULL;
}

static char *strip_template_markers(const char *tpl) {
    StrBuf sb;
    sb_init(&sb);
    const char *p = tpl;
    while (*p) {
        const char *nl = strchr(p, '\n');
        size_t len = nl ? (size_t)(nl - p) : strlen(p);
        size_t s = 0, e = len;
        while (s < e && (p[s] == ' ' || p[s] == '\t' || p[s] == '\r')) s++;
        while (e > s && (p[e - 1] == ' ' || p[e - 1] == '\t' || p[e - 1] == '\r')) e--;
        int is_mark = ((e - s == strlen(MARK_START) && strncmp(p + s, MARK_START, e - s) == 0) ||
                       (e - s == strlen(MARK_END) && strncmp(p + s, MARK_END, e - s) == 0));
        if (!is_mark) {
            for (size_t k = 0; k < len; k++) if (p[k] != '\r') sb_append_char(&sb, p[k]);
            if (nl) sb_append_char(&sb, '\n');
        }
        if (!nl) break;
        p = nl + 1;
    }
    return sb.data;
}

/* Parse single-slot template into prefix (locked), middle (editable), suffix (locked).
 * Any leading // prompt comment lines inside EDIT_START..EDIT_END are automatically
 * folded into the locked prefix so prompt comments remain read-only by default. */
static int split_template_3way(const char *tpl, StrBuf *prefix, StrBuf *middle, StrBuf *suffix) {
    sb_init(prefix); sb_init(middle); sb_init(suffix);
    int stage = 0; /* 0=prefix, 1=middle, 2=suffix */
    int in_lead_comments = 1;
    const char *p = tpl;
    while (*p) {
        const char *nl = strchr(p, '\n');
        size_t len = nl ? (size_t)(nl - p) : strlen(p);
        size_t s = 0, e = len;
        while (s < e && (p[s] == ' ' || p[s] == '\t' || p[s] == '\r')) s++;
        while (e > s && (p[e - 1] == ' ' || p[e - 1] == '\t' || p[e - 1] == '\r')) e--;
        if (stage == 0 && e - s == strlen(MARK_START) && strncmp(p + s, MARK_START, e - s) == 0) {
            stage = 1;
            in_lead_comments = 1;
        } else if (stage == 1 && e - s == strlen(MARK_END) && strncmp(p + s, MARK_END, e - s) == 0) {
            stage = 2;
        } else {
            StrBuf *dst = (stage == 0) ? prefix : (stage == 1 ? middle : suffix);
            if (stage == 1 && in_lead_comments) {
                if (e - s >= 2 && p[s] == '/' && p[s + 1] == '/') {
                    dst = prefix;
                } else {
                    in_lead_comments = 0;
                }
            }
            for (size_t k = 0; k < len; k++) if (p[k] != '\r') sb_append_char(dst, p[k]);
            if (nl) sb_append_char(dst, '\n');
        }
        if (!nl) break;
        p = nl + 1;
    }
    if (stage == 2 && middle->len == 0) {
        sb_append_char(middle, '\n');
    }
    return stage == 2;
}

static int set_editor_content(const char *tpl, const char *saved_code, int prefer_unlocked) {
    g_has_partial = template_has_markers(tpl);
    int use_locked = (g_has_partial && !prefer_unlocked);
    g_lock_prefix_len = 0;
    g_lock_suffix_len = 0;

    char *final_code = NULL;
    int caret_pos = 0;

    if (use_locked) {
        StrBuf pre, mid, suf;
        if (split_template_3way(tpl, &pre, &mid, &suf)) {
            if (saved_code != NULL) {
                /* Verify saved_code starts with pre and ends with suf */
                size_t slen = strlen(saved_code);
                int pre_ok = (slen >= pre.len && memcmp(saved_code, pre.data, pre.len) == 0);
                /* Allow optional trailing newline variance on suffix */
                size_t suf_cmp_len = suf.len;
                int suf_ok = (slen >= pre.len + suf_cmp_len &&
                              memcmp(saved_code + slen - suf_cmp_len, suf.data, suf_cmp_len) == 0);
                if (!suf_ok && suf_cmp_len > 0 && suf.data[suf_cmp_len - 1] == '\n') {
                    suf_cmp_len--;
                    suf_ok = (slen >= pre.len + suf_cmp_len &&
                              memcmp(saved_code + slen - suf_cmp_len, suf.data, suf_cmp_len) == 0);
                }
                if (pre_ok && suf_ok) {
                    final_code = xstrdup(saved_code);
                    g_lock_prefix_len = (int)pre.len;
                    g_lock_suffix_len = (int)suf_cmp_len;
                    caret_pos = (int)pre.len;
                } else {
                    use_locked = 0;
                }
            } else {
                StrBuf all; sb_init(&all);
                sb_append_mem(&all, pre.data, pre.len);
                sb_append_mem(&all, mid.data, mid.len);
                sb_append_mem(&all, suf.data, suf.len);
                final_code = all.data;
                g_lock_prefix_len = (int)pre.len;
                g_lock_suffix_len = (int)suf.len;
                size_t c_off = 0;
                if (mid.len > 1 && mid.data[0] == '\n') c_off = 1;
                while (c_off < mid.len && (mid.data[c_off] == ' ' || mid.data[c_off] == '\t')) c_off++;
                caret_pos = (int)(pre.len + c_off);
            }
        } else {
            use_locked = 0;
        }
        sb_free(&pre); sb_free(&mid); sb_free(&suf);
    }

    if (!use_locked) {
        final_code = saved_code ? xstrdup(saved_code) : strip_template_markers(tpl);
        g_lock_prefix_len = 0;
        g_lock_suffix_len = 0;
    }

    g_unlocked = !use_locked;
    g_loading_code = 1;
    sci(SCI_SETTEXT, 0, (LPARAM)final_code);
    sci(SCI_EMPTYUNDOBUFFER, 0, 0);
    sci(SCI_GOTOPOS, (WPARAM)caret_pos, 0);
    refresh_locked_line_shading();
    g_loading_code = 0;
    free(final_code);
    return !g_unlocked;
}

/* ---------- Scintilla Lock Guard, Smart Indent, Auto-Pair & Autocomplete ---------- */
static void get_edit_bounds(int *out_start, int *out_end) {
    int total = (int)sci(SCI_GETTEXTLENGTH, 0, 0);
    if (g_unlocked || !g_has_partial) {
        *out_start = 0;
        *out_end = total;
    } else {
        *out_start = g_lock_prefix_len;
        *out_end = total - g_lock_suffix_len;
        if (*out_end < *out_start) *out_end = *out_start;
    }
}

static int sci_can_edit_selection(int is_backspace, int is_delete) {
    if (g_unlocked || !g_has_partial) return 1;
    int es = 0, ee = 0;
    get_edit_bounds(&es, &ee);
    int s0 = (int)sci(SCI_GETSELECTIONSTART, 0, 0);
    int s1 = (int)sci(SCI_GETSELECTIONEND, 0, 0);

    if (s0 != s1) {
        return (s0 >= es && s1 <= ee);
    }
    if (is_backspace) return (s0 > es && s0 <= ee);
    if (is_delete) return (s0 >= es && s0 < ee);
    return (s0 >= es && s0 <= ee);
}

static void show_lock_hint(void) {
    SetWindowTextW(g_hwnd_status, L"当前位置在模板锁定区 · 点击「解锁全部编辑」可修改全文件");
}

static void ac_popup_hide(void) {
    if (g_ac_visible) {
        g_ac_visible = 0;
        g_ac_hover = -1;
        if (g_hwnd_ac_popup) ShowWindow(g_hwnd_ac_popup, SW_HIDE);
    }
}

static void update_scintilla_calltip(void) {
    if (!g_hwnd_sci) return;
    int pos = (int)sci(SCI_GETCURRENTPOS, 0, 0);
    char *code = get_editor_utf8();
    int len = (int)strlen(code);
    if (ci_is_in_comment_or_string(code, pos)) {
        free(code);
        if (sci(SCI_CALLTIPACTIVE, 0, 0)) sci(SCI_CALLTIPCANCEL, 0, 0);
        return;
    }
    char sig[CI_MAX_SIG] = {0};
    int hs = 0, he = 0, call_pos = pos;
    int ok = ci_get_calltip(code, len, pos, sig, sizeof(sig), &hs, &he, &call_pos);
    free(code);
    if (ok && sig[0]) {
        sci(SCI_CALLTIPSHOW, (WPARAM)call_pos, (LPARAM)sig);
        if (he > hs) sci(SCI_CALLTIPSETHLT, (WPARAM)hs, (LPARAM)he);
    } else if (sci(SCI_CALLTIPACTIVE, 0, 0)) {
        sci(SCI_CALLTIPCANCEL, 0, 0);
    }
}

static void ac_popup_move_sel(int delta) {
    if (!g_ac_visible || g_ac_res.count <= 0) return;
    g_ac_sel = clamp_i(g_ac_sel + delta, 0, g_ac_res.count - 1);
    if (g_ac_sel < g_ac_top) g_ac_top = g_ac_sel;
    else if (g_ac_sel >= g_ac_top + AC_MAX_VISIBLE) g_ac_top = g_ac_sel - AC_MAX_VISIBLE + 1;
    if (g_hwnd_ac_popup) InvalidateRect(g_hwnd_ac_popup, NULL, FALSE);
}

static void ac_popup_accept(int from_lparen) {
    if (!g_ac_visible || g_ac_sel < 0 || g_ac_sel >= g_ac_res.count) {
        ac_popup_hide();
        return;
    }
    CICand item = g_ac_res.items[g_ac_sel];
    int mode = g_ac_res.mode;
    int plen = g_ac_res.prefix_len;
    ac_popup_hide();

    if (!sci_can_edit_selection(0, 0)) { show_lock_hint(); return; }
    int pos = (int)sci(SCI_GETCURRENTPOS, 0, 0);
    int rep_start = max(0, pos - plen);
    int es = 0, ee = 0;
    get_edit_bounds(&es, &ee);
    if (rep_start < es) rep_start = es;

    sci(SCI_BEGINUNDOACTION, 0, 0);
    sci(SCI_SETSEL, (WPARAM)rep_start, (LPARAM)pos);
    sci(SCI_REPLACESEL, 0, (LPARAM)item.name);
    int npos = rep_start + (int)strlen(item.name);

    if (mode == 3) {
        if ((int)sci(SCI_GETCHARAT, (WPARAM)npos, 0) != '>') {
            sci(SCI_INSERTTEXT, (WPARAM)npos, (LPARAM)">");
        }
        sci(SCI_GOTOPOS, (WPARAM)(npos + 1), 0);
    } else if (item.kind == CI_FUNC || from_lparen) {
        if ((int)sci(SCI_GETCHARAT, (WPARAM)npos, 0) != '(') {
            sci(SCI_INSERTTEXT, (WPARAM)npos, (LPARAM)"()");
        }
        sci(SCI_GOTOPOS, (WPARAM)(npos + 1), 0);
    } else {
        sci(SCI_GOTOPOS, (WPARAM)npos, 0);
    }
    sci(SCI_ENDUNDOACTION, 0, 0);

    if (item.kind == CI_FUNC || from_lparen) {
        update_scintilla_calltip();
    }
}

static void trigger_scintilla_autocomplete(int force) {
    if (!g_hwnd_sci || !g_hwnd_ac_popup) return;
    int pos = (int)sci(SCI_GETCURRENTPOS, 0, 0);
    char *code = get_editor_utf8();
    int len = (int)strlen(code);
    CIResult next_res;
    int mode = ci_query_completions(code, len, pos, force, &next_res);
    free(code);

    if (mode <= 0 || next_res.count <= 0) {
        ac_popup_hide();
        return;
    }

    /* Keep selected item sticky if still present */
    char prev_name[CI_MAX_NAME] = {0};
    if (g_ac_visible && g_ac_sel >= 0 && g_ac_sel < g_ac_res.count) {
        strncpy(prev_name, g_ac_res.items[g_ac_sel].name, CI_MAX_NAME - 1);
    }
    g_ac_res = next_res;
    g_ac_sel = 0;
    g_ac_top = 0;
    g_ac_hover = -1;
    if (prev_name[0]) {
        for (int i = 0; i < g_ac_res.count; i++) {
            if (strcmp(g_ac_res.items[i].name, prev_name) == 0) {
                g_ac_sel = i;
                if (g_ac_sel >= AC_MAX_VISIBLE) g_ac_top = g_ac_sel - AC_MAX_VISIBLE + 1;
                break;
            }
        }
    }

    int vis_rows = min(g_ac_res.count, AC_MAX_VISIBLE);
    int rowH = S(24);
    int popH = vis_rows * rowH + S(8);
    int popW = S(460);

    int anchor_pos = max(0, pos - g_ac_res.prefix_len);
    int px = (int)sci(SCI_POINTXFROMPOSITION, 0, (LPARAM)anchor_pos);
    int py = (int)sci(SCI_POINTYFROMPOSITION, 0, (LPARAM)pos);
    int cur_line = (int)sci(SCI_LINEFROMPOSITION, (WPARAM)pos, 0);
    int lineH = max(S(16), (int)sci(SCI_TEXTHEIGHT, (WPARAM)cur_line, 0));

    POINT pt = { px - S(22), py + lineH + S(2) };
    ClientToScreen(g_hwnd_sci, &pt);

    RECT wa = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &wa, 0);
    if (pt.x + popW > wa.right - S(8)) pt.x = max(wa.left + S(8), wa.right - S(8) - popW);
    if (pt.x < wa.left + S(8)) pt.x = wa.left + S(8);
    if (pt.y + popH > wa.bottom - S(8)) {
        POINT pt_top = { px, py };
        ClientToScreen(g_hwnd_sci, &pt_top);
        pt.y = max(wa.top + S(8), pt_top.y - popH - S(2));
    }

    g_ac_visible = 1;
    SetWindowPos(g_hwnd_ac_popup, HWND_TOPMOST, pt.x, pt.y, popW, popH, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    InvalidateRect(g_hwnd_ac_popup, NULL, FALSE);
}

static LRESULT CALLBACK AcPopupWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT cl; GetClientRect(hwnd, &cl);
            int W = cl.right - cl.left, H = cl.bottom - cl.top;
            if (W > 0 && H > 0) {
                const Theme *th = T();
                HDC memdc = CreateCompatibleDC(hdc);
                HBITMAP membmp = CreateCompatibleBitmap(hdc, W, H);
                HBITMAP oldbmp = (HBITMAP)SelectObject(memdc, membmp);

                HBRUSH bgb = CreateSolidBrush(th->surface);
                HPEN bdp = CreatePen(PS_SOLID, 1, th->border_strong);
                HPEN op = (HPEN)SelectObject(memdc, bdp);
                HBRUSH ob = (HBRUSH)SelectObject(memdc, bgb);
                Rectangle(memdc, 0, 0, W, H);
                SelectObject(memdc, op); SelectObject(memdc, ob);
                DeleteObject(bgb); DeleteObject(bdp);

                int rowH = S(24);
                int has_sb = (g_ac_res.count > AC_MAX_VISIBLE);
                int contentW = has_sb ? (W - S(10)) : (W - S(4));
                int vis_rows = min(g_ac_res.count - g_ac_top, AC_MAX_VISIBLE);
                SetBkMode(memdc, TRANSPARENT);

                for (int r = 0; r < vis_rows; r++) {
                    int idx = g_ac_top + r;
                    const CICand *it = &g_ac_res.items[idx];
                    int y0 = S(4) + r * rowH;
                    RECT rrc = { S(3), y0, contentW, y0 + rowH };

                    if (idx == g_ac_sel || idx == g_ac_hover) {
                        COLORREF hi_col = (idx == g_ac_sel) ? th->accent_soft : th->surface_alt;
                        HBRUSH hbr = CreateSolidBrush(hi_col);
                        HPEN hpen = CreatePen(PS_SOLID, 1, hi_col);
                        HPEN op2 = (HPEN)SelectObject(memdc, hpen);
                        HBRUSH ob2 = (HBRUSH)SelectObject(memdc, hbr);
                        RoundRect(memdc, rrc.left, rrc.top, rrc.right, rrc.bottom, S(4), S(4));
                        SelectObject(memdc, op2); SelectObject(memdc, ob2);
                        DeleteObject(hbr); DeleteObject(hpen);
                    }

                    /* Kind badge pill */
                    COLORREF bcol = RGB(71, 85, 105);
                    const wchar_t *blet = L"K";
                    switch (it->kind) {
                        case CI_VAR:    bcol = RGB(5, 150, 105);  blet = L"v"; break;
                        case CI_FUNC:   bcol = RGB(37, 99, 235);  blet = L"f"; break;
                        case CI_MEMBER: bcol = RGB(217, 119, 6);  blet = L"m"; break;
                        case CI_TYPE:   bcol = RGB(8, 145, 178);  blet = L"T"; break;
                        case CI_MACRO:  bcol = RGB(124, 58, 237); blet = L"#"; break;
                        case CI_HEADER: bcol = RGB(13, 148, 136); blet = L"H"; break;
                        default: break;
                    }
                    int bsz = S(16);
                    RECT brc = { S(8), y0 + (rowH - bsz) / 2, S(8) + bsz, y0 + (rowH - bsz) / 2 + bsz };
                    HBRUSH kbr = CreateSolidBrush(bcol);
                    HPEN kpen = CreatePen(PS_SOLID, 1, bcol);
                    HPEN op3 = (HPEN)SelectObject(memdc, kpen);
                    HBRUSH ob3 = (HBRUSH)SelectObject(memdc, kbr);
                    RoundRect(memdc, brc.left, brc.top, brc.right, brc.bottom, S(4), S(4));
                    SelectObject(memdc, op3); SelectObject(memdc, ob3);
                    DeleteObject(kbr); DeleteObject(kpen);

                    HFONT of = (HFONT)SelectObject(memdc, g_font_ui_sm);
                    SetTextColor(memdc, RGB(255, 255, 255));
                    DrawTextW(memdc, blet, -1, &brc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

                    /* Identifier name */
                    wchar_t *wname = utf8_to_wide_alloc(it->name);
                    int nameX = brc.right + S(8);
                    SelectObject(memdc, g_font_code);
                    SIZE nsz = {0};
                    GetTextExtentPoint32W(memdc, wname, (int)wcslen(wname), &nsz);
                    int nameW = min(nsz.cx + S(6), S(190));
                    RECT nrc = { nameX, y0, nameX + nameW, y0 + rowH };
                    SetTextColor(memdc, (idx == g_ac_sel) ? th->accent : th->text);
                    DrawTextW(memdc, wname, -1, &nrc, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
                    free(wname);

                    /* Right-side detail (type / function signature / header) */
                    if (it->detail[0]) {
                        wchar_t *wdet = utf8_to_wide_alloc(it->detail);
                        RECT drc = { nameX + nameW + S(8), y0, contentW - S(6), y0 + rowH };
                        if (drc.right > drc.left + S(20)) {
                            SelectObject(memdc, g_font_ui_sm);
                            SetTextColor(memdc, th->text_muted);
                            DrawTextW(memdc, wdet, -1, &drc, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
                        }
                        free(wdet);
                    }
                    SelectObject(memdc, of);
                }

                /* Subtle custom scrollbar thumb when > AC_MAX_VISIBLE items */
                if (has_sb) {
                    int trackY0 = S(4), trackH = H - S(8);
                    int thumbH = max(S(18), trackH * AC_MAX_VISIBLE / g_ac_res.count);
                    int maxTop = max(1, g_ac_res.count - AC_MAX_VISIBLE);
                    int thumbY = trackY0 + (trackH - thumbH) * g_ac_top / maxTop;
                    RECT trc = { W - S(7), thumbY, W - S(3), thumbY + thumbH };
                    HBRUSH tbr = CreateSolidBrush(th->border_strong);
                    FillRect(memdc, &trc, tbr);
                    DeleteObject(tbr);
                }

                BitBlt(hdc, 0, 0, W, H, memdc, 0, 0, SRCCOPY);
                SelectObject(memdc, oldbmp);
                DeleteObject(membmp);
                DeleteDC(memdc);
            }
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_MOUSEMOVE: {
            int y = (short)HIWORD(lp);
            int r = (y - S(4)) / S(24);
            int idx = g_ac_top + r;
            int nh = (r >= 0 && r < AC_MAX_VISIBLE && idx < g_ac_res.count) ? idx : -1;
            if (nh != g_ac_hover) {
                g_ac_hover = nh;
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }

        case WM_LBUTTONDOWN: {
            int y = (short)HIWORD(lp);
            int r = (y - S(4)) / S(24);
            int idx = g_ac_top + r;
            if (r >= 0 && r < AC_MAX_VISIBLE && idx < g_ac_res.count) {
                g_ac_sel = idx;
                ac_popup_accept(0);
            }
            return 0;
        }

        case WM_MOUSEWHEEL: {
            int delta = (short)HIWORD(wp);
            int step = (delta > 0) ? -2 : 2;
            int maxTop = max(0, g_ac_res.count - AC_MAX_VISIBLE);
            g_ac_top = clamp_i(g_ac_top + step, 0, maxTop);
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static void update_brace_matching(void) {
    int pos = (int)sci(SCI_GETCURRENTPOS, 0, 0);
    int bpos = -1;
    int c1 = (int)sci(SCI_GETCHARAT, (WPARAM)pos, 0);
    int c0 = (pos > 0) ? (int)sci(SCI_GETCHARAT, (WPARAM)(pos - 1), 0) : 0;
    if (c0 == '(' || c0 == ')' || c0 == '[' || c0 == ']' || c0 == '{' || c0 == '}') bpos = pos - 1;
    else if (c1 == '(' || c1 == ')' || c1 == '[' || c1 == ']' || c1 == '{' || c1 == '}') bpos = pos;

    if (bpos >= 0) {
        int m = (int)sci(SCI_BRACEMATCH, (WPARAM)bpos, 0);
        if (m >= 0) sci(SCI_BRACEHIGHLIGHT, (WPARAM)bpos, (LPARAM)m);
        else sci(SCI_BRACEBADLIGHT, (WPARAM)bpos, 0);
    } else {
        sci(SCI_BRACEHIGHLIGHT, (WPARAM)-1, (LPARAM)-1);
    }
}

static void toggle_line_comment(void) {
    if (!sci_can_edit_selection(0, 0)) { show_lock_hint(); return; }
    int s0 = (int)sci(SCI_GETSELECTIONSTART, 0, 0);
    int s1 = (int)sci(SCI_GETSELECTIONEND, 0, 0);
    int line0 = (int)sci(SCI_LINEFROMPOSITION, (WPARAM)s0, 0);
    int line1 = (int)sci(SCI_LINEFROMPOSITION, (WPARAM)s1, 0);
    if (s1 > s0 && s1 == (int)sci(SCI_POSITIONFROMLINE, (WPARAM)line1, 0) && line1 > line0) line1--;

    int es = 0, ee = 0;
    get_edit_bounds(&es, &ee);
    if (!g_unlocked && g_has_partial) {
        int l0s = (int)sci(SCI_POSITIONFROMLINE, (WPARAM)line0, 0);
        int l1e = (int)sci(SCI_GETLINEENDPOSITION, (WPARAM)line1, 0);
        if (l0s < es || l1e > ee) { show_lock_hint(); return; }
    }

    int all_commented = 1;
    int any_nonblank = 0;
    for (int ln = line0; ln <= line1; ln++) {
        int ls = (int)sci(SCI_POSITIONFROMLINE, (WPARAM)ln, 0);
        int le = (int)sci(SCI_GETLINEENDPOSITION, (WPARAM)ln, 0);
        int p = ls;
        while (p < le && ((int)sci(SCI_GETCHARAT, (WPARAM)p, 0) == ' ' || (int)sci(SCI_GETCHARAT, (WPARAM)p, 0) == '\t')) p++;
        if (p < le) {
            any_nonblank = 1;
            if (!(p + 1 < le && (int)sci(SCI_GETCHARAT, (WPARAM)p, 0) == '/' && (int)sci(SCI_GETCHARAT, (WPARAM)(p + 1), 0) == '/')) {
                all_commented = 0;
            }
        }
    }

    sci(SCI_BEGINUNDOACTION, 0, 0);
    if (any_nonblank && all_commented) {
        for (int ln = line1; ln >= line0; ln--) {
            int ls = (int)sci(SCI_POSITIONFROMLINE, (WPARAM)ln, 0);
            int le = (int)sci(SCI_GETLINEENDPOSITION, (WPARAM)ln, 0);
            int p = ls;
            while (p < le && ((int)sci(SCI_GETCHARAT, (WPARAM)p, 0) == ' ' || (int)sci(SCI_GETCHARAT, (WPARAM)p, 0) == '\t')) p++;
            if (p + 1 < le && (int)sci(SCI_GETCHARAT, (WPARAM)p, 0) == '/' && (int)sci(SCI_GETCHARAT, (WPARAM)(p + 1), 0) == '/') {
                int del_n = (p + 2 < le && (int)sci(SCI_GETCHARAT, (WPARAM)(p + 2), 0) == ' ') ? 3 : 2;
                sci(SCI_DELETERANGE, (WPARAM)p, (LPARAM)del_n);
            }
        }
    } else {
        for (int ln = line1; ln >= line0; ln--) {
            int ls = (int)sci(SCI_POSITIONFROMLINE, (WPARAM)ln, 0);
            int le = (int)sci(SCI_GETLINEENDPOSITION, (WPARAM)ln, 0);
            int p = ls;
            while (p < le && ((int)sci(SCI_GETCHARAT, (WPARAM)p, 0) == ' ' || (int)sci(SCI_GETCHARAT, (WPARAM)p, 0) == '\t')) p++;
            if (p < le || line0 == line1) {
                sci(SCI_INSERTTEXT, (WPARAM)p, (LPARAM)"// ");
            }
        }
    }
    sci(SCI_ENDUNDOACTION, 0, 0);
}

static LRESULT CALLBACK ScintillaSubclassProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    /* Suppress queued WM_CHAR generated by TranslateMessage when WM_KEYDOWN already handled the key! */
    static int swallow_char = 0;

    if (msg == WM_KILLFOCUS || msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN) {
        ac_popup_hide();
    }
    if (msg == WM_MOUSEWHEEL) {
        if (g_ac_visible && g_hwnd_ac_popup) {
            POINT pt = { (short)LOWORD(lp), (short)HIWORD(lp) };
            if (WindowFromPoint(pt) == g_hwnd_ac_popup) {
                SendMessageW(g_hwnd_ac_popup, WM_MOUSEWHEEL, wp, lp);
                return 0;
            }
        }
        if (g_hwnd_tree && !(LOWORD(wp) & MK_CONTROL)) {
            POINT pt = { (short)LOWORD(lp), (short)HIWORD(lp) };
            if (WindowFromPoint(pt) == g_hwnd_tree) {
                SendMessageW(g_hwnd_tree, WM_MOUSEWHEEL, wp, lp);
                return 0;
            }
        }
        if (LOWORD(wp) & MK_CONTROL) {
            LRESULT r = CallWindowProcW(g_orig_sci_proc, hwnd, msg, wp, lp);
            int z = clamp_i((int)sci(SCI_GETZOOM, 0, 0), -4, 10);
            if (z != g_editor_zoom) {
                g_editor_zoom = z;
                sci(SCI_SETZOOM, (WPARAM)g_editor_zoom, 0);
                if (g_hwnd_sol_sci) SendMessageW(g_hwnd_sol_sci, SCI_SETZOOM, (WPARAM)g_editor_zoom, 0);
            }
            return r;
        }
    }
    if (msg == WM_KEYDOWN) {
        swallow_char = 0;
        BOOL ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;

        /* Custom RedPanda-style completion popup navigation & accept */
        if (g_ac_visible && !ctrl) {
            if (wp == VK_DOWN) { ac_popup_move_sel(1); return 0; }
            if (wp == VK_UP) { ac_popup_move_sel(-1); return 0; }
            if (wp == VK_NEXT) { ac_popup_move_sel(AC_MAX_VISIBLE); return 0; }
            if (wp == VK_PRIOR) { ac_popup_move_sel(-AC_MAX_VISIBLE); return 0; }
            if (wp == VK_RETURN) { swallow_char = '\r'; ac_popup_accept(0); return 0; }
            if (wp == VK_TAB) { swallow_char = '\t'; ac_popup_accept(0); return 0; }
            if (wp == VK_ESCAPE) { ac_popup_hide(); return 0; }
            if (wp == VK_LEFT || wp == VK_RIGHT || wp == VK_HOME || wp == VK_END) {
                ac_popup_hide();
            }
        }

        if (ctrl && wp == VK_OEM_2) {
            swallow_char = '/';
            ac_popup_hide();
            toggle_line_comment();
            return 0;
        }
        if (ctrl && (wp == '0' || wp == VK_NUMPAD0)) {
            g_editor_zoom = 0;
            sci(SCI_SETZOOM, 0, 0);
            if (g_hwnd_sol_sci) SendMessageW(g_hwnd_sol_sci, SCI_SETZOOM, 0, 0);
            return 0;
        }

        if (ctrl && wp == VK_RETURN) {
            swallow_char = '\n';
            ac_popup_hide();
            if (sci(SCI_CALLTIPACTIVE, 0, 0)) sci(SCI_CALLTIPCANCEL, 0, 0);
            trigger_grade_or_run(0);
            return 0;
        }
        if (wp == VK_F5) {
            ac_popup_hide();
            if (sci(SCI_CALLTIPACTIVE, 0, 0)) sci(SCI_CALLTIPCANCEL, 0, 0);
            trigger_grade_or_run(1);
            return 0;
        }
        if (wp == VK_ESCAPE) {
            if (sci(SCI_CALLTIPACTIVE, 0, 0)) sci(SCI_CALLTIPCANCEL, 0, 0);
            ac_popup_hide();
        }
        if (ctrl && wp == VK_SPACE) {
            swallow_char = ' ';
            if (sci_can_edit_selection(0, 0)) trigger_scintilla_autocomplete(1);
            return 0;
        }
        if (ctrl && wp == 'A' && !g_unlocked && g_has_partial) {
            ac_popup_hide();
            int es = 0, ee = 0;
            get_edit_bounds(&es, &ee);
            sci(SCI_SETSEL, (WPARAM)es, (LPARAM)ee);
            return 0;
        }
        if (ctrl && (wp == 'X' || wp == 'V')) {
            if (!sci_can_edit_selection(0, 0)) { show_lock_hint(); return 0; }
            ac_popup_hide();
        }
        if (wp == VK_BACK) {
            if (!sci_can_edit_selection(1, 0)) { swallow_char = '\b'; show_lock_hint(); return 0; }
            /* Auto-delete matching empty pair (), [], {}, "", '' */
            int s0 = (int)sci(SCI_GETSELECTIONSTART, 0, 0);
            int s1 = (int)sci(SCI_GETSELECTIONEND, 0, 0);
            if (s0 == s1 && s0 > 0) {
                int es = 0, ee = 0; get_edit_bounds(&es, &ee);
                int prev = (int)sci(SCI_GETCHARAT, (WPARAM)(s0 - 1), 0);
                int next = (int)sci(SCI_GETCHARAT, (WPARAM)s0, 0);
                if (s0 < ee &&
                    ((prev == '(' && next == ')') || (prev == '[' && next == ']') ||
                     (prev == '{' && next == '}') || (prev == '"' && next == '"') ||
                     (prev == '\'' && next == '\''))) {
                    swallow_char = '\b';
                    ac_popup_hide();
                    sci(SCI_BEGINUNDOACTION, 0, 0);
                    sci(SCI_DELETERANGE, (WPARAM)(s0 - 1), 2);
                    sci(SCI_ENDUNDOACTION, 0, 0);
                    update_scintilla_calltip();
                    return 0;
                }
            }
            if (g_ac_visible) {
                LRESULT r = CallWindowProcW(g_orig_sci_proc, hwnd, msg, wp, lp);
                trigger_scintilla_autocomplete(0);
                return r;
            }
        }
        if (wp == VK_DELETE) {
            if (!sci_can_edit_selection(0, 1)) { show_lock_hint(); return 0; }
            ac_popup_hide();
        }
        if (wp == VK_TAB && !ctrl) {
            if (!sci_can_edit_selection(0, 0)) { swallow_char = '\t'; show_lock_hint(); return 0; }
        }
        if (wp == VK_RETURN && !ctrl) {
            swallow_char = '\r';
            if (!sci_can_edit_selection(0, 0)) { show_lock_hint(); return 0; }
            /* Smart auto-indent Enter (creates exactly ONE newline, or splits {} cleanly) */
            int s0 = (int)sci(SCI_GETSELECTIONSTART, 0, 0);
            int s1 = (int)sci(SCI_GETSELECTIONEND, 0, 0);
            int line = (int)sci(SCI_LINEFROMPOSITION, (WPARAM)s0, 0);
            int lstart = (int)sci(SCI_POSITIONFROMLINE, (WPARAM)line, 0);
            int line_ind = (int)sci(SCI_GETLINEINDENTATION, (WPARAM)line, 0);
            int col = s0 - lstart;
            int ind = min(col, line_ind);
            int prev = (s0 > 0) ? (int)sci(SCI_GETCHARAT, (WPARAM)(s0 - 1), 0) : 0;
            int next = (int)sci(SCI_GETCHARAT, (WPARAM)s1, 0);

            sci(SCI_BEGINUNDOACTION, 0, 0);
            if (s0 != s1) sci(SCI_REPLACESEL, 0, (LPARAM)"");
            int pos = (int)sci(SCI_GETCURRENTPOS, 0, 0);

            if (prev == '{' && next == '}') {
                StrBuf ins; sb_init(&ins);
                sb_append_char(&ins, '\n');
                for (int k = 0; k < line_ind + 4; k++) sb_append_char(&ins, ' ');
                int mid_off = (int)ins.len;
                sb_append_char(&ins, '\n');
                for (int k = 0; k < line_ind; k++) sb_append_char(&ins, ' ');
                sci(SCI_INSERTTEXT, (WPARAM)pos, (LPARAM)ins.data);
                sci(SCI_GOTOPOS, (WPARAM)(pos + mid_off), 0);
                sb_free(&ins);
            } else {
                int next_ind = (prev == '{') ? (line_ind + 4) : ind;
                StrBuf ins; sb_init(&ins);
                sb_append_char(&ins, '\n');
                for (int k = 0; k < next_ind; k++) sb_append_char(&ins, ' ');
                sci(SCI_INSERTTEXT, (WPARAM)pos, (LPARAM)ins.data);
                sci(SCI_GOTOPOS, (WPARAM)(pos + (int)ins.len), 0);
                sb_free(&ins);
            }
            sci(SCI_ENDUNDOACTION, 0, 0);
            return 0;
        }
    } else if (msg == WM_PASTE || msg == WM_CUT || msg == WM_CLEAR || msg == WM_IME_STARTCOMPOSITION) {
        if (!sci_can_edit_selection(0, 0)) { show_lock_hint(); return 0; }
        ac_popup_hide();
    } else if (msg == WM_CHAR) {
        unsigned ch = (unsigned)wp;
        if (swallow_char != 0) {
            int expected = swallow_char;
            swallow_char = 0;
            if ((int)ch == expected || (expected == '\r' && ch == '\n') || (expected == '\n' && ch == '\r')) {
                return 0;
            }
        }
        if (ch >= 32) {
            if (!sci_can_edit_selection(0, 0)) { show_lock_hint(); return 0; }
            int s0 = (int)sci(SCI_GETSELECTIONSTART, 0, 0);
            int s1 = (int)sci(SCI_GETSELECTIONEND, 0, 0);

            /* Never hijack '(' into accepting a completion item: dismiss popup cleanly and pair () */
            if (ch == '(' && g_ac_visible) {
                ac_popup_hide();
            }

            /* Skip over matching closing bracket/quote */
            if (s0 == s1 && (ch == ')' || ch == ']' || ch == '}' || ch == '"' || ch == '\'')) {
                int next = (int)sci(SCI_GETCHARAT, (WPARAM)s0, 0);
                if (next == (int)ch) {
                    ac_popup_hide();
                    sci(SCI_GOTOPOS, (WPARAM)(s0 + 1), 0);
                    if (ch == ')') update_scintilla_calltip();
                    return 0;
                }
            }

            /* Smart outdent when typing '}' on an indented blank line */
            if (s0 == s1 && ch == '}') {
                ac_popup_hide();
                int line = (int)sci(SCI_LINEFROMPOSITION, (WPARAM)s0, 0);
                int lstart = (int)sci(SCI_POSITIONFROMLINE, (WPARAM)line, 0);
                int only_sp = 1;
                for (int k = lstart; k < s0; k++) {
                    if ((int)sci(SCI_GETCHARAT, (WPARAM)k, 0) != ' ') { only_sp = 0; break; }
                }
                int ind = s0 - lstart;
                if (only_sp && ind >= 4) {
                    sci(SCI_BEGINUNDOACTION, 0, 0);
                    sci(SCI_SETLINEINDENTATION, (WPARAM)line, (LPARAM)(ind - 4));
                    int npos = (int)sci(SCI_GETLINEENDPOSITION, (WPARAM)line, 0);
                    sci(SCI_INSERTTEXT, (WPARAM)npos, (LPARAM)"}");
                    sci(SCI_GOTOPOS, (WPARAM)(npos + 1), 0);
                    sci(SCI_ENDUNDOACTION, 0, 0);
                    return 0;
                }
            }

            /* Auto-pair open brackets & quotes */
            char close_ch = 0;
            if (ch == '(') close_ch = ')';
            else if (ch == '[') close_ch = ']';
            else if (ch == '{') close_ch = '}';
            else if (ch == '"') close_ch = '"';
            else if (ch == '\'') close_ch = '\'';

            if (close_ch) {
                ac_popup_hide();
                if (s0 != s1) {
                    char open_s[2] = { (char)ch, 0 };
                    char close_s[2] = { close_ch, 0 };
                    sci(SCI_BEGINUNDOACTION, 0, 0);
                    sci(SCI_INSERTTEXT, (WPARAM)s1, (LPARAM)close_s);
                    sci(SCI_INSERTTEXT, (WPARAM)s0, (LPARAM)open_s);
                    sci(SCI_SETSEL, (WPARAM)(s0 + 1), (LPARAM)(s1 + 1));
                    sci(SCI_ENDUNDOACTION, 0, 0);
                    if (ch == '(') update_scintilla_calltip();
                    return 0;
                }
                int prev = (s0 > 0) ? (int)sci(SCI_GETCHARAT, (WPARAM)(s0 - 1), 0) : 0;
                int next = (int)sci(SCI_GETCHARAT, (WPARAM)s0, 0);
                int is_quote = (ch == '"' || ch == '\'');
                int skip_auto = is_quote && (isalnum(prev) || prev == '_' || isalnum(next) || next == '_');
                if (!skip_auto) {
                    char pair[3] = { (char)ch, close_ch, 0 };
                    sci(SCI_BEGINUNDOACTION, 0, 0);
                    sci(SCI_INSERTTEXT, (WPARAM)s0, (LPARAM)pair);
                    sci(SCI_GOTOPOS, (WPARAM)(s0 + 1), 0);
                    sci(SCI_ENDUNDOACTION, 0, 0);
                    if (ch == '(') update_scintilla_calltip();
                    return 0;
                }
            }
        }
    }
    return CallWindowProcW(g_orig_sci_proc, hwnd, msg, wp, lp);
}

/* ---------- Modern Owner-Drawn Button ---------- */
static LRESULT CALLBACK ModernBtnProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_MOUSEMOVE) {
        if (g_hover_btn != hwnd) {
            HWND old = g_hover_btn;
            g_hover_btn = hwnd;
            if (old) InvalidateRect(old, NULL, FALSE);
            InvalidateRect(hwnd, NULL, FALSE);
            TRACKMOUSEEVENT tme;
            tme.cbSize = sizeof(tme);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            tme.dwHoverTime = 0;
            TrackMouseEvent(&tme);
        }
    } else if (msg == WM_MOUSELEAVE) {
        if (g_hover_btn == hwnd) { g_hover_btn = NULL; InvalidateRect(hwnd, NULL, FALSE); }
    }
    return CallWindowProcW(g_orig_btn_proc, hwnd, msg, wp, lp);
}

#define ID_SOL_CLOSE 201
#define ID_SOL_COPY  202

static void draw_modern_button(const DRAWITEMSTRUCT *dis) {
    const Theme *th = T();
    HDC hdc = dis->hDC;
    RECT rc = dis->rcItem;
    int id = (int)dis->CtlID;
    int disabled = (dis->itemState & ODS_DISABLED) != 0;
    int pressed = (dis->itemState & ODS_SELECTED) != 0;
    int hovered = (!disabled && g_hover_btn == dis->hwndItem);

    int is_top = (id == ID_BTN_PREV || id == ID_BTN_NEXT || id == ID_BTN_TOGGLE_NAV || id == ID_BTN_TOGGLE_REF || id == ID_BTN_FONT_SIZE || id == ID_BTN_THEME || id == ID_BTN_RESET_ALL);
    int is_flt = (id == ID_FLT_ALL || id == ID_FLT_TODO || id == ID_FLT_HARD);
    int flt_active = (id == ID_FLT_ALL && g_tree_filter == 0) ||
                     (id == ID_FLT_TODO && g_tree_filter == 1) ||
                     (id == ID_FLT_HARD && g_tree_filter == 2);
    int toggle_active = (id == ID_BTN_TOGGLE_NAV && g_nav_hidden) || (id == ID_BTN_TOGGLE_REF && g_ref_hidden);

    FillRect(hdc, &rc, is_top ? g_br_header : (is_flt ? g_br_surface : g_br_bg));

    COLORREF fill_col, border_col, txt_col;
    if (id == ID_BTN_SUBMIT) {
        fill_col = disabled ? th->border_strong : (pressed ? th->accent_hover : (hovered ? th->accent_hover : th->accent));
        border_col = fill_col;
        txt_col = RGB(255, 255, 255);
    } else if (id == ID_BTN_RUN || id == ID_SOL_COPY || toggle_active || flt_active) {
        fill_col = disabled ? th->surface_alt : (hovered ? th->accent_soft : ((toggle_active || flt_active) ? th->accent_soft : th->surface));
        border_col = disabled ? th->border : (flt_active ? th->accent : th->accent);
        txt_col = disabled ? th->text_muted : th->accent;
    } else if (is_flt) {
        fill_col = pressed ? th->border : (hovered ? th->surface_alt : th->surface);
        border_col = th->border;
        txt_col = th->text_muted;
    } else {
        fill_col = disabled ? th->surface_alt : (pressed ? th->border : (hovered ? th->surface_alt : th->surface));
        border_col = disabled ? th->border : th->border_strong;
        txt_col = disabled ? th->text_muted : th->text;
    }

    HPEN pen = CreatePen(PS_SOLID, 1, border_col);
    HBRUSH br = CreateSolidBrush(fill_col);
    HPEN oldp = (HPEN)SelectObject(hdc, pen);
    HBRUSH oldb = (HBRUSH)SelectObject(hdc, br);
    RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, S(6), S(6));
    SelectObject(hdc, oldp); SelectObject(hdc, oldb);
    DeleteObject(pen); DeleteObject(br);

    wchar_t text[64] = {0};
    GetWindowTextW(dis->hwndItem, text, 64);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, txt_col);
    HFONT oldf = (HFONT)SelectObject(hdc, (id == ID_BTN_SUBMIT || flt_active) ? g_font_ui_bold : g_font_ui_sm);
    DrawTextW(hdc, text, -1, (LPRECT)&rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, oldf);
}

/* ---------- Progress Helpers & Collapsible Word-Wrap Tree ---------- */
static int is_ex_passed(const char *ex_id) {
    return j_get_bool(j_obj_get(g_core.progress, "passed"), ex_id, 0);
}
static int is_quest_mode(void) {
    return j_get_bool(j_obj_get(g_core.progress, "settings"), "quest_mode", 0);
}
static int is_quest_allowed(int flat_idx) {
    if (!is_quest_mode() || flat_idx <= 0) return 1;
    if (is_ex_passed(g_flat[flat_idx].id)) return 1;
    return is_ex_passed(g_flat[flat_idx - 1].id);
}

static void format_tree_row_text(const TreeRow *row, wchar_t *out, int out_cch, wchar_t *badge, int badge_cch) {
    badge[0] = L'\0';
    if (row->is_mod) {
        JVal *mod = g_core.modules->items[row->mod_idx];
        JVal *exs = j_obj_get(mod, "exercises");
        int mdone = 0, mtotal = exs ? (int)exs->arr_len : 0;
        for (int j = 0; j < mtotal; j++) if (is_ex_passed(j_get_str(exs->items[j], "id", ""))) mdone++;
        wchar_t *wtitle = utf8_to_wide_alloc(j_get_str(mod, "title", ""));
        _snwprintf(out, (size_t)out_cch, L"%ls %ls", g_mod_collapsed[row->mod_idx] ? L"▸" : L"▾", wtitle);
        free(wtitle);
        if (mdone == mtotal && mtotal > 0) _snwprintf(badge, (size_t)badge_cch, L"已通关");
        else _snwprintf(badge, (size_t)badge_cch, L"%d/%d", mdone, mtotal);
    } else {
        JVal *ex = g_flat[row->flat_idx].ex;
        wchar_t *wcode = utf8_to_wide_alloc(j_get_str(ex, "code", ""));
        wchar_t *wtitle = utf8_to_wide_alloc(j_get_str(ex, "title", ""));
        _snwprintf(out, (size_t)out_cch, L"%ls  %ls", wcode, wtitle);
        free(wcode); free(wtitle);
    }
}

static int calc_tree_row_height(int row_idx, int list_w) {
    if (row_idx < 0 || row_idx >= g_tree_row_count) return S(28);
    const TreeRow *row = &g_tree_rows[row_idx];
    wchar_t text[256], badge[32];
    format_tree_row_text(row, text, 256, badge, 32);

    int avail_w = list_w - (row->is_mod ? S(62) : S(38));
    if (avail_w < S(80)) avail_w = S(80);
    HDC hdc = GetDC(g_hwnd_tree);
    HFONT of = (HFONT)SelectObject(hdc, row->is_mod ? g_font_tree_bold : g_font_tree);
    RECT rc = { 0, 0, avail_w, 0 };
    DrawTextW(hdc, text, -1, &rc, DT_WORDBREAK | DT_CALCRECT);
    SelectObject(hdc, of);
    ReleaseDC(g_hwnd_tree, hdc);
    int h = rc.bottom + (row->is_mod ? S(14) : S(12));
    return max(h, row->is_mod ? S(30) : S(26));
}

static void tree_update_scrollbar(void) {
    if (!g_hwnd_tree) return;
    RECT rc; GetClientRect(g_hwnd_tree, &rc);
    int clientH = max(1, rc.bottom - rc.top);
    int maxScroll = max(0, g_tree_total_h - clientH);
    g_tree_scroll_y = clamp_i(g_tree_scroll_y, 0, maxScroll);

    SCROLLINFO si;
    memset(&si, 0, sizeof(si));
    si.cbSize = sizeof(si);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMin = 0;
    si.nMax = max(0, g_tree_total_h - 1);
    si.nPage = (UINT)clientH;
    si.nPos = g_tree_scroll_y;
    SetScrollInfo(g_hwnd_tree, SB_VERT, &si, TRUE);
}

static void tree_scroll_to(int new_y) {
    if (!g_hwnd_tree) return;
    RECT rc; GetClientRect(g_hwnd_tree, &rc);
    int clientH = max(1, rc.bottom - rc.top);
    int maxScroll = max(0, g_tree_total_h - clientH);
    int clamped = clamp_i(new_y, 0, maxScroll);
    if (clamped != g_tree_scroll_y) {
        g_tree_scroll_y = clamped;
        tree_update_scrollbar();
        InvalidateRect(g_hwnd_tree, NULL, FALSE);
    }
}

static int tree_hit_test(int client_y) {
    int y = client_y + g_tree_scroll_y;
    for (int i = 0; i < g_tree_row_count; i++) {
        if (y >= g_tree_rows[i].y_top && y < g_tree_rows[i].y_top + g_tree_rows[i].height) {
            return i;
        }
    }
    return -1;
}

static void recalc_tree_row_heights(void) {
    if (!g_hwnd_tree) return;
    RECT rc; GetClientRect(g_hwnd_tree, &rc);
    int list_w = rc.right - rc.left;
    if (list_w < S(100)) list_w = S(100);

    int y = S(4);
    for (int i = 0; i < g_tree_row_count; i++) {
        int h = calc_tree_row_height(i, list_w);
        g_tree_rows[i].y_top = y;
        g_tree_rows[i].height = h;
        y += h;
    }
    g_tree_total_h = y + S(6);
    tree_update_scrollbar();

    /* If scrollbar visibility toggled and altered client width, re-measure once */
    RECT rc2; GetClientRect(g_hwnd_tree, &rc2);
    int list_w2 = rc2.right - rc2.left;
    if (list_w2 >= S(100) && list_w2 != list_w) {
        y = S(4);
        for (int i = 0; i < g_tree_row_count; i++) {
            int h = calc_tree_row_height(i, list_w2);
            g_tree_rows[i].y_top = y;
            g_tree_rows[i].height = h;
            y += h;
        }
        g_tree_total_h = y + S(6);
        tree_update_scrollbar();
    }
    InvalidateRect(g_hwnd_tree, NULL, FALSE);
}

static void sync_tree_selection(void) {
    if (!g_hwnd_tree) return;
    RECT rc; GetClientRect(g_hwnd_tree, &rc);
    int clientH = max(1, rc.bottom - rc.top);
    for (int i = 0; i < g_tree_row_count; i++) {
        if (!g_tree_rows[i].is_mod && g_tree_rows[i].flat_idx == g_cur_flat) {
            int top = g_tree_rows[i].y_top;
            int bot = top + g_tree_rows[i].height;
            if (top < g_tree_scroll_y) {
                tree_scroll_to(max(0, top - S(4)));
            } else if (bot > g_tree_scroll_y + clientH) {
                tree_scroll_to(bot - clientH + S(4));
            }
            InvalidateRect(g_hwnd_tree, NULL, FALSE);
            return;
        }
    }
}

static int ex_matches_tree_filter(int fi) {
    if (g_tree_filter == 0) return 1;
    if (g_tree_filter == 1) return !is_ex_passed(g_flat[fi].id);
    if (g_tree_filter == 2) {
        const char *k = j_get_str(g_flat[fi].ex, "kind", "single");
        return (strcmp(k, "stage") == 0 || strcmp(k, "special") == 0);
    }
    return 1;
}

static void rebuild_tree_rows(int preserve_scroll) {
    g_tree_row_count = 0;
    size_t nmods = g_core.modules ? g_core.modules->arr_len : 0;
    for (size_t m = 0; m < nmods && m < 32; m++) {
        int match_cnt = 0;
        for (int fi = 0; fi < g_flat_count; fi++) {
            if (g_flat[fi].mod_idx == (int)m && ex_matches_tree_filter(fi)) match_cnt++;
        }
        if (g_tree_filter != 0 && match_cnt == 0) continue;

        int r = g_tree_row_count++;
        g_tree_rows[r].is_mod = 1; g_tree_rows[r].mod_idx = (int)m; g_tree_rows[r].flat_idx = -1;
        g_tree_rows[r].y_top = 0; g_tree_rows[r].height = 0;
        if (!g_mod_collapsed[m]) {
            for (int fi = 0; fi < g_flat_count; fi++) {
                if (g_flat[fi].mod_idx == (int)m && ex_matches_tree_filter(fi) && g_tree_row_count < 300) {
                    int er = g_tree_row_count++;
                    g_tree_rows[er].is_mod = 0; g_tree_rows[er].mod_idx = (int)m; g_tree_rows[er].flat_idx = fi;
                    g_tree_rows[er].y_top = 0; g_tree_rows[er].height = 0;
                }
            }
        }
    }
    if (!preserve_scroll) g_tree_scroll_y = 0;
    g_tree_hover_row = -1;
    recalc_tree_row_heights();
    if (!preserve_scroll) sync_tree_selection();
}

static void draw_tree_row_at(HDC hdc, int row_idx, const RECT *rc_in, int hovered) {
    if (row_idx < 0 || row_idx >= g_tree_row_count) return;
    const Theme *th = T();
    RECT rc = *rc_in;
    const TreeRow *row = &g_tree_rows[row_idx];

    wchar_t text[256], badge[32];
    format_tree_row_text(row, text, 256, badge, 32);

    int active = (!row->is_mod && row->flat_idx == g_cur_flat);

    if (row->is_mod) {
        if (row_idx > 0) {
            HPEN dpen = CreatePen(PS_SOLID, 1, th->border);
            HPEN op = (HPEN)SelectObject(hdc, dpen);
            MoveToEx(hdc, rc.left + S(6), rc.top + 1, NULL);
            LineTo(hdc, rc.right - S(6), rc.top + 1);
            SelectObject(hdc, op); DeleteObject(dpen);
        }
        if (hovered) {
            RECT hi = { rc.left + S(4), rc.top + S(3), rc.right - S(4), rc.bottom - S(1) };
            HBRUSH hbr = CreateSolidBrush(th->surface_alt);
            HPEN hpen = CreatePen(PS_SOLID, 1, th->surface_alt);
            HPEN op = (HPEN)SelectObject(hdc, hpen); HBRUSH ob = (HBRUSH)SelectObject(hdc, hbr);
            RoundRect(hdc, hi.left, hi.top, hi.right, hi.bottom, S(5), S(5));
            SelectObject(hdc, op); SelectObject(hdc, ob);
            DeleteObject(hbr); DeleteObject(hpen);
        }
        RECT trc = { rc.left + S(8), rc.top + S(7), rc.right - S(52), rc.bottom - S(5) };
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, th->text);
        HFONT of = (HFONT)SelectObject(hdc, g_font_tree_bold);
        DrawTextW(hdc, text, -1, &trc, DT_LEFT | DT_WORDBREAK);

        RECT brc = { rc.right - S(50), rc.top + S(7), rc.right - S(6), rc.top + S(25) };
        int is_clear = (wcscmp(badge, L"已通关") == 0);
        if (is_clear) {
            HBRUSH bbr = CreateSolidBrush(th->pass_soft);
            HPEN bpen = CreatePen(PS_SOLID, 1, th->pass_soft);
            HPEN op = (HPEN)SelectObject(hdc, bpen); HBRUSH ob = (HBRUSH)SelectObject(hdc, bbr);
            RoundRect(hdc, brc.left, brc.top, brc.right, brc.bottom, S(5), S(5));
            SelectObject(hdc, op); SelectObject(hdc, ob);
            DeleteObject(bbr); DeleteObject(bpen);
        }
        SetTextColor(hdc, is_clear ? th->pass : th->text_muted);
        SelectObject(hdc, g_font_ui_sm);
        DrawTextW(hdc, badge, -1, &brc, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, of);
    } else {
        int pass = is_ex_passed(g_flat[row->flat_idx].id);
        int allow = is_quest_allowed(row->flat_idx);
        const char *kind = j_get_str(g_flat[row->flat_idx].ex, "kind", "single");

        if (active || hovered) {
            RECT hi = { rc.left + S(4), rc.top + S(1), rc.right - S(4), rc.bottom - S(1) };
            COLORREF bgc = active ? th->accent_soft : th->surface_alt;
            HBRUSH hbr = CreateSolidBrush(bgc);
            HPEN hpen = CreatePen(PS_SOLID, 1, bgc);
            HPEN op = (HPEN)SelectObject(hdc, hpen); HBRUSH ob = (HBRUSH)SelectObject(hdc, hbr);
            RoundRect(hdc, hi.left, hi.top, hi.right, hi.bottom, S(6), S(6));
            SelectObject(hdc, op); SelectObject(hdc, ob);
            DeleteObject(hbr); DeleteObject(hpen);

            if (active) {
                RECT bar = { hi.left, hi.top + S(4), hi.left + S(3), hi.bottom - S(4) };
                HBRUSH abr = CreateSolidBrush(th->accent);
                FillRect(hdc, &bar, abr); DeleteObject(abr);
            }
        }

        SetBkMode(hdc, TRANSPARENT);
        RECT irc = { rc.left + S(12), rc.top + S(6), rc.left + S(28), rc.top + S(24) };
        const wchar_t *glyph = pass ? L"✓" : (!allow ? L"·" : L"○");
        COLORREF gcol = pass ? th->pass : (!allow ? th->border_strong : (strcmp(kind, "stage") == 0 ? th->warn : (strcmp(kind, "special") == 0 ? th->accent : th->text_muted)));
        SetTextColor(hdc, gcol);
        HFONT of = (HFONT)SelectObject(hdc, g_font_ui_bold);
        DrawTextW(hdc, glyph, -1, &irc, DT_CENTER | DT_TOP | DT_SINGLELINE);

        RECT trc = { rc.left + S(28), rc.top + S(6), rc.right - S(8), rc.bottom - S(5) };
        SetTextColor(hdc, !allow ? th->text_muted : (active ? th->accent : th->text));
        SelectObject(hdc, active ? g_font_tree_bold : g_font_tree);
        DrawTextW(hdc, text, -1, &trc, DT_LEFT | DT_WORDBREAK);
        SelectObject(hdc, of);
    }
}

static LRESULT CALLBACK TreeWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_ERASEBKGND:
            return 1;

        case WM_SIZE:
            recalc_tree_row_heights();
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT cl; GetClientRect(hwnd, &cl);
            int W = cl.right - cl.left, H = cl.bottom - cl.top;
            if (W > 0 && H > 0) {
                HDC memdc = CreateCompatibleDC(hdc);
                HBITMAP membmp = CreateCompatibleBitmap(hdc, W, H);
                HBITMAP oldbmp = (HBITMAP)SelectObject(memdc, membmp);

                FillRect(memdc, &cl, g_br_surface);
                if (g_tree_row_count == 0) {
                    RECT erc = { S(12), S(24), W - S(12), S(84) };
                    SetBkMode(memdc, TRANSPARENT);
                    SetTextColor(memdc, g_tree_filter == 1 ? T()->pass : T()->text_muted);
                    HFONT of = (HFONT)SelectObject(memdc, g_font_ui_sm);
                    DrawTextW(memdc, g_tree_filter == 1 ? L"全部题目均已通过 ✓" : L"当前筛选无匹配题目", -1, &erc, DT_CENTER | DT_WORDBREAK);
                    SelectObject(memdc, of);
                } else {
                    for (int i = 0; i < g_tree_row_count; i++) {
                        int ry0 = g_tree_rows[i].y_top - g_tree_scroll_y;
                        int ry1 = ry0 + g_tree_rows[i].height;
                        if (ry1 < 0) continue;
                        if (ry0 > H) break;
                        RECT rrc = { 0, ry0, W, ry1 };
                        draw_tree_row_at(memdc, i, &rrc, i == g_tree_hover_row);
                    }
                }

                BitBlt(hdc, 0, 0, W, H, memdc, 0, 0, SRCCOPY);
                SelectObject(memdc, oldbmp);
                DeleteObject(membmp);
                DeleteDC(memdc);
            }
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_MOUSEWHEEL: {
            int delta = (short)HIWORD(wp);
            g_tree_wheel_num += -delta * S(54);
            int px = g_tree_wheel_num / 120;
            g_tree_wheel_num %= 120;
            if (px != 0) {
                tree_scroll_to(g_tree_scroll_y + px);
                POINT pt; GetCursorPos(&pt); ScreenToClient(hwnd, &pt);
                RECT cl; GetClientRect(hwnd, &cl);
                int nh = (pt.x >= 0 && pt.x < cl.right && pt.y >= 0 && pt.y < cl.bottom) ? tree_hit_test(pt.y) : -1;
                if (nh != g_tree_hover_row) {
                    g_tree_hover_row = nh;
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            }
            return 0;
        }

        case WM_VSCROLL: {
            RECT cl; GetClientRect(hwnd, &cl);
            int clientH = max(1, cl.bottom - cl.top);
            int req = LOWORD(wp);
            if (req == SB_LINEUP) tree_scroll_to(g_tree_scroll_y - S(30));
            else if (req == SB_LINEDOWN) tree_scroll_to(g_tree_scroll_y + S(30));
            else if (req == SB_PAGEUP) tree_scroll_to(g_tree_scroll_y - max(S(60), clientH - S(30)));
            else if (req == SB_PAGEDOWN) tree_scroll_to(g_tree_scroll_y + max(S(60), clientH - S(30)));
            else if (req == SB_TOP) tree_scroll_to(0);
            else if (req == SB_BOTTOM) tree_scroll_to(g_tree_total_h);
            else if (req == SB_THUMBTRACK || req == SB_THUMBPOSITION) {
                SCROLLINFO si;
                memset(&si, 0, sizeof(si));
                si.cbSize = sizeof(si);
                si.fMask = SIF_TRACKPOS;
                GetScrollInfo(hwnd, SB_VERT, &si);
                tree_scroll_to(si.nTrackPos);
            }
            return 0;
        }

        case WM_MOUSEMOVE: {
            int y = (short)HIWORD(lp);
            int hit = tree_hit_test(y);
            if (hit != g_tree_hover_row) {
                g_tree_hover_row = hit;
                InvalidateRect(hwnd, NULL, FALSE);
            }
            TRACKMOUSEEVENT tme;
            tme.cbSize = sizeof(tme);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            tme.dwHoverTime = 0;
            TrackMouseEvent(&tme);
            return 0;
        }

        case WM_MOUSELEAVE:
            if (g_tree_hover_row != -1) {
                g_tree_hover_row = -1;
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;

        case WM_LBUTTONDOWN: {
            SetFocus(hwnd);
            int y = (short)HIWORD(lp);
            int hit = tree_hit_test(y);
            if (hit >= 0 && hit < g_tree_row_count) {
                TreeRow r = g_tree_rows[hit];
                if (r.is_mod) {
                    g_mod_collapsed[r.mod_idx] = !g_mod_collapsed[r.mod_idx];
                    rebuild_tree_rows(1);
                } else if (r.flat_idx != g_cur_flat) {
                    if (!is_quest_allowed(r.flat_idx)) {
                        SetWindowTextW(g_hwnd_status, L"闯关模式已开启：请先通过上一题，或取消勾选「闯关模式」");
                    } else {
                        load_exercise(r.flat_idx, 0);
                    }
                }
            }
            return 0;
        }

        case WM_KEYDOWN: {
            if (wp == VK_UP && g_cur_flat > 0) {
                load_exercise(g_cur_flat - 1, 1);
                return 0;
            }
            if (wp == VK_DOWN && g_cur_flat < g_flat_count - 1) {
                if (is_quest_allowed(g_cur_flat + 1)) load_exercise(g_cur_flat + 1, 1);
                return 0;
            }
            if (wp == VK_PRIOR) { SendMessageW(hwnd, WM_VSCROLL, SB_PAGEUP, 0); return 0; }
            if (wp == VK_NEXT) { SendMessageW(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0); return 0; }
            if (wp == VK_HOME) { tree_scroll_to(0); return 0; }
            if (wp == VK_END) { tree_scroll_to(g_tree_total_h); return 0; }
            break;
        }
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

/* ---------- Progress Save & Flat Table Init ---------- */
static void save_current_draft(void) {
    if (g_cur_flat < 0 || g_cur_flat >= g_flat_count || !g_hwnd_sci) return;
    char *code = get_editor_utf8();
    const char *id = g_flat[g_cur_flat].id;
    EnterCriticalSection(&g_core.lock);
    j_obj_set(j_obj_get(g_core.progress, "drafts"), id, j_str(code));
    j_obj_set(j_obj_get(g_core.progress, "unlocked"), id, j_bool(g_unlocked));
    LeaveCriticalSection(&g_core.lock);
    core_save_progress();
    free(code);
}

static void refresh_progress_and_tree(void) {
    int done = 0;
    for (int i = 0; i < g_flat_count; i++) if (is_ex_passed(g_flat[i].id)) done++;
    g_done_count = done;
    InvalidateRect(g_hwnd_main, &g_rc_header, FALSE);
    InvalidateRect(g_hwnd_tree, NULL, TRUE);
}

static void init_flat_exercises(void) {
    g_flat_count = 0;
    size_t nmods = g_core.modules ? g_core.modules->arr_len : 0;
    for (size_t m = 0; m < nmods && m < 32; m++) {
        JVal *mod = g_core.modules->items[m];
        JVal *exs = j_obj_get(mod, "exercises");
        if (exs && exs->type == J_ARR) {
            for (size_t j = 0; j < exs->arr_len && g_flat_count < 256; j++) {
                int fi = g_flat_count++;
                g_flat[fi].mod_idx = (int)m; g_flat[fi].ex_idx = (int)j;
                g_flat[fi].mod = mod; g_flat[fi].ex = exs->items[j];
                g_flat[fi].id = j_get_str(exs->items[j], "id", "");
            }
        }
    }
}

/* ---------- Problem Description & Separate Sample I/O Panel ---------- */
static void render_sample_panel(const JVal *ex) {
    const Theme *th = T();
    int ntests = j_get_int(ex, "test_count", 0);
    wchar_t hdr[64];
    _snwprintf(hdr, 64, L"示例输入与预期输出（共 %d 组测试）", ntests);
    SetWindowTextW(g_hwnd_lbl_sample, hdr);

    SendMessageW(g_hwnd_sample, WM_SETREDRAW, FALSE, 0);
    re_clear(g_hwnd_sample);

    JVal *sio = j_obj_get(ex, "sample_io");
    if (!sio || sio->type != J_OBJ) {
        re_append(g_hwnd_sample, L"无示例数据", th->text_muted, 0, 17, L"Microsoft YaHei UI");
    } else {
        const char *sin = j_get_str(sio, "stdin", "");
        const char *sout = j_get_str(sio, "expected_stdout", "");
        JVal *inf = j_obj_get(sio, "input_files");
        JVal *outf = j_obj_get(sio, "expected_files");
        int has_in_files = (inf && inf->type == J_OBJ && inf->obj_len > 0);
        int has_out_files = (outf && outf->type == J_OBJ && outf->obj_len > 0);
        int first = 1;

        if (sin[0]) {
            re_append(g_hwnd_sample, L"▸ 示例输入 (stdin)\n", th->accent, 1, 17, L"Microsoft YaHei UI");
            re_append_code_block_utf8(g_hwnd_sample, sin, th->text, th->accent_soft);
            first = 0;
        } else if (!has_in_files) {
            re_append(g_hwnd_sample, L"▸ 示例输入：", th->text_muted, 1, 17, L"Microsoft YaHei UI");
            re_append(g_hwnd_sample, L"无（无需键盘输入）\n", th->text_muted, 0, 17, L"Microsoft YaHei UI");
            first = 0;
        }

        if (has_in_files) {
            for (size_t i = 0; i < inf->obj_len; i++) {
                if (!first) re_append(g_hwnd_sample, L"\n", th->text, 0, 10, L"Microsoft YaHei UI");
                re_append(g_hwnd_sample, L"▸ 预置文件 ", th->accent, 1, 17, L"Microsoft YaHei UI");
                re_append_utf8(g_hwnd_sample, inf->pairs[i].key, th->accent, 1, 17, L"Consolas");
                re_append(g_hwnd_sample, L"\n", th->accent, 0, 17, L"Microsoft YaHei UI");
                re_append_code_block_utf8(g_hwnd_sample, inf->pairs[i].val->sval, th->text, th->accent_soft);
                first = 0;
            }
        }

        if (sout[0]) {
            if (!first) re_append(g_hwnd_sample, L"\n", th->text, 0, 10, L"Microsoft YaHei UI");
            re_append(g_hwnd_sample, L"▸ 预期输出 (stdout)\n", th->pass, 1, 17, L"Microsoft YaHei UI");
            re_append_code_block_utf8(g_hwnd_sample, sout, th->text, th->pass_soft);
            first = 0;
        }

        if (has_out_files) {
            for (size_t i = 0; i < outf->obj_len; i++) {
                if (!first) re_append(g_hwnd_sample, L"\n", th->text, 0, 10, L"Microsoft YaHei UI");
                re_append(g_hwnd_sample, L"▸ 预期生成文件 ", th->pass, 1, 17, L"Microsoft YaHei UI");
                re_append_utf8(g_hwnd_sample, outf->pairs[i].key, th->pass, 1, 17, L"Consolas");
                re_append(g_hwnd_sample, L"\n", th->pass, 0, 17, L"Microsoft YaHei UI");
                re_append_code_block_utf8(g_hwnd_sample, outf->pairs[i].val->sval, th->text, th->pass_soft);
                first = 0;
            }
        }
    }

    re_scroll_top(g_hwnd_sample);
    SendMessageW(g_hwnd_sample, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(g_hwnd_sample, NULL, TRUE);
}

static void render_problem_panel(const JVal *ex) {
    const Theme *th = T();
    SendMessageW(g_hwnd_problem, WM_SETREDRAW, FALSE, 0);
    re_clear(g_hwnd_problem);

    const char *kind = j_get_str(ex, "kind", "single");
    const wchar_t *kbadge = L"[单点补全]  ";
    COLORREF kcol = th->accent;
    if (strcmp(kind, "stage") == 0) { kbadge = L"[阶段综合]  "; kcol = th->warn; }
    else if (strcmp(kind, "special") == 0) { kbadge = L"[专项强化]  "; kcol = th->pass; }

    re_append(g_hwnd_problem, kbadge, kcol, 1, 20, L"Microsoft YaHei UI");
    re_append_utf8(g_hwnd_problem, j_get_str(ex, "code", ""), th->text, 1, 21, L"Microsoft YaHei UI");
    re_append(g_hwnd_problem, L"  ", th->text, 1, 21, L"Microsoft YaHei UI");
    re_append_utf8(g_hwnd_problem, j_get_str(ex, "title", ""), th->text, 1, 21, L"Microsoft YaHei UI");
    re_append(g_hwnd_problem, L"\n", th->text, 0, 18, L"Microsoft YaHei UI");

    re_append_utf8(g_hwnd_problem, j_get_str(ex, "summary", ""), th->text_muted, 0, 17, L"Microsoft YaHei UI");
    re_append(g_hwnd_problem, L"\n\n任务要求：\n", th->text, 1, 18, L"Microsoft YaHei UI");
    re_append_utf8(g_hwnd_problem, j_get_str(ex, "goal", ""), th->text, 0, 18, L"Microsoft YaHei UI");

    re_scroll_top(g_hwnd_problem);
    SendMessageW(g_hwnd_problem, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(g_hwnd_problem, NULL, TRUE);

    render_sample_panel(ex);
}

static void re_append_tag_utf8(HWND hwnd, const char *utf8, COLORREF fg, COLORREF bg, int bold, int half_pt, const wchar_t *face) {
    if (!utf8 || !utf8[0]) return;
    wchar_t *w = utf8_to_wide_alloc(utf8);
    if (!w) return;
    MY_CHARRANGE cr = { -1, -1 };
    SendMessageW(hwnd, EM_EXSETSEL, 0, (LPARAM)&cr);
    MY_CHARFORMAT2W cf;
    memset(&cf, 0, sizeof(cf));
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_COLOR | CFM_BACKCOLOR | CFM_BOLD | CFM_SIZE | CFM_FACE;
    cf.crTextColor = fg;
    cf.crBackColor = bg;
    cf.dwEffects = bold ? CFE_BOLD : 0;
    cf.yHeight = FP(half_pt);
    wcsncpy(cf.szFaceName, face ? face : L"Consolas", 31);
    SendMessageW(hwnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
    SendMessageW(hwnd, EM_REPLACESEL, FALSE, (LPARAM)w);
    free(w);
}

static void render_reference_panel(const JVal *ex) {
    const Theme *th = T();
    SendMessageW(g_hwnd_ref, WM_SETREDRAW, FALSE, 0);
    re_clear(g_hwnd_ref);

    const char *kind = j_get_str(ex, "kind", "single");
    if (strcmp(kind, "single") == 0) {
        SetWindowTextW(g_hwnd_lbl_ref, L"函数参数与语法说明");
        JVal *docs = j_obj_get(ex, "param_docs");
        if (docs && docs->type == J_ARR) {
            for (size_t i = 0; i < docs->arr_len; i++) {
                JVal *d = docs->items[i];
                if (i > 0) re_append(g_hwnd_ref, L"\n─────────────────────\n\n", th->border_strong, 0, 14, L"Microsoft YaHei UI");
                re_append(g_hwnd_ref, L" ", th->accent, 1, 20, L"Consolas");
                re_append_tag_utf8(g_hwnd_ref, j_get_str(d, "name", ""), th->accent, th->accent_soft, 1, 20, L"Consolas");
                re_append(g_hwnd_ref, L"  ", th->text, 0, 17, L"Consolas");
                re_append_utf8(g_hwnd_ref, j_get_str(d, "header", ""), th->syn_pre, 0, 17, L"Consolas");
                re_append(g_hwnd_ref, L"\n\n原型：\n", th->text_muted, 1, 17, L"Microsoft YaHei UI");
                re_append_code_block_utf8(g_hwnd_ref, j_get_str(d, "prototype", ""), th->text, th->locked_bg);
                re_append(g_hwnd_ref, L"\n作用：\n", th->text_muted, 1, 17, L"Microsoft YaHei UI");
                re_append_utf8(g_hwnd_ref, j_get_str(d, "purpose", ""), th->text, 0, 18, L"Microsoft YaHei UI");

                JVal *params = j_obj_get(d, "params");
                if (params && params->type == J_ARR && params->arr_len > 0) {
                    re_append(g_hwnd_ref, L"\n\n参数说明：\n", th->text_muted, 1, 17, L"Microsoft YaHei UI");
                    for (size_t k = 0; k < params->arr_len; k++) {
                        JVal *pm = params->items[k];
                        re_append(g_hwnd_ref, L"• ", th->accent, 1, 18, L"Microsoft YaHei UI");
                        re_append_utf8(g_hwnd_ref, j_get_str(pm, "name", ""), th->text, 1, 18, L"Consolas");
                        re_append(g_hwnd_ref, L"  [", th->text_muted, 0, 17, L"Microsoft YaHei UI");
                        re_append_utf8(g_hwnd_ref, j_get_str(pm, "type", ""), th->pass, 0, 17, L"Consolas");
                        re_append(g_hwnd_ref, L"]\n  ", th->text_muted, 0, 17, L"Microsoft YaHei UI");
                        re_append_utf8(g_hwnd_ref, j_get_str(pm, "desc", ""), th->text, 0, 18, L"Microsoft YaHei UI");
                        re_append(g_hwnd_ref, L"\n\n", th->text, 0, 14, L"Microsoft YaHei UI");
                    }
                }
                const char *ret = j_get_str(d, "returns", "");
                if (ret[0]) {
                    re_append(g_hwnd_ref, L"返回值：\n", th->text_muted, 1, 17, L"Microsoft YaHei UI");
                    re_append_utf8(g_hwnd_ref, ret, th->text, 0, 18, L"Microsoft YaHei UI");
                    re_append(g_hwnd_ref, L"\n", th->text, 0, 17, L"Microsoft YaHei UI");
                }
                JVal *notes = j_obj_get(d, "notes");
                if (notes && notes->type == J_ARR && notes->arr_len > 0) {
                    re_append(g_hwnd_ref, L"\n注意点：\n", th->warn, 1, 17, L"Microsoft YaHei UI");
                    for (size_t k = 0; k < notes->arr_len; k++) {
                        re_append(g_hwnd_ref, L"  - ", th->text_muted, 0, 17, L"Microsoft YaHei UI");
                        re_append_utf8(g_hwnd_ref, notes->items[k]->sval, th->text_muted, 0, 17, L"Microsoft YaHei UI");
                        re_append(g_hwnd_ref, L"\n", th->text_muted, 0, 17, L"Microsoft YaHei UI");
                    }
                }
            }
        }
    } else {
        SetWindowTextW(g_hwnd_lbl_ref, strcmp(kind, "special") == 0 ? L"专项指导与速查" : L"阶段指导与速查");
        JVal *guide = j_obj_get(ex, "guidance");
        if (guide && guide->type == J_ARR && guide->arr_len > 0) {
            re_append(g_hwnd_ref, L"■ 实现步骤指导\n", th->accent, 1, 18, L"Microsoft YaHei UI");
            for (size_t i = 0; i < guide->arr_len; i++) {
                re_append_utf8(g_hwnd_ref, guide->items[i]->sval, th->text, 0, 18, L"Microsoft YaHei UI");
                re_append(g_hwnd_ref, L"\n\n", th->text, 0, 14, L"Microsoft YaHei UI");
            }
        }
        JVal *sref = j_obj_get(ex, "sidebar_refs");
        JVal *fns = sref ? j_obj_get(sref, "functions") : NULL;
        if (fns && fns->type == J_ARR && fns->arr_len > 0) {
            re_append(g_hwnd_ref, L"■ 可能用到的函数\n", th->syn_type, 1, 18, L"Microsoft YaHei UI");
            for (size_t i = 0; i < fns->arr_len; i++) {
                JVal *f = fns->items[i];
                re_append_tag_utf8(g_hwnd_ref, j_get_str(f, "name", ""), th->accent, th->accent_soft, 1, 18, L"Consolas");
                re_append(g_hwnd_ref, L"\n", th->text, 0, 14, L"Consolas");
                re_append_code_block_utf8(g_hwnd_ref, j_get_str(f, "sig", ""), th->text, th->locked_bg);
                re_append_utf8(g_hwnd_ref, j_get_str(f, "desc", ""), th->text_muted, 0, 17, L"Microsoft YaHei UI");
                re_append(g_hwnd_ref, L"\n\n", th->text_muted, 0, 14, L"Microsoft YaHei UI");
            }
        }
        JVal *sts = sref ? j_obj_get(sref, "structures") : NULL;
        if (sts && sts->type == J_ARR && sts->arr_len > 0) {
            re_append(g_hwnd_ref, L"■ 常用固定结构参考\n", th->pass, 1, 18, L"Microsoft YaHei UI");
            for (size_t i = 0; i < sts->arr_len; i++) {
                JVal *st = sts->items[i];
                re_append_utf8(g_hwnd_ref, j_get_str(st, "name", ""), th->text, 1, 17, L"Microsoft YaHei UI");
                re_append(g_hwnd_ref, L"\n", th->text, 0, 14, L"Consolas");
                re_append_code_block_utf8(g_hwnd_ref, j_get_str(st, "code", ""), th->text, th->locked_bg);
                re_append(g_hwnd_ref, L"\n", th->text, 0, 12, L"Microsoft YaHei UI");
            }
        }
    }
    re_scroll_top(g_hwnd_ref);
    SendMessageW(g_hwnd_ref, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(g_hwnd_ref, NULL, TRUE);
}

static void render_console_report(const char *report_utf8, int compiled, int passed) {
    if (g_last_console_utf8 != report_utf8) {
        free(g_last_console_utf8);
        g_last_console_utf8 = report_utf8 ? xstrdup(report_utf8) : NULL;
    }
    g_last_console_compiled = compiled;
    g_last_console_passed = passed;

    const Theme *th = T();
    SendMessageW(g_hwnd_console, WM_SETREDRAW, FALSE, 0);
    re_clear(g_hwnd_console);

    if (!report_utf8 || !report_utf8[0]) {
        SendMessageW(g_hwnd_console, WM_SETREDRAW, TRUE, 0);
        return;
    }

    int block_mode = 0; /* 0=normal, 1=expected, 2=actual */
    const char *p = report_utf8;
    while (*p) {
        const char *nl = strchr(p, '\n');
        size_t len = nl ? (size_t)(nl - p) : strlen(p);
        char *line = (char *)malloc(len + 2);
        if (!line) break;
        memcpy(line, p, len);
        line[len] = '\0';

        if (strncmp(line, "    [期望输出]", 16) == 0) {
            block_mode = 1;
            re_append(g_hwnd_console, L"    ▸ 期望输出\n", th->pass, 1, 17, L"Microsoft YaHei UI");
        } else if (strncmp(line, "    [实际输出]", 16) == 0) {
            block_mode = 2;
            re_append(g_hwnd_console, L"    ▸ 实际输出\n", th->fail, 1, 17, L"Microsoft YaHei UI");
        } else if (block_mode != 0 && strncmp(line, "      ", 6) == 0) {
            re_append(g_hwnd_console, L"      ", th->text, 0, 18, L"Consolas");
            re_append_code_block_utf8(g_hwnd_console, line + 6, th->text, block_mode == 1 ? th->pass_soft : th->fail_soft);
        } else {
            block_mode = 0;
            COLORREF col = th->text;
            int bold = 0;
            if (strncmp(line, "[全部通过]", 14) == 0) { col = th->pass; bold = 1; }
            else if (strncmp(line, "[未全部通过]", 17) == 0 || strncmp(line, "[编译失败]", 14) == 0 ||
                     strncmp(line, "[运行超时]", 14) == 0 || strncmp(line, "[错误]", 8) == 0) { col = th->fail; bold = 1; }
            else if (strncmp(line, "[试运行完成]", 17) == 0) { col = th->accent; bold = 1; }
            else if (strncmp(line, "  [通过]", 10) == 0) { col = th->pass; bold = 1; }
            else if (strncmp(line, "  [未过]", 10) == 0) { col = th->fail; bold = 1; }
            else if (strncmp(line, "    [差异定位]", 18) == 0) { col = th->warn; bold = 1; }
            else if (strncmp(line, "---- ", 5) == 0) { col = th->accent; bold = 1; }
            else if (strncmp(line, "  ! ", 4) == 0) { col = th->warn; bold = 1; }
            else if (!compiled) { col = th->fail; }
            else if (passed == 1) { col = th->pass; }

            re_append_utf8(g_hwnd_console, line, col, bold, 18, L"Consolas");
            re_append(g_hwnd_console, L"\n", col, 0, 18, L"Consolas");
        }

        free(line);
        if (!nl) break;
        p = nl + 1;
    }

    re_scroll_top(g_hwnd_console);
    SendMessageW(g_hwnd_console, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(g_hwnd_console, NULL, TRUE);
}

static void update_editor_toolbar_ui(void) {
    if (!g_has_partial) {
        SetWindowTextW(g_hwnd_mode_lbl, L"完整程序 · 全文件可编辑");
        ShowWindow(g_hwnd_btn_unlock, SW_HIDE);
    } else {
        ShowWindow(g_hwnd_btn_unlock, SW_SHOW);
        if (g_unlocked) {
            SetWindowTextW(g_hwnd_mode_lbl, L"已解锁 · 全文件可编辑");
            SetWindowTextW(g_hwnd_btn_unlock, L"恢复局部编辑");
        } else {
            SetWindowTextW(g_hwnd_mode_lbl, L"默认 · 仅编辑空白区");
            SetWindowTextW(g_hwnd_btn_unlock, L"解锁全部编辑");
        }
    }
    EnableWindow(g_hwnd_btn_solution, (g_cur_flat >= 0 && g_cur_flat < g_flat_count) ? TRUE : FALSE);
    EnableWindow(g_hwnd_btn_prev, g_cur_flat > 0);
    EnableWindow(g_hwnd_btn_next, g_cur_flat < g_flat_count - 1);

    int has_sample_stdin = 0;
    if (g_cur_flat >= 0 && g_cur_flat < g_flat_count) {
        JVal *sio = j_obj_get(g_flat[g_cur_flat].ex, "sample_io");
        const char *sin = sio ? j_get_str(sio, "stdin", "") : "";
        has_sample_stdin = (sin && sin[0] != '\0');
    }
    if (g_hwnd_btn_fill_stdin) {
        EnableWindow(g_hwnd_btn_fill_stdin, has_sample_stdin ? TRUE : FALSE);
        InvalidateRect(g_hwnd_btn_fill_stdin, NULL, FALSE);
    }
    InvalidateRect(g_hwnd_btn_unlock, NULL, FALSE);
    InvalidateRect(g_hwnd_btn_solution, NULL, FALSE);
    InvalidateRect(g_hwnd_btn_prev, NULL, FALSE);
    InvalidateRect(g_hwnd_btn_next, NULL, FALSE);
}

static void rebuild_scaled_fonts(void) {
    int pct = font_pct();
    if (g_font_tree) DeleteObject(g_font_tree);
    if (g_font_tree_bold) DeleteObject(g_font_tree_bold);
    if (g_font_code) DeleteObject(g_font_code);
    g_font_tree = CreateFontW(-S((12 * pct + 50) / 100), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Microsoft YaHei UI");
    g_font_tree_bold = CreateFontW(-S((13 * pct + 50) / 100), 0, 0, 0, FW_SEMIBOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Microsoft YaHei UI");
    g_font_code = CreateFontW(-S((14 * pct + 50) / 100), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, FIXED_PITCH, L"Consolas");
    if (g_hwnd_stdin) SendMessageW(g_hwnd_stdin, WM_SETFONT, (WPARAM)g_font_code, TRUE);
}

static void apply_theme_visuals(void) {
    const Theme *th = T();
    if (g_br_bg) DeleteObject(g_br_bg);
    if (g_br_surface) DeleteObject(g_br_surface);
    if (g_br_surface_alt) DeleteObject(g_br_surface_alt);
    if (g_br_header) DeleteObject(g_br_header);
    g_br_bg = CreateSolidBrush(th->bg);
    g_br_surface = CreateSolidBrush(th->surface);
    g_br_surface_alt = CreateSolidBrush(th->surface_alt);
    g_br_header = CreateSolidBrush(th->header_bg);

    rebuild_scaled_fonts();

    SendMessageW(g_hwnd_problem, EM_SETBKGNDCOLOR, 0, (LPARAM)th->surface);
    SendMessageW(g_hwnd_sample, EM_SETBKGNDCOLOR, 0, (LPARAM)th->surface_alt);
    SendMessageW(g_hwnd_ref, EM_SETBKGNDCOLOR, 0, (LPARAM)th->surface);
    SendMessageW(g_hwnd_console, EM_SETBKGNDCOLOR, 0, (LPARAM)th->surface_alt);

    setup_scintilla_styles();
    refresh_locked_line_shading();

    SetWindowTextW(g_hwnd_btn_theme, g_dark_mode ? L"浅色外观" : L"深色外观");
    if (g_hwnd_btn_font) SetWindowTextW(g_hwnd_btn_font, font_level_label());
    if (g_cur_flat >= 0 && g_cur_flat < g_flat_count) {
        render_problem_panel(g_flat[g_cur_flat].ex);
        render_reference_panel(g_flat[g_cur_flat].ex);
        if (g_last_console_utf8) {
            char *snap = xstrdup(g_last_console_utf8);
            render_console_report(snap, g_last_console_compiled, g_last_console_passed);
            free(snap);
        } else {
            re_clear(g_hwnd_console);
            int ntests = j_get_int(g_flat[g_cur_flat].ex, "test_count", 0);
            wchar_t msg[192];
            _snwprintf(msg, 192, L"本题共 %d 个测试点 · F5 试运行 / Ctrl+Enter 提交检测 · Ctrl+/ 注释 · Ctrl+滚轮 缩放字号", ntests);
            re_append(g_hwnd_console, msg, th->text_muted, 0, 17, L"Consolas");
        }
    }
    recalc_tree_row_heights();
    InvalidateRect(g_hwnd_main, NULL, TRUE);
    RedrawWindow(g_hwnd_main, NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_ERASE);
    if (g_hwnd_sol && IsWindow(g_hwnd_sol)) {
        InvalidateRect(g_hwnd_sol, NULL, TRUE);
        RedrawWindow(g_hwnd_sol, NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_ERASE);
    }
}

static void load_exercise(int flat_idx, int sync_tree) {
    if (flat_idx < 0 || flat_idx >= g_flat_count) return;
    if (g_cur_flat >= 0 && g_cur_flat < g_flat_count && g_cur_flat != flat_idx) {
        KillTimer(g_hwnd_main, IDT_AUTOSAVE);
        save_current_draft();
    }
    ac_popup_hide();
    if (sci(SCI_CALLTIPACTIVE, 0, 0)) sci(SCI_CALLTIPCANCEL, 0, 0);
    clear_error_markers();
    g_cur_flat = flat_idx;

    int m = g_flat[flat_idx].mod_idx;
    if (g_mod_collapsed[m]) {
        g_mod_collapsed[m] = 0;
        rebuild_tree_rows(1);
    }

    JVal *ex = g_flat[flat_idx].ex;
    const char *id = g_flat[flat_idx].id;
    const char *tpl = j_get_str(ex, "template", "");
    JVal *draft_v = j_obj_get(j_obj_get(g_core.progress, "drafts"), id);
    const char *saved = (draft_v && draft_v->type == J_STR) ? draft_v->sval : NULL;
    int pref_un = j_get_bool(j_obj_get(g_core.progress, "unlocked"), id, 0);

    render_problem_panel(ex);
    render_reference_panel(ex);
    set_editor_content(tpl, saved, pref_un);
    update_editor_toolbar_ui();

    RECT rc_now;
    GetClientRect(g_hwnd_main, &rc_now);
    if (rc_now.right > 0 && rc_now.bottom > 0) layout_controls(rc_now.right, rc_now.bottom);

    free(g_last_console_utf8);
    g_last_console_utf8 = NULL;
    re_clear(g_hwnd_console);
    int ntests = j_get_int(ex, "test_count", 0);
    wchar_t msg[192];
    _snwprintf(msg, 192, L"本题共 %d 个测试点 · F5 试运行 / Ctrl+Enter 提交检测 · Ctrl+/ 注释 · Ctrl+滚轮 缩放字号", ntests);
    re_append(g_hwnd_console, msg, T()->text_muted, 0, 17, L"Consolas");
    SetWindowTextW(g_hwnd_stdin, L"");

    if (sync_tree) sync_tree_selection();
    InvalidateRect(g_hwnd_tree, NULL, FALSE);
}

static void copy_utf8_to_clipboard(HWND owner, const char *utf8) {
    if (!utf8) return;
    wchar_t *w = utf8_to_wide_alloc(utf8);
    if (!w) return;
    size_t bytes = (wcslen(w) + 1) * sizeof(wchar_t);
    HGLOBAL hmem = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (hmem) {
        void *dst = GlobalLock(hmem);
        if (dst) {
            memcpy(dst, w, bytes);
            GlobalUnlock(hmem);
            if (OpenClipboard(owner)) {
                EmptyClipboard();
                SetClipboardData(CF_UNICODETEXT, hmem);
                CloseClipboard();
                hmem = NULL;
            }
        }
        if (hmem) GlobalFree(hmem);
    }
    free(w);
}

/* ---------- Async Compile / Grade Worker ---------- */
static DWORD WINAPI grade_worker_thread(LPVOID param) {
    GradeJob *job = (GradeJob *)param;
    JVal *ex = g_flat[job->flat_idx].ex;
    job->report = core_grade_or_run(ex, job->code_utf8, job->run_only, job->stdin_utf8);
    PostMessageW(g_hwnd_main, WM_APP_GRADE_DONE, 0, (LPARAM)job);
    return 0;
}

static void trigger_grade_or_run(int run_only) {
    if (g_busy || g_cur_flat < 0 || g_cur_flat >= g_flat_count) return;
    ac_popup_hide();
    if (sci(SCI_CALLTIPACTIVE, 0, 0)) sci(SCI_CALLTIPCANCEL, 0, 0);
    KillTimer(g_hwnd_main, IDT_AUTOSAVE);
    clear_error_markers();
    save_current_draft();

    g_busy = 1;
    EnableWindow(g_hwnd_btn_run, FALSE);
    EnableWindow(g_hwnd_btn_submit, FALSE);
    InvalidateRect(g_hwnd_btn_run, NULL, FALSE);
    InvalidateRect(g_hwnd_btn_submit, NULL, FALSE);

    re_clear(g_hwnd_console);
    re_append(g_hwnd_console, run_only ? L"正在使用内置 TCC C99 编译并试运行...\n" : L"正在使用内置 TCC C99 编译并评测全部测试点...\n", T()->accent, 1, 18, L"Consolas");

    GradeJob *job = (GradeJob *)calloc(1, sizeof(GradeJob));
    job->flat_idx = g_cur_flat;
    job->run_only = run_only;
    job->code_utf8 = get_editor_utf8();
    wchar_t *wsin = get_window_text_alloc(g_hwnd_stdin);
    job->stdin_utf8 = wide_to_utf8_alloc(wsin);
    free(wsin);

    HANDLE th = CreateThread(NULL, 0, grade_worker_thread, job, 0, NULL);
    if (th) CloseHandle(th);
}

static HWND make_modern_btn(HWND parent, const wchar_t *text, int id);

static LRESULT CALLBACK SolutionWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE: {
            HINSTANCE hi = GetModuleHandleW(NULL);
            g_hwnd_sol_sci = CreateWindowExW(
                0, L"Scintilla", L"",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                0, 0, 0, 0, hwnd, NULL, hi, NULL
            );
            apply_scintilla_styles_to(g_hwnd_sol_sci);
            g_hwnd_sol_copy = make_modern_btn(hwnd, L"复制参考代码", ID_SOL_COPY);
            g_hwnd_sol_close = make_modern_btn(hwnd, L"关闭 (Esc)", ID_SOL_CLOSE);
            return 0;
        }
        case WM_SIZE: {
            int W = LOWORD(lp), H = HIWORD(lp);
            int pad = S(10), barH = S(44);
            if (g_hwnd_sol_sci) {
                MoveWindow(g_hwnd_sol_sci, pad, pad, max(S(100), W - pad * 2), max(S(100), H - pad - barH), TRUE);
            }
            if (g_hwnd_sol_close) {
                MoveWindow(g_hwnd_sol_close, W - pad - S(96), H - barH + S(7), S(96), S(30), TRUE);
            }
            if (g_hwnd_sol_copy) {
                MoveWindow(g_hwnd_sol_copy, W - pad - S(96) - S(8) - S(108), H - barH + S(7), S(108), S(30), TRUE);
            }
            return 0;
        }
        case WM_DRAWITEM: {
            DRAWITEMSTRUCT *dis = (DRAWITEMSTRUCT *)lp;
            if (dis->CtlType == ODT_BUTTON) { draw_modern_button(dis); return TRUE; }
            return FALSE;
        }
        case WM_ERASEBKGND: {
            RECT rc; GetClientRect(hwnd, &rc);
            FillRect((HDC)wp, &rc, g_br_bg);
            return 1;
        }
        case WM_KEYDOWN:
            if (wp == VK_ESCAPE) { DestroyWindow(hwnd); return 0; }
            break;
        case WM_COMMAND:
            if (LOWORD(wp) == ID_SOL_CLOSE) { DestroyWindow(hwnd); return 0; }
            if (LOWORD(wp) == ID_SOL_COPY && g_hwnd_sol_sci) {
                int len = (int)SendMessageW(g_hwnd_sol_sci, SCI_GETTEXTLENGTH, 0, 0);
                char *buf = (char *)malloc((size_t)len + 2);
                if (buf) {
                    SendMessageW(g_hwnd_sol_sci, SCI_GETTEXT, (WPARAM)(len + 1), (LPARAM)buf);
                    buf[len] = '\0';
                    copy_utf8_to_clipboard(hwnd, buf);
                    free(buf);
                    SetWindowTextW(g_hwnd_sol_copy, L"已复制到剪贴板 ✓");
                    InvalidateRect(g_hwnd_sol_copy, NULL, FALSE);
                }
                return 0;
            }
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            g_hwnd_sol = NULL;
            g_hwnd_sol_sci = NULL;
            g_hwnd_sol_copy = NULL;
            g_hwnd_sol_close = NULL;
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static void show_solution_window(void) {
    if (g_cur_flat < 0 || g_cur_flat >= g_flat_count) return;

    static int sol_cls_reg = 0;
    if (!sol_cls_reg) {
        HINSTANCE hi = GetModuleHandleW(NULL);
        WNDCLASSW sc;
        memset(&sc, 0, sizeof(sc));
        sc.lpfnWndProc = SolutionWndProc;
        sc.hInstance = hi;
        sc.hIcon = LoadIconW(hi, MAKEINTRESOURCEW(1));
        sc.hCursor = LoadCursorW(NULL, (LPCWSTR)IDC_ARROW);
        sc.hbrBackground = NULL;
        sc.lpszClassName = L"CReviewSolutionWnd";
        RegisterClassW(&sc);
        sol_cls_reg = 1;
    }

    JVal *ex = g_flat[g_cur_flat].ex;
    wchar_t *wcode = utf8_to_wide_alloc(j_get_str(ex, "code", ""));
    wchar_t *wtitle = utf8_to_wide_alloc(j_get_str(ex, "title", ""));
    wchar_t cap[256];
    _snwprintf(cap, 256, L"参考实现 · %ls %ls", wcode, wtitle);
    free(wcode); free(wtitle);

    if (!g_hwnd_sol || !IsWindow(g_hwnd_sol)) {
        RECT mr; GetWindowRect(g_hwnd_main, &mr);
        int w = S(760), h = S(540);
        int x = mr.left + max(20, ((mr.right - mr.left) - w) / 2);
        int y = mr.top + max(20, ((mr.bottom - mr.top) - h) / 2);
        g_hwnd_sol = CreateWindowExW(
            0, L"CReviewSolutionWnd", cap,
            WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN,
            x, y, w, h,
            g_hwnd_main, NULL, GetModuleHandleW(NULL), NULL
        );
    } else {
        SetWindowTextW(g_hwnd_sol, cap);
        ShowWindow(g_hwnd_sol, SW_RESTORE);
        SetForegroundWindow(g_hwnd_sol);
    }

    if (g_hwnd_sol_copy) {
        SetWindowTextW(g_hwnd_sol_copy, L"复制参考代码");
        InvalidateRect(g_hwnd_sol_copy, NULL, FALSE);
    }
    if (g_hwnd_sol_sci) {
        const char *sol = j_get_str(ex, "solution", "");
        SendMessageW(g_hwnd_sol_sci, SCI_SETREADONLY, 0, 0);
        SendMessageW(g_hwnd_sol_sci, SCI_SETTEXT, 0, (LPARAM)sol);
        SendMessageW(g_hwnd_sol_sci, SCI_SETREADONLY, 1, 0);
        SendMessageW(g_hwnd_sol_sci, SCI_GOTOPOS, 0, 0);
    }
}

static void save_layout_settings(HWND hwnd) {
    JVal *gs = j_obj_get(g_core.progress, "settings");
    if (!gs) return;
    j_obj_set(gs, "nav_hidden", j_bool(g_nav_hidden));
    j_obj_set(gs, "ref_hidden", j_bool(g_ref_hidden));
    j_obj_set(gs, "tree_filter", j_num((double)g_tree_filter));
    j_obj_set(gs, "editor_zoom", j_num((double)g_editor_zoom));
    j_obj_set(gs, "font_level", j_num((double)g_font_level));
    if (g_nav_w > 0) j_obj_set(gs, "nav_w", j_num((double)g_nav_w));
    if (g_ref_w > 0) j_obj_set(gs, "ref_w", j_num((double)g_ref_w));
    if (g_prob_h > 0) j_obj_set(gs, "prob_h", j_num((double)g_prob_h));
    if (g_cons_h > 0) j_obj_set(gs, "cons_h", j_num((double)g_cons_h));
    if (hwnd && IsWindow(hwnd)) {
        int maxed = IsZoomed(hwnd) ? 1 : 0;
        j_obj_set(gs, "win_max", j_bool(maxed));
        if (!maxed && !IsIconic(hwnd)) {
            RECT wr; GetWindowRect(hwnd, &wr);
            j_obj_set(gs, "win_w", j_num((double)(wr.right - wr.left)));
            j_obj_set(gs, "win_h", j_num((double)(wr.bottom - wr.top)));
        }
    }
    core_save_progress();
}

/* ---------- Layout & Draggable Splitters ---------- */
static void layout_controls(int W, int H) {
    if (W < S(600) || H < S(400)) return;
    const int topH = S(50);
    const int pad = S(12);
    const int gap = S(10);

    if (g_nav_w == 0) g_nav_w = clamp_i((W * 21) / 100, S(240), S(300));
    if (g_ref_w == 0) g_ref_w = clamp_i((W * 24) / 100, S(270), S(360));

    int minCenter = S(380);
    int navW = 0, refW = 0, centerX = pad, centerW = W - pad * 2, refX = W;
    const int bodyY = topH + pad;
    const int bodyH = H - bodyY - pad;

    memset(&g_rc_split_left, 0, sizeof(g_rc_split_left));
    memset(&g_rc_split_right, 0, sizeof(g_rc_split_right));

    if (!g_nav_hidden && !g_ref_hidden) {
        int maxSides = W - pad * 2 - gap * 2 - minCenter;
        navW = clamp_i(g_nav_w, S(190), min(S(480), max(S(190), maxSides - S(210))));
        refW = clamp_i(g_ref_w, S(210), min(S(520), max(S(210), maxSides - navW)));
        centerX = pad + navW + gap;
        centerW = max(minCenter, W - navW - refW - pad * 2 - gap * 2);
        refX = centerX + centerW + gap;

        g_rc_split_left.left = pad + navW; g_rc_split_left.top = bodyY;
        g_rc_split_left.right = centerX; g_rc_split_left.bottom = bodyY + bodyH;
        g_rc_split_right.left = centerX + centerW; g_rc_split_right.top = bodyY;
        g_rc_split_right.right = refX; g_rc_split_right.bottom = bodyY + bodyH;
    } else if (!g_nav_hidden && g_ref_hidden) {
        int maxNav = min(S(480), W - pad * 2 - gap - minCenter);
        navW = clamp_i(g_nav_w, S(190), max(S(190), maxNav));
        centerX = pad + navW + gap;
        centerW = max(minCenter, W - navW - pad * 2 - gap);

        g_rc_split_left.left = pad + navW; g_rc_split_left.top = bodyY;
        g_rc_split_left.right = centerX; g_rc_split_left.bottom = bodyY + bodyH;
    } else if (g_nav_hidden && !g_ref_hidden) {
        int maxRef = min(S(520), W - pad * 2 - gap - minCenter);
        refW = clamp_i(g_ref_w, S(210), max(S(210), maxRef));
        centerX = pad;
        centerW = max(minCenter, W - refW - pad * 2 - gap);
        refX = centerX + centerW + gap;

        g_rc_split_right.left = centerX + centerW; g_rc_split_right.top = bodyY;
        g_rc_split_right.right = refX; g_rc_split_right.bottom = bodyY + bodyH;
    } else {
        centerX = pad;
        centerW = max(minCenter, W - pad * 2);
    }

    g_rc_header.left = 0; g_rc_header.top = 0; g_rc_header.right = W; g_rc_header.bottom = topH;

    int rx = W - pad;
    int bResetW = S(70), bThemeW = S(70), bFontW = S(74), bRefW = S(70), bDirW = S(70), bNavW = S(54), chkW = S(74), btnH = S(30);
    int topY = (topH - btnH) / 2;

    rx -= bResetW; MoveWindow(g_hwnd_btn_reset_all, rx, topY, bResetW, btnH, TRUE);
    rx -= (bThemeW + S(6)); MoveWindow(g_hwnd_btn_theme, rx, topY, bThemeW, btnH, TRUE);
    rx -= (bFontW + S(6)); MoveWindow(g_hwnd_btn_font, rx, topY, bFontW, btnH, TRUE);
    rx -= (bRefW + S(6)); MoveWindow(g_hwnd_btn_toggle_ref, rx, topY, bRefW, btnH, TRUE);
    rx -= (bDirW + S(6)); MoveWindow(g_hwnd_btn_toggle_nav, rx, topY, bDirW, btnH, TRUE);
    rx -= (bNavW + S(6)); MoveWindow(g_hwnd_btn_next, rx, topY, bNavW, btnH, TRUE);
    rx -= (bNavW + S(6)); MoveWindow(g_hwnd_btn_prev, rx, topY, bNavW, btnH, TRUE);
    rx -= (chkW + S(8)); MoveWindow(g_hwnd_chk_quest, rx, topY + S(3), chkW, S(24), TRUE);

    int pLeft = S(290);
    int pRight = min(pLeft + S(105), rx - S(132));
    if (pRight > pLeft + S(28)) {
        g_rc_progbar.left = pLeft; g_rc_progbar.top = S(21);
        g_rc_progbar.right = pRight; g_rc_progbar.bottom = S(29);
    } else {
        g_rc_progbar.left = g_rc_progbar.right = 0;
    }

    /* Left Tree Card */
    if (g_nav_hidden) {
        memset(&g_rc_card_nav, 0, sizeof(g_rc_card_nav));
        ShowWindow(g_hwnd_lbl_nav, SW_HIDE);
        ShowWindow(g_hwnd_flt_all, SW_HIDE);
        ShowWindow(g_hwnd_flt_todo, SW_HIDE);
        ShowWindow(g_hwnd_flt_hard, SW_HIDE);
        ShowWindow(g_hwnd_tree, SW_HIDE);
        ShowWindow(g_hwnd_version, SW_HIDE);
    } else {
        g_rc_card_nav.left = pad; g_rc_card_nav.top = bodyY; g_rc_card_nav.right = pad + navW; g_rc_card_nav.bottom = bodyY + bodyH;
        ShowWindow(g_hwnd_lbl_nav, SW_SHOW);
        ShowWindow(g_hwnd_flt_all, SW_SHOW);
        ShowWindow(g_hwnd_flt_todo, SW_SHOW);
        ShowWindow(g_hwnd_flt_hard, SW_SHOW);
        ShowWindow(g_hwnd_tree, SW_SHOW);
        ShowWindow(g_hwnd_version, SW_SHOW);
        MoveWindow(g_hwnd_lbl_nav, pad + S(12), bodyY + S(9), navW - S(24), S(20), TRUE);

        int fltY = bodyY + S(40);
        int fltH = S(24);
        int fltGap = S(4);
        int fltInnerW = navW - S(16);
        int w1 = (fltInnerW - fltGap * 2) * 28 / 100;
        int w2 = (fltInnerW - fltGap * 2) * 32 / 100;
        int w3 = fltInnerW - fltGap * 2 - w1 - w2;
        MoveWindow(g_hwnd_flt_all, pad + S(8), fltY, w1, fltH, TRUE);
        MoveWindow(g_hwnd_flt_todo, pad + S(8) + w1 + fltGap, fltY, w2, fltH, TRUE);
        MoveWindow(g_hwnd_flt_hard, pad + S(8) + w1 + w2 + fltGap * 2, fltY, w3, fltH, TRUE);

        MoveWindow(g_hwnd_tree, pad + S(4), fltY + fltH + S(6), navW - S(8), max(S(60), bodyH - (fltY - bodyY + fltH + S(36))), TRUE);
        MoveWindow(g_hwnd_version, pad + S(12), bodyY + bodyH - S(24), navW - S(24), S(18), TRUE);
        recalc_tree_row_heights();
    }

    /* Right Reference Card */
    if (g_ref_hidden) {
        memset(&g_rc_card_ref, 0, sizeof(g_rc_card_ref));
        ShowWindow(g_hwnd_lbl_ref, SW_HIDE);
        ShowWindow(g_hwnd_ref, SW_HIDE);
    } else {
        g_rc_card_ref.left = refX; g_rc_card_ref.top = bodyY; g_rc_card_ref.right = refX + refW; g_rc_card_ref.bottom = bodyY + bodyH;
        ShowWindow(g_hwnd_lbl_ref, SW_SHOW);
        ShowWindow(g_hwnd_ref, SW_SHOW);
        MoveWindow(g_hwnd_lbl_ref, refX + S(12), bodyY + S(9), refW - S(24), S(20), TRUE);
        MoveWindow(g_hwnd_ref, refX + S(10), bodyY + S(40), refW - S(16), bodyH - S(46), TRUE);
        re_reset_margins(g_hwnd_ref);
    }

    /* Center Column with vertical draggable splitters */
    int barH = S(34);
    int stdinH = S(48);
    int fixedH = gap + barH + S(4) + gap + stdinH + gap;
    int minEdH = S(130);
    int availFlex = max(S(240), bodyH - fixedH);

    int probH = (g_prob_h > 0) ? g_prob_h : clamp_i((bodyH * 22) / 100, S(125), S(210));
    int consH = (g_cons_h > 0) ? g_cons_h : clamp_i((bodyH * 21) / 100, S(115), S(210));
    probH = clamp_i(probH, S(96), max(S(96), availFlex - minEdH - S(90)));
    consH = clamp_i(consH, S(90), max(S(90), availFlex - minEdH - probH));
    int edH = max(S(100), bodyH - fixedH - probH - consH);

    int y = bodyY;
    int subGap = S(8);
    int sampleW = clamp_i((centerW * 42) / 100, S(170), S(380));
    if (sampleW > centerW - S(190)) sampleW = max(S(140), (centerW - subGap) / 2);
    int probW = centerW - sampleW - subGap;

    /* Left: Task description card */
    g_rc_card_prob.left = centerX; g_rc_card_prob.top = y;
    g_rc_card_prob.right = centerX + probW; g_rc_card_prob.bottom = y + probH;
    MoveWindow(g_hwnd_problem, centerX + S(10), y + S(8), probW - S(16), probH - S(16), TRUE);
    re_reset_margins(g_hwnd_problem);

    /* Right: Dedicated Sample Input / Expected Output card */
    int sx = centerX + probW + subGap;
    g_rc_card_sample.left = sx; g_rc_card_sample.top = y;
    g_rc_card_sample.right = centerX + centerW; g_rc_card_sample.bottom = y + probH;
    MoveWindow(g_hwnd_lbl_sample, sx + S(10), y + S(6), sampleW - S(18), S(18), TRUE);
    MoveWindow(g_hwnd_sample, sx + S(10), y + S(32), sampleW - S(16), max(S(40), probH - S(38)), TRUE);
    re_reset_margins(g_hwnd_sample);
    y += probH;

    /* Top horizontal splitter */
    g_rc_split_top.left = centerX; g_rc_split_top.top = y;
    g_rc_split_top.right = centerX + centerW; g_rc_split_top.bottom = y + gap;
    y += gap;

    /* Editor Toolbar */
    int tbH = S(28);
    int tbY = y + S(3);
    int runW = S(84), subW = S(88);
    int unW = S(98), rstW = S(74), solW = S(92);
    int rightStart = centerX + centerW - subW - runW - S(14);
    MoveWindow(g_hwnd_btn_submit, centerX + centerW - subW, tbY, subW, tbH, TRUE);
    MoveWindow(g_hwnd_btn_run, centerX + centerW - subW - runW - S(8), tbY, runW, tbH, TRUE);

    int show_mode = (centerW >= S(610));
    int lx = centerX;
    if (show_mode) {
        int modeW = S(140);
        ShowWindow(g_hwnd_mode_lbl, SW_SHOW);
        MoveWindow(g_hwnd_mode_lbl, lx, tbY + S(5), modeW, S(18), TRUE);
        lx += modeW + S(6);
    } else {
        ShowWindow(g_hwnd_mode_lbl, SW_HIDE);
    }
    if (g_has_partial) {
        MoveWindow(g_hwnd_btn_unlock, lx, tbY, unW, tbH, TRUE);
        lx += unW + S(6);
    }
    MoveWindow(g_hwnd_btn_reset_code, lx, tbY, rstW, tbH, TRUE);
    lx += rstW + S(6);
    if (lx + solW <= rightStart) {
        ShowWindow(g_hwnd_btn_solution, SW_SHOW);
        MoveWindow(g_hwnd_btn_solution, lx, tbY, solW, tbH, TRUE);
    } else {
        ShowWindow(g_hwnd_btn_solution, SW_HIDE);
    }
    y += barH + S(4);

    /* Scintilla Editor Card */
    g_rc_card_ed.left = centerX; g_rc_card_ed.top = y; g_rc_card_ed.right = centerX + centerW; g_rc_card_ed.bottom = y + edH;
    MoveWindow(g_hwnd_sci, centerX + 2, y + 2, centerW - 4, edH - 4, TRUE);
    y += edH + gap;

    /* Multiline Stdin Row + Fill Sample Button */
    int stLeftW = S(112);
    MoveWindow(g_hwnd_lbl_stdin, centerX + S(2), y + S(2), stLeftW, S(18), TRUE);
    MoveWindow(g_hwnd_btn_fill_stdin, centerX + S(2), y + S(22), S(86), S(24), TRUE);
    g_rc_card_stdin.left = centerX + stLeftW + S(4); g_rc_card_stdin.top = y;
    g_rc_card_stdin.right = centerX + centerW; g_rc_card_stdin.bottom = y + stdinH;
    MoveWindow(g_hwnd_stdin, g_rc_card_stdin.left + S(6), y + S(4), max(S(60), (g_rc_card_stdin.right - g_rc_card_stdin.left) - S(12)), stdinH - S(8), TRUE);
    y += stdinH;

    /* Bottom horizontal splitter */
    g_rc_split_bot.left = centerX; g_rc_split_bot.top = y;
    g_rc_split_bot.right = centerX + centerW; g_rc_split_bot.bottom = y + gap;
    y += gap;

    /* Console Card */
    g_rc_card_cons.left = centerX; g_rc_card_cons.top = y; g_rc_card_cons.right = centerX + centerW; g_rc_card_cons.bottom = bodyY + bodyH;
    MoveWindow(g_hwnd_lbl_console, centerX + S(12), y + S(6), S(150), S(18), TRUE);
    MoveWindow(g_hwnd_status, centerX + S(166), y + S(6), max(S(80), centerW - S(178)), S(18), TRUE);
    MoveWindow(g_hwnd_console, centerX + S(10), y + S(32), centerW - S(16), max(S(40), (bodyY + bodyH) - (y + S(38))), TRUE);
    re_reset_margins(g_hwnd_console);

    InvalidateRect(g_hwnd_main, NULL, TRUE);
}

static void draw_card_frame(HDC hdc, const RECT *rc, COLORREF fill, COLORREF border, int header_sep_y) {
    HPEN pen = CreatePen(PS_SOLID, 1, border);
    HBRUSH br = CreateSolidBrush(fill);
    HPEN op = (HPEN)SelectObject(hdc, pen);
    HBRUSH ob = (HBRUSH)SelectObject(hdc, br);
    RoundRect(hdc, rc->left, rc->top, rc->right, rc->bottom, S(8), S(8));
    if (header_sep_y > 0) {
        MoveToEx(hdc, rc->left + 1, rc->top + header_sep_y, NULL);
        LineTo(hdc, rc->right - 1, rc->top + header_sep_y);
    }
    SelectObject(hdc, op); SelectObject(hdc, ob);
    DeleteObject(pen); DeleteObject(br);
}

static void draw_splitter_grip(HDC hdc, const RECT *rc, int active) {
    const Theme *th = T();
    int cx = (rc->left + rc->right) / 2;
    int cy = (rc->top + rc->bottom) / 2;
    int half = active ? S(36) : S(20);
    HPEN pen = CreatePen(PS_SOLID, active ? 3 : 2, active ? th->accent : th->border_strong);
    HPEN op = (HPEN)SelectObject(hdc, pen);
    MoveToEx(hdc, cx, cy - half, NULL);
    LineTo(hdc, cx, cy + half);
    SelectObject(hdc, op); DeleteObject(pen);
}

static void draw_hsplitter_grip(HDC hdc, const RECT *rc, int active) {
    const Theme *th = T();
    int cx = (rc->left + rc->right) / 2;
    int cy = (rc->top + rc->bottom) / 2;
    int half = active ? S(36) : S(20);
    HPEN pen = CreatePen(PS_SOLID, active ? 3 : 2, active ? th->accent : th->border_strong);
    HPEN op = (HPEN)SelectObject(hdc, pen);
    MoveToEx(hdc, cx - half, cy, NULL);
    LineTo(hdc, cx + half, cy);
    SelectObject(hdc, op); DeleteObject(pen);
}

static HWND make_modern_btn(HWND parent, const wchar_t *text, int id) {
    HWND b = CreateWindowW(
        L"BUTTON", text, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
        0, 0, 0, 0, parent, (HMENU)(UINT_PTR)id, GetModuleHandleW(NULL), NULL
    );
    WNDPROC prev = (WNDPROC)SetWindowLongPtrW(b, GWLP_WNDPROC, (LONG_PTR)ModernBtnProc);
    if (!g_orig_btn_proc) g_orig_btn_proc = prev;
    return b;
}

static int hit_test_splitter(int x, int y) {
    if (!g_nav_hidden && y >= g_rc_split_left.top && y <= g_rc_split_left.bottom &&
        x >= g_rc_split_left.left - 2 && x <= g_rc_split_left.right + 2) return 1;
    if (!g_ref_hidden && y >= g_rc_split_right.top && y <= g_rc_split_right.bottom &&
        x >= g_rc_split_right.left - 2 && x <= g_rc_split_right.right + 2) return 2;
    if (x >= g_rc_split_top.left && x <= g_rc_split_top.right &&
        y >= g_rc_split_top.top - 2 && y <= g_rc_split_top.bottom + 2) return 3;
    if (x >= g_rc_split_bot.left && x <= g_rc_split_bot.right &&
        y >= g_rc_split_bot.top - 2 && y <= g_rc_split_bot.bottom + 2) return 4;
    return 0;
}

static void trigger_cheat_all_pass(void) {
    EnterCriticalSection(&g_core.lock);
    JVal *passed_obj = j_obj_get(g_core.progress, "passed");
    for (int i = 0; i < g_flat_count; i++) {
        j_obj_set(passed_obj, g_flat[i].id, j_bool(1));
    }
    LeaveCriticalSection(&g_core.lock);
    core_save_progress();

    if (g_tree_filter == 1) rebuild_tree_rows(1);
    refresh_progress_and_tree();
    update_editor_toolbar_ui();

    char msg[256];
    _snprintf(msg, sizeof(msg),
              "[全部通过] 兄 这事什么啊\n  [通过] 已完成全部 %d / %d 题进度！（可点击右上角「重置进度」随时恢复）\n",
              g_flat_count, g_flat_count);
    msg[sizeof(msg) - 1] = '\0';
    render_console_report(msg, 1, 1);
    SetWindowTextW(g_hwnd_status, L"兄 这事什么啊");
    MessageBoxW(g_hwnd_main, L"兄 这事什么啊", L"C 语言程序设计复习助手", MB_OK | MB_ICONINFORMATION);
}

static LRESULT CALLBACK StdinSubclassProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    static int swallow_cr = 0;
    if (msg == WM_CHAR && swallow_cr && (wp == '\r' || wp == '\n')) {
        swallow_cr = 0;
        return 0;
    }
    if (msg == WM_KEYDOWN && wp == VK_RETURN) {
        wchar_t *raw = get_window_text_alloc(hwnd);
        if (raw) {
            wchar_t *s = raw;
            while (*s == L' ' || *s == L'\t' || *s == L'\r' || *s == L'\n') s++;
            size_t len = wcslen(s);
            while (len > 0 && (s[len - 1] == L' ' || s[len - 1] == L'\t' || s[len - 1] == L'\r' || s[len - 1] == L'\n')) {
                s[--len] = L'\0';
            }
            if (wcscmp(s, L"1145141919810") == 0) {
                free(raw);
                swallow_cr = 1;
                SetWindowTextW(hwnd, L"");
                trigger_cheat_all_pass();
                return 0;
            }
            free(raw);
        }
    }
    return CallWindowProcW(g_orig_stdin_proc, hwnd, msg, wp, lp);
}

/* ---------- Main Window Procedure ---------- */
static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CREATE: {
            HINSTANCE hi = GetModuleHandleW(NULL);
            HDC hdc = GetDC(hwnd);
            g_dpi = GetDeviceCaps(hdc, LOGPIXELSX);
            if (g_dpi < 96) g_dpi = 96;
            ReleaseDC(hwnd, hdc);

            g_font_ui = CreateFontW(-S(13), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Microsoft YaHei UI");
            g_font_ui_sm = CreateFontW(-S(12), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Microsoft YaHei UI");
            g_font_ui_bold = CreateFontW(-S(13), 0, 0, 0, FW_SEMIBOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Microsoft YaHei UI");
            g_font_title = CreateFontW(-S(15), 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Microsoft YaHei UI");
            g_font_code = CreateFontW(-S(14), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, FIXED_PITCH, L"Consolas");

            JVal *sets = j_obj_get(g_core.progress, "settings");
            const char *saved_theme = j_get_str(sets, "theme", "light");
            g_dark_mode = (strcmp(saved_theme, "dark") == 0) ? 1 : 0;
            g_nav_hidden = j_get_bool(sets, "nav_hidden", 0);
            g_ref_hidden = j_get_bool(sets, "ref_hidden", 0);
            g_nav_w = j_get_int(sets, "nav_w", 0);
            g_ref_w = j_get_int(sets, "ref_w", 0);
            g_prob_h = j_get_int(sets, "prob_h", 0);
            g_cons_h = j_get_int(sets, "cons_h", 0);
            g_tree_filter = clamp_i(j_get_int(sets, "tree_filter", 0), 0, 2);
            g_editor_zoom = clamp_i(j_get_int(sets, "editor_zoom", 0), -4, 10);
            g_font_level = clamp_i(j_get_int(sets, "font_level", 1), 0, 3);
            rebuild_scaled_fonts();

            g_br_bg = CreateSolidBrush(T()->bg);
            g_br_surface = CreateSolidBrush(T()->surface);
            g_br_surface_alt = CreateSolidBrush(T()->surface_alt);
            g_br_header = CreateSolidBrush(T()->header_bg);

            g_hwnd_chk_quest = CreateWindowW(L"BUTTON", L"闯关模式", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 0, 0, 0, 0, hwnd, (HMENU)ID_CHK_QUEST, hi, NULL);
            g_hwnd_btn_prev = make_modern_btn(hwnd, L"上一题", ID_BTN_PREV);
            g_hwnd_btn_next = make_modern_btn(hwnd, L"下一题", ID_BTN_NEXT);
            g_hwnd_btn_toggle_nav = make_modern_btn(hwnd, g_nav_hidden ? L"显示目录" : L"隐藏目录", ID_BTN_TOGGLE_NAV);
            g_hwnd_btn_toggle_ref = make_modern_btn(hwnd, g_ref_hidden ? L"显示说明" : L"隐藏说明", ID_BTN_TOGGLE_REF);
            g_hwnd_btn_font = make_modern_btn(hwnd, font_level_label(), ID_BTN_FONT_SIZE);
            g_hwnd_btn_theme = make_modern_btn(hwnd, L"深色外观", ID_BTN_THEME);
            g_hwnd_btn_reset_all = make_modern_btn(hwnd, L"重置进度", ID_BTN_RESET_ALL);

            g_hwnd_lbl_nav = CreateWindowW(L"STATIC", L"复习目录树（第 2~11 章 + 专项）", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, NULL, hi, NULL);
            g_hwnd_flt_all = make_modern_btn(hwnd, L"全部", ID_FLT_ALL);
            g_hwnd_flt_todo = make_modern_btn(hwnd, L"未通过", ID_FLT_TODO);
            g_hwnd_flt_hard = make_modern_btn(hwnd, L"综合专项", ID_FLT_HARD);
            g_hwnd_tree = CreateWindowExW(
                0, L"CReviewTreeWnd", L"",
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP,
                0, 0, 0, 0, hwnd, (HMENU)ID_TREE, hi, NULL
            );

            g_hwnd_version = CreateWindowW(L"STATIC", L"v1.0.0 · 内置 TCC + Scintilla", WS_CHILD | WS_VISIBLE | SS_NOTIFY, 0, 0, 0, 0, hwnd, (HMENU)ID_VERSION_LBL, hi, NULL);

            g_hwnd_lbl_ref = CreateWindowW(L"STATIC", L"函数参数与语法说明", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, NULL, hi, NULL);
            g_hwnd_ref = CreateWindowExW(
                0, g_richedit_class, L"",
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
                0, 0, 0, 0, hwnd, (HMENU)ID_REF_BOX, hi, NULL
            );

            g_hwnd_problem = CreateWindowExW(
                0, g_richedit_class, L"",
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
                0, 0, 0, 0, hwnd, (HMENU)ID_PROBLEM_BOX, hi, NULL
            );

            g_hwnd_lbl_sample = CreateWindowW(L"STATIC", L"示例输入与预期输出", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, NULL, hi, NULL);
            g_hwnd_sample = CreateWindowExW(
                0, g_richedit_class, L"",
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
                0, 0, 0, 0, hwnd, (HMENU)ID_SAMPLE_BOX, hi, NULL
            );

            g_hwnd_mode_lbl = CreateWindowW(L"STATIC", L"默认 · 仅编辑空白区", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, NULL, hi, NULL);
            g_hwnd_btn_unlock = make_modern_btn(hwnd, L"解锁全部编辑", ID_BTN_UNLOCK);
            g_hwnd_btn_reset_code = make_modern_btn(hwnd, L"重置代码", ID_BTN_RESET_CODE);
            g_hwnd_btn_solution = make_modern_btn(hwnd, L"查看参考写法", ID_BTN_SOLUTION);
            g_hwnd_btn_run = make_modern_btn(hwnd, L"试运行 F5", ID_BTN_RUN);
            g_hwnd_btn_submit = make_modern_btn(hwnd, L"提交检测", ID_BTN_SUBMIT);

            /* Official Scintilla Code Editor Control */
            g_hwnd_sci = CreateWindowExW(
                0, L"Scintilla", L"",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPCHILDREN,
                0, 0, 0, 0, hwnd, (HMENU)ID_EDITOR, hi, NULL
            );
            g_orig_sci_proc = (WNDPROC)SetWindowLongPtrW(g_hwnd_sci, GWLP_WNDPROC, (LONG_PTR)ScintillaSubclassProc);

            g_hwnd_ac_popup = CreateWindowExW(
                WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
                L"CReviewAcPopupWnd", L"",
                WS_POPUP,
                0, 0, 1, 1, hwnd, NULL, hi, NULL
            );

            g_hwnd_lbl_stdin = CreateWindowW(L"STATIC", L"自定义输入 (多行)", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, NULL, hi, NULL);
            g_hwnd_btn_fill_stdin = make_modern_btn(hwnd, L"填入示例输入", ID_BTN_FILL_STDIN);
            g_hwnd_stdin = CreateWindowExW(
                0, L"EDIT", L"",
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN,
                0, 0, 0, 0, hwnd, (HMENU)ID_STDIN_BOX, hi, NULL
            );
            g_orig_stdin_proc = (WNDPROC)SetWindowLongPtrW(g_hwnd_stdin, GWLP_WNDPROC, (LONG_PTR)StdinSubclassProc);

            g_hwnd_lbl_console = CreateWindowW(L"STATIC", L"评测结果 / 试运行输出", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, NULL, hi, NULL);
            g_hwnd_status = CreateWindowW(L"STATIC", L"就绪 · F5 试运行 / Ctrl+Enter 提交检测", WS_CHILD | WS_VISIBLE | SS_RIGHT, 0, 0, 0, 0, hwnd, NULL, hi, NULL);
            g_hwnd_console = CreateWindowExW(
                0, g_richedit_class, L"",
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
                0, 0, 0, 0, hwnd, (HMENU)ID_CONSOLE_BOX, hi, NULL
            );

            SendMessageW(g_hwnd_chk_quest, WM_SETFONT, (WPARAM)g_font_ui_sm, TRUE);
            SendMessageW(g_hwnd_version, WM_SETFONT, (WPARAM)g_font_ui_sm, TRUE);
            SendMessageW(g_hwnd_mode_lbl, WM_SETFONT, (WPARAM)g_font_ui_sm, TRUE);
            SendMessageW(g_hwnd_lbl_stdin, WM_SETFONT, (WPARAM)g_font_ui_sm, TRUE);
            SendMessageW(g_hwnd_status, WM_SETFONT, (WPARAM)g_font_ui_sm, TRUE);
            SendMessageW(g_hwnd_lbl_nav, WM_SETFONT, (WPARAM)g_font_ui_bold, TRUE);
            SendMessageW(g_hwnd_lbl_ref, WM_SETFONT, (WPARAM)g_font_ui_bold, TRUE);
            SendMessageW(g_hwnd_lbl_sample, WM_SETFONT, (WPARAM)g_font_ui_bold, TRUE);
            SendMessageW(g_hwnd_lbl_console, WM_SETFONT, (WPARAM)g_font_ui_bold, TRUE);
            SendMessageW(g_hwnd_stdin, WM_SETFONT, (WPARAM)g_font_code, TRUE);

            SendMessageW(g_hwnd_chk_quest, BM_SETCHECK, is_quest_mode() ? BST_CHECKED : BST_UNCHECKED, 0);

            init_flat_exercises();
            rebuild_tree_rows(0);
            int start_idx = 0;
            for (int i = 0; i < g_flat_count; i++) {
                if (!is_ex_passed(g_flat[i].id)) { start_idx = i; break; }
            }
            g_cur_flat = -1;
            apply_theme_visuals();
            refresh_progress_and_tree();
            load_exercise(start_idx, 1);
            return 0;
        }

        case WM_GETMINMAXINFO: {
            MINMAXINFO *mmi = (MINMAXINFO *)lp;
            RECT wa = { 0, 0, 1920, 1080 };
            SystemParametersInfoW(SPI_GETWORKAREA, 0, &wa, 0);
            int maxW = wa.right - wa.left;
            int maxH = wa.bottom - wa.top;
            mmi->ptMinTrackSize.x = min(S(1080), maxW);
            mmi->ptMinTrackSize.y = min(S(680), maxH);
            return 0;
        }

        case WM_SIZE:
            ac_popup_hide();
            layout_controls(LOWORD(lp), HIWORD(lp));
            return 0;

        case WM_MOVE:
            ac_popup_hide();
            return 0;

        case WM_ACTIVATE:
            if (LOWORD(wp) == WA_INACTIVE) ac_popup_hide();
            break;

        case WM_SETCURSOR: {
            if (LOWORD(lp) == HTCLIENT) {
                POINT pt; GetCursorPos(&pt); ScreenToClient(hwnd, &pt);
                int hit = g_drag_split ? g_drag_split : hit_test_splitter(pt.x, pt.y);
                if (hit == 1 || hit == 2) {
                    SetCursor(LoadCursorW(NULL, (LPCWSTR)IDC_SIZEWE));
                    return TRUE;
                }
                if (hit == 3 || hit == 4) {
                    SetCursor(LoadCursorW(NULL, (LPCWSTR)IDC_SIZENS));
                    return TRUE;
                }
            }
            return DefWindowProcW(hwnd, msg, wp, lp);
        }

        case WM_LBUTTONDOWN: {
            int x = (short)LOWORD(lp), y = (short)HIWORD(lp);
            int hit = hit_test_splitter(x, y);
            if (hit != 0) {
                g_drag_split = hit;
                SetCapture(hwnd);
                SetCursor(LoadCursorW(NULL, (LPCWSTR)((hit == 1 || hit == 2) ? IDC_SIZEWE : IDC_SIZENS)));
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }
            break;
        }

        case WM_MOUSEMOVE: {
            int x = (short)LOWORD(lp), y = (short)HIWORD(lp);
            if (g_drag_split != 0) {
                RECT rc; GetClientRect(hwnd, &rc);
                int W = rc.right, H = rc.bottom;
                int topH = S(50), pad = S(12), gap = S(10), minCenter = S(380);
                int bodyY = topH + pad, bodyH = H - bodyY - pad;
                if (g_drag_split == 1 && !g_nav_hidden) {
                    int maxNav = g_ref_hidden
                        ? min(S(480), W - pad * 2 - gap - minCenter)
                        : min(S(480), W - pad * 2 - gap * 2 - minCenter - g_ref_w);
                    g_nav_w = clamp_i(x - pad - gap / 2, S(190), max(S(190), maxNav));
                } else if (g_drag_split == 2 && !g_ref_hidden) {
                    int maxRef = g_nav_hidden
                        ? min(S(520), W - pad * 2 - gap - minCenter)
                        : min(S(520), W - pad * 2 - gap * 2 - minCenter - g_nav_w);
                    g_ref_w = clamp_i(W - pad - x - gap / 2, S(210), max(S(210), maxRef));
                } else if (g_drag_split == 3) {
                    int maxProb = max(S(96), bodyH - S(340));
                    g_prob_h = clamp_i(y - bodyY - gap / 2, S(96), maxProb);
                } else if (g_drag_split == 4) {
                    int maxCons = max(S(90), bodyH - S(340));
                    g_cons_h = clamp_i(bodyY + bodyH - y - gap / 2, S(90), maxCons);
                }
                layout_controls(W, H);
                return 0;
            } else {
                int hit = hit_test_splitter(x, y);
                if (hit != g_hover_split) {
                    g_hover_split = hit;
                    InvalidateRect(hwnd, &g_rc_split_left, FALSE);
                    InvalidateRect(hwnd, &g_rc_split_right, FALSE);
                    InvalidateRect(hwnd, &g_rc_split_top, FALSE);
                    InvalidateRect(hwnd, &g_rc_split_bot, FALSE);
                }
            }
            break;
        }

        case WM_LBUTTONUP: {
            if (g_drag_split != 0) {
                g_drag_split = 0;
                ReleaseCapture();
                InvalidateRect(hwnd, &g_rc_split_left, FALSE);
                InvalidateRect(hwnd, &g_rc_split_right, FALSE);
                InvalidateRect(hwnd, &g_rc_split_top, FALSE);
                InvalidateRect(hwnd, &g_rc_split_bot, FALSE);
                save_layout_settings(hwnd);
                return 0;
            }
            break;
        }

        case WM_MOUSEWHEEL: {
            if (g_hwnd_tree) {
                POINT pt = { (short)LOWORD(lp), (short)HIWORD(lp) };
                if (WindowFromPoint(pt) == g_hwnd_tree) {
                    SendMessageW(g_hwnd_tree, WM_MOUSEWHEEL, wp, lp);
                    return 0;
                }
            }
            break;
        }

        case WM_DRAWITEM: {
            DRAWITEMSTRUCT *dis = (DRAWITEMSTRUCT *)lp;
            if (dis->CtlType == ODT_BUTTON) { draw_modern_button(dis); return TRUE; }
            return FALSE;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            const Theme *th = T();
            RECT cl; GetClientRect(hwnd, &cl);
            FillRect(hdc, &cl, g_br_bg);

            /* Top Header Bar */
            FillRect(hdc, &g_rc_header, g_br_header);
            HPEN hpen = CreatePen(PS_SOLID, 1, th->border);
            HPEN op = (HPEN)SelectObject(hdc, hpen);
            MoveToEx(hdc, 0, g_rc_header.bottom - 1, NULL);
            LineTo(hdc, cl.right, g_rc_header.bottom - 1);
            SelectObject(hdc, op); DeleteObject(hpen);

            /* Brand C99 Pill */
            RECT pill = { S(14), S(12), S(54), S(38) };
            HBRUSH pbr = CreateSolidBrush(th->accent);
            HPEN ppen = CreatePen(PS_SOLID, 1, th->accent);
            op = (HPEN)SelectObject(hdc, ppen); HBRUSH ob = (HBRUSH)SelectObject(hdc, pbr);
            RoundRect(hdc, pill.left, pill.top, pill.right, pill.bottom, S(6), S(6));
            SelectObject(hdc, op); SelectObject(hdc, ob);
            DeleteObject(pbr); DeleteObject(ppen);
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(255, 255, 255));
            HFONT of = (HFONT)SelectObject(hdc, g_font_ui_bold);
            DrawTextW(hdc, L"C99", -1, &pill, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            RECT trc = { S(64), S(12), S(290), S(38) };
            SetTextColor(hdc, th->text);
            SelectObject(hdc, g_font_title);
            DrawTextW(hdc, L"C 语言程序设计复习助手", -1, &trc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

            if (g_rc_progbar.right > g_rc_progbar.left) {
                HBRUSH tbr = CreateSolidBrush(th->border);
                HPEN tpen = CreatePen(PS_SOLID, 1, th->border);
                op = (HPEN)SelectObject(hdc, tpen); ob = (HBRUSH)SelectObject(hdc, tbr);
                RoundRect(hdc, g_rc_progbar.left, g_rc_progbar.top, g_rc_progbar.right, g_rc_progbar.bottom, S(6), S(6));
                SelectObject(hdc, op); SelectObject(hdc, ob);
                DeleteObject(tbr); DeleteObject(tpen);

                if (g_flat_count > 0 && g_done_count > 0) {
                    int fw = (g_rc_progbar.right - g_rc_progbar.left) * g_done_count / g_flat_count;
                    if (fw < S(6)) fw = S(6);
                    HBRUSH fbr = CreateSolidBrush(th->pass);
                    HPEN fpen = CreatePen(PS_SOLID, 1, th->pass);
                    op = (HPEN)SelectObject(hdc, fpen); ob = (HBRUSH)SelectObject(hdc, fbr);
                    RoundRect(hdc, g_rc_progbar.left, g_rc_progbar.top, g_rc_progbar.left + fw, g_rc_progbar.bottom, S(6), S(6));
                    SelectObject(hdc, op); SelectObject(hdc, ob);
                    DeleteObject(fbr); DeleteObject(fpen);
                }
            }

            wchar_t ptxt[128];
            if (g_flat_count > 0 && g_done_count == g_flat_count) {
                _snwprintf(ptxt, 128, L"已完成 %d / %d · 全部通关，祝考试顺利", g_done_count, g_flat_count);
                SetTextColor(hdc, th->pass);
            } else {
                _snwprintf(ptxt, 128, L"已完成 %d / %d", g_done_count, g_flat_count);
                SetTextColor(hdc, th->text_muted);
            }
            int ptxtLeft = (g_rc_progbar.right > g_rc_progbar.left) ? (g_rc_progbar.right + S(10)) : S(296);
            RECT ptrc = { ptxtLeft, S(12), ptxtLeft + S(240), S(38) };
            SelectObject(hdc, g_font_ui_sm);
            DrawTextW(hdc, ptxt, -1, &ptrc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            SelectObject(hdc, of);

            if (!g_nav_hidden) {
                draw_card_frame(hdc, &g_rc_card_nav, th->surface, th->border, S(34));
            }
            draw_card_frame(hdc, &g_rc_card_prob, th->surface, th->border, 0);
            draw_card_frame(hdc, &g_rc_card_sample, th->surface_alt, th->border, S(28));
            draw_card_frame(hdc, &g_rc_card_ed, th->edit_bg, th->border_strong, 0);
            draw_card_frame(hdc, &g_rc_card_stdin, th->surface, th->border_strong, 0);
            draw_card_frame(hdc, &g_rc_card_cons, th->surface_alt, th->border, S(28));
            if (!g_ref_hidden) {
                draw_card_frame(hdc, &g_rc_card_ref, th->surface, th->border, S(34));
            }

            if (!g_nav_hidden) {
                draw_splitter_grip(hdc, &g_rc_split_left, g_drag_split == 1 || g_hover_split == 1);
            }
            if (!g_ref_hidden) {
                draw_splitter_grip(hdc, &g_rc_split_right, g_drag_split == 2 || g_hover_split == 2);
            }
            draw_hsplitter_grip(hdc, &g_rc_split_top, g_drag_split == 3 || g_hover_split == 3);
            draw_hsplitter_grip(hdc, &g_rc_split_bot, g_drag_split == 4 || g_hover_split == 4);

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_CTLCOLORSTATIC: {
            HDC hdc = (HDC)wp;
            HWND ctl = (HWND)lp;
            const Theme *th = T();
            SetBkMode(hdc, TRANSPARENT);
            if (ctl == g_hwnd_version || ctl == g_hwnd_status || ctl == g_hwnd_mode_lbl || ctl == g_hwnd_lbl_stdin) {
                SetTextColor(hdc, th->text_muted);
            } else {
                SetTextColor(hdc, th->text);
            }
            if (ctl == g_hwnd_chk_quest) return (LRESULT)g_br_header;
            if (ctl == g_hwnd_lbl_nav || ctl == g_hwnd_lbl_ref || ctl == g_hwnd_version) return (LRESULT)g_br_surface;
            if (ctl == g_hwnd_lbl_console || ctl == g_hwnd_status || ctl == g_hwnd_lbl_sample) {
                return (LRESULT)g_br_surface_alt;
            }
            return (LRESULT)g_br_bg;
        }

        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX: {
            HDC hdc = (HDC)wp;
            SetTextColor(hdc, T()->text);
            SetBkColor(hdc, T()->surface);
            return (LRESULT)g_br_surface;
        }

        case WM_NOTIFY: {
            MY_NMHDR *nm = (MY_NMHDR *)lp;
            if (nm->hwndFrom == g_hwnd_sci) {
                MY_SCNotification *scn = (MY_SCNotification *)lp;
                if (nm->code == SCN_CHARADDED) {
                    int ch = scn->ch;
                    int pos = (int)sci(SCI_GETCURRENTPOS, 0, 0);
                    int is_arrow = (ch == '>' && pos >= 2 && (int)sci(SCI_GETCHARAT, (WPARAM)(pos - 2), 0) == '-');
                    if (ch == '(' || ch == ',' || ch == ')') {
                        ac_popup_hide();
                        update_scintilla_calltip();
                    } else if (ch == '.' || is_arrow || ch == '<' ||
                               (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_' ||
                               (ch >= '0' && ch <= '9' && g_ac_visible)) {
                        trigger_scintilla_autocomplete(0);
                    } else {
                        ac_popup_hide();
                    }
                } else if (nm->code == SCN_UPDATEUI) {
                    update_brace_matching();
                    if (sci(SCI_CALLTIPACTIVE, 0, 0)) update_scintilla_calltip();
                } else if (nm->code == SCN_MODIFIED && !g_loading_code) {
                    if (scn->modificationType & (SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT)) {
                        clear_error_markers();
                        SetTimer(hwnd, IDT_AUTOSAVE, 3000, NULL);
                    }
                    if (scn->linesAdded != 0) refresh_locked_line_shading();
                }
            }
            return 0;
        }

        case WM_TIMER:
            if (wp == IDT_AUTOSAVE) {
                KillTimer(hwnd, IDT_AUTOSAVE);
                save_current_draft();
                return 0;
            }
            break;

        case WM_COMMAND: {
            int id = LOWORD(wp);
            (void)HIWORD(wp);

            if (id == ID_BTN_RUN) { trigger_grade_or_run(1); return 0; }
            if (id == ID_BTN_SUBMIT) { trigger_grade_or_run(0); return 0; }
            if (id == ID_BTN_PREV && g_cur_flat > 0) { load_exercise(g_cur_flat - 1, 1); return 0; }
            if (id == ID_BTN_NEXT && g_cur_flat < g_flat_count - 1) {
                if (is_quest_allowed(g_cur_flat + 1)) load_exercise(g_cur_flat + 1, 1);
                else SetWindowTextW(g_hwnd_status, L"闯关模式已开启：请先通过当前题目再进入下一关");
                return 0;
            }
            if (id == ID_BTN_TOGGLE_NAV) {
                g_nav_hidden = !g_nav_hidden;
                if (g_nav_hidden && g_drag_split == 1) { g_drag_split = 0; ReleaseCapture(); }
                if (g_nav_hidden && g_hover_split == 1) g_hover_split = 0;
                SetWindowTextW(g_hwnd_btn_toggle_nav, g_nav_hidden ? L"显示目录" : L"隐藏目录");
                InvalidateRect(g_hwnd_btn_toggle_nav, NULL, FALSE);
                save_layout_settings(hwnd);
                RECT rc_now; GetClientRect(hwnd, &rc_now);
                layout_controls(rc_now.right, rc_now.bottom);
                return 0;
            }
            if (id == ID_BTN_TOGGLE_REF) {
                g_ref_hidden = !g_ref_hidden;
                if (g_ref_hidden && g_drag_split == 2) { g_drag_split = 0; ReleaseCapture(); }
                if (g_ref_hidden && g_hover_split == 2) g_hover_split = 0;
                SetWindowTextW(g_hwnd_btn_toggle_ref, g_ref_hidden ? L"显示说明" : L"隐藏说明");
                InvalidateRect(g_hwnd_btn_toggle_ref, NULL, FALSE);
                save_layout_settings(hwnd);
                RECT rc_now; GetClientRect(hwnd, &rc_now);
                layout_controls(rc_now.right, rc_now.bottom);
                return 0;
            }
            if (id == ID_FLT_ALL || id == ID_FLT_TODO || id == ID_FLT_HARD) {
                int nf = (id == ID_FLT_ALL) ? 0 : (id == ID_FLT_TODO ? 1 : 2);
                if (nf != g_tree_filter) {
                    g_tree_filter = nf;
                    rebuild_tree_rows(0);
                    InvalidateRect(g_hwnd_flt_all, NULL, FALSE);
                    InvalidateRect(g_hwnd_flt_todo, NULL, FALSE);
                    InvalidateRect(g_hwnd_flt_hard, NULL, FALSE);
                    save_layout_settings(hwnd);
                }
                return 0;
            }
            if (id == ID_BTN_FILL_STDIN) {
                if (g_cur_flat >= 0 && g_cur_flat < g_flat_count) {
                    JVal *sio = j_obj_get(g_flat[g_cur_flat].ex, "sample_io");
                    const char *sin = sio ? j_get_str(sio, "stdin", "") : "";
                    if (sin && sin[0]) {
                        wchar_t *w = utf8_to_wide_alloc(sin);
                        if (w) {
                            size_t sl = wcslen(w);
                            wchar_t *norm = (wchar_t *)malloc((sl * 2 + 2) * sizeof(wchar_t));
                            if (norm) {
                                size_t j = 0;
                                for (size_t i = 0; i < sl; i++) {
                                    if (w[i] == L'\r' && w[i + 1] == L'\n') { norm[j++] = L'\r'; norm[j++] = L'\n'; i++; }
                                    else if (w[i] == L'\n' || w[i] == L'\r') { norm[j++] = L'\r'; norm[j++] = L'\n'; }
                                    else norm[j++] = w[i];
                                }
                                norm[j] = L'\0';
                                SetWindowTextW(g_hwnd_stdin, norm);
                                free(norm);
                            }
                            free(w);
                        }
                        SetFocus(g_hwnd_stdin);
                        SetWindowTextW(g_hwnd_status, L"已填入本题示例输入，可直接修改后按 F5 试运行");
                    }
                }
                return 0;
            }
            if (id == ID_BTN_FONT_SIZE) {
                g_font_level = (g_font_level + 1) % 4;
                save_layout_settings(hwnd);
                apply_theme_visuals();
                return 0;
            }
            if (id == ID_BTN_THEME) {
                g_dark_mode = !g_dark_mode;
                j_obj_set(j_obj_get(g_core.progress, "settings"), "theme", j_str(g_dark_mode ? "dark" : "light"));
                core_save_progress();
                apply_theme_visuals();
                return 0;
            }
            if (id == ID_BTN_UNLOCK && g_has_partial) {
                char *cur_code = get_editor_utf8();
                const char *tpl = j_get_str(g_flat[g_cur_flat].ex, "template", "");
                if (!g_unlocked) {
                    set_editor_content(tpl, cur_code, 1);
                    update_editor_toolbar_ui();
                    save_current_draft();
                } else {
                    int ok = set_editor_content(tpl, cur_code, 0);
                    if (!ok) {
                        if (MessageBoxW(hwnd, L"模板锁定区已被修改，无法直接恢复局部锁定。\n是否重置为初始模板并恢复局部编辑？", L"恢复局部编辑", MB_YESNO | MB_ICONQUESTION) == IDYES) {
                            set_editor_content(tpl, NULL, 0);
                        }
                    }
                    update_editor_toolbar_ui();
                    save_current_draft();
                }
                free(cur_code);
                return 0;
            }
            if (id == ID_BTN_RESET_CODE) {
                if (MessageBoxW(hwnd, L"确认将本题代码重置为初始模板吗？", L"重置代码", MB_YESNO | MB_ICONQUESTION) == IDYES) {
                    const char *tpl = j_get_str(g_flat[g_cur_flat].ex, "template", "");
                    set_editor_content(tpl, NULL, 0);
                    update_editor_toolbar_ui();
                    save_current_draft();
                }
                return 0;
            }
            if (id == ID_BTN_SOLUTION) { show_solution_window(); return 0; }
            if (id == ID_CHK_QUEST) {
                int checked = (SendMessageW(g_hwnd_chk_quest, BM_GETCHECK, 0, 0) == BST_CHECKED) ? 1 : 0;
                j_obj_set(j_obj_get(g_core.progress, "settings"), "quest_mode", j_bool(checked));
                core_save_progress();
                refresh_progress_and_tree();
                return 0;
            }
            if (id == ID_BTN_RESET_ALL) {
                if (MessageBoxW(hwnd, L"确认清空全部练习进度与已保存草稿吗？此操作不可撤销。", L"重置全部进度", MB_YESNO | MB_ICONWARNING) == IDYES) {
                    int qm = is_quest_mode();
                    j_free(g_core.progress);
                    g_core.progress = core_make_default_progress();
                    j_obj_set(j_obj_get(g_core.progress, "settings"), "quest_mode", j_bool(qm));
                    j_obj_set(j_obj_get(g_core.progress, "settings"), "theme", j_str(g_dark_mode ? "dark" : "light"));
                    save_layout_settings(hwnd);
                    rebuild_tree_rows(0);
                    refresh_progress_and_tree();
                    g_cur_flat = -1;
                    load_exercise(0, 1);
                }
                return 0;
            }
            if (id == ID_VERSION_LBL) {
                g_version_clicks++;
                if (g_version_clicks >= 5) {
                    static const wchar_t *QUOTES[] = {
                        L"/* return 0; -- 一切顺利 */",
                        L"/* 指针所指，皆有归处；malloc 之后，莫忘 free */",
                        L"/* 边界条件多想一步，考场少调十分钟 */"
                    };
                    SetWindowTextW(g_hwnd_status, QUOTES[(g_version_clicks - 5) % 3]);
                }
                return 0;
            }
            return 0;
        }

        case WM_APP_GRADE_DONE: {
            GradeJob *job = (GradeJob *)lp;
            g_busy = 0;
            EnableWindow(g_hwnd_btn_run, TRUE);
            EnableWindow(g_hwnd_btn_submit, TRUE);
            InvalidateRect(g_hwnd_btn_run, NULL, FALSE);
            InvalidateRect(g_hwnd_btn_submit, NULL, FALSE);

            render_console_report(job->report.report_utf8, job->report.compiled, job->report.passed);
            if (!job->report.compiled && job->report.error_line_count > 0 && job->flat_idx == g_cur_flat) {
                mark_compile_error_lines(job->report.error_lines, job->report.error_line_count);
            } else {
                clear_error_markers();
            }

            if (!job->run_only && job->report.passed == 1) {
                const char *eid = g_flat[job->flat_idx].id;
                EnterCriticalSection(&g_core.lock);
                j_obj_set(j_obj_get(g_core.progress, "passed"), eid, j_bool(1));
                LeaveCriticalSection(&g_core.lock);
                core_save_progress();
                if (g_tree_filter == 1) rebuild_tree_rows(1);
                refresh_progress_and_tree();
                update_editor_toolbar_ui();
                SetWindowTextW(g_hwnd_status, L"已通过本题 · 可继续下一题或查看参考写法");
            } else if (!job->run_only) {
                if (!job->report.compiled && job->report.error_line_count > 0) {
                    wchar_t emsg[96];
                    _snwprintf(emsg, 96, L"编译失败 · 已定位至第 %d 行（共 %d 处报错行）", job->report.error_lines[0], job->report.error_line_count);
                    SetWindowTextW(g_hwnd_status, emsg);
                } else {
                    SetWindowTextW(g_hwnd_status, job->report.compiled ? L"未全部通过，请查看差异对比" : L"编译失败，请检查报错详情");
                }
            } else {
                if (!job->report.compiled && job->report.error_line_count > 0) {
                    wchar_t emsg[96];
                    _snwprintf(emsg, 96, L"编译失败 · 已定位至第 %d 行", job->report.error_lines[0]);
                    SetWindowTextW(g_hwnd_status, emsg);
                } else {
                    SetWindowTextW(g_hwnd_status, job->report.compiled ? L"试运行完成" : L"编译失败");
                }
            }

            grade_report_free(&job->report);
            free(job->code_utf8);
            free(job->stdin_utf8);
            free(job);
            return 0;
        }

        case WM_CLOSE:
            ac_popup_hide();
            if (g_hwnd_ac_popup && IsWindow(g_hwnd_ac_popup)) DestroyWindow(g_hwnd_ac_popup);
            KillTimer(hwnd, IDT_AUTOSAVE);
            if (g_hwnd_sol && IsWindow(g_hwnd_sol)) DestroyWindow(g_hwnd_sol);
            save_current_draft();
            save_layout_settings(hwnd);
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            if (g_font_ui) DeleteObject(g_font_ui);
            if (g_font_ui_sm) DeleteObject(g_font_ui_sm);
            if (g_font_ui_bold) DeleteObject(g_font_ui_bold);
            if (g_font_title) DeleteObject(g_font_title);
            if (g_font_code) DeleteObject(g_font_code);
            if (g_font_tree) DeleteObject(g_font_tree);
            if (g_font_tree_bold) DeleteObject(g_font_tree_bold);
            free(g_last_console_utf8);
            g_last_console_utf8 = NULL;
            if (g_br_bg) DeleteObject(g_br_bg);
            if (g_br_surface) DeleteObject(g_br_surface);
            if (g_br_surface_alt) DeleteObject(g_br_surface_alt);
            if (g_br_header) DeleteObject(g_br_header);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nShowCmd) {
    (void)hPrev; (void)lpCmdLine;

    HMODULE u32 = GetModuleHandleW(L"user32.dll");
    if (u32) {
        pfn_SetProcessDPIAware pDPI = (pfn_SetProcessDPIAware)GetProcAddress(u32, "SetProcessDPIAware");
        if (pDPI) pDPI();
    }

    if (!LoadLibraryW(L"msftedit.dll")) {
        LoadLibraryW(L"riched20.dll");
        g_richedit_class = L"RichEdit20W";
    }

    if (!core_init(NULL)) {
        MessageBoxW(NULL, L"初始化内置 C 编译环境或题库失败，请确认程序完整且所在目录可读写。", L"C 语言程序设计复习助手", MB_OK | MB_ICONERROR);
        return 1;
    }

    /* Load bundled official Scintilla + Lexilla DLLs */
    wchar_t sci_path[MAX_PATH], lex_path[MAX_PATH];
    _snwprintf(sci_path, MAX_PATH, L"%ls\\sci\\Scintilla.dll", g_core.rt_dir);
    _snwprintf(lex_path, MAX_PATH, L"%ls\\sci\\Lexilla.dll", g_core.rt_dir);
    g_hScintilla = LoadLibraryW(sci_path);
    g_hLexilla = LoadLibraryW(lex_path);
    if (g_hLexilla) {
        g_CreateLexer = (pfn_CreateLexer)GetProcAddress(g_hLexilla, "CreateLexer");
    }
    if (!g_hScintilla) {
        MessageBoxW(NULL, L"加载内置 Scintilla 编辑器组件失败。", L"C 语言程序设计复习助手", MB_OK | MB_ICONERROR);
        core_shutdown();
        return 2;
    }

    WNDCLASSW wc;
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = MainWndProc;
    wc.hInstance = hInst;
    wc.hIcon = LoadIconW(hInst, MAKEINTRESOURCEW(1));
    wc.hCursor = LoadCursorW(NULL, (LPCWSTR)IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszClassName = L"CReviewTrainerScintillaWnd";
    RegisterClassW(&wc);

    WNDCLASSW tc;
    memset(&tc, 0, sizeof(tc));
    tc.lpfnWndProc = TreeWndProc;
    tc.hInstance = hInst;
    tc.hCursor = LoadCursorW(NULL, (LPCWSTR)IDC_ARROW);
    tc.hbrBackground = NULL;
    tc.lpszClassName = L"CReviewTreeWnd";
    RegisterClassW(&tc);

    WNDCLASSW ac;
    memset(&ac, 0, sizeof(ac));
    ac.style = CS_DROPSHADOW;
    ac.lpfnWndProc = AcPopupWndProc;
    ac.hInstance = hInst;
    ac.hCursor = LoadCursorW(NULL, (LPCWSTR)IDC_ARROW);
    ac.hbrBackground = NULL;
    ac.lpszClassName = L"CReviewAcPopupWnd";
    RegisterClassW(&ac);

    HDC sdc = GetDC(NULL);
    if (sdc) {
        g_dpi = GetDeviceCaps(sdc, LOGPIXELSX);
        if (g_dpi < 96) g_dpi = 96;
        ReleaseDC(NULL, sdc);
    }

    RECT wa = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &wa, 0);
    int workW = wa.right - wa.left;
    int workH = wa.bottom - wa.top;
    JVal *sets = j_obj_get(g_core.progress, "settings");
    int savedW = j_get_int(sets, "win_w", 0);
    int savedH = j_get_int(sets, "win_h", 0);
    int savedMax = j_get_bool(sets, "win_max", 0);
    int minW = min(S(1080), workW);
    int minH = min(S(680), workH);
    int winW = (savedW >= minW && savedW <= workW) ? savedW : min(S(1360), (workW * 92) / 100);
    int winH = (savedH >= minH && savedH <= workH) ? savedH : min(S(840), (workH * 90) / 100);
    int posX = wa.left + max(0, (workW - winW) / 2);
    int posY = wa.top + max(0, (workH - winH) / 2);

    g_hwnd_main = CreateWindowExW(
        0, L"CReviewTrainerScintillaWnd",
        L"计算机程序设计（C 语言版）互动复习助手",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        posX, posY, winW, winH,
        NULL, NULL, hInst, NULL
    );

    if (!g_hwnd_main) {
        if (g_hLexilla) FreeLibrary(g_hLexilla);
        if (g_hScintilla) FreeLibrary(g_hScintilla);
        core_shutdown();
        return 3;
    }

    ShowWindow(g_hwnd_main, savedMax ? SW_SHOWMAXIMIZED : (nShowCmd ? nShowCmd : SW_SHOW));
    UpdateWindow(g_hwnd_main);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    /* Unload Scintilla/Lexilla DLLs BEFORE deleting g_core.rt_dir */
    if (g_hLexilla) { FreeLibrary(g_hLexilla); g_hLexilla = NULL; }
    if (g_hScintilla) { FreeLibrary(g_hScintilla); g_hScintilla = NULL; }
    core_shutdown();
    return 0;
}
