"""Module 2: Control structures and basic algorithms (Chapter 4)."""

MODULE = {
    "id": "m2",
    "title": "模块 2：控制结构与基本算法",
    "subtitle": "对应第 4 章",
    "badge": "逻辑控制",
    "exercises": [
        {
            "id": "m2_1",
            "code": "2.1",
            "title": "if / else if / else 多分支结构",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握关系表达式、逻辑运算符 && / || 以及多分支顺序判断。",
            "goal": "读入一个年份 year，判断是否为闰年（能被 400 整除，或者能被 4 整除且不能被 100 整除）。是闰年输出 YES，否则输出 NO。",
            "param_docs": [
                {
                    "name": "if / else if / else",
                    "header": "C 语言控制语句",
                    "prototype": "if (条件1) { 语句1; } else if (条件2) { 语句2; } else { 语句3; }",
                    "purpose": "按顺序检验条件表达式的真假（非 0 为真，0 为假），仅执行第一个满足条件的分支。",
                    "params": [
                        {
                            "name": "条件表达式",
                            "type": "标量表达式",
                            "desc": "常由关系运算符（==、!=、<、<=、>、>=）和逻辑运算符（&& 且、|| 或、! 非）组合而成。",
                        },
                        {
                            "name": "分支语句块",
                            "type": "复合语句 { ... }",
                            "desc": "条件成立时执行的语句集合；建议始终加上花括号 {}，避免悬空 else 错误。",
                        },
                    ],
                    "returns": "语句本身无返回值。",
                    "notes": [
                        "判断相等使用 ==，不能写成赋值运算符 =。",
                        "&& 的优先级高于 ||，复杂条件建议使用括号明确分组。",
                    ],
                }
            ],
            "template": """#include <stdio.h>

int main(void) {
    int year;
    scanf("%d", &year);

    // <<<EDIT_START>>>
    // 判断闰年并输出 YES 或 NO（带换行）

    // <<<EDIT_END>>>
    return 0;
}
""",
            "solution": """#include <stdio.h>

int main(void) {
    int year;
    scanf("%d", &year);

    if ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0)) {
        printf("YES\\n");
    } else {
        printf("NO\\n");
    }
    return 0;
}
""",
            "tests": [
                {"name": "普通闰年 2024", "stdin": "2024\n", "expected_stdout": "YES\n"},
                {"name": "整百年非闰年 1900", "stdin": "1900\n", "expected_stdout": "NO\n"},
                {"name": "四百年闰年 2000", "stdin": "2000\n", "expected_stdout": "YES\n"},
                {"name": "普通平年 2023", "stdin": "2023\n", "expected_stdout": "NO\n"},
            ],
        },
        {
            "id": "m2_2",
            "code": "2.2",
            "title": "switch / case 多路分支",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握 switch 整型/字符表达式匹配、break 防止贯穿与 default 分支。",
            "goal": "读入两个整数 a、b 和一个运算符字符 op（+、-、*、/），使用 switch 计算并输出整数结果；若为其他字符则输出 ERR。",
            "param_docs": [
                {
                    "name": "switch / case",
                    "header": "C 语言控制语句",
                    "prototype": "switch (整型表达式) { case 常量1: 语句; break; default: 语句; }",
                    "purpose": "根据整型或字符表达式的值，直接跳转到对应的 case 常量标签执行。",
                    "params": [
                        {
                            "name": "整型表达式",
                            "type": "int / char / enum 等整型",
                            "desc": "只能是整型或字符类型，不能是 float、double 或字符串。",
                        },
                        {
                            "name": "case 常量",
                            "type": "整型常量表达式",
                            "desc": "每个 case 后的值必须是编译期常量（如 '+', '-', 1）且互不相同。",
                        },
                        {
                            "name": "break",
                            "type": "跳转语句",
                            "desc": "结束当前 switch；省略 break 会继续顺序执行下一个 case 的语句。",
                        },
                    ],
                    "returns": "语句本身无返回值。",
                    "notes": [
                        "用 scanf(\"%d %c %d\", &a, &op, &b) 时，%c 前面的空格会跳过空白字符。",
                    ],
                }
            ],
            "template": """#include <stdio.h>

int main(void) {
    int a, b;
    char op;
    scanf("%d %c %d", &a, &op, &b);

    // <<<EDIT_START>>>
    // 使用 switch(op) 处理 '+', '-', '*', '/' 与 default

    // <<<EDIT_END>>>
    return 0;
}
""",
            "solution": """#include <stdio.h>

int main(void) {
    int a, b;
    char op;
    scanf("%d %c %d", &a, &op, &b);

    switch (op) {
        case '+':
            printf("%d\\n", a + b);
            break;
        case '-':
            printf("%d\\n", a - b);
            break;
        case '*':
            printf("%d\\n", a * b);
            break;
        case '/':
            printf("%d\\n", a / b);
            break;
        default:
            printf("ERR\\n");
            break;
    }
    return 0;
}
""",
            "code_checks": [
                {"pattern": r"\bswitch\s*\(", "message": "请使用 switch 语句实现多路分支。"},
            ],
            "tests": [
                {"name": "加法", "stdin": "10 + 5\n", "expected_stdout": "15\n"},
                {"name": "乘法", "stdin": "6 * 7\n", "expected_stdout": "42\n"},
                {"name": "整除", "stdin": "17 / 3\n", "expected_stdout": "5\n"},
                {"name": "非法运算符", "stdin": "5 ? 2\n", "expected_stdout": "ERR\n"},
            ],
        },
        {
            "id": "m2_3",
            "code": "2.3",
            "title": "for / while 循环与嵌套循环",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握循环初始化、条件控制、步进更新及双层循环控制行列输出。",
            "goal": "读入正整数 n，输出 n 行的数字直角三角形：第 i 行（从 1 开始）依次输出 1 到 i，数字之间用单个空格分隔，每行末尾无多余空格。",
            "param_docs": [
                {
                    "name": "for 循环",
                    "header": "C 语言控制语句",
                    "prototype": "for (初始化; 循环条件; 更新表达式) { 循环体; }",
                    "purpose": "在已知循环次数或明确计数器变化规律时，重复执行循环体。",
                    "params": [
                        {
                            "name": "初始化",
                            "type": "表达式或声明",
                            "desc": "循环开始前仅执行一次，常用于设置循环变量初值（如 int i = 1）。",
                        },
                        {
                            "name": "循环条件",
                            "type": "条件表达式",
                            "desc": "每轮进入循环体前检测；为真则执行循环体，为假则结束循环。",
                        },
                        {
                            "name": "更新表达式",
                            "type": "表达式",
                            "desc": "每轮循环体执行完毕后执行，常用于递增或递减循环变量（如 i++）。",
                        },
                    ],
                    "returns": "语句本身无返回值。",
                    "notes": [
                        "双层嵌套循环中，外层循环通常控制行号 i，内层循环控制当前行的列号 j。",
                    ],
                }
            ],
            "template": """#include <stdio.h>

int main(void) {
    int n;
    scanf("%d", &n);

    // <<<EDIT_START>>>
    // 使用嵌套循环输出 n 行数字三角形

    // <<<EDIT_END>>>
    return 0;
}
""",
            "solution": """#include <stdio.h>

int main(void) {
    int n;
    scanf("%d", &n);

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= i; j++) {
            if (j > 1) {
                putchar(' ');
            }
            printf("%d", j);
        }
        putchar('\\n');
    }
    return 0;
}
""",
            "tests": [
                {"name": "n=1", "stdin": "1\n", "expected_stdout": "1\n"},
                {"name": "n=4", "stdin": "4\n", "expected_stdout": "1\n1 2\n1 2 3\n1 2 3 4\n"},
            ],
        },
        {
            "id": "m2_4",
            "code": "2.4",
            "title": "break 与 continue 流程控制",
            "kind": "single",
            "editable_default": "partial",
            "summary": "区分 break（结束当前循环）与 continue（跳过本轮剩余语句）。",
            "goal": "循环读取一串整数：遇到负数时跳过不累加（continue）；遇到 0 时立即停止读取（break）；最后输出所有正数的总和。",
            "param_docs": [
                {
                    "name": "break 与 continue",
                    "header": "C 语言控制转向语句",
                    "prototype": "break;  |  continue;",
                    "purpose": "在循环执行过程中提前结束整个循环，或跳过当前轮次的剩余语句。",
                    "params": [
                        {
                            "name": "break",
                            "type": "关键字",
                            "desc": "立即跳出离它最近的一层循环（for/while/do-while）或 switch 语句。",
                        },
                        {
                            "name": "continue",
                            "type": "关键字",
                            "desc": "结束当前这一轮循环体的剩余代码，直接进入下一轮循环的条件判断（在 for 循环中会先执行更新表达式）。",
                        },
                    ],
                    "returns": "无返回值。",
                    "notes": [
                        "在 while(1) 中配合 scanf 返回值与 break 是处理不定长输入的常用固定结构。",
                    ],
                }
            ],
            "template": """#include <stdio.h>

int main(void) {
    int x, sum = 0;

    while (scanf("%d", &x) == 1) {
        // <<<EDIT_START>>>
        // 若 x == 0 则跳出循环；若 x < 0 则跳过本轮；否则累加到 sum

        // <<<EDIT_END>>>
    }

    printf("%d\\n", sum);
    return 0;
}
""",
            "solution": """#include <stdio.h>

int main(void) {
    int x, sum = 0;

    while (scanf("%d", &x) == 1) {
        if (x == 0) {
            break;
        }
        if (x < 0) {
            continue;
        }
        sum += x;
    }

    printf("%d\\n", sum);
    return 0;
}
""",
            "code_checks": [
                {"pattern": r"\bbreak\s*;", "message": "请使用 break 在遇到 0 时终止循环。"},
                {"pattern": r"\bcontinue\s*;", "message": "请使用 continue 跳过负数。"},
            ],
            "tests": [
                {"name": "含正负数与0哨兵", "stdin": "3 -2 5 -10 7 0 99\n", "expected_stdout": "15\n"},
                {"name": "立即遇0", "stdin": "0 10 20\n", "expected_stdout": "0\n"},
                {"name": "全是负数后遇0", "stdin": "-1 -2 -3 0\n", "expected_stdout": "0\n"},
            ],
        },
        {
            "id": "m2_5",
            "code": "2.5",
            "title": "while 辗转相除法求最大公约数与最小公倍数",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握 while 条件循环与欧几里得辗转相除法，以及先除后乘避免整数溢出的技巧。",
            "goal": "读入两个正整数 a 和 b（若任一数 <= 0 则输出 ERROR 并结束）。使用 while 循环通过辗转相除法求出最大公约数 gcd 和最小公倍数 lcm，按 \"gcd=值 lcm=值\" 格式输出。",
            "param_docs": [
                {
                    "name": "while 与 do-while 循环",
                    "header": "C 语言循环控制语句",
                    "prototype": "while (条件) { 循环体; }  |  do { 循环体; } while (条件);",
                    "purpose": "在循环次数预先未知、仅依赖结束条件时重复执行代码块。",
                    "params": [
                        {"name": "条件", "type": "整型表达式", "desc": "非 0 为真继续循环，0 为假终止循环。"},
                        {"name": "do-while 分号", "type": "语法要求", "desc": "do-while 先执行一次循环体再判断条件，末尾的 while(条件); 必须带分号。"},
                    ],
                    "returns": "无返回值。",
                    "notes": [
                        "辗转相除法步骤：当 y != 0 时反复执行 t = x % y; x = y; y = t;，循环结束时 x 即为最大公约数。",
                        "计算最小公倍数时应写作 (long long)(a / gcd) * b，先除后乘可防止 a * b 发生 int 溢出。",
                    ],
                }
            ],
            "template": """#include <stdio.h>

int main(void) {
    int a, b;
    scanf("%d %d", &a, &b);

    // <<<EDIT_START>>>
    // 1. 若 a <= 0 或 b <= 0，输出 ERROR 并换行后 return 0
    // 2. 用 while 辗转相除法求 gcd，再计算 lcm（注意防溢出）
    // 3. 输出: gcd=%d lcm=%lld\\n

    // <<<EDIT_END>>>
    return 0;
}
""",
            "solution": """#include <stdio.h>

int main(void) {
    int a, b;
    scanf("%d %d", &a, &b);

    if (a <= 0 || b <= 0) {
        printf("ERROR\\n");
        return 0;
    }

    int x = a, y = b;
    while (y != 0) {
        int t = x % y;
        x = y;
        y = t;
    }
    int gcd = x;
    long long lcm = (long long)(a / gcd) * b;
    printf("gcd=%d lcm=%lld\\n", gcd, lcm);
    return 0;
}
""",
            "code_checks": [
                {"pattern": r"\bwhile\s*\(", "message": "请使用 while 循环实现辗转相除法。"},
            ],
            "tests": [
                {"name": "常规正整数", "stdin": "24 36\n", "expected_stdout": "gcd=12 lcm=72\n"},
                {"name": "互质数", "stdin": "17 13\n", "expected_stdout": "gcd=1 lcm=221\n"},
                {"name": "非法输入", "stdin": "0 12\n", "expected_stdout": "ERROR\n"},
                {"name": "较大数防溢出", "stdin": "100000 150000\n", "expected_stdout": "gcd=50000 lcm=300000\n"},
            ],
        },
        {
            "id": "m2_6",
            "code": "2.6",
            "title": "<math.h> 数学函数与一元二次方程求根",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握 math.h 中 sqrt 与 fabs 的用法，并用浮点容差 fabs(disc) <= 1e-6 处理实根与共轭复根。",
            "goal": "输入一元二次方程 ax^2 + bx + c = 0 的三个系数 a, b, c（double，保证 a > 0）。计算判别式 disc = b*b - 4*a*c：\n- 若 fabs(disc) <= 1e-6，输出 \"x1=x2=%.2f\\n\"（注意当结果绝对值小于 1e-7 时按 0.00 输出避免 -0.00）；\n- 若 disc > 1e-6，输出两不等实根 \"x1=%.2f x2=%.2f\\n\"（其中 x1 = (-b + sqrt(disc))/(2*a)，x2 = (-b - sqrt(disc))/(2*a)）；\n- 若 disc < -1e-6，输出共轭复根 \"x1=%.2f+%.2fi x2=%.2f-%.2fi\\n\"（实部 p = -b/(2*a)，虚部 q = sqrt(fabs(disc))/(2*a)）。",
            "param_docs": [
                {
                    "name": "sqrt / fabs / pow",
                    "header": "#include <math.h>",
                    "prototype": "double sqrt(double x);\ndouble fabs(double x);\ndouble pow(double base, double exp);",
                    "purpose": "执行实数的平方根、浮点数绝对值与幂次运算。",
                    "params": [
                        {"name": "x", "type": "double", "desc": "sqrt 要求 x >= 0，传入负数会产生 NaN；fabs 返回 double 的非负绝对值（不要对浮点数使用整型 abs）。"},
                        {"name": "base, exp", "type": "double", "desc": "pow 计算 base 的 exp 次方；平方等简单整数次幂直接写 b * b 更高效且精确。"},
                    ],
                    "returns": "返回 double 类型的计算结果。",
                    "notes": [
                        "浮点数存在二进制表示误差，判断浮点数是否等于 0 应使用 fabs(x) <= 1e-6，而非 x == 0。",
                        "若计算出的 -0.0 在 %.2f 下可能显示为 -0.00，可在输出前判断 if (fabs(v) < 1e-7) v = 0.0;。",
                    ],
                }
            ],
            "template": """#include <stdio.h>
#include <math.h>

int main(void) {
    double a, b, c;
    scanf("%lf %lf %lf", &a, &b, &c);

    // <<<EDIT_START>>>
    // 计算 disc = b * b - 4 * a * c，使用 fabs 与 sqrt 分三种情况求解并按要求格式输出

    // <<<EDIT_END>>>
    return 0;
}
""",
            "solution": """#include <stdio.h>
#include <math.h>

int main(void) {
    double a, b, c;
    scanf("%lf %lf %lf", &a, &b, &c);

    double disc = b * b - 4.0 * a * c;
    if (fabs(disc) <= 1e-6) {
        double x = -b / (2.0 * a);
        if (fabs(x) < 1e-7) x = 0.0;
        printf("x1=x2=%.2f\\n", x);
    } else if (disc > 1e-6) {
        double x1 = (-b + sqrt(disc)) / (2.0 * a);
        double x2 = (-b - sqrt(disc)) / (2.0 * a);
        if (fabs(x1) < 1e-7) x1 = 0.0;
        if (fabs(x2) < 1e-7) x2 = 0.0;
        printf("x1=%.2f x2=%.2f\\n", x1, x2);
    } else {
        double p = -b / (2.0 * a);
        double q = sqrt(fabs(disc)) / (2.0 * a);
        if (fabs(p) < 1e-7) p = 0.0;
        printf("x1=%.2f+%.2fi x2=%.2f-%.2fi\\n", p, q, p, q);
    }
    return 0;
}
""",
            "code_checks": [
                {"pattern": r"\bsqrt\s*\(", "message": "请使用 math.h 中的 sqrt 函数求平方根。"},
                {"pattern": r"\bfabs\s*\(", "message": "请使用 math.h 中的 fabs 函数进行浮点绝对值与判零处理。"},
            ],
            "tests": [
                {"name": "两不等实根", "stdin": "1 -5 6\n", "expected_stdout": "x1=3.00 x2=2.00\n"},
                {"name": "两相等实根", "stdin": "1 -4 4\n", "expected_stdout": "x1=x2=2.00\n"},
                {"name": "共轭复根", "stdin": "1 2 5\n", "expected_stdout": "x1=-1.00+2.00i x2=-1.00-2.00i\n"},
                {"name": "纯虚根(防-0.00)", "stdin": "1 0 4\n", "expected_stdout": "x1=0.00+2.00i x2=0.00-2.00i\n"},
            ],
        },
        {
            "id": "m2_stage_a",
            "code": "★ 阶段 2A",
            "title": "阶段综合：递推法求数列与交错级数",
            "kind": "stage",
            "editable_default": "partial",
            "summary": "掌握第 4.6.1 节递推法思想：利用前项状态高效递推后项。",
            "goal": "输入正整数 n（1 <= n <= 40）。第一行输出斐波那契数列第 n 项 F(n)（其中 F(1)=1, F(2)=1, F(k)=F(k-1)+F(k-2)，用 long long 存储）；第二行输出交错调和级数 S(n) = 1 - 1/2 + 1/3 - ... + (-1)^(n-1)/n（保留 4 位小数）。",
            "guidance": [
                "1. 处理斐波那契递推：对 n <= 2 直接得 1；否则维护 prev2 = 1, prev1 = 1，循环从 3 到 n 计算 cur = prev1 + prev2 并滚动更新。",
                "2. 处理交错级数：维护符号变量 sign = 1.0 和累加器 sum = 0.0，循环 i = 1..n 时执行 sum += sign / i; sign = -sign;。",
                "3. 输出 long long 使用 %lld，输出 4 位小数使用 %.4f。",
            ],
            "sidebar_refs": {
                "functions": [
                    {"name": "scanf / printf", "sig": "%d, %lld, %.4f", "desc": "long long 整型用 %lld 输入输出。"},
                ],
                "structures": [
                    {
                        "name": "两项滚动递推模板",
                        "code": "long long a = 1, b = 1, c = 1;\nfor (int i = 3; i <= n; i++) {\n    c = a + b;\n    a = b;\n    b = c;\n}",
                    },
                    {
                        "name": "符号交替累加模板",
                        "code": "double sum = 0.0, sign = 1.0;\nfor (int i = 1; i <= n; i++) {\n    sum += sign / i;\n    sign = -sign;\n}",
                    },
                ],
            },
            "template": """#include <stdio.h>

int main(void) {
    int n;
    scanf("%d", &n);

    // <<<EDIT_START>>>
    // 1. 递推计算第 n 项斐波那契数（long long）
    // 2. 递推累加前 n 项交错调和级数（double）
    // 3. 分两行输出结果

    // <<<EDIT_END>>>
    return 0;
}
""",
            "solution": """#include <stdio.h>

int main(void) {
    int n;
    scanf("%d", &n);

    long long fib = 1;
    if (n >= 3) {
        long long a = 1, b = 1;
        for (int i = 3; i <= n; i++) {
            fib = a + b;
            a = b;
            b = fib;
        }
    }

    double sum = 0.0, sign = 1.0;
    for (int i = 1; i <= n; i++) {
        sum += sign / i;
        sign = -sign;
    }

    printf("%lld\\n%.4f\\n", fib, sum);
    return 0;
}
""",
            "tests": [
                {"name": "n=1 边界", "stdin": "1\n", "expected_stdout": "1\n1.0000\n"},
                {"name": "n=5", "stdin": "5\n", "expected_stdout": "5\n0.7833\n"},
                {"name": "n=10", "stdin": "10\n", "expected_stdout": "55\n0.6456\n"},
                {"name": "n=35 较大值", "stdin": "35\n", "expected_stdout": "9227465\n0.7072\n"},
            ],
        },
        {
            "id": "m2_stage_b",
            "code": "★ 阶段 2B",
            "title": "阶段综合：枚举法与贪心找零",
            "kind": "stage",
            "editable_default": "full",
            "summary": "独立编写完整程序，练习第 4.6.2～4.6.3 节贪心法与枚举法。",
            "goal": "输入一个正整数金额 amount（单位：分，1 <= amount <= 500）。\n第一行：使用贪心法（面额按 50、20、10、5、1 分从大到小优先选取），输出凑出 amount 所需的最少硬币总数。\n第二行：使用枚举法统计仅用 5 分、2 分、1 分硬币凑出 amount 且每种硬币至少 0 枚的不同方案总数。",
            "guidance": [
                "1. 贪心部分：用临时变量 rem = amount，依次对 50、20、10、5、1 做整除累加个数并取余更新 rem。",
                "2. 枚举部分：外层枚举 5 分硬币枚数 c5（0 到 amount/5），内层枚举 2 分硬币枚数 c2（0 到 (amount - 5*c5)/2），剩余金额全部由 1 分补齐即构成一种合法方案，计数器 ways++。",
                "3. 分别在两行输出最少硬币数和方案总数。",
            ],
            "sidebar_refs": {
                "functions": [
                    {"name": "scanf / printf", "sig": "stdio.h", "desc": "读取整数 amount 并分两行输出整数结果。"},
                ],
                "structures": [
                    {
                        "name": "贪心面额分解结构",
                        "code": "int rem = amount, cnt = 0;\ncnt += rem / 50; rem %= 50;\ncnt += rem / 20; rem %= 20;",
                    },
                    {
                        "name": "两层受限枚举结构",
                        "code": "int ways = 0;\nfor (int c5 = 0; c5 * 5 <= amount; c5++) {\n    for (int c2 = 0; c5 * 5 + c2 * 2 <= amount; c2++) {\n        ways++;\n    }\n}",
                    },
                ],
            },
            "template": """#include <stdio.h>

int main(void) {
    // 请完整编写程序

    return 0;
}
""",
            "solution": """#include <stdio.h>

int main(void) {
    int amount;
    scanf("%d", &amount);

    int rem = amount, min_coins = 0;
    int coins[5] = {50, 20, 10, 5, 1};
    for (int i = 0; i < 5; i++) {
        min_coins += rem / coins[i];
        rem %= coins[i];
    }

    int ways = 0;
    for (int c5 = 0; c5 * 5 <= amount; c5++) {
        for (int c2 = 0; c5 * 5 + c2 * 2 <= amount; c2++) {
            ways++;
        }
    }

    printf("%d\\n%d\\n", min_coins, ways);
    return 0;
}
""",
            "tests": [
                {"name": "amount=10", "stdin": "10\n", "expected_stdout": "1\n10\n"},
                {"name": "amount=37", "stdin": "37\n", "expected_stdout": "5\n84\n"},
                {"name": "amount=100", "stdin": "100\n", "expected_stdout": "2\n541\n"},
            ],
        },
    ],
}
