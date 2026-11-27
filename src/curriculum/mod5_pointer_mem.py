"""Module 5: Pointers and dynamic memory allocation (Chapter 8)."""

MODULE = {
    "id": "m5",
    "title": "模块 5：指针与动态内存",
    "subtitle": "对应第 8 章",
    "badge": "内存掌控",
    "exercises": [
        {
            "id": "m5_1",
            "code": "5.1",
            "title": "指针定义、& 取址与 * 解引用",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握指针变量声明、保存变量地址以及通过指针间接读写目标变量。",
            "goal": "定义整型指针 p 指向变量 x，再通过 *p 将 x 的值修改为原值的 3 倍并加上 5，最后输出修改后的 x。",
            "param_docs": [
                {
                    "name": "指针运算符 & 与 *",
                    "header": "C 语言指针基础（第 8.3～8.4 节）",
                    "prototype": "int *p = &x;   *p = 新值;",
                    "purpose": "保存变量的内存地址，并通过地址间接读取或修改该内存单元的值。",
                    "params": [
                        {"name": "&x", "type": "取地址运算符", "desc": "取得变量 x 在内存中的起始地址，类型为 int *。"},
                        {"name": "*p", "type": "解引用运算符", "desc": "访问指针 p 所指向地址上的变量本身，可读取也可赋值。"},
                        {"name": "NULL", "type": "空指针常量", "desc": "表示指针当前不指向任何有效内存地址；解引用 NULL 会导致程序崩溃。"},
                    ],
                    "returns": "无返回值。",
                    "notes": ["声明 int *p, q; 时只有 p 是指针，q 是普通 int；定义多个指针需写作 int *p, *q;。"],
                }
            ],
            "template": """#include <stdio.h>

int main(void) {
    int x;
    scanf("%d", &x);

    // <<<EDIT_START>>>
    // 1. 定义 int *p 指向 x
    // 2. 仅通过 *p 将 x 修改为 (*p) * 3 + 5

    // <<<EDIT_END>>>

    printf("%d\\n", x);
    return 0;
}
""",
            "solution": """#include <stdio.h>

int main(void) {
    int x;
    scanf("%d", &x);

    int *p = &x;
    *p = (*p) * 3 + 5;

    printf("%d\\n", x);
    return 0;
}
""",
            "code_checks": [
                {"pattern": r"int\s*\*\s*\w+", "message": "请定义整型指针变量 int *p。"},
                {"pattern": r"&\s*x", "message": "请使用 &x 获取变量 x 的地址。"},
            ],
            "tests": [
                {"name": "正数", "stdin": "4\n", "expected_stdout": "17\n"},
                {"name": "零", "stdin": "0\n", "expected_stdout": "5\n"},
                {"name": "负数", "stdin": "-2\n", "expected_stdout": "-1\n"},
            ],
        },
        {
            "id": "m5_2",
            "code": "5.2",
            "title": "指针作为函数参数",
            "kind": "single",
            "editable_default": "partial",
            "summary": "通过传递变量地址实现函数修改外部多个变量（如交换值、返回多结果）。",
            "goal": "补全 swap_and_diff(int *a, int *b, int *diff)：交换 *a 和 *b 的值，并将交换后 |*a - *b| 的非负差值写入 *diff。",
            "param_docs": [
                {
                    "name": "指针作形参",
                    "header": "C 语言地址传递（第 8.7 节）",
                    "prototype": "void func(int *a, int *b);",
                    "purpose": "C 语言函数默认按值传递；传入变量地址后，函数内部通过解引用可以直接修改主调函数中的实参。",
                    "params": [
                        {"name": "a, b", "type": "int *", "desc": "接收调用处传入的变量地址（如 &x, &y）。"},
                        {"name": "diff", "type": "int *", "desc": "输出型参数，用于把额外计算结果写回主调函数变量。"},
                    ],
                    "returns": "void；结果直接写入指针指向的外部变量。",
                    "notes": ["交换两个指针所指值时，应交换 *a 与 *b 的内容，而不是交换形参指针 a 与 b 本身。"],
                }
            ],
            "template": """#include <stdio.h>

void swap_and_diff(int *a, int *b, int *diff) {
    // 1. 交换 *a 与 *b
    // 2. 计算非负差值写入 *diff
// <<<EDIT_START>>>
    
// <<<EDIT_END>>>
}

int main(void) {
    int x, y, d = 0;
    scanf("%d %d", &x, &y);
    swap_and_diff(&x, &y, &d);
    printf("%d %d %d\\n", x, y, d);
    return 0;
}
""",
            "solution": """#include <stdio.h>

void swap_and_diff(int *a, int *b, int *diff) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
    *diff = (*a >= *b) ? (*a - *b) : (*b - *a);
}

int main(void) {
    int x, y, d = 0;
    scanf("%d %d", &x, &y);
    swap_and_diff(&x, &y, &d);
    printf("%d %d %d\\n", x, y, d);
    return 0;
}
""",
            "tests": [
                {"name": "常规", "stdin": "3 10\n", "expected_stdout": "10 3 7\n"},
                {"name": "相等", "stdin": "8 8\n", "expected_stdout": "8 8 0\n"},
                {"name": "含负数", "stdin": "-4 6\n", "expected_stdout": "6 -4 10\n"},
            ],
        },
        {
            "id": "m5_3",
            "code": "5.3",
            "title": "指针遍历数组与字符串",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握指针算术运算 p++ 与使用 char * 实现自定义字符串长度和查找。",
            "goal": "不使用 string.h，用指针遍历方式补全 my_count_char(const char *s, char target)，统计字符串 s 中字符 target 出现的次数。",
            "param_docs": [
                {
                    "name": "指针算术运算",
                    "header": "C 语言数组与字符串指针（第 8.5～8.8 节）",
                    "prototype": "const char *p = s; while (*p != '\\0') { ... p++; }",
                    "purpose": "指针加 1（p++）会使地址按所指向类型的字节数向后移动一个元素位置。",
                    "params": [
                        {"name": "s", "type": "const char *", "desc": "指向字符串首字符的只读指针，不允许通过它修改字符内容。"},
                        {"name": "*p", "type": "char", "desc": "当前指向的字符；字符串以 '\\0' 作为结束标志。"},
                    ],
                    "returns": "返回 target 在字符串中出现的次数。",
                    "notes": ["对于 int *p，p + 1 地址实际增加 sizeof(int) 字节；对于 char *p，增加 1 字节。"],
                }
            ],
            "template": """#include <stdio.h>

int my_count_char(const char *s, char target) {
    // 使用指针遍历统计 target 在 s 中出现的次数
// <<<EDIT_START>>>
    return 0;
// <<<EDIT_END>>>
}

int main(void) {
    char s[128], target;
    scanf("%s %c", s, &target);
    printf("%d\\n", my_count_char(s, target));
    return 0;
}
""",
            "solution": """#include <stdio.h>

int my_count_char(const char *s, char target) {
    int cnt = 0;
    for (const char *p = s; *p != '\\0'; p++) {
        if (*p == target) {
            cnt++;
        }
    }
    return cnt;
}

int main(void) {
    char s[128], target;
    scanf("%s %c", s, &target);
    printf("%d\\n", my_count_char(s, target));
    return 0;
}
""",
            "tests": [
                {"name": "多次出现", "stdin": "programming g\n", "expected_stdout": "2\n"},
                {"name": "未出现", "stdin": "hello z\n", "expected_stdout": "0\n"},
                {"name": "全部匹配", "stdin": "aaaa a\n", "expected_stdout": "4\n"},
            ],
        },
        {
            "id": "m5_4",
            "code": "5.4",
            "title": "malloc 与 free 动态一维数组",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握 stdlib.h 中动态内存分配 malloc、判空、按数组使用以及 free 释放。",
            "goal": "读入正整数 n，使用 malloc 动态分配存放 n 个 int 的内存空间，读入这 n 个整数，计算并输出它们的总和，最后调用 free 释放内存。",
            "param_docs": [
                {
                    "name": "malloc",
                    "header": "#include <stdlib.h>",
                    "prototype": "void *malloc(size_t size);",
                    "purpose": "在堆区动态分配连续 size 个字节的未初始化内存空间。",
                    "params": [
                        {"name": "size", "type": "size_t", "desc": "需要申请的总字节数；存放 n 个 int 应传入 n * sizeof(int)。"},
                    ],
                    "returns": "成功返回所分配内存的起始地址（void *，可赋给目标类型指针）；内存不足时返回 NULL。",
                    "notes": ["分配后必须检查返回值是否为 NULL；使用完毕必须调用 free 释放。"],
                },
                {
                    "name": "free",
                    "header": "#include <stdlib.h>",
                    "prototype": "void free(void *p);",
                    "purpose": "释放先前由 malloc、calloc 或 realloc 分配的动态内存。",
                    "params": [
                        {"name": "p", "type": "void *", "desc": "指向动态内存块起始地址的指针；传入 NULL 时不执行任何操作。"},
                    ],
                    "returns": "无返回值。",
                    "notes": ["释放后建议将指针置为 NULL，避免后续误用已释放内存（悬空指针）。"],
                },
            ],
            "template": """#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int n;
    scanf("%d", &n);

    // <<<EDIT_START>>>
    // 1. 使用 malloc 分配 n 个 int 空间，并检查是否返回 NULL
    // 2. 读入 n 个整数并求和，输出总和（long long 或 int 均可，此处用 long long）
    // 3. 调用 free 释放内存

    // <<<EDIT_END>>>
    return 0;
}
""",
            "solution": """#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int n;
    scanf("%d", &n);

    int *arr = (int *)malloc((size_t)n * sizeof(int));
    if (arr == NULL) {
        return 1;
    }
    long long sum = 0;
    for (int i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
        sum += arr[i];
    }
    printf("%lld\\n", sum);
    free(arr);
    arr = NULL;
    return 0;
}
""",
            "code_checks": [
                {"pattern": r"\bmalloc\s*\(", "message": "请使用 malloc 动态分配数组内存。"},
                {"pattern": r"\bfree\s*\(", "message": "请在程序结束前调用 free 释放动态内存。"},
            ],
            "tests": [
                {"name": "常规求和", "stdin": "5\n10 20 30 40 50\n", "expected_stdout": "150\n"},
                {"name": "含负数", "stdin": "4\n-5 10 -3 8\n", "expected_stdout": "10\n"},
            ],
        },
        {
            "id": "m5_5",
            "code": "5.5",
            "title": "calloc / realloc 与函数指针作参数",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握函数指针作为函数参数（第 8.15.2 节）及 calloc 初始化清零特性。",
            "goal": "补全通用数组汇总函数 apply_op(const int *a, int n, int (*op)(int, int)) 和两个运算函数 add_op / mul_op：当模式为 1 时求数组元素之和，模式为 2 时求数组元素之积。",
            "param_docs": [
                {
                    "name": "函数指针作为形参",
                    "header": "C 语言函数指针（第 8.15 节）",
                    "prototype": "int (*op)(int, int)",
                    "purpose": "把函数地址作为参数传递给另一个函数，使调用者可以在运行时决定具体执行哪个操作函数。",
                    "params": [
                        {"name": "op", "type": "int (*)(int, int)", "desc": "指向“接收两个 int 参数并返回 int”的函数的指针；括号 (*op) 不可省略。"},
                        {"name": "调用方式", "type": "op(x, y)", "desc": "在函数体内直接写 op(acc, a[i]) 即可调用所指向的函数。"},
                    ],
                    "returns": "apply_op 返回依次结合所有元素后的结果。",
                    "notes": ["与 malloc 不同，calloc(n, sizeof(int)) 会将分配的内存全部初始化为 0；realloc(p, new_size) 用于调整已有动态内存块大小。"],
                }
            ],
            "template": """#include <stdio.h>
#include <stdlib.h>

int add_op(int x, int y) { return x + y; }
int mul_op(int x, int y) { return x * y; }

int apply_op(const int *a, int n, int (*op)(int, int)) {
    // 以 a[0] 为初值，从 i = 1 到 n - 1 依次执行 acc = op(acc, a[i]) 并返回
// <<<EDIT_START>>>
    return 0;
// <<<EDIT_END>>>
}

int main(void) {
    int n, mode;
    scanf("%d %d", &n, &mode);
    int *a = (int *)calloc((size_t)n, sizeof(int));
    for (int i = 0; i < n; i++) scanf("%d", &a[i]);

    int res = (mode == 1) ? apply_op(a, n, add_op) : apply_op(a, n, mul_op);
    printf("%d\\n", res);
    free(a);
    return 0;
}
""",
            "solution": """#include <stdio.h>
#include <stdlib.h>

int add_op(int x, int y) { return x + y; }
int mul_op(int x, int y) { return x * y; }

int apply_op(const int *a, int n, int (*op)(int, int)) {
    int acc = a[0];
    for (int i = 1; i < n; i++) {
        acc = op(acc, a[i]);
    }
    return acc;
}

int main(void) {
    int n, mode;
    scanf("%d %d", &n, &mode);
    int *a = (int *)calloc((size_t)n, sizeof(int));
    for (int i = 0; i < n; i++) scanf("%d", &a[i]);

    int res = (mode == 1) ? apply_op(a, n, add_op) : apply_op(a, n, mul_op);
    printf("%d\\n", res);
    free(a);
    return 0;
}
""",
            "tests": [
                {"name": "模式1求和", "stdin": "4 1\n2 3 4 5\n", "expected_stdout": "14\n"},
                {"name": "模式2求积", "stdin": "4 2\n2 3 4 5\n", "expected_stdout": "120\n"},
                {"name": "单元素", "stdin": "1 2\n7\n", "expected_stdout": "7\n"},
            ],
        },
        {
            "id": "m5_stage_a",
            "code": "★ 阶段 5A",
            "title": "阶段综合：指针数组实现多字符串排序",
            "kind": "stage",
            "editable_default": "partial",
            "summary": "掌握第 8.10 节指针数组：通过交换指针（而非复制整个字符串内容）对多个字符串排序。",
            "goal": "输入正整数 n（1 <= n <= 50）和 n 个单词（每个长度不超过 30）。使用 char *ptrs[50] 指针数组指向各单词，仅通过交换指针完成按字典序升序排列，最后逐行输出排序后的单词。",
            "guidance": [
                "1. 先将 words[i] 的首地址依次赋给 ptrs[i]。",
                "2. 使用选择排序或冒泡排序：用 strcmp(ptrs[i], ptrs[j]) 比较字符串字典序。",
                "3. 当需要交换顺序时，只交换两个指针变量 char *tmp = ptrs[i]; ptrs[i] = ptrs[j]; ptrs[j] = tmp;。",
                "4. 遍历输出 ptrs[0..n-1]。",
            ],
            "sidebar_refs": {
                "functions": [
                    {"name": "strcmp", "sig": "int strcmp(const char *s1, const char *s2);", "desc": "s1 > s2 时返回正数，需交换以实现升序。"},
                ],
                "structures": [
                    {
                        "name": "指针数组初始化与交换",
                        "code": "char *ptrs[50];\nfor (int i = 0; i < n; i++) ptrs[i] = words[i];\n// 交换指针:\nchar *tmp = ptrs[i]; ptrs[i] = ptrs[j]; ptrs[j] = tmp;",
                    },
                ],
            },
            "template": """#include <stdio.h>
#include <string.h>

int main(void) {
    int n;
    char words[50][32];
    char *ptrs[50];

    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        scanf("%31s", words[i]);
        ptrs[i] = words[i];
    }

    // <<<EDIT_START>>>
    // 仅通过交换 ptrs 中的指针实现字典序升序排序，并逐行输出

    // <<<EDIT_END>>>
    return 0;
}
""",
            "solution": """#include <stdio.h>
#include <string.h>

int main(void) {
    int n;
    char words[50][32];
    char *ptrs[50];

    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        scanf("%31s", words[i]);
        ptrs[i] = words[i];
    }

    for (int i = 0; i < n - 1; i++) {
        int min_i = i;
        for (int j = i + 1; j < n; j++) {
            if (strcmp(ptrs[j], ptrs[min_i]) < 0) {
                min_i = j;
            }
        }
        char *tmp = ptrs[i];
        ptrs[i] = ptrs[min_i];
        ptrs[min_i] = tmp;
    }

    for (int i = 0; i < n; i++) {
        printf("%s\\n", ptrs[i]);
    }
    return 0;
}
""",
            "tests": [
                {
                    "name": "5个单词排序",
                    "stdin": "5\npear apple orange banana grape\n",
                    "expected_stdout": "apple\nbanana\ngrape\norange\npear\n",
                },
                {
                    "name": "前缀相同单词",
                    "stdin": "3\ncode coder c\n",
                    "expected_stdout": "c\ncode\ncoder\n",
                },
            ],
        },
        {
            "id": "m5_stage_b",
            "code": "★ 阶段 5B",
            "title": "阶段综合：动态扩容数组与去重过滤",
            "kind": "stage",
            "editable_default": "full",
            "summary": "综合运用 malloc、realloc、free 与指针操作实现不定长正整数过滤与统计。",
            "goal": "循环读取若干整数直到遇到 -1 结束（数据个数预先未知，可能超过初始容量）。请用动态数组（初始容量设为 4，满时用 realloc 翻倍扩容）保存所有大于 0 的数。\n第一行输出保存的正数个数；第二行按输入顺序输出所有正数（空格分隔；若个数为 0 则第二行输出 EMPTY）。最后释放动态内存。",
            "guidance": [
                "1. 初始化 cap = 4, len = 0, int *arr = (int *)malloc(cap * sizeof(int));。",
                "2. 循环 scanf(\"%d\", &x) == 1：若 x == -1 则 break；若 x <= 0 则 continue。",
                "3. 当 len == cap 时，令 cap *= 2，用 int *tmp = (int *)realloc(arr, cap * sizeof(int)); 判空后更新 arr = tmp。",
                "4. 存入 arr[len++] = x；结束后按要求格式输出，最后调用 free(arr)。",
            ],
            "sidebar_refs": {
                "functions": [
                    {"name": "malloc", "sig": "void *malloc(size_t size);", "desc": "分配初始动态数组空间。"},
                    {"name": "realloc", "sig": "void *realloc(void *ptr, size_t new_size);", "desc": "将已有内存块调整为 new_size 字节并保留原数据，建议先用临时指针接收返回值。"},
                    {"name": "free", "sig": "void free(void *ptr);", "desc": "释放动态内存防止内存泄漏。"},
                ],
                "structures": [
                    {
                        "name": "动态数组倍增扩容模板",
                        "code": "if (len == cap) {\n    cap *= 2;\n    int *tmp = (int *)realloc(arr, (size_t)cap * sizeof(int));\n    if (tmp == NULL) { free(arr); return 1; }\n    arr = tmp;\n}",
                    },
                ],
            },
            "template": """#include <stdio.h>
#include <stdlib.h>

int main(void) {
    // 请完整编写程序

    return 0;
}
""",
            "solution": """#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int cap = 4, len = 0;
    int *arr = (int *)malloc((size_t)cap * sizeof(int));
    if (arr == NULL) return 1;

    int x;
    while (scanf("%d", &x) == 1) {
        if (x == -1) break;
        if (x <= 0) continue;
        if (len == cap) {
            cap *= 2;
            int *tmp = (int *)realloc(arr, (size_t)cap * sizeof(int));
            if (tmp == NULL) {
                free(arr);
                return 1;
            }
            arr = tmp;
        }
        arr[len++] = x;
    }

    printf("%d\\n", len);
    if (len == 0) {
        printf("EMPTY\\n");
    } else {
        for (int i = 0; i < len; i++) {
            if (i > 0) putchar(' ');
            printf("%d", arr[i]);
        }
        putchar('\\n');
    }
    free(arr);
    return 0;
}
""",
            "code_checks": [
                {"pattern": r"\bmalloc\s*\(", "message": "请使用 malloc 分配初始动态数组。"},
                {"pattern": r"\brealloc\s*\(", "message": "请使用 realloc 实现动态扩容。"},
                {"pattern": r"\bfree\s*\(", "message": "请调用 free 释放动态内存。"},
            ],
            "tests": [
                {"name": "触发多次扩容", "stdin": "3 -5 0 8 1 2 9 4 7 6 -1\n", "expected_stdout": "8\n3 8 1 2 9 4 7 6\n"},
                {"name": "无正数", "stdin": "0 -2 -9 -1\n", "expected_stdout": "0\nEMPTY\n"},
            ],
        },
    ],
}
