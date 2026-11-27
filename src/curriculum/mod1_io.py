"""Module 1: Program structure, data types, operators, and basic I/O (Chapters 2-3)."""

MODULE = {
    "id": "m1",
    "title": "模块 1：程序结构与基本输入输出",
    "subtitle": "对应第 2～3 章",
    "badge": "基础筑基",
    "exercises": [
        {
            "id": "m1_1",
            "code": "1.1",
            "title": "printf 格式化输出",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握 printf 的格式控制符、小数精度指定及转义换行。",
            "goal": "补全 printf 调用，按格式输出学号（整数）、成绩（保留 2 位小数）与等级（单个字符）：ID=1024 Score=93.50 Grade=A，并在末尾换行。",
            "param_docs": [
                {
                    "name": "printf",
                    "header": "#include <stdio.h>",
                    "prototype": "int printf(const char *format, ...);",
                    "purpose": "按 format 字符串中指定的格式，将后续参数的值依次输出到标准输出。",
                    "params": [
                        {
                            "name": "format",
                            "type": "const char *",
                            "desc": "格式控制字符串。普通字符原样输出；以 % 开头的格式说明符（如 %d、%.2f、%c）会被对应参数的值替换。",
                        },
                        {
                            "name": "...",
                            "type": "可变参数",
                            "desc": "待输出的变量或表达式，其顺序和类型必须与 format 中的格式说明符一一匹配。",
                        },
                    ],
                    "returns": "成功时返回实际输出的字符数；出错时返回负值。",
                    "notes": [
                        "%d 输出 int，%f 输出 float/double，%.2f 表示保留 2 位小数，%c 输出 char，%o/%x 分别输出八进制/十六进制。",
                        "附加标志：- 左对齐，+ 强制输出正负号，0 不足最小宽度时前导补零（如 %+7d、%+06d、%-7d）。",
                        "\\n 表示换行符；若需输出字符 % 本身，需写作 %%。",
                    ],
                }
            ],
            "template": """#include <stdio.h>

int main(void) {
    int id = 1024;
    double score = 93.5;
    char grade = 'A';

    // <<<EDIT_START>>>
    // 请使用 printf 输出: ID=1024 Score=93.50 Grade=A（末尾换行）

    // <<<EDIT_END>>>
    return 0;
}
""",
            "solution": """#include <stdio.h>

int main(void) {
    int id = 1024;
    double score = 93.5;
    char grade = 'A';

    printf("ID=%d Score=%.2f Grade=%c\\n", id, score, grade);
    return 0;
}
""",
            "code_checks": [
                {"pattern": r"\bprintf\s*\(", "message": "请调用 printf 函数完成输出。"},
                {"pattern": r"%d", "message": "请使用 %d 输出变量 id，不要直接写死数字。"},
                {"pattern": r"%\.2f", "message": "请使用 %.2f 控制浮点数的两位小数精度。"},
            ],
            "tests": [
                {
                    "name": "基本格式化输出",
                    "stdin": "",
                    "expected_stdout": "ID=1024 Score=93.50 Grade=A\n",
                }
            ],
        },
        {
            "id": "m1_2",
            "code": "1.2",
            "title": "scanf 格式化输入",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握 scanf 的格式说明符及变量地址传递规则。",
            "goal": "从标准输入依次读入两个整数 a、b 和一个双精度浮点数 x（以空格分隔），补全 scanf 语句，程序将输出它们的和。",
            "param_docs": [
                {
                    "name": "scanf",
                    "header": "#include <stdio.h>",
                    "prototype": "int scanf(const char *format, ...);",
                    "purpose": "从标准输入按 format 指定的格式读取数据，并存入后续参数指向的内存地址中。",
                    "params": [
                        {
                            "name": "format",
                            "type": "const char *",
                            "desc": "输入格式字符串。读取 int 用 %d，读取 float 用 %f，读取 double 必须用 %lf，读取 char 用 %c。",
                        },
                        {
                            "name": "...",
                            "type": "变量地址列表",
                            "desc": "接收数据的变量地址。对于基本类型变量，必须在变量名前加取地址运算符 &（如 &a）。",
                        },
                    ],
                    "returns": "成功读入并赋值的输入项个数；遇输入结束返回 EOF（通常为 -1）。",
                    "notes": [
                        "scanf 读取 double 必须写 %lf，不能写 %f；printf 输出 double 用 %f 或 %lf 均可。",
                        "格式字符串末尾不要随意添加 \\n，否则会等待额外非空白字符输入。",
                    ],
                }
            ],
            "template": """#include <stdio.h>

int main(void) {
    int a = 0, b = 0;
    double x = 0.0;

    // <<<EDIT_START>>>
    // 请使用 scanf 依次读取 a, b, x

    // <<<EDIT_END>>>

    printf("%d %.2f\\n", a + b, x * 2.0);
    return 0;
}
""",
            "solution": """#include <stdio.h>

int main(void) {
    int a = 0, b = 0;
    double x = 0.0;

    scanf("%d %d %lf", &a, &b, &x);

    printf("%d %.2f\\n", a + b, x * 2.0);
    return 0;
}
""",
            "code_checks": [
                {"pattern": r"\bscanf\s*\(", "message": "请调用 scanf 函数读取输入。"},
            ],
            "tests": [
                {"name": "正数输入", "stdin": "12 8 3.25\n", "expected_stdout": "20 6.50\n"},
                {"name": "负数与零", "stdin": "-5 5 0.5\n", "expected_stdout": "0 1.00\n"},
                {"name": "较大数值", "stdin": "100 250 12.345\n", "expected_stdout": "350 24.69\n"},
            ],
        },
        {
            "id": "m1_3",
            "code": "1.3",
            "title": "getchar 与 putchar 单字符读写",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握单字符读取、ASCII 大小写转换与单字符输出。",
            "goal": "使用 getchar 读入一个小写字母，将其转换为对应的大写字母（减去 32 或 'a' - 'A'），再使用 putchar 输出该大写字母和换行符。",
            "param_docs": [
                {
                    "name": "getchar",
                    "header": "#include <stdio.h>",
                    "prototype": "int getchar(void);",
                    "purpose": "从标准输入读取下一个字符。",
                    "params": [
                        {
                            "name": "void",
                            "type": "无参数",
                            "desc": "不接收任何参数，调用时写作 getchar()。",
                        }
                    ],
                    "returns": "成功时返回读入字符的 unsigned char 值（转换为 int）；到达输入末尾或出错时返回 EOF。",
                    "notes": [
                        "建议用 int 变量接收返回值，以便正确与 EOF 比较。",
                    ],
                },
                {
                    "name": "putchar",
                    "header": "#include <stdio.h>",
                    "prototype": "int putchar(int ch);",
                    "purpose": "向标准输出写入单个字符。",
                    "params": [
                        {
                            "name": "ch",
                            "type": "int",
                            "desc": "要输出的字符（或其对应的 ASCII 整数值），例如 'A' 或 '\\n'。",
                        }
                    ],
                    "returns": "成功时返回写入的字符；出错时返回 EOF。",
                    "notes": [
                        "ASCII 码中小写字母 'a'~'z' 连续排列（97~122），大写字母 'A'~'Z' 连续排列（65~90），差值为 32。",
                    ],
                },
            ],
            "template": """#include <stdio.h>

int main(void) {
    int ch;

    // <<<EDIT_START>>>
    // 1. 使用 getchar() 读取一个字符到 ch
    // 2. 将小写字母转换为大写字母
    // 3. 使用 putchar() 输出转换后的字符和 '\\n'

    // <<<EDIT_END>>>
    return 0;
}
""",
            "solution": """#include <stdio.h>

int main(void) {
    int ch;

    ch = getchar();
    ch = ch - ('a' - 'A');
    putchar(ch);
    putchar('\\n');
    return 0;
}
""",
            "code_checks": [
                {"pattern": r"\bgetchar\s*\(", "message": "请使用 getchar() 读取字符。"},
                {"pattern": r"\bputchar\s*\(", "message": "请使用 putchar() 输出字符。"},
            ],
            "tests": [
                {"name": "字母 a", "stdin": "a\n", "expected_stdout": "A\n"},
                {"name": "字母 m", "stdin": "m\n", "expected_stdout": "M\n"},
                {"name": "字母 z", "stdin": "z\n", "expected_stdout": "Z\n"},
            ],
        },
        {
            "id": "m1_4",
            "code": "1.4",
            "title": "类型转换、整除取模与条件运算符",
            "kind": "single",
            "editable_default": "partial",
            "summary": "区分整数除法与浮点除法，掌握显式类型转换与三目条件表达式。",
            "goal": "给定两个正整数 a 和 b：计算整数商 q、余数 r、浮点商 ratio（保留 2 位小数），以及两者中的较大值 mx（使用条件运算符 ?:）。",
            "param_docs": [
                {
                    "name": "显式类型转换与运算符",
                    "header": "C 语言内置运算符",
                    "prototype": "(类型名)表达式  |  条件 ? 表达式1 : 表达式2",
                    "purpose": "控制运算中的数据类型转换，避免整数除法截断小数；使用条件运算符在一行内按条件取值。",
                    "params": [
                        {
                            "name": "(double)a / b",
                            "type": "显式类型转换",
                            "desc": "先将 a 转为 double，再与 b 做浮点除法；注意 (double)(a / b) 会先做整除再转浮点，结果会丢失小数部分。",
                        },
                        {
                            "name": "a / b 与 a % b",
                            "type": "整除与取模",
                            "desc": "当操作数均为整数时，/ 舍去小数取商，% 取余数（% 只能用于整数类型）。",
                        },
                        {
                            "name": "cond ? v1 : v2",
                            "type": "条件运算符",
                            "desc": "若 cond 为非 0（真），整个表达式取 v1 的值；否则取 v2 的值。",
                        },
                    ],
                    "returns": "表达式求值结果为对应类型的值。",
                    "notes": [
                        "强制类型转换运算符 (double) 的优先级高于除法 /。",
                    ],
                }
            ],
            "template": """#include <stdio.h>

int main(void) {
    int a, b;
    scanf("%d %d", &a, &b);

    int q = 0, r = 0, mx = 0;
    double ratio = 0.0;

    // <<<EDIT_START>>>
    // 计算 q(整除商), r(余数), ratio(浮点商), mx(较大值)

    // <<<EDIT_END>>>

    printf("%d %d %.2f %d\\n", q, r, ratio, mx);
    return 0;
}
""",
            "solution": """#include <stdio.h>

int main(void) {
    int a, b;
    scanf("%d %d", &a, &b);

    int q = 0, r = 0, mx = 0;
    double ratio = 0.0;

    q = a / b;
    r = a % b;
    ratio = (double)a / b;
    mx = (a > b) ? a : b;

    printf("%d %d %.2f %d\\n", q, r, ratio, mx);
    return 0;
}
""",
            "tests": [
                {"name": "常规整除与余数", "stdin": "7 2\n", "expected_stdout": "3 1 3.50 7\n"},
                {"name": "被除数小于除数", "stdin": "3 4\n", "expected_stdout": "0 3 0.75 4\n"},
                {"name": "刚好整除", "stdin": "15 5\n", "expected_stdout": "3 0 3.00 15\n"},
            ],
        },
        {
            "id": "m1_5",
            "code": "1.5",
            "title": "printf 进阶格式控制与 sizeof 运算符",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握 printf 的宽度、正负号、补零、对齐及进制格式符，以及 sizeof 运算符的求值规则。",
            "goal": "输入两个整数 a 和 b。第一行用 printf 按指定格式输出：[宽度为 6 且强制带正负号的 a] 空格 [宽度为 6、强制带正负号且前导补零的 b] 空格 [带 0x 前缀的小写十六进制 (unsigned)a]；第二行用 sizeof 输出数组 arr 的总字节数、字符串常量 \"C99\" 的字节数，以及执行 sizeof(x = 99) 后的字节数与变量 x 的值。",
            "param_docs": [
                {
                    "name": "printf 宽度与标志修饰符",
                    "header": "#include <stdio.h>",
                    "prototype": "%[标志][最小宽度][.精度]转换字符",
                    "purpose": "精确控制数值输出时的占位宽度、对齐方向、符号显示、前导零与进制前缀。",
                    "params": [
                        {"name": "+ / - / 0 / #", "type": "标志字符", "desc": "+ 使正数也输出正号；- 使输出左对齐（默认右对齐）；0 在宽度不足时用前导 0 填充；# 为 %x/%o 输出 0x/0 前缀。"},
                        {"name": "最小宽度 m", "type": "正整数", "desc": "指定最少占用的字符列数，例如 %+6d 对 12 输出 '   +12'，%+06d 对 -12 输出 '-00012'。"},
                    ],
                    "returns": "返回实际输出的字符数。",
                    "notes": ["输出无符号十六进制时，建议将整数转为 unsigned int 后配合 %#x（或 %x）输出。"],
                },
                {
                    "name": "sizeof 运算符",
                    "header": "C 语言单目运算符",
                    "prototype": "sizeof(类型名)  |  sizeof 表达式",
                    "purpose": "在编译期计算数据类型或表达式结果所占用的内存字节数，返回 size_t 类型。",
                    "params": [
                        {"name": "类型名 / 表达式", "type": "操作数", "desc": "对类型名必须加括号如 sizeof(double)；对数组名返回整个数组字节数；对字符串常量包含末尾 '\\0'。"},
                    ],
                    "returns": "无符号整数（size_t，输出时可用 %zu 或强制转为 (int) 后用 %d）。",
                    "notes": [
                        "sizeof 的操作数表达式在运行期不会被求值，因此 sizeof(x = 99) 不会改变变量 x 的值。",
                        "在标准 C 中，字符常量（如 'A'）的类型为 int，其 sizeof 结果与 sizeof(int) 相同。",
                    ],
                },
            ],
            "template": """#include <stdio.h>

int main(void) {
    int a, b;
    scanf("%d %d", &a, &b);

    float arr[7];
    short x = 1;

    // <<<EDIT_START>>>
    // 第一行：用 printf 按 "[%+6d] [%+06d] [%#x]\\n" 格式输出 a, b, (unsigned int)a
    // 第二行：用 printf 输出 sizeof(arr)、sizeof("C99")、sizeof(x = 99) 以及 x（均转 int 用 %d，空格分隔）

    // <<<EDIT_END>>>
    return 0;
}
""",
            "solution": """#include <stdio.h>

int main(void) {
    int a, b;
    scanf("%d %d", &a, &b);

    float arr[7];
    short x = 1;

    printf("[%+6d] [%+06d] [%#x]\\n", a, b, (unsigned int)a);
    printf("%d %d %d %d\\n", (int)sizeof(arr), (int)sizeof("C99"), (int)sizeof(x = 99), (int)x);
    return 0;
}
""",
            "code_checks": [
                {"pattern": r"\bprintf\s*\(", "message": "请使用 printf 输出格式化结果。"},
                {"pattern": r"\bsizeof\b", "message": "请使用 sizeof 运算符计算字节数，不要写死数字。"},
            ],
            "tests": [
                {"name": "正数与负数", "stdin": "26 -13\n", "expected_stdout": "[   +26] [-00013] [0x1a]\n28 4 2 1\n"},
                {"name": "较大正数", "stdin": "255 7\n", "expected_stdout": "[  +255] [+00007] [0xff]\n28 4 2 1\n"},
            ],
        },
        {
            "id": "m1_stage",
            "code": "★ 阶段 1",
            "title": "阶段综合：时间换算与平均速度",
            "kind": "stage",
            "editable_default": "partial",
            "summary": "综合运用 scanf、printf、整除取模与显式浮点转换完成多步计算。",
            "goal": "输入总秒数 total_sec（非负整数）与行驶总距离 dist（单位：千米，double）。请将 total_sec 拆分为 时:分:秒（两位补零格式，如 01:05:09），并在第二行输出平均速度（单位：km/h，保留 2 位小数）。",
            "guidance": [
                "1. 使用 scanf 按 \"%d %lf\" 读入 total_sec 和 dist。",
                "2. 计算小时 h = total_sec / 3600，分钟 m = (total_sec % 3600) / 60，秒 s = total_sec % 60。",
                "3. 用 %02d 输出两位宽、不足左补 0 的整数，格式为 \"%02d:%02d:%02d\\n\"。",
                "4. 将总秒数转为小时数 (double)total_sec / 3600.0，再计算速度 speed = dist / hours，按 \"%.2f\\n\" 输出。",
            ],
            "sidebar_refs": {
                "functions": [
                    {
                        "name": "scanf",
                        "sig": "int scanf(const char *format, ...);",
                        "desc": "读取 int 用 %d，读取 double 用 %lf，变量前加 &。",
                    },
                    {
                        "name": "printf",
                        "sig": "int printf(const char *format, ...);",
                        "desc": "%02d 表示至少占 2 位、不足左边补 0；%.2f 保留 2 位小数。",
                    },
                ],
                "structures": [
                    {
                        "name": "标准 C 主程序框架",
                        "code": "#include <stdio.h>\nint main(void) {\n    // 定义变量 -> 输入 -> 计算 -> 输出\n    return 0;\n}",
                    },
                    {
                        "name": "避免整数截断的浮点计算",
                        "code": "double hours = (double)total_sec / 3600.0;",
                    },
                ],
            },
            "template": """#include <stdio.h>

int main(void) {
    // <<<EDIT_START>>>
    // 请定义变量、读入 total_sec 和 dist，完成换算并按两行格式输出

    // <<<EDIT_END>>>
    return 0;
}
""",
            "solution": """#include <stdio.h>

int main(void) {
    int total_sec;
    double dist;
    scanf("%d %lf", &total_sec, &dist);

    int h = total_sec / 3600;
    int m = (total_sec % 3600) / 60;
    int s = total_sec % 60;
    double hours = (double)total_sec / 3600.0;
    double speed = dist / hours;

    printf("%02d:%02d:%02d\\n", h, m, s);
    printf("%.2f\\n", speed);
    return 0;
}
""",
            "tests": [
                {
                    "name": "标准测试",
                    "stdin": "3665 100.0\n",
                    "expected_stdout": "01:01:05\n98.23\n",
                },
                {
                    "name": "不足一小时",
                    "stdin": "1800 45.0\n",
                    "expected_stdout": "00:30:00\n90.00\n",
                },
                {
                    "name": "跨两小时",
                    "stdin": "7200 150.5\n",
                    "expected_stdout": "02:00:00\n75.25\n",
                },
            ],
        },
    ],
}
