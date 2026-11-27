"""Module 6: Structs, linked lists, unions, and enums (Chapter 9)."""

MODULE = {
    "id": "m6",
    "title": "模块 6：结构体、链表与复合类型",
    "subtitle": "对应第 9 章",
    "badge": "数据结构",
    "exercises": [
        {
            "id": "m6_1",
            "code": "6.1",
            "title": "结构体定义、typedef 与 . / -> 访问",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握 struct 与 typedef 定义新类型，区分普通变量用点 .、结构体指针用箭头 -> 访问成员。",
            "goal": "补全函数 add_bonus(Student *s, int bonus)：通过指针 s 给学生成绩 score 加上 bonus（上限封顶为 100），再输出学生姓名与最终成绩。",
            "param_docs": [
                {
                    "name": "struct / typedef / ->",
                    "header": "C 语言自定义类型（第 9.1～9.3、9.5.3 节）",
                    "prototype": "typedef struct { int id; char name[20]; int score; } Student;",
                    "purpose": "将多个不同类型的相关数据项组合成一个整体类型；typedef 为该结构体类型起简短别名。",
                    "params": [
                        {"name": "stu.score", "type": "点运算符 .", "desc": "用于结构体变量本身访问其成员。"},
                        {"name": "p->score", "type": "箭头运算符 ->", "desc": "用于指向结构体的指针访问其成员，等价于 (*p).score。"},
                    ],
                    "returns": "结构体作为函数参数时，传指针可避免整块复制并允许修改原结构体。",
                }
            ],
            "template": """#include <stdio.h>

typedef struct {
    char name[32];
    int score;
} Student;

void add_bonus(Student *s, int bonus) {
    // 使用 -> 运算符给 s 指向的学生加分，超过 100 则截断为 100
// <<<EDIT_START>>>
    
// <<<EDIT_END>>>
}

int main(void) {
    Student stu;
    int bonus;
    scanf("%31s %d %d", stu.name, &stu.score, &bonus);
    add_bonus(&stu, bonus);
    printf("%s %d\\n", stu.name, stu.score);
    return 0;
}
""",
            "solution": """#include <stdio.h>

typedef struct {
    char name[32];
    int score;
} Student;

void add_bonus(Student *s, int bonus) {
    s->score += bonus;
    if (s->score > 100) {
        s->score = 100;
    }
}

int main(void) {
    Student stu;
    int bonus;
    scanf("%31s %d %d", stu.name, &stu.score, &bonus);
    add_bonus(&stu, bonus);
    printf("%s %d\\n", stu.name, stu.score);
    return 0;
}
""",
            "code_checks": [
                {"pattern": r"->", "message": "请使用结构体指针成员运算符 -> 访问 score。"},
            ],
            "tests": [
                {"name": "正常加分", "stdin": "Alice 85 10\n", "expected_stdout": "Alice 95\n"},
                {"name": "超过100截断", "stdin": "Bob 96 8\n", "expected_stdout": "Bob 100\n"},
            ],
        },
        {
            "id": "m6_2",
            "code": "6.2",
            "title": "结构体数组与多关键字排序",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握结构体数组的整体赋值交换及按主/次字段比较规则。",
            "goal": "输入 n 个学生的姓名与成绩（1 <= n <= 50），补全排序代码：按成绩从高到低排列；若成绩相同，按姓名字典序从小到大排列。",
            "param_docs": [
                {
                    "name": "结构体数组排序",
                    "header": "C 语言结构体数组（第 9.3 节）",
                    "prototype": "Student list[50];  Student tmp = list[i]; list[i] = list[j]; list[j] = tmp;",
                    "purpose": "同一类型的结构体变量可以直接使用 = 进行整体成员复制，便于排序交换。",
                    "params": [
                        {"name": "比较条件", "type": "主关键字 + 次关键字", "desc": "先比较 list[j].score > list[best].score；若成绩相等，再用 strcmp(list[j].name, list[best].name) < 0 比较姓名。"},
                    ],
                    "returns": "排序完成后按顺序输出各元素。",
                }
            ],
            "template": """#include <stdio.h>
#include <string.h>

typedef struct {
    char name[32];
    int score;
} Student;

int main(void) {
    int n;
    Student a[50];
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        scanf("%31s %d", a[i].name, &a[i].score);
    }

    // <<<EDIT_START>>>
    // 按成绩降序排列；成绩相同时按姓名升序排列

    // <<<EDIT_END>>>

    for (int i = 0; i < n; i++) {
        printf("%s %d\\n", a[i].name, a[i].score);
    }
    return 0;
}
""",
            "solution": """#include <stdio.h>
#include <string.h>

typedef struct {
    char name[32];
    int score;
} Student;

int main(void) {
    int n;
    Student a[50];
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        scanf("%31s %d", a[i].name, &a[i].score);
    }

    for (int i = 0; i < n - 1; i++) {
        int best = i;
        for (int j = i + 1; j < n; j++) {
            if (a[j].score > a[best].score ||
                (a[j].score == a[best].score && strcmp(a[j].name, a[best].name) < 0)) {
                best = j;
            }
        }
        Student tmp = a[i];
        a[i] = a[best];
        a[best] = tmp;
    }

    for (int i = 0; i < n; i++) {
        printf("%s %d\\n", a[i].name, a[i].score);
    }
    return 0;
}
""",
            "tests": [
                {
                    "name": "含同分次关键字",
                    "stdin": "4\nTom 90\nAlice 95\nBob 90\nDave 80\n",
                    "expected_stdout": "Alice 95\nBob 90\nTom 90\nDave 80\n",
                },
            ],
        },
        {
            "id": "m6_3",
            "code": "6.3",
            "title": "单链表建立与遍历（尾插法）",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握链表结点结构、动态创建结点、维护 head/tail 指针建表与遍历释放。",
            "goal": "补全尾插法建表函数 build_list(int n)：依次读入 n 个整数并按原顺序构成单链表，返回头指针 head。",
            "param_docs": [
                {
                    "name": "单链表结点与尾插法",
                    "header": "第 9.4.2 节（需 stdlib.h）",
                    "prototype": "typedef struct Node { int val; struct Node *next; } Node;",
                    "purpose": "通过结构体内部指向同类型的指针 next 将动态分配的结点串联成线性表。",
                    "params": [
                        {"name": "head", "type": "Node *", "desc": "指向链表第一个结点的指针；空表时为 NULL。"},
                        {"name": "tail", "type": "Node *", "desc": "尾插法中记录当前最后一个结点，便于 O(1) 追加新结点。"},
                        {"name": "node->next = NULL", "type": "必须初始化", "desc": "新建结点的 next 必须显式置为 NULL，作为链表结束标志。"},
                    ],
                    "returns": "返回建好链表的头指针 head。",
                }
            ],
            "template": """#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int val;
    struct Node *next;
} Node;

Node *build_list(int n) {
    // 依次读入 n 个整数，用尾插法建立单链表并返回头指针
// <<<EDIT_START>>>
    return NULL;
// <<<EDIT_END>>>
}

int main(void) {
    int n;
    scanf("%d", &n);
    Node *head = build_list(n);
    for (Node *p = head; p != NULL; p = p->next) {
        if (p != head) putchar(' ');
        printf("%d", p->val);
    }
    putchar('\\n');
    while (head != NULL) {
        Node *tmp = head;
        head = head->next;
        free(tmp);
    }
    return 0;
}
""",
            "solution": """#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int val;
    struct Node *next;
} Node;

Node *build_list(int n) {
    Node *head = NULL, *tail = NULL;
    for (int i = 0; i < n; i++) {
        Node *node = (Node *)malloc(sizeof(Node));
        if (node == NULL) break;
        scanf("%d", &node->val);
        node->next = NULL;
        if (head == NULL) {
            head = tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
    }
    return head;
}

int main(void) {
    int n;
    scanf("%d", &n);
    Node *head = build_list(n);
    for (Node *p = head; p != NULL; p = p->next) {
        if (p != head) putchar(' ');
        printf("%d", p->val);
    }
    putchar('\\n');
    while (head != NULL) {
        Node *tmp = head;
        head = head->next;
        free(tmp);
    }
    return 0;
}
""",
            "code_checks": [
                {"pattern": r"\bmalloc\s*\(", "message": "请使用 malloc 动态分配链表结点。"},
            ],
            "tests": [
                {"name": "常规建表", "stdin": "4\n10 20 30 40\n", "expected_stdout": "10 20 30 40\n"},
                {"name": "单结点", "stdin": "1\n99\n", "expected_stdout": "99\n"},
            ],
        },
        {
            "id": "m6_4",
            "code": "6.4",
            "title": "单链表结点的删除操作",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握删除链表指定值结点时处理头结点与中间/尾结点的指针断链重连与 free。",
            "goal": "补全 delete_all(Node *head, int target)：删除链表中所有 val 等于 target 的结点（并 free 释放内存），返回更新后的头指针。",
            "param_docs": [
                {
                    "name": "链表删除（双指针 / 哨兵头结点）",
                    "header": "第 9.4.3 节",
                    "prototype": "Node dummy = {0, head}; Node *prev = &dummy; Node *cur = head;",
                    "purpose": "删除结点时需修改其前驱结点的 next 指针指向 cur->next，再释放 cur。",
                    "params": [
                        {"name": "dummy 哨兵结点", "type": "栈上临时 Node", "desc": "令 dummy.next = head 后，头结点也有了前驱 prev，无需为删除头结点单独写分支。"},
                        {"name": "断链顺序", "type": "操作顺序", "desc": "先执行 prev->next = cur->next 保存后续链表，再执行 free(cur)，不能先 free 再读 cur->next。"},
                    ],
                    "returns": "返回 dummy.next 作为可能已变化的新头指针。",
                }
            ],
            "template": """#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int val;
    struct Node *next;
} Node;

Node *delete_all(Node *head, int target) {
    // 删除所有 val == target 的结点并 free，返回新头指针
// <<<EDIT_START>>>
    return head;
// <<<EDIT_END>>>
}

int main(void) {
    int n, target;
    scanf("%d", &n);
    Node *head = NULL, *tail = NULL;
    for (int i = 0; i < n; i++) {
        Node *node = (Node *)malloc(sizeof(Node));
        scanf("%d", &node->val);
        node->next = NULL;
        if (!head) head = tail = node;
        else { tail->next = node; tail = node; }
    }
    scanf("%d", &target);

    head = delete_all(head, target);

    if (head == NULL) {
        printf("EMPTY\\n");
    } else {
        for (Node *p = head; p != NULL; p = p->next) {
            if (p != head) putchar(' ');
            printf("%d", p->val);
        }
        putchar('\\n');
    }
    while (head) { Node *t = head; head = head->next; free(t); }
    return 0;
}
""",
            "solution": """#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int val;
    struct Node *next;
} Node;

Node *delete_all(Node *head, int target) {
    Node dummy;
    dummy.next = head;
    Node *prev = &dummy;
    Node *cur = head;
    while (cur != NULL) {
        if (cur->val == target) {
            prev->next = cur->next;
            free(cur);
            cur = prev->next;
        } else {
            prev = cur;
            cur = cur->next;
        }
    }
    return dummy.next;
}

int main(void) {
    int n, target;
    scanf("%d", &n);
    Node *head = NULL, *tail = NULL;
    for (int i = 0; i < n; i++) {
        Node *node = (Node *)malloc(sizeof(Node));
        scanf("%d", &node->val);
        node->next = NULL;
        if (!head) head = tail = node;
        else { tail->next = node; tail = node; }
    }
    scanf("%d", &target);

    head = delete_all(head, target);

    if (head == NULL) {
        printf("EMPTY\\n");
    } else {
        for (Node *p = head; p != NULL; p = p->next) {
            if (p != head) putchar(' ');
            printf("%d", p->val);
        }
        putchar('\\n');
    }
    while (head) { Node *t = head; head = head->next; free(t); }
    return 0;
}
""",
            "code_checks": [
                {"pattern": r"\bfree\s*\(", "message": "删除结点时请调用 free 释放被删结点内存。"},
            ],
            "tests": [
                {"name": "删除头尾和中间", "stdin": "6\n5 1 5 2 3 5\n5\n", "expected_stdout": "1 2 3\n"},
                {"name": "全部删除", "stdin": "3\n7 7 7\n7\n", "expected_stdout": "EMPTY\n"},
                {"name": "无匹配项", "stdin": "3\n1 2 3\n9\n", "expected_stdout": "1 2 3\n"},
            ],
        },
        {
            "id": "m6_5",
            "code": "6.5",
            "title": "枚举 enum 与共用体 union",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握 enum 命名整型常量及 union 多成员共享同一块内存空间的用法。",
            "goal": "程序使用带类型标签（enum）的共用体（union）存储一个整数或双精度浮点数。补全 print_value 函数：当 kind == VAL_INT 时按 %d 输出 data.i；当 kind == VAL_DOUBLE 时按 %.2f 输出 data.d。",
            "param_docs": [
                {
                    "name": "enum 与 union",
                    "header": "第 9.5.1～9.5.2 节",
                    "prototype": "enum Kind { VAL_INT = 0, VAL_DOUBLE = 1 };  union Data { int i; double d; };",
                    "purpose": "enum 为一组有限离散整数起可读名称；union 允许不同类型成员共享同一段内存，同一时刻仅一个成员有效。",
                    "params": [
                        {"name": "enum 枚举常量", "type": "int 常量", "desc": "默认从 0 开始依次递增，可直接用于 switch 的 case 标签。"},
                        {"name": "union 内存大小", "type": "共享存储", "desc": "sizeof(union) 至少等于最大成员的字节数；通常需要配合一个外部 enum 字段记录当前存的是哪种成员。"},
                    ],
                    "returns": "无返回值。",
                }
            ],
            "template": """#include <stdio.h>

typedef enum {
    VAL_INT = 1,
    VAL_DOUBLE = 2
} ValKind;

typedef struct {
    ValKind kind;
    union {
        int i;
        double d;
    } data;
} Box;

void print_box(const Box *b) {
    // 根据 b->kind 输出 INT:值 或 DOUBLE:值(保留两位小数)，末尾换行
// <<<EDIT_START>>>
    
// <<<EDIT_END>>>
}

int main(void) {
    int k;
    Box b;
    scanf("%d", &k);
    b.kind = (ValKind)k;
    if (b.kind == VAL_INT) scanf("%d", &b.data.i);
    else scanf("%lf", &b.data.d);
    print_box(&b);
    return 0;
}
""",
            "solution": """#include <stdio.h>

typedef enum {
    VAL_INT = 1,
    VAL_DOUBLE = 2
} ValKind;

typedef struct {
    ValKind kind;
    union {
        int i;
        double d;
    } data;
} Box;

void print_box(const Box *b) {
    switch (b->kind) {
        case VAL_INT:
            printf("INT:%d\\n", b->data.i);
            break;
        case VAL_DOUBLE:
            printf("DOUBLE:%.2f\\n", b->data.d);
            break;
    }
}

int main(void) {
    int k;
    Box b;
    scanf("%d", &k);
    b.kind = (ValKind)k;
    if (b.kind == VAL_INT) scanf("%d", &b.data.i);
    else scanf("%lf", &b.data.d);
    print_box(&b);
    return 0;
}
""",
            "tests": [
                {"name": "整数分支", "stdin": "1 42\n", "expected_stdout": "INT:42\n"},
                {"name": "浮点分支", "stdin": "2 3.14159\n", "expected_stdout": "DOUBLE:3.14\n"},
            ],
        },
        {
            "id": "m6_stage",
            "code": "★ 阶段 6",
            "title": "阶段综合：有序单链表的插入、删除与遍历",
            "kind": "stage",
            "editable_default": "full",
            "summary": "综合编写链表建表、有序插入、按值删除、输出和全表释放。",
            "goal": "输入操作次数 q，随后 q 行命令：\n- `I x`：将整数 x 插入到始终保持**非降序（升序）**的单链表中；\n- `D x`：删除链表中**第一个**值等于 x 的结点（若不存在则忽略）；\n所有命令执行完毕后，按顺序输出链表中所有整数（空格分隔；若为空输出 EMPTY），并释放全部动态内存。",
            "guidance": [
                "1. 定义结点 struct Node { int val; struct Node *next; };，使用局部哨兵结点 dummy（dummy.next = NULL）可统一处理空表和表头操作。",
                "2. 有序插入 I x：从 prev = &dummy 出发，只要 prev->next != NULL && prev->next->val < x 就令 prev = prev->next；然后分配新结点，设 node->next = prev->next; prev->next = node;。",
                "3. 删除首个 D x：从 prev = &dummy 出发寻找 prev->next->val == x，找到后断链并 free 随即 break。",
                "4. 最后遍历 dummy.next 输出所有值，并用循环释放所有结点。",
            ],
            "sidebar_refs": {
                "functions": [
                    {"name": "malloc / free", "sig": "#include <stdlib.h>", "desc": "插入时 malloc 分配结点，删除和程序结束时 free 释放。"},
                ],
                "structures": [
                    {
                        "name": "哨兵结点有序插入模板",
                        "code": "Node *prev = &dummy;\nwhile (prev->next != NULL && prev->next->val < x) {\n    prev = prev->next;\n}\nNode *node = (Node *)malloc(sizeof(Node));\nnode->val = x;\nnode->next = prev->next;\nprev->next = node;",
                    },
                    {
                        "name": "整条链表安全释放模板",
                        "code": "Node *p = dummy.next;\nwhile (p != NULL) {\n    Node *nxt = p->next;\n    free(p);\n    p = nxt;\n}",
                    },
                ],
            },
            "template": """#include <stdio.h>
#include <stdlib.h>

// 请完整编写有序单链表程序

int main(void) {
    return 0;
}
""",
            "solution": """#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int val;
    struct Node *next;
} Node;

int main(void) {
    int q;
    scanf("%d", &q);

    Node dummy = {0, NULL};
    for (int i = 0; i < q; i++) {
        char cmd;
        int x;
        scanf(" %c %d", &cmd, &x);
        if (cmd == 'I') {
            Node *prev = &dummy;
            while (prev->next != NULL && prev->next->val < x) {
                prev = prev->next;
            }
            Node *node = (Node *)malloc(sizeof(Node));
            if (node == NULL) return 1;
            node->val = x;
            node->next = prev->next;
            prev->next = node;
        } else if (cmd == 'D') {
            Node *prev = &dummy;
            while (prev->next != NULL) {
                if (prev->next->val == x) {
                    Node *del = prev->next;
                    prev->next = del->next;
                    free(del);
                    break;
                }
                prev = prev->next;
            }
        }
    }

    if (dummy.next == NULL) {
        printf("EMPTY\\n");
    } else {
        for (Node *p = dummy.next; p != NULL; p = p->next) {
            if (p != dummy.next) putchar(' ');
            printf("%d", p->val);
        }
        putchar('\\n');
    }

    Node *p = dummy.next;
    while (p != NULL) {
        Node *nxt = p->next;
        free(p);
        p = nxt;
    }
    return 0;
}
""",
            "code_checks": [
                {"pattern": r"\bmalloc\s*\(", "message": "请使用动态分配结点实现链表。"},
                {"pattern": r"\bfree\s*\(", "message": "请使用 free 释放链表内存。"},
            ],
            "tests": [
                {
                    "name": "混合插入与删除",
                    "stdin": "7\nI 30\nI 10\nI 20\nI 20\nD 20\nD 99\nI 5\n",
                    "expected_stdout": "5 10 20 30\n",
                },
                {
                    "name": "全部删空",
                    "stdin": "4\nI 8\nI 3\nD 3\nD 8\n",
                    "expected_stdout": "EMPTY\n",
                },
            ],
        },
    ],
}
