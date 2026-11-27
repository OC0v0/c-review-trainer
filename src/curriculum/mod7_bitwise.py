"""Module 7: Bitwise operations (Chapter 10)."""

MODULE = {
    "id": "m7",
    "title": "模块 7：位运算",
    "subtitle": "对应第 10 章",
    "badge": "底层操作",
    "exercises": [
        {
            "id": "m7_1",
            "code": "7.1",
            "title": "按位与 &、或 |、异或 ^、取反 ~",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握四个基本按位逻辑运算符，以及用掩码对指定二进制位置 1、清 0、翻转。",
            "goal": "输入无符号整数 x 和位编号 k（0 <= k <= 15，最低位为第 0 位）。依次输出：\n1) 将 x 的第 k 位置 1 后的结果；\n2) 将 x 的第 k 位清 0 后的结果；\n3) 将 x 的第 k 位翻转后的结果（均用无符号十进制输出，空格分隔）。",
            "param_docs": [
                {
                    "name": "位运算与位掩码",
                    "header": "C 语言位运算符（第 10.2 节）",
                    "prototype": "x | (1u << k)   |   x & ~(1u << k)   |   x ^ (1u << k)",
                    "purpose": "对整型数据的每一个二进制位独立执行逻辑运算。",
                    "params": [
                        {"name": "&（按位与）", "type": "二元运算符", "desc": "两位均为 1 时结果为 1，否则为 0；常用于提取位或清零指定位。"},
                        {"name": "|（按位或）", "type": "二元运算符", "desc": "任一位为 1 时结果为 1；常用于将指定位置 1。"},
                        {"name": "^（按位异或）", "type": "二元运算符", "desc": "两位不同为 1、相同为 0；与 1 异或可翻转指定位。"},
                        {"name": "~（按位取反）", "type": "一元运算符", "desc": "将每一个二进制位 0 变 1、1 变 0。"},
                    ],
                    "returns": "返回按位运算后的整型值。",
                    "notes": ["位运算符优先级低于 == 和 !=，构造判断条件时务必加括号，如 ((x & mask) != 0)。"],
                }
            ],
            "template": """#include <stdio.h>

int main(void) {
    unsigned int x;
    int k;
    scanf("%u %d", &x, &k);

    unsigned int set_bit = 0, clear_bit = 0, toggle_bit = 0;

    // <<<EDIT_START>>>
    // 计算第 k 位置 1 (set_bit)、清 0 (clear_bit)、翻转 (toggle_bit)

    // <<<EDIT_END>>>

    printf("%u %u %u\\n", set_bit, clear_bit, toggle_bit);
    return 0;
}
""",
            "solution": """#include <stdio.h>

int main(void) {
    unsigned int x;
    int k;
    scanf("%u %d", &x, &k);

    unsigned int mask = 1u << k;
    unsigned int set_bit = x | mask;
    unsigned int clear_bit = x & (~mask);
    unsigned int toggle_bit = x ^ mask;

    printf("%u %u %u\\n", set_bit, clear_bit, toggle_bit);
    return 0;
}
""",
            "tests": [
                {"name": "第1位原为0", "stdin": "5 1\n", "expected_stdout": "7 5 7\n"},
                {"name": "第2位原为1", "stdin": "5 2\n", "expected_stdout": "5 1 1\n"},
                {"name": "第0位", "stdin": "0 0\n", "expected_stdout": "1 0 1\n"},
            ],
        },
        {
            "id": "m7_2",
            "code": "7.2",
            "title": "移位运算 << / >> 与字节提取",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握无符号整数移位提取指定位段（如低 8 位、高 8 位）。",
            "goal": "输入一个 16 位无符号范围的整数 x（0 <= x <= 65535），提取其高 8 位 high 和低 8 位 low，并交换高低 8 位构成新值 swapped，按空格分隔输出 high、low、swapped。",
            "param_docs": [
                {
                    "name": "左移 << 与右移 >>",
                    "header": "C 语言移位运算符（第 10.2.5～10.2.6 节）",
                    "prototype": "x << n   |   x >> n",
                    "purpose": "将二进制位整体向左或向右移动 n 位。",
                    "params": [
                        {"name": "x << n", "type": "左移", "desc": "低位补 0；对无符号数在不溢出时等价于乘以 2^n。"},
                        {"name": "x >> n", "type": "右移", "desc": "对 unsigned 类型执行逻辑右移（高位补 0）；提取第 8~15 位可写 (x >> 8) & 0xFFu。"},
                    ],
                    "returns": "返回移位后的整数值。",
                    "notes": ["进行位提取与移位时应优先使用 unsigned int，避免有符号数右移的符号扩展差异。"],
                }
            ],
            "template": """#include <stdio.h>

int main(void) {
    unsigned int x;
    scanf("%u", &x);

    unsigned int high = 0, low = 0, swapped = 0;

    // <<<EDIT_START>>>
    // 提取 high（高 8 位）、low（低 8 位），以及高低 8 位互换后的 swapped

    // <<<EDIT_END>>>

    printf("%u %u %u\\n", high, low, swapped);
    return 0;
}
""",
            "solution": """#include <stdio.h>

int main(void) {
    unsigned int x;
    scanf("%u", &x);

    unsigned int high = (x >> 8) & 0xFFu;
    unsigned int low = x & 0xFFu;
    unsigned int swapped = (low << 8) | high;

    printf("%u %u %u\\n", high, low, swapped);
    return 0;
}
""",
            "tests": [
                {"name": "0x0102 = 258", "stdin": "258\n", "expected_stdout": "1 2 513\n"},
                {"name": "0xFF00 = 65280", "stdin": "65280\n", "expected_stdout": "255 0 255\n"},
            ],
        },
        {
            "id": "m7_stage",
            "code": "★ 阶段 7",
            "title": "阶段综合：二进制形式输出与统计 1 的个数",
            "kind": "stage",
            "editable_default": "full",
            "summary": "对应课本第 10.4 节两大经典实例：输出二进制形式与统计二进制中 1 的位数。",
            "goal": "输入一个无符号整数 x（0 <= x <= 65535）。\n第一行：以 16 位定长二进制形式输出 x（从最高第 15 位到最低第 0 位，每 4 位之间加一个空格，如 `0000 0000 0000 1101`）。\n第二行：输出 x 的二进制表示中 `1` 的总个数。",
            "guidance": [
                "1. 从 bit = 15 递减到 0：用 (x >> bit) & 1u 取出当前位并输出 '0' 或 '1'。",
                "2. 每输出 4 位且不是最后一位时（例如 bit > 0 && bit % 4 == 0）输出一个空格。",
                "3. 同时统计当前位为 1 的次数（或另用 n &= (n - 1) 技巧统计），在第二行输出总数。",
            ],
            "sidebar_refs": {
                "functions": [
                    {"name": "scanf / putchar / printf", "sig": "unsigned int 用 %u 读入", "desc": "标准输入输出函数。"},
                ],
                "structures": [
                    {
                        "name": "逐位从高到低输出模板",
                        "code": "for (int i = 15; i >= 0; i--) {\n    unsigned int b = (x >> i) & 1u;\n    putchar(b ? '1' : '0');\n    if (i > 0 && i % 4 == 0) putchar(' ');\n}",
                    },
                    {
                        "name": "Brian Kernighan 统计 1 位数",
                        "code": "int ones = 0;\nunsigned int t = x;\nwhile (t != 0) { t &= (t - 1); ones++; }",
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
    unsigned int x;
    scanf("%u", &x);

    int ones = 0;
    for (int i = 15; i >= 0; i--) {
        unsigned int b = (x >> i) & 1u;
        putchar(b ? '1' : '0');
        if (b) ones++;
        if (i > 0 && i % 4 == 0) putchar(' ');
    }
    putchar('\\n');
    printf("%d\\n", ones);
    return 0;
}
""",
            "tests": [
                {"name": "x=13", "stdin": "13\n", "expected_stdout": "0000 0000 0000 1101\n3\n"},
                {"name": "x=0", "stdin": "0\n", "expected_stdout": "0000 0000 0000 0000\n0\n"},
                {"name": "x=43690 (0xAAAA)", "stdin": "43690\n", "expected_stdout": "1010 1010 1010 1010\n8\n"},
            ],
        },
    ],
}
