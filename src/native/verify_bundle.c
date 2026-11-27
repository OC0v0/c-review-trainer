/*
 * Headless console verifier that loads the EXACT bundled payload inside c-review-trainer.exe
 * via core_init(exe_path) and verifies all bundled exercises, File I/O, compile errors, timeouts,
 * and progress persistence.
 */
#define _CRT_SECURE_NO_WARNINGS
#include "core.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: verify_bundle.exe <path-to-c-review-trainer.exe>\n");
        return 2;
    }
    wchar_t wexe[MAX_PATH];
    utf8_to_wide(argv[1], wexe, MAX_PATH);
    if (!core_init(wexe)) {
        fprintf(stderr, "core_init FAILED on %s\n", argv[1]);
        return 3;
    }

    int total = 0, passed = 0;
    JVal *ex_first = NULL;
    for (size_t m = 0; m < g_core.modules->arr_len; m++) {
        JVal *exs = j_obj_get(g_core.modules->items[m], "exercises");
        if (!exs || exs->type != J_ARR) continue;
        for (size_t j = 0; j < exs->arr_len; j++) {
            JVal *ex = exs->items[j];
            if (!ex_first) ex_first = ex;
            total++;
            const char *id = j_get_str(ex, "id", "");
            const char *sol = j_get_str(ex, "solution", "");
            GradeReport gr = core_grade_or_run(ex, sol, 0, NULL);
            if (gr.compiled && gr.passed == 1) {
                passed++;
            } else {
                printf("FAIL %s:\n%s\n", id, gr.report_utf8 ? gr.report_utf8 : "");
            }
            grade_report_free(&gr);
        }
    }

    /* Test compile error detection */
    GradeReport r_err = core_grade_or_run(ex_first, "#include <stdio.h>\nint main(void){ int a = ; return 0; }\n", 0, NULL);
    int err_ok = (!r_err.compiled);
    grade_report_free(&r_err);

    /* Test infinite loop timeout */
    GradeReport r_to = core_grade_or_run(ex_first, "#include <stdio.h>\nint main(void){ while(1){} return 0; }\n", 0, NULL);
    int to_ok = (r_to.compiled && r_to.passed == 0 && r_to.timed_out == 1);
    grade_report_free(&r_to);

    /* Test progress save */
    j_obj_set(j_obj_get(g_core.progress, "passed"), "m1_1", j_bool(1));
    core_save_progress();
    int prog_ok = (GetFileAttributesW(g_core.progress_path) != INVALID_FILE_ATTRIBUTES);
    DeleteFileW(g_core.progress_path);

    printf("NATIVE_BUNDLE_VERIFY passed=%d/%d compile_err_ok=%d timeout_ok=%d prog_ok=%d\n",
           passed, total, err_ok, to_ok, prog_ok);

    core_shutdown();
    return (total >= 66 && passed == total && err_ok && to_ok && prog_ok) ? 0 : 1;
}
