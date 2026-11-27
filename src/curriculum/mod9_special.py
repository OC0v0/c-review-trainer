"""Module 9: Special intensive cross-chapter exam drills."""

MODULE = {
    "id": "m9",
    "title": "专项强化：跨章节综合演练",
    "subtitle": "考前拔高与易错点深挖",
    "badge": "考前冲刺",
    "exercises": [
        {
            "id": "m9_s1",
            "code": "专项 1",
            "title": "双指针原地去重与区间翻转",
            "kind": "special",
            "editable_default": "partial",
            "summary": "深化指针与数组关系：使用快慢双指针在 O(n) 时间内完成有序数组原地去重并循环右移。",
            "goal": "输入正整数 n（1 <= n <= 1000）、n 个已按升序排列的整数，以及非负整数 k。\n1. 编写函数 unique_inplace(int *a, int n) 利用快慢指针原地去除重复元素，返回去重后的新长度 m；\n2. 编写函数 rotate_right(int *a, int m, int k) 通过三次区间翻转将去重后的数组循环右移 k 位（注意先对 m 取模）；\n3. 第一行输出 m，第二行输出最终数组（空格分隔）。",
            "guidance": [
                "1. 快慢指针去重：slow 指向最后一个已保留元素的下标（初值 0），fast 从 1 遍历到 n-1；若 a[fast] != a[slow]，则令 a[++slow] = a[fast]；最后返回 slow + 1。",
                "2. 三次翻转循环右移 k 位（k %= m）：先整体翻转 [0, m-1]，再翻转 [0, k-1]，最后翻转 [k, m-1]。",
                "3. 注意处理 k == 0 或 m <= 1 的边界情况。",
            ],
            "sidebar_refs": {
                "functions": [
                    {"name": "reverse_range", "sig": "void reverse_range(int *l, int *r);", "desc": "可用左右指针 l < r 交换 *l 与 *r，逐步向中间靠拢。"},
                ],
                "structures": [
                    {
                        "name": "快慢指针原地去重结构",
                        "code": "int slow = 0;\nfor (int fast = 1; fast < n; fast++) {\n    if (a[fast] != a[slow]) a[++slow] = a[fast];\n}\nreturn slow + 1;",
                    },
                    {
                        "name": "三次翻转循环右移",
                        "code": "k %= m;\nif (k > 0) {\n    rev(a, a + m - 1);\n    rev(a, a + k - 1);\n    rev(a + k, a + m - 1);\n}",
                    },
                ],
            },
            "template": """#include <stdio.h>

// 1. unique_inplace: 请用双指针实现有序数组原地去重，返回新长度
// 2. rotate_right: 请将 a[0..m-1] 原地循环右移 k 位
// <<<EDIT_START>>>
int unique_inplace(int *a, int n) {
    return n;
}

void rotate_right(int *a, int m, int k) {
    
}
// <<<EDIT_END>>>

int main(void) {
    int n, k, a[1000];
    scanf("%d", &n);
    for (int i = 0; i < n; i++) scanf("%d", &a[i]);
    scanf("%d", &k);

    int m = unique_inplace(a, n);
    rotate_right(a, m, k);

    printf("%d\\n", m);
    for (int i = 0; i < m; i++) {
        if (i > 0) putchar(' ');
        printf("%d", a[i]);
    }
    putchar('\\n');
    return 0;
}
""",
            "solution": """#include <stdio.h>

static void rev_ptr(int *l, int *r) {
    while (l < r) {
        int t = *l;
        *l = *r;
        *r = t;
        l++;
        r--;
    }
}

int unique_inplace(int *a, int n) {
    if (n <= 0) return 0;
    int slow = 0;
    for (int fast = 1; fast < n; fast++) {
        if (a[fast] != a[slow]) {
            a[++slow] = a[fast];
        }
    }
    return slow + 1;
}

void rotate_right(int *a, int m, int k) {
    if (m <= 1) return;
    k %= m;
    if (k == 0) return;
    rev_ptr(a, a + m - 1);
    rev_ptr(a, a + k - 1);
    rev_ptr(a + k, a + m - 1);
}

int main(void) {
    int n, k, a[1000];
    scanf("%d", &n);
    for (int i = 0; i < n; i++) scanf("%d", &a[i]);
    scanf("%d", &k);

    int m = unique_inplace(a, n);
    rotate_right(a, m, k);

    printf("%d\\n", m);
    for (int i = 0; i < m; i++) {
        if (i > 0) putchar(' ');
        printf("%d", a[i]);
    }
    putchar('\\n');
    return 0;
}
""",
            "tests": [
                {"name": "常规去重并右移", "stdin": "8\n1 1 2 3 3 3 4 5\n2\n", "expected_stdout": "5\n4 5 1 2 3\n"},
                {"name": "k大于m", "stdin": "6\n2 2 4 6 6 8\n9\n", "expected_stdout": "4\n8 2 4 6\n"},
                {"name": "全部相同", "stdin": "4\n7 7 7 7\n3\n", "expected_stdout": "1\n7\n"},
            ],
        },
        {
            "id": "m9_s2",
            "code": "专项 2",
            "title": "字符串分词、边界处理与最长单词提取",
            "kind": "special",
            "editable_default": "full",
            "summary": "攻克字符串扫描中最易出错的多空格、首尾标点与 '\\0' 边界截断问题。",
            "goal": "从标准输入读入一行英文文本（长度不超过 255，可能包含多个连续空格或标点符号）。定义“单词”为连续的英文字母序列（A-Z 或 a-z）。\n请输出：单词总数 以及 最长的单词（若有多个长度相同的最长单词，输出最先出现的那个），中间用单个空格分隔。若没有任何单词，输出 `0 NONE`。",
            "guidance": [
                "1. 用 fgets 读入整行文本。",
                "2. 用索引 i 遍历字符串：跳过非字母字符；一旦遇到字母，记录起点 start = i，继续推进直到遇到非字母或 '\\0'，此时单词长度为 cur_len = i - start。",
                "3. 统计 word_count++；若 cur_len > best_len，更新 best_len = cur_len 且 best_start = start。",
                "4. 结束后若 word_count == 0 输出 0 NONE；否则输出 word_count 以及从 best_start 开始的 best_len 个字符（可用 printf(\"%d %.*s\\n\", word_count, best_len, line + best_start) 或循环 putchar）。",
            ],
            "sidebar_refs": {
                "functions": [
                    {"name": "fgets", "sig": "char *fgets(char *buf, int n, FILE *stream);", "desc": "读取整行含空格文本。"},
                    {"name": "isalpha", "sig": "#include <ctype.h> int isalpha(int ch);", "desc": "判断是否为英文字母（建议先转 (unsigned char)c）。"},
                ],
                "structures": [
                    {
                        "name": "双段扫描提取单词模板",
                        "code": "int i = 0;\nwhile (s[i] != '\\0') {\n    while (s[i] && !isalpha((unsigned char)s[i])) i++;\n    if (!s[i]) break;\n    int start = i;\n    while (isalpha((unsigned char)s[i])) i++;\n    int len = i - start;\n}",
                    },
                ],
            },
            "template": """#include <stdio.h>
#include <ctype.h>

int main(void) {
    // 请完整编写单词统计与最长词提取程序

    return 0;
}
""",
            "solution": """#include <stdio.h>
#include <ctype.h>

int main(void) {
    char s[300];
    if (fgets(s, sizeof(s), stdin) == NULL) {
        printf("0 NONE\\n");
        return 0;
    }

    int i = 0, count = 0, best_start = 0, best_len = 0;
    while (s[i] != '\\0') {
        while (s[i] != '\\0' && !isalpha((unsigned char)s[i])) {
            i++;
        }
        if (s[i] == '\\0') break;
        int start = i;
        while (isalpha((unsigned char)s[i])) {
            i++;
        }
        int len = i - start;
        count++;
        if (len > best_len) {
            best_len = len;
            best_start = start;
        }
    }

    if (count == 0) {
        printf("0 NONE\\n");
    } else {
        printf("%d %.*s\\n", count, best_len, s + best_start);
    }
    return 0;
}
""",
            "tests": [
                {"name": "多空格与标点", "stdin": "   C, programming!! is   super-fun 123  \n", "expected_stdout": "5 programming\n"},
                {"name": "同长度取首个", "stdin": "cat dog fox ant\n", "expected_stdout": "4 cat\n"},
                {"name": "无英文单词", "stdin": "12345 --- !@#\n", "expected_stdout": "0 NONE\n"},
            ],
        },
        {
            "id": "m9_s3",
            "code": "专项 3",
            "title": "递归回溯：字典序全排列（呼应第 6.7.3 节）",
            "kind": "special",
            "editable_default": "full",
            "summary": "掌握递归回溯的标准三步法（选择 -> 递归 -> 撤销现场），攻克课本第 6.7.3 节全排列问题。",
            "goal": "输入正整数 n（1 <= n <= 6），按字典序从小到大输出数字 1 到 n 的全部全排列，每行一个排列，数字之间用单个空格分隔。",
            "guidance": [
                "1. 开两个数组：path[10] 记录当前已选序列，used[10] 记录数字 1..n 是否已被使用（初值为 0）。",
                "2. 编写递归函数 dfs(int depth, int n)：当 depth == n 时输出 path[0..n-1] 并换行返回。",
                "3. 否则循环 num = 1..n：若 !used[num]，则设 used[num] = 1; path[depth] = num;，递归调用 dfs(depth + 1, n);，返回后恢复现场 used[num] = 0;。",
            ],
            "sidebar_refs": {
                "functions": [
                    {"name": "dfs(depth, n)", "sig": "void dfs(int depth, int n);", "desc": "按位置 depth 从 0 填到 n-1，因为每层从小到大枚举数字，输出自然为字典序。"},
                ],
                "structures": [
                    {
                        "name": "回溯搜索经典模板",
                        "code": "for (int x = 1; x <= n; x++) {\n    if (!used[x]) {\n        used[x] = 1;\n        path[depth] = x;\n        dfs(depth + 1, n);\n        used[x] = 0;\n    }\n}",
                    },
                ],
            },
            "template": """#include <stdio.h>

// 请完整编写字典序全排列递归回溯程序

int main(void) {
    return 0;
}
""",
            "solution": """#include <stdio.h>

int path[10];
int used[10];

void dfs(int depth, int n) {
    if (depth == n) {
        for (int i = 0; i < n; i++) {
            if (i > 0) putchar(' ');
            printf("%d", path[i]);
        }
        putchar('\\n');
        return;
    }
    for (int x = 1; x <= n; x++) {
        if (!used[x]) {
            used[x] = 1;
            path[depth] = x;
            dfs(depth + 1, n);
            used[x] = 0;
        }
    }
}

int main(void) {
    int n;
    scanf("%d", &n);
    dfs(0, n);
    return 0;
}
""",
            "tests": [
                {"name": "n=1", "stdin": "1\n", "expected_stdout": "1\n"},
                {
                    "name": "n=3",
                    "stdin": "3\n",
                    "expected_stdout": "1 2 3\n1 3 2\n2 1 3\n2 3 1\n3 1 2\n3 2 1\n",
                },
            ],
        },
        {
            "id": "m9_s4",
            "code": "专项 4",
            "title": "链表反转与合并两个有序链表",
            "kind": "special",
            "editable_default": "partial",
            "summary": "深化结构体指针与链表断链操作：原地反转链表并归并两个有序链表。",
            "goal": "给定两个按升序排列的单链表 L1 和 L2（由框架读入建立）。请补全：\n1. merge_lists(Node *a, Node *b)：将两个升序链表合并为一个升序链表（直接复用原结点，无需重新 malloc）；\n2. reverse_list(Node *head)：将合并后的单链表原地反转为降序链表并返回新头指针。",
            "guidance": [
                "1. 合并有序链表：创建局部哨兵 Node dummy = {0, NULL}; Node *tail = &dummy;，当 a && b 时每次把值更小的结点接到 tail->next 并前进指针；循环后把非空的剩余链表直接挂在 tail->next。",
                "2. 原地反转链表：维护三个指针 prev = NULL, cur = head；每轮保存 nxt = cur->next，执行 cur->next = prev，再整体前移 prev = cur; cur = nxt;，最后返回 prev。",
            ],
            "sidebar_refs": {
                "functions": [
                    {"name": "指针重连", "sig": "Node *nxt = cur->next; cur->next = prev;", "desc": "反转前必须先保存 cur->next，否则后续链表地址会丢失。"},
                ],
                "structures": [
                    {
                        "name": "三指针原地反转链表",
                        "code": "Node *prev = NULL, *cur = head;\nwhile (cur != NULL) {\n    Node *nxt = cur->next;\n    cur->next = prev;\n    prev = cur;\n    cur = nxt;\n}\nreturn prev;",
                    },
                ],
            },
            "template": """#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int val;
    struct Node *next;
} Node;

Node *read_list(int n) {
    Node dummy = {0, NULL}, *tail = &dummy;
    for (int i = 0; i < n; i++) {
        Node *p = (Node *)malloc(sizeof(Node));
        scanf("%d", &p->val);
        p->next = NULL;
        tail->next = p;
        tail = p;
    }
    return dummy.next;
}

// 1. merge_lists: 合并两个升序链表为一个升序链表
// 2. reverse_list: 原地反转单链表并返回新头指针
// <<<EDIT_START>>>
Node *merge_lists(Node *a, Node *b) {
    return NULL;
}

Node *reverse_list(Node *head) {
    return head;
}
// <<<EDIT_END>>>

int main(void) {
    int n1, n2;
    scanf("%d", &n1);
    Node *l1 = read_list(n1);
    scanf("%d", &n2);
    Node *l2 = read_list(n2);

    Node *merged = merge_lists(l1, l2);
    Node *rev = reverse_list(merged);

    if (rev == NULL) {
        printf("EMPTY\\n");
    } else {
        for (Node *p = rev; p != NULL; p = p->next) {
            if (p != rev) putchar(' ');
            printf("%d", p->val);
        }
        putchar('\\n');
    }
    while (rev) { Node *t = rev; rev = rev->next; free(t); }
    return 0;
}
""",
            "solution": """#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int val;
    struct Node *next;
} Node;

Node *read_list(int n) {
    Node dummy = {0, NULL}, *tail = &dummy;
    for (int i = 0; i < n; i++) {
        Node *p = (Node *)malloc(sizeof(Node));
        scanf("%d", &p->val);
        p->next = NULL;
        tail->next = p;
        tail = p;
    }
    return dummy.next;
}

Node *merge_lists(Node *a, Node *b) {
    Node dummy = {0, NULL};
    Node *tail = &dummy;
    while (a != NULL && b != NULL) {
        if (a->val <= b->val) {
            tail->next = a;
            a = a->next;
        } else {
            tail->next = b;
            b = b->next;
        }
        tail = tail->next;
    }
    tail->next = (a != NULL) ? a : b;
    return dummy.next;
}

Node *reverse_list(Node *head) {
    Node *prev = NULL, *cur = head;
    while (cur != NULL) {
        Node *nxt = cur->next;
        cur->next = prev;
        prev = cur;
        cur = nxt;
    }
    return prev;
}

int main(void) {
    int n1, n2;
    scanf("%d", &n1);
    Node *l1 = read_list(n1);
    scanf("%d", &n2);
    Node *l2 = read_list(n2);

    Node *merged = merge_lists(l1, l2);
    Node *rev = reverse_list(merged);

    if (rev == NULL) {
        printf("EMPTY\\n");
    } else {
        for (Node *p = rev; p != NULL; p = p->next) {
            if (p != rev) putchar(' ');
            printf("%d", p->val);
        }
        putchar('\\n');
    }
    while (rev) { Node *t = rev; rev = rev->next; free(t); }
    return 0;
}
""",
            "tests": [
                {"name": "交叉合并并反转", "stdin": "3\n1 4 7\n4\n2 3 4 9\n", "expected_stdout": "9 7 4 4 3 2 1\n"},
                {"name": "其中一个为空表", "stdin": "0\n3\n5 10 15\n", "expected_stdout": "15 10 5\n"},
            ],
        },
        {
            "id": "m9_s5",
            "code": "专项 5",
            "title": "全流程大综合：结构体二进制读写与文本报表生成",
            "kind": "special",
            "editable_default": "full",
            "summary": "打通结构体、动态内存、二进制文件读取（fread + fseek/ftell）、排序与文本报表输出（fprintf）。",
            "goal": "当前目录已有二进制文件 \"records.bin\"，顺序存储了若干条固定大小结构体记录：\n`typedef struct { int id; char name[16]; int score; } Record;`（注意 name 字段已是 '\\0' 结尾字符串，结构体大小为 24 字节）。\n请编写程序：\n1. 以 \"rb\" 打开 \"records.bin\"，利用 fseek + ftell 计算记录条数 n，并用 malloc 分配数组通过 fread 一次性读入；\n2. 按 score 降序排列（同分按 id 升序）；\n3. 将排序结果以 `id name score` 格式逐行写入文本文件 \"report.txt\"，并在标准输出打印最高分记录的 `name score`，最后释放内存并关闭全部文件。",
            "guidance": [
                "1. 严格按题目定义 Record 结构体（int id; char name[16]; int score;），保证 sizeof(Record) 为 24 字节且与文件布局一致。",
                "2. 以 \"rb\" 打开 \"records.bin\"：fseek(fp, 0L, SEEK_END) + ftell(fp) 得到字节数，除以 sizeof(Record) 得条数 n，再 rewind(fp) 并 fread(arr, sizeof(Record), n, fp)。",
                "3. 排序后以 \"w\" 打开 \"report.txt\"，用 fprintf 逐行输出，并在 stdout 打印 arr[0].name 与 arr[0].score。",
                "4. 释放 arr 并关闭文件指针。",
            ],
            "sidebar_refs": {
                "functions": [
                    {"name": "fseek / ftell / rewind", "sig": "stdio.h", "desc": "先定位到 SEEK_END 求总字节数，再 rewind 回开头读取。"},
                    {"name": "fread", "sig": "size_t fread(void *ptr, size_t size, size_t count, FILE *fp);", "desc": "读取 n 个 sizeof(Record) 结构体。"},
                    {"name": "malloc / free", "sig": "stdlib.h", "desc": "根据文件实际记录数动态申请数组并释放。"},
                ],
                "structures": [
                    {
                        "name": "结构体定义（严格一致）",
                        "code": "typedef struct {\n    int id;\n    char name[16];\n    int score;\n} Record;",
                    },
                    {
                        "name": "二进制文件测长并动态读入",
                        "code": "fseek(fp, 0L, SEEK_END);\nlong n = ftell(fp) / (long)sizeof(Record);\nrewind(fp);\nRecord *arr = (Record *)malloc((size_t)n * sizeof(Record));\nfread(arr, sizeof(Record), (size_t)n, fp);",
                    },
                ],
            },
            "template": """#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int id;
    char name[16];
    int score;
} Record;

int main(void) {
    // 请完整编写程序

    return 0;
}
""",
            "solution": """#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int id;
    char name[16];
    int score;
} Record;

int main(void) {
    FILE *in = fopen("records.bin", "rb");
    if (in == NULL) return 1;

    fseek(in, 0L, SEEK_END);
    long bytes = ftell(in);
    long n = bytes / (long)sizeof(Record);
    rewind(in);

    Record *arr = (Record *)malloc((size_t)n * sizeof(Record));
    if (arr == NULL) {
        fclose(in);
        return 1;
    }
    fread(arr, sizeof(Record), (size_t)n, in);
    fclose(in);

    for (long i = 0; i < n - 1; i++) {
        long best = i;
        for (long j = i + 1; j < n; j++) {
            if (arr[j].score > arr[best].score ||
                (arr[j].score == arr[best].score && arr[j].id < arr[best].id)) {
                best = j;
            }
        }
        Record t = arr[i];
        arr[i] = arr[best];
        arr[best] = t;
    }

    FILE *out = fopen("report.txt", "w");
    if (out == NULL) {
        free(arr);
        return 1;
    }
    for (long i = 0; i < n; i++) {
        fprintf(out, "%d %s %d\\n", arr[i].id, arr[i].name, arr[i].score);
    }
    fclose(out);

    if (n > 0) {
        printf("%s %d\\n", arr[0].name, arr[0].score);
    }
    free(arr);
    return 0;
}
""",
            "tests": [
                {
                    "name": "4条二进制结构体记录处理",
                    "input_files": {
                        "records.bin": {
                            "type": "binary_struct",
                            "fmt": "<i16si",
                            "rows": [
                                [103, "Charlie", 88],
                                [101, "Alice", 95],
                                [102, "Bob", 95],
                                [104, "David", 72],
                            ],
                        }
                    },
                    "expected_stdout": "Alice 95\n",
                    "expected_files": {
                        "report.txt": {
                            "type": "text",
                            "content": "101 Alice 95\n102 Bob 95\n103 Charlie 88\n104 David 72\n",
                        }
                    },
                }
            ],
        },
        {
            "id": "m9_s6",
            "code": "专项 6",
            "title": "自顶向下模块化综合：排序、中位数与二分定位",
            "kind": "special",
            "editable_default": "partial",
            "summary": "呼应第 15 讲与第 18 讲模块化设计：将排序、奇偶中位数计算与有序二分查找拆分为独立子函数协同完成。",
            "goal": "输入正整数 n（1 <= n <= 200）、n 个整数以及待查目标值 target。\n请分别补全三个职责清晰的子函数：\n1. void sort_asc(int a[], int n)：将数组原地升序排列；\n2. double calc_median(const int a[], int n)：在已升序数组中求中位数（n 为奇数取 a[n/2]；n 为偶数取 (a[n/2 - 1] + a[n/2]) / 2.0）；\n3. int bin_search(const int a[], int n, int target)：在已升序数组中二分查找 target，找到返回任意匹配下标（本题保证无重复元素），找不到返回 -1。\n主函数将输出：排序后的数组、中位数（保留 1 位小数）与查找结果下标。",
            "guidance": [
                "1. sort_asc：使用插入排序、选择排序或冒泡排序均可，确保升序。",
                "2. calc_median：注意当 n 为偶数时必须用 / 2.0 进行浮点除法，避免整数除法截断 0.5。",
                "3. bin_search：维护闭区间 [l, r]，循环条件 l <= r，mid = l + (r - l) / 2，避免越界或死循环。",
            ],
            "sidebar_refs": {
                "functions": [
                    {"name": "sort_asc / calc_median / bin_search", "sig": "模块化子函数划分", "desc": "每个函数仅负责单一子任务，通过参数与返回值传递数据，避免依赖全局变量。"},
                ],
                "structures": [
                    {
                        "name": "奇偶中位数计算模板",
                        "code": "if (n % 2 == 1) return (double)a[n / 2];\nreturn (a[n / 2 - 1] + a[n / 2]) / 2.0;",
                    },
                    {
                        "name": "闭区间二分查找模板",
                        "code": "int l = 0, r = n - 1;\nwhile (l <= r) {\n    int mid = l + (r - l) / 2;\n    if (a[mid] == target) return mid;\n    else if (a[mid] < target) l = mid + 1;\n    else r = mid - 1;\n}\nreturn -1;",
                    },
                ],
            },
            "template": """#include <stdio.h>

void sort_asc(int a[], int n);
double calc_median(const int a[], int n);
int bin_search(const int a[], int n, int target);

// <<<EDIT_START>>>
// 请在此实现 sort_asc、calc_median 与 bin_search 三个函数

// <<<EDIT_END>>>

int main(void) {
    int n, a[200], target;
    scanf("%d", &n);
    for (int i = 0; i < n; i++) scanf("%d", &a[i]);
    scanf("%d", &target);

    sort_asc(a, n);
    for (int i = 0; i < n; i++) {
        if (i > 0) putchar(' ');
        printf("%d", a[i]);
    }
    printf("\\n%.1f\\n%d\\n", calc_median(a, n), bin_search(a, n, target));
    return 0;
}
""",
            "solution": """#include <stdio.h>

void sort_asc(int a[], int n);
double calc_median(const int a[], int n);
int bin_search(const int a[], int n, int target);

void sort_asc(int a[], int n) {
    for (int i = 1; i < n; i++) {
        int key = a[i];
        int j = i - 1;
        while (j >= 0 && a[j] > key) {
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = key;
    }
}

double calc_median(const int a[], int n) {
    if (n % 2 == 1) {
        return (double)a[n / 2];
    }
    return (a[n / 2 - 1] + a[n / 2]) / 2.0;
}

int bin_search(const int a[], int n, int target) {
    int l = 0, r = n - 1;
    while (l <= r) {
        int mid = l + (r - l) / 2;
        if (a[mid] == target) return mid;
        else if (a[mid] < target) l = mid + 1;
        else r = mid - 1;
    }
    return -1;
}

int main(void) {
    int n, a[200], target;
    scanf("%d", &n);
    for (int i = 0; i < n; i++) scanf("%d", &a[i]);
    scanf("%d", &target);

    sort_asc(a, n);
    for (int i = 0; i < n; i++) {
        if (i > 0) putchar(' ');
        printf("%d", a[i]);
    }
    printf("\\n%.1f\\n%d\\n", calc_median(a, n), bin_search(a, n, target));
    return 0;
}
""",
            "tests": [
                {"name": "偶数长度含小数中位数", "stdin": "6\n9 2 7 4 1 6\n7\n", "expected_stdout": "1 2 4 6 7 9\n5.0\n4\n"},
                {"name": "偶数长度.5中位数且未命中", "stdin": "4\n10 3 6 1\n5\n", "expected_stdout": "1 3 6 10\n4.5\n-1\n"},
                {"name": "奇数长度", "stdin": "5\n8 -2 5 0 3\n-2\n", "expected_stdout": "-2 0 3 5 8\n3.0\n0\n"},
            ],
        },
        {
            "id": "m9_s7",
            "code": "专项 7",
            "title": "指针与内存综合：二维动态矩阵转置与行极值统计",
            "kind": "special",
            "editable_default": "full",
            "summary": "打通二级指针动态内存分配、函数封装（alloc/transpose/free）与矩阵统计输出。",
            "goal": "输入正整数 r、c（1 <= r, c <= 50）以及 r 行 c 列的整数矩阵。\n请编写模块化函数完成：\n1. alloc_matrix(int r, int c)：用二级指针 int ** 动态分配 r 行 c 列矩阵；\n2. transpose_matrix(int **mat, int r, int c)：动态分配并返回 c 行 r 列的转置矩阵；\n3. free_matrix(int **mat, int r)：按正确顺序释放 r 行二维动态数组；\n第一行输出原矩阵每一行的最大值（空格分隔）；随后 c 行输出转置矩阵（每行元素空格分隔），最后释放两块二维动态内存。",
            "guidance": [
                "1. alloc_matrix 返回 int **：先 malloc(r * sizeof(int *))，再循环对每行 malloc(c * sizeof(int))。",
                "2. transpose_matrix 调用 alloc_matrix(c, r) 得到 res，填入 res[j][i] = mat[i][j] 并返回 res。",
                "3. free_matrix 先循环 free(mat[i])，再 free(mat)。",
                "4. 原矩阵和转置矩阵行数分别为 r 和 c，释放时分别传入对应行数。",
            ],
            "sidebar_refs": {
                "functions": [
                    {"name": "malloc / free", "sig": "#include <stdlib.h>", "desc": "成对分配与释放二维动态数组。"},
                ],
                "structures": [
                    {
                        "name": "二维动态矩阵分配与释放函数",
                        "code": "int **alloc_matrix(int r, int c) {\n    int **m = (int **)malloc((size_t)r * sizeof(int *));\n    for (int i = 0; i < r; i++)\n        m[i] = (int *)malloc((size_t)c * sizeof(int));\n    return m;\n}\nvoid free_matrix(int **m, int r) {\n    for (int i = 0; i < r; i++) free(m[i]);\n    free(m);\n}",
                    },
                ],
            },
            "template": """#include <stdio.h>
#include <stdlib.h>

// 请完整编写 alloc_matrix、transpose_matrix、free_matrix 与 main

int main(void) {
    return 0;
}
""",
            "solution": """#include <stdio.h>
#include <stdlib.h>

int **alloc_matrix(int r, int c) {
    int **m = (int **)malloc((size_t)r * sizeof(int *));
    if (m == NULL) return NULL;
    for (int i = 0; i < r; i++) {
        m[i] = (int *)malloc((size_t)c * sizeof(int));
        if (m[i] == NULL) return NULL;
    }
    return m;
}

int **transpose_matrix(int **mat, int r, int c) {
    int **t = alloc_matrix(c, r);
    if (t == NULL) return NULL;
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            t[j][i] = mat[i][j];
        }
    }
    return t;
}

void free_matrix(int **m, int r) {
    if (m == NULL) return;
    for (int i = 0; i < r; i++) {
        free(m[i]);
    }
    free(m);
}

int main(void) {
    int r, c;
    scanf("%d %d", &r, &c);
    int **mat = alloc_matrix(r, c);
    if (mat == NULL) return 1;

    for (int i = 0; i < r; i++) {
        int rmax = 0;
        for (int j = 0; j < c; j++) {
            scanf("%d", &mat[i][j]);
            if (j == 0 || mat[i][j] > rmax) rmax = mat[i][j];
        }
        if (i > 0) putchar(' ');
        printf("%d", rmax);
    }
    putchar('\\n');

    int **tr = transpose_matrix(mat, r, c);
    if (tr == NULL) {
        free_matrix(mat, r);
        return 1;
    }
    for (int i = 0; i < c; i++) {
        for (int j = 0; j < r; j++) {
            if (j > 0) putchar(' ');
            printf("%d", tr[i][j]);
        }
        putchar('\\n');
    }

    free_matrix(tr, c);
    free_matrix(mat, r);
    return 0;
}
""",
            "code_checks": [
                {"pattern": r"\bmalloc\s*\(", "message": "请使用 malloc 动态分配二维矩阵。"},
                {"pattern": r"\bfree\s*\(", "message": "请使用 free 释放动态分配的二维矩阵。"},
            ],
            "tests": [
                {
                    "name": "2x3矩阵行极值与转置",
                    "stdin": "2 3\n3 -1 5\n2 9 4\n",
                    "expected_stdout": "5 9\n3 2\n-1 9\n5 4\n",
                },
                {
                    "name": "3x2全负数矩阵",
                    "stdin": "3 2\n-5 -2\n-9 -4\n-1 -8\n",
                    "expected_stdout": "-2 -4 -1\n-5 -9 -1\n-2 -4 -8\n",
                },
            ],
        },
    ],
}
