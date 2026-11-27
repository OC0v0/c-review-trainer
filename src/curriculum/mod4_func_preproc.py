"""Module 4: Functions, recursion, storage classes, and preprocessor (Chapters 6-7)."""

MODULE = {
    "id": "m4",
    "title": "模块 4：函数、递归与预处理",
    "subtitle": "对应第 6～7 章",
    "badge": "模块化",
    "exercises": [
        {
            "id": "m4_1",
            "code": "4.1",
            "title": "函数定义、原型声明与返回值",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握自定义函数的返回类型、形参列表、函数原型声明与调用。",
            "goal": "补全函数 is_prime(int n) 的定义：若 n 为素数返回 1，否则返回 0。主函数会调用它统计区间 [L, R] 内的素数个数。",
            "param_docs": [
                {
                    "name": "函数定义与原型声明",
                    "header": "C 语言函数机制",
                    "prototype": "返回类型 函数名(参数类型 参数名, ...);",
                    "purpose": "将独立子任务封装为可重复调用的代码模块；若函数定义位于 main 之后，需先写出原型声明。",
                    "params": [
                        {"name": "返回类型", "type": "类型名", "desc": "函数通过 return 返回的值的类型；不需要返回值时写 void。"},
                        {"name": "形参列表", "type": "变量声明列表", "desc": "接收调用处传入的实参副本（值传递）；修改形参不会影响外部实参。"},
                    ],
                    "returns": "执行 return 表达式; 后立即结束函数并将值返回给调用者。",
                    "notes": ["判断素数时：n < 2 返回 0；只需检验 i * i <= n 的因子即可提高效率。"],
                }
            ],
            "template": """#include <stdio.h>

int is_prime(int n);

int main(void) {
    int L, R, count = 0;
    scanf("%d %d", &L, &R);
    for (int x = L; x <= R; x++) {
        if (is_prime(x)) count++;
    }
    printf("%d\\n", count);
    return 0;
}

int is_prime(int n) {
    // 请实现素数判断：素数返回 1，否则返回 0
// <<<EDIT_START>>>
    return 0;
// <<<EDIT_END>>>
}
""",
            "solution": """#include <stdio.h>

int is_prime(int n);

int main(void) {
    int L, R, count = 0;
    scanf("%d %d", &L, &R);
    for (int x = L; x <= R; x++) {
        if (is_prime(x)) count++;
    }
    printf("%d\\n", count);
    return 0;
}

int is_prime(int n) {
    if (n < 2) return 0;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return 0;
    }
    return 1;
}
""",
            "tests": [
                {"name": "1 到 10", "stdin": "1 10\n", "expected_stdout": "4\n"},
                {"name": "10 到 30", "stdin": "10 30\n", "expected_stdout": "6\n"},
                {"name": "单个素数 2", "stdin": "2 2\n", "expected_stdout": "1\n"},
            ],
        },
        {
            "id": "m4_2",
            "code": "4.2",
            "title": "数组作为函数参数",
            "kind": "single",
            "editable_default": "partial",
            "summary": "理解一维数组传参传递首地址（可原地修改元素），需额外传入长度 n。",
            "goal": "补全函数 reverse_arr(int a[], int n)，将数组 a 的前 n 个元素原地逆置。",
            "param_docs": [
                {
                    "name": "数组形参",
                    "header": "C 语言函数参数规则",
                    "prototype": "void func(int a[], int n);  // 等价于 void func(int *a, int n);",
                    "purpose": "将数组首元素地址传入函数，函数内对 a[i] 的修改会直接作用于调用方的原数组。",
                    "params": [
                        {"name": "a[]", "type": "int *（数组退化为指针）", "desc": "接收实参数组首地址；函数内无法用 sizeof(a) 获取原数组长度。"},
                        {"name": "n", "type": "int", "desc": "显式传入数组有效元素个数。"},
                    ],
                    "returns": "原地修改数组，返回类型为 void。",
                    "notes": ["二维数组作形参时，必须显式写出列数，例如 void f(int m[][10], int rows)。"],
                }
            ],
            "template": """#include <stdio.h>

void reverse_arr(int a[], int n) {
    // 请将数组 a[0..n-1] 原地逆置
// <<<EDIT_START>>>
    
// <<<EDIT_END>>>
}

int main(void) {
    int n, a[100];
    scanf("%d", &n);
    for (int i = 0; i < n; i++) scanf("%d", &a[i]);

    reverse_arr(a, n);

    for (int i = 0; i < n; i++) {
        if (i > 0) putchar(' ');
        printf("%d", a[i]);
    }
    putchar('\\n');
    return 0;
}
""",
            "solution": """#include <stdio.h>

void reverse_arr(int a[], int n) {
    for (int i = 0, j = n - 1; i < j; i++, j--) {
        int tmp = a[i];
        a[i] = a[j];
        a[j] = tmp;
    }
}

int main(void) {
    int n, a[100];
    scanf("%d", &n);
    for (int i = 0; i < n; i++) scanf("%d", &a[i]);

    reverse_arr(a, n);

    for (int i = 0; i < n; i++) {
        if (i > 0) putchar(' ');
        printf("%d", a[i]);
    }
    putchar('\\n');
    return 0;
}
""",
            "tests": [
                {"name": "偶数长度", "stdin": "4\n1 2 3 4\n", "expected_stdout": "4 3 2 1\n"},
                {"name": "奇数长度", "stdin": "5\n10 20 30 40 50\n", "expected_stdout": "50 40 30 20 10\n"},
                {"name": "单元素", "stdin": "1\n9\n", "expected_stdout": "9\n"},
            ],
        },
        {
            "id": "m4_3",
            "code": "4.3",
            "title": "函数的递归调用",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握递归的两大要素：终止条件（基例）与向基例收敛的递归调用。",
            "goal": "使用递归实现欧几里得算法函数 gcd(int a, int b) 求两个正整数的最大公约数：当 b == 0 时返回 a，否则返回 gcd(b, a % b)。",
            "param_docs": [
                {
                    "name": "递归函数结构",
                    "header": "C 语言函数调用机制",
                    "prototype": "int gcd(int a, int b) { if (b == 0) return a; return gcd(b, a % b); }",
                    "purpose": "函数在其函数体内直接或间接调用自身，将原问题分解为规模更小的同类子问题。",
                    "params": [
                        {"name": "终止条件", "type": "if 分支", "desc": "不再继续递归调用的最小情形；缺少终止条件会导致无限递归和栈溢出。"},
                        {"name": "递归关系", "type": "自调用表达式", "desc": "每次调用必须使参数规模严格减小并趋近终止条件。"},
                    ],
                    "returns": "逐层返回子问题结果并汇总为最终返回值。",
                }
            ],
            "template": """#include <stdio.h>

int gcd(int a, int b) {
    // 请使用递归实现最大公约数计算
// <<<EDIT_START>>>
    return 0;
// <<<EDIT_END>>>
}

int main(void) {
    int a, b;
    scanf("%d %d", &a, &b);
    printf("%d\\n", gcd(a, b));
    return 0;
}
""",
            "solution": """#include <stdio.h>

int gcd(int a, int b) {
    if (b == 0) return a;
    return gcd(b, a % b);
}

int main(void) {
    int a, b;
    scanf("%d %d", &a, &b);
    printf("%d\\n", gcd(a, b));
    return 0;
}
""",
            "code_checks": [
                {"pattern": r"gcd\s*\([^)]*gcd\s*\(|return\s+gcd\s*\(", "message": "请在 gcd 函数内部通过递归调用 gcd 实现。"},
            ],
            "tests": [
                {"name": "常规", "stdin": "48 18\n", "expected_stdout": "6\n"},
                {"name": "互素", "stdin": "17 13\n", "expected_stdout": "1\n"},
                {"name": "倍数关系", "stdin": "25 100\n", "expected_stdout": "25\n"},
            ],
        },
        {
            "id": "m4_4",
            "code": "4.4",
            "title": "static 静态局部变量",
            "kind": "single",
            "editable_default": "partial",
            "summary": "理解静态局部变量在函数多次调用间保持数值且仅初始化一次的特性。",
            "goal": "补全函数 running_sum(int x)：在函数内部使用 static int total = 0; 累加每次传入的 x 并返回当前的累计总和。",
            "param_docs": [
                {
                    "name": "static 静态局部变量",
                    "header": "C 语言存储类别（第 6.5.3 节）",
                    "prototype": "static 类型 变量名 = 常量初值;",
                    "purpose": "声明一个作用域仍限于当前函数内部、但生存期贯穿整个程序运行过程的变量。",
                    "params": [
                        {"name": "初始化时机", "type": "静态存储区", "desc": "仅在程序启动/首次进入时初始化一次；函数返回后不销毁，下次调用保留上次结果。"},
                        {"name": "作用域", "type": "局部作用域", "desc": "其他函数不能直接按名称访问该变量。"},
                    ],
                    "returns": "变量本身可直接参与运算与返回。",
                    "notes": ["普通局部变量每次进入函数都会重新初始化，而 static 局部变量不会。"],
                }
            ],
            "template": """#include <stdio.h>

int running_sum(int x) {
    // 使用 static 局部变量保存累计和并返回
// <<<EDIT_START>>>
    return 0;
// <<<EDIT_END>>>
}

int main(void) {
    int n, x;
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &x);
        if (i > 0) putchar(' ');
        printf("%d", running_sum(x));
    }
    putchar('\\n');
    return 0;
}
""",
            "solution": """#include <stdio.h>

int running_sum(int x) {
    static int total = 0;
    total += x;
    return total;
}

int main(void) {
    int n, x;
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &x);
        if (i > 0) putchar(' ');
        printf("%d", running_sum(x));
    }
    putchar('\\n');
    return 0;
}
""",
            "code_checks": [
                {"pattern": r"\bstatic\b", "message": "请在函数内使用 static 修饰局部变量。"},
            ],
            "tests": [
                {"name": "正数前缀和", "stdin": "4\n1 2 3 4\n", "expected_stdout": "1 3 6 10\n"},
                {"name": "含负数", "stdin": "5\n10 -3 5 -2 8\n", "expected_stdout": "10 7 12 10 18\n"},
            ],
        },
        {
            "id": "m4_5",
            "code": "4.5",
            "title": "宏定义 #define 与带参宏括号规范",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握无参宏与带参宏的文本替换本质及防优先级错误的括号写法。",
            "goal": "定义无参宏 PI 为 3.14159，带参宏 SQR(x) 计算 x 的平方，带参宏 MAX2(a, b) 返回两者较大值。注意为每个形参和整体表达式加上括号，确保 SQR(a + 1) 等表达式结果正确。",
            "param_docs": [
                {
                    "name": "#define 宏定义",
                    "header": "C 预处理命令（第 7.2 节）",
                    "prototype": "#define 宏名 替换文本   |   #define 宏名(x, y) ((x) + (y))",
                    "purpose": "在编译前的预处理阶段，将代码中出现的宏名原样文本替换为指定内容。",
                    "params": [
                        {"name": "宏名与左括号之间", "type": "语法要求", "desc": "带参宏的宏名与 ( 之间不能有空格，例如 #define SQR(x) 正确，#define SQR (x) 错误。"},
                        {"name": "括号保护", "type": "防错规范", "desc": "每个参数出现处和整个宏展开体都必须用括号包裹，防止 SQR(a+1) 展开成 a+1*a+1。"},
                    ],
                    "returns": "预处理指令末尾不加分号 ;。",
                    "notes": ["条件编译 #ifndef / #define / #endif 常用于防止头文件重复包含。"],
                }
            ],
            "template": """#include <stdio.h>

// <<<EDIT_START>>>
// 请在此定义宏 PI, SQR(x), MAX2(a, b)

// <<<EDIT_END>>>

int main(void) {
    int a, b;
    scanf("%d %d", &a, &b);
    printf("%d %d %.2f\\n", SQR(a + 1), MAX2(a + 2, b * 2), PI * SQR(a));
    return 0;
}
""",
            "solution": """#include <stdio.h>

#define PI 3.14159
#define SQR(x) ((x) * (x))
#define MAX2(a, b) (((a) > (b)) ? (a) : (b))

int main(void) {
    int a, b;
    scanf("%d %d", &a, &b);
    printf("%d %d %.2f\\n", SQR(a + 1), MAX2(a + 2, b * 2), PI * SQR(a));
    return 0;
}
""",
            "code_checks": [
                {"pattern": r"#\s*define\s+PI\b", "message": "请使用 #define 定义 PI。"},
                {"pattern": r"#\s*define\s+SQR\s*\(", "message": "请使用 #define 定义带参宏 SQR(x)。"},
                {"pattern": r"#\s*define\s+MAX2\s*\(", "message": "请使用 #define 定义带参宏 MAX2(a, b)。"},
            ],
            "tests": [
                {"name": "表达式参数测试", "stdin": "3 4\n", "expected_stdout": "16 8 28.27\n"},
                {"name": "零与负数", "stdin": "2 -5\n", "expected_stdout": "9 4 12.57\n"},
            ],
        },
        {
            "id": "m4_stage",
            "code": "★ 阶段 4",
            "title": "阶段综合：递归进制转换与组合数计算",
            "kind": "stage",
            "editable_default": "full",
            "summary": "模块化编写递归函数，完成非负整数转任意进制（2～16）与递归组合数 C(n, k)。",
            "goal": "输入三个非负整数 num、base（2 <= base <= 16）以及 n、k（0 <= k <= n <= 15）。\n第一行：编写递归函数 print_base(int num, int base) 输出 num 的 base 进制表示（10~15 用大写字母 A~F 表示；当 num == 0 时输出 0）。\n第二行：编写递归函数 comb(int n, int k) 按递推式 C(n, k) = C(n-1, k-1) + C(n-1, k) 输出组合数值。",
            "guidance": [
                "1. 进制转换递归：若 num >= base，先递归调用 print_base(num / base, base)；再输出当前余数 num % base 对应的字符（可用字符数组 \"0123456789ABCDEF\" 查表）。",
                "2. 组合数递归：基例为 k == 0 或 k == n 时返回 1；其余返回 comb(n - 1, k - 1) + comb(n - 1, k)。",
                "3. 主函数读入 num, base, n, k，依次调用两个函数并换行输出。",
            ],
            "sidebar_refs": {
                "functions": [
                    {"name": "putchar", "sig": "int putchar(int ch);", "desc": "递归回溯阶段逐位输出转换后的数字字符。"},
                ],
                "structures": [
                    {
                        "name": "递归进制转换结构",
                        "code": "void print_base(int num, int base) {\n    const char digits[] = \"0123456789ABCDEF\";\n    if (num >= base) print_base(num / base, base);\n    putchar(digits[num % base]);\n}",
                    },
                    {
                        "name": "帕斯卡递推组合数",
                        "code": "int comb(int n, int k) {\n    if (k == 0 || k == n) return 1;\n    return comb(n - 1, k - 1) + comb(n - 1, k);\n}",
                    },
                ],
            },
            "template": """#include <stdio.h>

// 请在此声明并定义 print_base 和 comb 函数，再在 main 中调用

int main(void) {
    return 0;
}
""",
            "solution": """#include <stdio.h>

void print_base(int num, int base) {
    const char digits[] = "0123456789ABCDEF";
    if (num >= base) {
        print_base(num / base, base);
    }
    putchar(digits[num % base]);
}

int comb(int n, int k) {
    if (k == 0 || k == n) return 1;
    return comb(n - 1, k - 1) + comb(n - 1, k);
}

int main(void) {
    int num, base, n, k;
    scanf("%d %d %d %d", &num, &base, &n, &k);
    print_base(num, base);
    putchar('\\n');
    printf("%d\\n", comb(n, k));
    return 0;
}
""",
            "tests": [
                {"name": "十六进制与 C(5,2)", "stdin": "255 16 5 2\n", "expected_stdout": "FF\n10\n"},
                {"name": "二进制与 C(6,3)", "stdin": "13 2 6 3\n", "expected_stdout": "1101\n20\n"},
                {"name": "num=0 与 k=0", "stdin": "0 8 7 0\n", "expected_stdout": "0\n1\n"},
            ],
        },
    ],
}
