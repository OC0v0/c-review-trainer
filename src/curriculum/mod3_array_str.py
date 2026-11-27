"""Module 3: Arrays and string processing (Chapter 5)."""

MODULE = {
    "id": "m3",
    "title": "模块 3：数组与字符串处理",
    "subtitle": "对应第 5 章",
    "badge": "数据序列",
    "exercises": [
        {
            "id": "m3_1",
            "code": "3.1",
            "title": "一维数组遍历与极值统计",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握一维数组下标 0..n-1 遍历、最值初值设定与平均值计算。",
            "goal": "读入 n 个整数存入数组 a，补全代码求出最大值 max_val、最小值 min_val 与平均值 avg（保留 2 位小数）。",
            "param_docs": [
                {
                    "name": "一维数组定义与访问",
                    "header": "C 语言内置复合类型",
                    "prototype": "类型 数组名[常量长度];   访问：数组名[下标]",
                    "purpose": "在连续内存空间中存放多个相同类型的数据，通过从 0 开始的整数下标访问元素。",
                    "params": [
                        {
                            "name": "长度",
                            "type": "正整数常量",
                            "desc": "数组能容纳的元素个数；合法下标范围为 0 到 长度-1。",
                        },
                        {
                            "name": "最值初值",
                            "type": "初始化规范",
                            "desc": "求数组最大值/最小值时，应将初值设为 a[0]，不能随意设为 0（避免所有元素均为负数或正数时出错）。",
                        },
                    ],
                    "returns": "a[i] 表示第 i+1 个元素的左值，可读可写。",
                    "notes": [
                        "数组越界（访问 a[n] 或 a[-1]）属于未定义行为，考试中需严格检查循环边界 i < n。",
                    ],
                }
            ],
            "template": """#include <stdio.h>

int main(void) {
    int n, a[100];
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &a[i]);
    }

    int max_val = 0, min_val = 0;
    double avg = 0.0;

    // <<<EDIT_START>>>
    // 计算 max_val, min_val 和 avg（注意处理负数输入）

    // <<<EDIT_END>>>

    printf("%d %d %.2f\\n", max_val, min_val, avg);
    return 0;
}
""",
            "solution": """#include <stdio.h>

int main(void) {
    int n, a[100];
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &a[i]);
    }

    int max_val = 0, min_val = 0;
    double avg = 0.0;

    max_val = a[0];
    min_val = a[0];
    int sum = 0;
    for (int i = 0; i < n; i++) {
        if (a[i] > max_val) max_val = a[i];
        if (a[i] < min_val) min_val = a[i];
        sum += a[i];
    }
    avg = (double)sum / n;

    printf("%d %d %.2f\\n", max_val, min_val, avg);
    return 0;
}
""",
            "tests": [
                {"name": "常规正数", "stdin": "5\n10 20 5 30 15\n", "expected_stdout": "30 5 16.00\n"},
                {"name": "全为负数", "stdin": "4\n-10 -3 -25 -7\n", "expected_stdout": "-3 -25 -11.25\n"},
                {"name": "单元素", "stdin": "1\n42\n", "expected_stdout": "42 42 42.00\n"},
            ],
        },
        {
            "id": "m3_2",
            "code": "3.2",
            "title": "fgets 与 puts 字符串行读写",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握使用 fgets 安全读取含空格的一行字符串、去除末尾换行符及 puts 输出。",
            "goal": "使用 fgets 从标准输入读取一行可能包含空格的字符串到 buf 中，去掉可能存在的末尾 '\\n' 或 '\\r'，将所有小写字母转为大写后用 puts 输出。",
            "param_docs": [
                {
                    "name": "fgets",
                    "header": "#include <stdio.h>",
                    "prototype": "char *fgets(char *buf, int n, FILE *stream);",
                    "purpose": "从指定输入流读取最多 n-1 个字符，遇换行符或文件结束时停止，并在末尾自动添加 '\\0'。",
                    "params": [
                        {"name": "buf", "type": "char *", "desc": "存放读入字符串的字符数组首地址。"},
                        {"name": "n", "type": "int", "desc": "缓冲区总大小（通常传 sizeof(buf)），最多读入 n-1 个有效字符。"},
                        {"name": "stream", "type": "FILE *", "desc": "输入流指针；从键盘/标准输入读取时传 stdin。"},
                    ],
                    "returns": "成功返回 buf；遇文件结束或出错返回 NULL。",
                    "notes": [
                        "fgets 会把行尾的 '\\n' 一并存入 buf，常需遍历并替换为 '\\0'。",
                    ],
                },
                {
                    "name": "puts",
                    "header": "#include <stdio.h>",
                    "prototype": "int puts(const char *str);",
                    "purpose": "将 str 指向的以 '\\0' 结尾的字符串输出到标准输出，并在末尾自动追加一个换行符。",
                    "params": [
                        {"name": "str", "type": "const char *", "desc": "待输出的字符串首地址。"},
                    ],
                    "returns": "成功返回非负值；失败返回 EOF。",
                    "notes": ["puts 会自动换行，因此不要在字符串末尾重复保留 '\\n'。"],
                },
            ],
            "template": """#include <stdio.h>

int main(void) {
    char buf[128];

    // <<<EDIT_START>>>
    // 1. 使用 fgets(buf, sizeof(buf), stdin) 读取一行
    // 2. 去掉末尾的 '\\n' 或 '\\r'，并将 'a'~'z' 转为 'A'~'Z'
    // 3. 使用 puts(buf) 输出

    // <<<EDIT_END>>>
    return 0;
}
""",
            "solution": """#include <stdio.h>

int main(void) {
    char buf[128];

    if (fgets(buf, sizeof(buf), stdin) != NULL) {
        for (int i = 0; buf[i] != '\\0'; i++) {
            if (buf[i] == '\\n' || buf[i] == '\\r') {
                buf[i] = '\\0';
                break;
            }
            if (buf[i] >= 'a' && buf[i] <= 'z') {
                buf[i] = (char)(buf[i] - ('a' - 'A'));
            }
        }
        puts(buf);
    }
    return 0;
}
""",
            "code_checks": [
                {"pattern": r"\bfgets\s*\(", "message": "请使用 fgets 读取整行字符串。"},
                {"pattern": r"\bputs\s*\(", "message": "请使用 puts 输出字符串。"},
            ],
            "tests": [
                {"name": "含空格句子", "stdin": "hello c language 2026\n", "expected_stdout": "HELLO C LANGUAGE 2026\n"},
                {"name": "混合大小写", "stdin": "AbC_xYz!\n", "expected_stdout": "ABC_XYZ!\n"},
            ],
        },
        {
            "id": "m3_3",
            "code": "3.3",
            "title": "字符串库函数 strlen / strcpy / strcat / strcmp",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握 string.h 中四个核心字符串函数的参数顺序与返回值语义。",
            "goal": "读入两个不含空格的单词 s1、s2：将字典序较小的单词复制到 res，再把 \"-\" 和较大的单词拼接到 res 后面，最后输出 res 及 res 的长度。",
            "param_docs": [
                {
                    "name": "strlen",
                    "header": "#include <string.h>",
                    "prototype": "size_t strlen(const char *str);",
                    "purpose": "统计字符串 str 中有效字符个数，不包含末尾的 '\\0'。",
                    "params": [{"name": "str", "type": "const char *", "desc": "以 '\\0' 结尾的字符串。"}],
                    "returns": "返回字符个数（无符号整数，printf 可用 %zu 或强转 (int) 后用 %d 输出）。",
                },
                {
                    "name": "strcmp",
                    "header": "#include <string.h>",
                    "prototype": "int strcmp(const char *s1, const char *s2);",
                    "purpose": "按字符 ASCII 码逐位比较 s1 与 s2 的字典序大小。",
                    "params": [
                        {"name": "s1", "type": "const char *", "desc": "第一个字符串。"},
                        {"name": "s2", "type": "const char *", "desc": "第二个字符串。"},
                    ],
                    "returns": "s1 < s2 返回负整数；s1 == s2 返回 0；s1 > s2 返回正整数。",
                },
                {
                    "name": "strcpy / strcat",
                    "header": "#include <string.h>",
                    "prototype": "char *strcpy(char *dst, const char *src);  |  char *strcat(char *dst, const char *src);",
                    "purpose": "strcpy 将 src 复制覆盖到 dst；strcat 将 src 拼接到 dst 现有字符串末尾（覆盖原 '\\0' 并补新 '\\0'）。",
                    "params": [
                        {"name": "dst", "type": "char *", "desc": "目标字符数组，必须有足够剩余空间容纳结果和 '\\0'。"},
                        {"name": "src", "type": "const char *", "desc": "源字符串。"},
                    ],
                    "returns": "均返回目标地址 dst。",
                    "notes": ["不能用 dst = src 对字符数组整体赋值，也不能用 s1 == s2 比较字符串内容。"],
                },
            ],
            "template": """#include <stdio.h>
#include <string.h>

int main(void) {
    char s1[64], s2[64], res[160];
    scanf("%s %s", s1, s2);

    // <<<EDIT_START>>>
    // 使用 strcmp 比较 s1 和 s2，并用 strcpy 和 strcat 构造 "较小词-较大词" 存入 res
    // 然后输出 res 和它的长度（用 strlen），格式: "%s %d\\n"

    // <<<EDIT_END>>>
    return 0;
}
""",
            "solution": """#include <stdio.h>
#include <string.h>

int main(void) {
    char s1[64], s2[64], res[160];
    scanf("%s %s", s1, s2);

    if (strcmp(s1, s2) <= 0) {
        strcpy(res, s1);
        strcat(res, "-");
        strcat(res, s2);
    } else {
        strcpy(res, s2);
        strcat(res, "-");
        strcat(res, s1);
    }
    printf("%s %d\\n", res, (int)strlen(res));
    return 0;
}
""",
            "code_checks": [
                {"pattern": r"\bstrcmp\s*\(", "message": "请使用 strcmp 比较字符串大小。"},
                {"pattern": r"\bstrcpy\s*\(", "message": "请使用 strcpy 复制字符串。"},
                {"pattern": r"\bstrcat\s*\(", "message": "请使用 strcat 拼接字符串。"},
                {"pattern": r"\bstrlen\s*\(", "message": "请使用 strlen 获取字符串长度。"},
            ],
            "tests": [
                {"name": "逆序输入", "stdin": "banana apple\n", "expected_stdout": "apple-banana 12\n"},
                {"name": "正序输入", "stdin": "cat dog\n", "expected_stdout": "cat-dog 7\n"},
                {"name": "相等字符串", "stdin": "ustc ustc\n", "expected_stdout": "ustc-ustc 9\n"},
            ],
        },
        {
            "id": "m3_4",
            "code": "3.4",
            "title": "二维数组与矩阵转置",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握二维数组行列索引 mat[i][j] 及行列互换规律。",
            "goal": "输入 r 行 c 列的整数矩阵（1 <= r, c <= 10），输出其转置矩阵（c 行 r 列），每行元素之间用单个空格隔开。",
            "param_docs": [
                {
                    "name": "二维数组",
                    "header": "C 语言内置复合类型",
                    "prototype": "类型 数组名[行数][列数];   访问：数组名[行下标][列下标]",
                    "purpose": "按行优先顺序连续存储二维表格或矩阵数据。",
                    "params": [
                        {"name": "行下标 i", "type": "int", "desc": "范围 0 到 r-1。"},
                        {"name": "列下标 j", "type": "int", "desc": "范围 0 到 c-1。"},
                    ],
                    "returns": "mat[i][j] 表示第 i 行第 j 列的元素；转置后的第 j 行第 i 列即原矩阵的 mat[i][j]。",
                    "notes": ["输出转置矩阵时，外层循环遍历原列下标 j (0..c-1)，内层遍历原行下标 i (0..r-1)。"],
                }
            ],
            "template": """#include <stdio.h>

int main(void) {
    int r, c, mat[10][10];
    scanf("%d %d", &r, &c);
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            scanf("%d", &mat[i][j]);
        }
    }

    // <<<EDIT_START>>>
    // 输出 mat 的转置矩阵（c 行 r 列）

    // <<<EDIT_END>>>
    return 0;
}
""",
            "solution": """#include <stdio.h>

int main(void) {
    int r, c, mat[10][10];
    scanf("%d %d", &r, &c);
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            scanf("%d", &mat[i][j]);
        }
    }

    for (int j = 0; j < c; j++) {
        for (int i = 0; i < r; i++) {
            if (i > 0) putchar(' ');
            printf("%d", mat[i][j]);
        }
        putchar('\\n');
    }
    return 0;
}
""",
            "tests": [
                {"name": "2x3 矩阵", "stdin": "2 3\n1 2 3\n4 5 6\n", "expected_stdout": "1 4\n2 5\n3 6\n"},
                {"name": "1x3 行向量", "stdin": "1 3\n7 8 9\n", "expected_stdout": "7\n8\n9\n"},
            ],
        },
        {
            "id": "m3_5",
            "code": "3.5",
            "title": "strncpy、strstr 子串计数与 sizeof / strlen 辨析",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握有长度上限的字符串复制 strncpy、子串查找 strstr 及 sizeof 与 strlen 的区别。",
            "goal": "读入主串 text、子串 pat 和截取长度 k（均不含空格，1 <= k <= 15）。\n第一行：使用 strstr 统计 pat 在 text 中不重叠出现的次数，并输出该次数；\n第二行：使用 strncpy 将 text 的前 k 个字符复制到定长数组 prefix[16]（必须确保末尾有 '\\0'），输出 \"prefix sizeof(prefix) strlen(prefix)\"。",
            "param_docs": [
                {
                    "name": "strstr",
                    "header": "#include <string.h>",
                    "prototype": "char *strstr(const char *haystack, const char *needle);",
                    "purpose": "在字符串 haystack 中查找子串 needle 首次出现的位置。",
                    "params": [
                        {"name": "haystack", "type": "const char *", "desc": "被查找的主字符串。"},
                        {"name": "needle", "type": "const char *", "desc": "要查找的目标子串。"},
                    ],
                    "returns": "找到时返回子串首次出现的起始地址指针；未找到时返回 NULL。",
                    "notes": ["统计不重叠出现次数时，每次找到 p = strstr(p, pat) 后，将扫描起点推进 p += strlen(pat)。"],
                },
                {
                    "name": "strncpy",
                    "header": "#include <string.h>",
                    "prototype": "char *strncpy(char *dest, const char *src, size_t n);",
                    "purpose": "从 src 最多复制 n 个字符到 dest。",
                    "params": [
                        {"name": "dest", "type": "char *", "desc": "目标字符数组。"},
                        {"name": "src", "type": "const char *", "desc": "源字符串。"},
                        {"name": "n", "type": "size_t", "desc": "最多复制的字符数。"},
                    ],
                    "returns": "返回 dest。",
                    "notes": [
                        "当 src 的长度 >= n 时，strncpy 不会自动在 dest 末尾写入 '\\0'，必须手动添加 dest[n] = '\\0'。",
                        "sizeof(prefix) 是数组分配的总字节数（编译期常量 16）；strlen(prefix) 是 '\\0' 之前的实际字符数。",
                    ],
                },
            ],
            "template": """#include <stdio.h>
#include <string.h>

int main(void) {
    char text[256], pat[64], prefix[16];
    int k;
    scanf("%255s %63s %d", text, pat, &k);

    // <<<EDIT_START>>>
    // 1. 使用 strstr 循环统计 pat 在 text 中的不重叠出现次数 cnt 并输出
    // 2. 使用 strncpy 复制前 k 个字符到 prefix，手动补 '\\0'，输出 prefix、sizeof(prefix) 和 strlen(prefix)

    // <<<EDIT_END>>>
    return 0;
}
""",
            "solution": """#include <stdio.h>
#include <string.h>

int main(void) {
    char text[256], pat[64], prefix[16];
    int k;
    scanf("%255s %63s %d", text, pat, &k);

    int cnt = 0;
    size_t plen = strlen(pat);
    const char *p = text;
    if (plen > 0) {
        while ((p = strstr(p, pat)) != NULL) {
            cnt++;
            p += plen;
        }
    }
    printf("%d\\n", cnt);

    strncpy(prefix, text, (size_t)k);
    prefix[k] = '\\0';
    printf("%s %d %d\\n", prefix, (int)sizeof(prefix), (int)strlen(prefix));
    return 0;
}
""",
            "code_checks": [
                {"pattern": r"\bstrstr\s*\(", "message": "请使用 strstr 查找子串位置。"},
                {"pattern": r"\bstrncpy\s*\(", "message": "请使用 strncpy 按长度 k 复制前缀。"},
            ],
            "tests": [
                {"name": "多次不重叠匹配", "stdin": "abababa aba 4\n", "expected_stdout": "2\nabab 16 4\n"},
                {"name": "连续相同子串", "stdin": "aaaaa aa 3\n", "expected_stdout": "2\naaa 16 3\n"},
                {"name": "未匹配且k大于长度", "stdin": "hello xyz 5\n", "expected_stdout": "0\nhello 16 5\n"},
            ],
        },
        {
            "id": "m3_6",
            "code": "3.6",
            "title": "直接插入排序（Insertion Sort）",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握直接插入排序将当前元素插入前方有序区间的核心步骤与边界条件。",
            "goal": "补全 insertion_sort(int a[], int n)：使用直接插入排序将数组 a[0..n-1] 排为升序，并返回元素向后移动（或相邻交换）的总次数（本题按标准后移赋值 a[j+1] = a[j] 的次数统计）。",
            "param_docs": [
                {
                    "name": "直接插入排序",
                    "header": "基本数组排序算法（第 10 讲）",
                    "prototype": "for (i = 1; i < n; i++) { key = a[i]; j = i - 1; while (j >= 0 && a[j] > key) { ... } a[j+1] = key; }",
                    "purpose": "将数组前 i 个元素维护为有序区间，逐个取出 a[i] 插入到合适位置。",
                    "params": [
                        {"name": "key = a[i]", "type": "int", "desc": "暂存当前待插入元素，防止后续元素右移时被覆盖。"},
                        {"name": "j >= 0 && a[j] > key", "type": "循环条件", "desc": "必须把 j >= 0 写在 && 左侧，利用逻辑短路防止 j == -1 时越界访问 a[-1]。"},
                    ],
                    "returns": "返回内层循环中元素右移（a[j + 1] = a[j]）的总次数。",
                    "notes": ["对于已经有序的数组，直接插入排序的移动次数为 0，时间复杂度为 O(n)。"],
                }
            ],
            "template": """#include <stdio.h>

int insertion_sort(int a[], int n) {
    // 补全直接插入排序，将 a[0..n-1] 升序排列，并返回元素右移的总次数
// <<<EDIT_START>>>
    return 0;
// <<<EDIT_END>>>
}

int main(void) {
    int n, a[100];
    scanf("%d", &n);
    for (int i = 0; i < n; i++) scanf("%d", &a[i]);

    int shifts = insertion_sort(a, n);
    for (int i = 0; i < n; i++) {
        if (i > 0) putchar(' ');
        printf("%d", a[i]);
    }
    printf("\\n%d\\n", shifts);
    return 0;
}
""",
            "solution": """#include <stdio.h>

int insertion_sort(int a[], int n) {
    int shifts = 0;
    for (int i = 1; i < n; i++) {
        int key = a[i];
        int j = i - 1;
        while (j >= 0 && a[j] > key) {
            a[j + 1] = a[j];
            j--;
            shifts++;
        }
        a[j + 1] = key;
    }
    return shifts;
}

int main(void) {
    int n, a[100];
    scanf("%d", &n);
    for (int i = 0; i < n; i++) scanf("%d", &a[i]);

    int shifts = insertion_sort(a, n);
    for (int i = 0; i < n; i++) {
        if (i > 0) putchar(' ');
        printf("%d", a[i]);
    }
    printf("\\n%d\\n", shifts);
    return 0;
}
""",
            "tests": [
                {"name": "常规乱序", "stdin": "5\n6 3 5 2 4\n", "expected_stdout": "2 3 4 5 6\n7\n"},
                {"name": "已有序(0次移动)", "stdin": "4\n1 2 3 4\n", "expected_stdout": "1 2 3 4\n0\n"},
                {"name": "完全逆序", "stdin": "4\n9 7 5 1\n", "expected_stdout": "1 5 7 9\n6\n"},
            ],
        },
        {
            "id": "m3_stage_a",
            "code": "★ 阶段 3A",
            "title": "阶段综合：选择/冒泡排序与二分查找",
            "kind": "stage",
            "editable_default": "partial",
            "summary": "掌握第 5.5.2～5.5.3 节核心数组算法：升序排序与有序数组二分查找。",
            "goal": "输入整数 n（1 <= n <= 100）、n 个互不相同的整数，以及一个待查找的目标值 target。\n第一行输出升序排列后的数组（空格分隔）；第二行输出 target 在排序后数组中的下标（从 0 开始，找不到输出 -1）。",
            "guidance": [
                "1. 使用冒泡排序或选择排序将 a[0..n-1] 排为升序。",
                "2. 输出排序后的数组元素，注意相邻元素间输出一个空格，末尾换行。",
                "3. 使用二分查找：设置 left = 0, right = n - 1，循环条件为 left <= right，取 mid = left + (right - left) / 2 比较 a[mid] 与 target。",
            ],
            "sidebar_refs": {
                "functions": [
                    {"name": "scanf / printf", "sig": "stdio.h", "desc": "数组元素读入与格式化输出。"},
                ],
                "structures": [
                    {
                        "name": "选择排序框架（升序）",
                        "code": "for (int i = 0; i < n - 1; i++) {\n    int min_idx = i;\n    for (int j = i + 1; j < n; j++) {\n        if (a[j] < a[min_idx]) min_idx = j;\n    }\n    int tmp = a[i]; a[i] = a[min_idx]; a[min_idx] = tmp;\n}",
                    },
                    {
                        "name": "二分查找框架",
                        "code": "int l = 0, r = n - 1, pos = -1;\nwhile (l <= r) {\n    int mid = l + (r - l) / 2;\n    if (a[mid] == target) { pos = mid; break; }\n    else if (a[mid] < target) l = mid + 1;\n    else r = mid - 1;\n}",
                    },
                ],
            },
            "template": """#include <stdio.h>

int main(void) {
    int n, a[100], target;
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &a[i]);
    }
    scanf("%d", &target);

    // <<<EDIT_START>>>
    // 1. 对数组 a 进行升序排序并输出
    // 2. 对排序后的数组执行二分查找并输出 target 下标或 -1

    // <<<EDIT_END>>>
    return 0;
}
""",
            "solution": """#include <stdio.h>

int main(void) {
    int n, a[100], target;
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &a[i]);
    }
    scanf("%d", &target);

    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            if (a[j] < a[min_idx]) min_idx = j;
        }
        int tmp = a[i];
        a[i] = a[min_idx];
        a[min_idx] = tmp;
    }

    for (int i = 0; i < n; i++) {
        if (i > 0) putchar(' ');
        printf("%d", a[i]);
    }
    putchar('\\n');

    int l = 0, r = n - 1, pos = -1;
    while (l <= r) {
        int mid = l + (r - l) / 2;
        if (a[mid] == target) {
            pos = mid;
            break;
        } else if (a[mid] < target) {
            l = mid + 1;
        } else {
            r = mid - 1;
        }
    }
    printf("%d\\n", pos);
    return 0;
}
""",
            "tests": [
                {"name": "存在目标", "stdin": "6\n9 3 7 1 5 11\n7\n", "expected_stdout": "1 3 5 7 9 11\n3\n"},
                {"name": "目标不存在", "stdin": "5\n20 10 40 30 50\n25\n", "expected_stdout": "10 20 30 40 50\n-1\n"},
                {"name": "首尾边界", "stdin": "4\n8 -2 0 4\n-2\n", "expected_stdout": "-2 0 4 8\n0\n"},
            ],
        },
        {
            "id": "m3_stage_b",
            "code": "★ 阶段 3B",
            "title": "阶段综合：文本字符统计与回文判断",
            "kind": "stage",
            "editable_default": "full",
            "summary": "综合运用字符数组、fgets、strlen 及双下标扫描完成文本分析。",
            "goal": "读入一行字符串（长度不超过 200，不含中文）。\n第一行依次输出该行中：英文字母个数、数字字符个数、空格个数（用单个空格分隔，不含末尾换行符）。\n第二行：仅提取其中的英文字母（不区分大小写，统一转小写），判断提取后的字母序列是否为回文串，是则输出 PALINDROME，否则输出 NOT。",
            "guidance": [
                "1. 使用 fgets 读入整行，并先去掉末尾的 '\\n' 与 '\\r'。",
                "2. 遍历原字符串：分别统计字母（'A'~'Z' 或 'a'~'z'）、数字（'0'~'9'）和空格（' '）的数量；同时把字母统一转小写存入新数组 letters[]。",
                "3. 对 letters[0..m-1] 用双下标 l = 0, r = m - 1 向中间靠拢比较，判断是否为回文。",
            ],
            "sidebar_refs": {
                "functions": [
                    {"name": "fgets", "sig": "char *fgets(char *buf, int n, FILE *stream);", "desc": "安全读取整行，需剔除末尾 '\\n'。"},
                    {"name": "isalpha / isdigit / tolower", "sig": "#include <ctype.h>", "desc": "也可直接用范围比较 'a'<=c&&c<='z' 判断。"},
                ],
                "structures": [
                    {
                        "name": "去换行符标准写法",
                        "code": "int len = (int)strlen(s);\nwhile (len > 0 && (s[len-1] == '\\n' || s[len-1] == '\\r')) {\n    s[--len] = '\\0';\n}",
                    },
                    {
                        "name": "双下标回文检测",
                        "code": "int ok = 1;\nfor (int l = 0, r = m - 1; l < r; l++, r--) {\n    if (letters[l] != letters[r]) { ok = 0; break; }\n}",
                    },
                ],
            },
            "template": """#include <stdio.h>
#include <string.h>

int main(void) {
    // 请完整编写程序

    return 0;
}
""",
            "solution": """#include <stdio.h>
#include <string.h>

int main(void) {
    char s[256], letters[256];
    if (fgets(s, sizeof(s), stdin) == NULL) return 0;

    int len = (int)strlen(s);
    while (len > 0 && (s[len - 1] == '\\n' || s[len - 1] == '\\r')) {
        s[--len] = '\\0';
    }

    int cnt_alpha = 0, cnt_digit = 0, cnt_space = 0, m = 0;
    for (int i = 0; i < len; i++) {
        char c = s[i];
        if (c >= 'A' && c <= 'Z') {
            cnt_alpha++;
            letters[m++] = (char)(c - 'A' + 'a');
        } else if (c >= 'a' && c <= 'z') {
            cnt_alpha++;
            letters[m++] = c;
        } else if (c >= '0' && c <= '9') {
            cnt_digit++;
        } else if (c == ' ') {
            cnt_space++;
        }
    }

    int is_pal = 1;
    for (int l = 0, r = m - 1; l < r; l++, r--) {
        if (letters[l] != letters[r]) {
            is_pal = 0;
            break;
        }
    }

    printf("%d %d %d\\n", cnt_alpha, cnt_digit, cnt_space);
    printf("%s\\n", is_pal ? "PALINDROME" : "NOT");
    return 0;
}
""",
            "tests": [
                {"name": "忽略大小写与数字的回文", "stdin": "A man 123 a plan a canal Panama\n", "expected_stdout": "21 3 7\nPALINDROME\n"},
                {"name": "非回文句子", "stdin": "Hello C 2026!\n", "expected_stdout": "6 4 2\nNOT\n"},
                {"name": "单字母", "stdin": "7 Z 9\n", "expected_stdout": "1 2 2\nPALINDROME\n"},
            ],
        },
    ],
}
