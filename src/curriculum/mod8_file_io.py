"""Module 8: Streams and File I/O (Chapter 11) with full Windows/TCC compatibility."""

MODULE = {
    "id": "m8",
    "title": "模块 8：流与文件操作",
    "subtitle": "对应第 11 章",
    "badge": "文件持久",
    "exercises": [
        {
            "id": "m8_1",
            "code": "8.1",
            "title": "fopen 与 fclose 文件打开和关闭",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握 FILE * 文件指针、fopen 打开模式、判空处理以及 fclose 刷盘关闭。",
            "goal": "补全代码：以只写文本模式打开文件 \"greeting.txt\"，检查文件指针是否为 NULL，向其中写入 \"Hello File\\n\" 后关闭文件；并在标准输出打印 OK。",
            "param_docs": [
                {
                    "name": "fopen",
                    "header": "#include <stdio.h>",
                    "prototype": "FILE *fopen(const char *filename, const char *mode);",
                    "purpose": "以 mode 指定的模式打开名为 filename 的文件，并建立关联的文件流。",
                    "params": [
                        {"name": "filename", "type": "const char *", "desc": "文件名或路径字符串，例如 \"data.txt\"。"},
                        {
                            "name": "mode",
                            "type": "const char *",
                            "desc": "打开模式字符串：\"r\" 只读文本（文件必须存在）；\"w\" 只写文本（不存在则创建，存在则清空）；\"a\" 追加写入；读写二进制文件需加 b，如 \"rb\"、\"wb\"。",
                        },
                    ],
                    "returns": "成功时返回非空 FILE * 指针；打开失败时返回 NULL。",
                    "notes": ["Windows 下读写二进制文件必须带 \"b\"，否则会触发换行符 \\n 与 \\r\\n 的自动转换。"],
                },
                {
                    "name": "fclose",
                    "header": "#include <stdio.h>",
                    "prototype": "int fclose(FILE *fp);",
                    "purpose": "将缓冲区中尚未写入磁盘的数据刷新写入文件，并释放文件流资源。",
                    "params": [{"name": "fp", "type": "FILE *", "desc": "由 fopen 返回的非空文件指针。"}],
                    "returns": "成功返回 0；出错返回 EOF。",
                },
            ],
            "template": """#include <stdio.h>

int main(void) {
    // <<<EDIT_START>>>
    // 1. 使用 fopen 以 "w" 模式打开 "greeting.txt" 并判空
    // 2. 使用 fprintf(fp, "Hello File\\n") 写入文本
    // 3. 调用 fclose(fp) 关闭文件

    // <<<EDIT_END>>>

    printf("OK\\n");
    return 0;
}
""",
            "solution": """#include <stdio.h>

int main(void) {
    FILE *fp = fopen("greeting.txt", "w");
    if (fp == NULL) {
        return 1;
    }
    fprintf(fp, "Hello File\\n");
    fclose(fp);

    printf("OK\\n");
    return 0;
}
""",
            "code_checks": [
                {"pattern": r"\bfopen\s*\(", "message": "请使用 fopen 打开文件。"},
                {"pattern": r"\bfclose\s*\(", "message": "请使用 fclose 关闭文件。"},
            ],
            "tests": [
                {
                    "name": "创建并写入文件",
                    "stdin": "",
                    "expected_stdout": "OK\n",
                    "expected_files": {"greeting.txt": {"type": "text", "content": "Hello File\n"}},
                }
            ],
        },
        {
            "id": "m8_2",
            "code": "8.2",
            "title": "fscanf 与 fprintf 格式化文件读写",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握通过 fscanf 循环读完整个文本文件数据，并用 fprintf 按格式写入结果文件。",
            "goal": "当前目录下已有输入文件 \"scores.txt\"，每行包含一个整数。请用 fscanf 循环读取全部整数直到返回 EOF（或 != 1），计算个数与总和，并用 fprintf 将 \"COUNT=个数 SUM=总和\\n\" 写入 \"result.txt\"。",
            "param_docs": [
                {
                    "name": "fscanf / fprintf",
                    "header": "#include <stdio.h>",
                    "prototype": "int fscanf(FILE *fp, const char *format, ...);  |  int fprintf(FILE *fp, const char *format, ...);",
                    "purpose": "与 scanf / printf 用法相同，仅在第一个参数多出一个指定目标文件流的 FILE *fp。",
                    "params": [
                        {"name": "fp", "type": "FILE *", "desc": "已打开的文件指针（也可传 stdin / stdout）。"},
                        {"name": "format, ...", "type": "格式串与参数", "desc": "fscanf 后面的变量参数同样需要取地址 &。"},
                    ],
                    "returns": "fscanf 返回成功读入的项数（文件结束返回 EOF）；fprintf 返回输出的字符数。",
                    "notes": ["循环读完整文件推荐写法：while (fscanf(in, \"%d\", &x) == 1) { ... }，避免用 while (!feof(in)) 导致末尾多读一次。"],
                }
            ],
            "template": """#include <stdio.h>

int main(void) {
    FILE *in = fopen("scores.txt", "r");
    FILE *out = fopen("result.txt", "w");
    if (in == NULL || out == NULL) {
        if (in) fclose(in);
        if (out) fclose(out);
        return 1;
    }

    int x, count = 0, sum = 0;

    // <<<EDIT_START>>>
    // 1. 使用 while (fscanf(in, "%d", &x) == 1) 统计 count 和 sum
    // 2. 使用 fprintf 向 out 写入 "COUNT=%d SUM=%d\\n"

    // <<<EDIT_END>>>

    fclose(in);
    fclose(out);
    return 0;
}
""",
            "solution": """#include <stdio.h>

int main(void) {
    FILE *in = fopen("scores.txt", "r");
    FILE *out = fopen("result.txt", "w");
    if (in == NULL || out == NULL) {
        if (in) fclose(in);
        if (out) fclose(out);
        return 1;
    }

    int x, count = 0, sum = 0;

    while (fscanf(in, "%d", &x) == 1) {
        count++;
        sum += x;
    }
    fprintf(out, "COUNT=%d SUM=%d\\n", count, sum);

    fclose(in);
    fclose(out);
    return 0;
}
""",
            "code_checks": [
                {"pattern": r"\bfscanf\s*\(", "message": "请使用 fscanf 从文件读取数据。"},
                {"pattern": r"\bfprintf\s*\(", "message": "请使用 fprintf 向文件写入结果。"},
            ],
            "tests": [
                {
                    "name": "常规多行数字文件",
                    "input_files": {"scores.txt": {"type": "text", "content": "80\n90\n85\n95\n"}},
                    "expected_files": {"result.txt": {"type": "text", "content": "COUNT=4 SUM=350\n"}},
                },
                {
                    "name": "单行空格分隔数字",
                    "input_files": {"scores.txt": {"type": "text", "content": "10 20 30\n"}},
                    "expected_files": {"result.txt": {"type": "text", "content": "COUNT=3 SUM=60\n"}},
                },
            ],
        },
        {
            "id": "m8_3",
            "code": "8.3",
            "title": "fgetc / fputc 与 fgets / fputs 文本读写",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握逐字符文件复制转换及用 int 接收 fgetc 返回值判断 EOF。",
            "goal": "读取文本文件 \"src.txt\" 中的全部字符，将其中所有小写字母转为大写后写入 \"dst.txt\"（其他字符原样保留）。",
            "param_docs": [
                {
                    "name": "fgetc / fputc",
                    "header": "#include <stdio.h>",
                    "prototype": "int fgetc(FILE *fp);  |  int fputc(int ch, FILE *fp);",
                    "purpose": "fgetc 从文件流读取下一个字符；fputc 向文件流写入单个字符。",
                    "params": [
                        {"name": "fp", "type": "FILE *", "desc": "已打开的文件流指针。"},
                        {"name": "ch", "type": "int", "desc": "要写入的字符。"},
                    ],
                    "returns": "fgetc 读到末尾返回 EOF（必须用 int 变量接收，不能用 char）；fputc 成功返回写入字符，失败返回 EOF。",
                    "notes": ["按行读写可使用 fgets(buf, sizeof(buf), fp) 与 fputs(buf, fp)；feof(fp) 仅在尝试越过文件尾读取失败后才返回非 0。"],
                }
            ],
            "template": """#include <stdio.h>

int main(void) {
    FILE *in = fopen("src.txt", "r");
    FILE *out = fopen("dst.txt", "w");
    if (in == NULL || out == NULL) {
        if (in) fclose(in);
        if (out) fclose(out);
        return 1;
    }

    // <<<EDIT_START>>>
    // 使用 int ch; while ((ch = fgetc(in)) != EOF) 逐字符转换并 fputc 到 out

    // <<<EDIT_END>>>

    fclose(in);
    fclose(out);
    return 0;
}
""",
            "solution": """#include <stdio.h>

int main(void) {
    FILE *in = fopen("src.txt", "r");
    FILE *out = fopen("dst.txt", "w");
    if (in == NULL || out == NULL) {
        if (in) fclose(in);
        if (out) fclose(out);
        return 1;
    }

    int ch;
    while ((ch = fgetc(in)) != EOF) {
        if (ch >= 'a' && ch <= 'z') {
            ch = ch - ('a' - 'A');
        }
        fputc(ch, out);
    }

    fclose(in);
    fclose(out);
    return 0;
}
""",
            "code_checks": [
                {"pattern": r"\bfgetc\s*\(", "message": "请使用 fgetc 读取字符。"},
                {"pattern": r"\bfputc\s*\(", "message": "请使用 fputc 写入字符。"},
            ],
            "tests": [
                {
                    "name": "多行大小写转换",
                    "input_files": {"src.txt": {"type": "text", "content": "Hello World!\nc language 101\n"}},
                    "expected_files": {"dst.txt": {"type": "text", "content": "HELLO WORLD!\nC LANGUAGE 101\n"}},
                }
            ],
        },
        {
            "id": "m8_4",
            "code": "8.4",
            "title": "fread 与 fwrite 二进制数据块读写",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握二进制模式 \"rb\" / \"wb\" 及 fread / fwrite 的四个参数含义。",
            "goal": "二进制文件 \"nums.bin\" 中顺序存放了 5 个 int 整数。请用 \"rb\" 打开并使用 fread 读入数组 arr[5]，将每个元素乘以 2 后，用 \"wb\" 打开 \"doubled.bin\" 并用 fwrite 写入全部 5 个整数。",
            "param_docs": [
                {
                    "name": "fread / fwrite",
                    "header": "#include <stdio.h>",
                    "prototype": "size_t fread(void *ptr, size_t size, size_t count, FILE *fp);\nsize_t fwrite(const void *ptr, size_t size, size_t count, FILE *fp);",
                    "purpose": "按原始字节块从二进制文件读取或写入连续的 count 个数据项（数组或结构体）。",
                    "params": [
                        {"name": "ptr", "type": "void * / const void *", "desc": "存放数据或待写出数据的内存缓冲区首地址。"},
                        {"name": "size", "type": "size_t", "desc": "单个数据项的字节大小，例如 sizeof(int) 或 sizeof(Student)。"},
                        {"name": "count", "type": "size_t", "desc": "计划读/写的数据项个数。"},
                        {"name": "fp", "type": "FILE *", "desc": "以二进制模式（\"rb\" / \"wb\"）打开的文件指针。"},
                    ],
                    "returns": "返回实际成功读/写的数据项个数（不是字节数）。",
                    "notes": ["Windows 下二进制读写必须指定 \"rb\" / \"wb\"，防止字节 0x0A 或 0x1A 被文本模式错误转换。"],
                }
            ],
            "template": """#include <stdio.h>

int main(void) {
    int arr[5] = {0};

    // <<<EDIT_START>>>
    // 1. 以 "rb" 打开 "nums.bin"，用 fread 读取 5 个 int 到 arr，然后 fclose
    // 2. 将 arr[0..4] 每个元素乘以 2
    // 3. 以 "wb" 打开 "doubled.bin"，用 fwrite 写入 5 个 int，然后 fclose

    // <<<EDIT_END>>>
    return 0;
}
""",
            "solution": """#include <stdio.h>

int main(void) {
    int arr[5] = {0};

    FILE *in = fopen("nums.bin", "rb");
    if (in == NULL) return 1;
    fread(arr, sizeof(int), 5, in);
    fclose(in);

    for (int i = 0; i < 5; i++) {
        arr[i] *= 2;
    }

    FILE *out = fopen("doubled.bin", "wb");
    if (out == NULL) return 1;
    fwrite(arr, sizeof(int), 5, out);
    fclose(out);
    return 0;
}
""",
            "code_checks": [
                {"pattern": r"\bfread\s*\(", "message": "请使用 fread 读取二进制数据块。"},
                {"pattern": r"\bfwrite\s*\(", "message": "请使用 fwrite 写入二进制数据块。"},
            ],
            "tests": [
                {
                    "name": "5个int二进制读写",
                    "input_files": {"nums.bin": {"type": "binary_ints", "values": [1, -3, 10, 26, 100]}},
                    "expected_files": {"doubled.bin": {"type": "binary_ints", "values": [2, -6, 20, 52, 200]}},
                }
            ],
        },
        {
            "id": "m8_5",
            "code": "8.5",
            "title": "fseek、ftell 与 rewind 随机定位",
            "kind": "single",
            "editable_default": "partial",
            "summary": "掌握二进制文件的随机位置跳转 fseek、获取当前偏移 ftell 与回到开头 rewind。",
            "goal": "二进制文件 \"arr.bin\" 存放了若干个 int 整数（数量未知）。请通过 fseek + ftell 计算文件总字节数与整数个数 n，再读取指定下标 idx（从标准输入读入，0 <= idx < n）对应的整数，输出 \"n 值\"。",
            "param_docs": [
                {
                    "name": "fseek / ftell / rewind",
                    "header": "#include <stdio.h>",
                    "prototype": "int fseek(FILE *fp, long offset, int origin);\nlong ftell(FILE *fp);\nvoid rewind(FILE *fp);",
                    "purpose": "直接移动或查询文件内部的当前读写位置指针，实现随机访问。",
                    "params": [
                        {"name": "fp", "type": "FILE *", "desc": "已以二进制模式（如 \"rb\"）打开的文件指针。"},
                        {"name": "offset", "type": "long", "desc": "相对 origin 的字节偏移量（可正可负），第 idx 个 int 的偏移为 (long)idx * (long)sizeof(int)。"},
                        {"name": "origin", "type": "int", "desc": "基准位置常量：SEEK_SET（文件开头，0）、SEEK_CUR（当前位置，1）、SEEK_END（文件末尾，2）。"},
                    ],
                    "returns": "fseek 成功返回 0，失败返回非 0；ftell 返回当前位置距文件头的字节数；rewind 将位置重置到开头并清除错误/EOF 标志。",
                }
            ],
            "template": """#include <stdio.h>

int main(void) {
    int idx;
    scanf("%d", &idx);

    FILE *fp = fopen("arr.bin", "rb");
    if (fp == NULL) return 1;

    long count = 0;
    int val = 0;

    // <<<EDIT_START>>>
    // 1. fseek 到文件末尾 SEEK_END，用 ftell 计算总字节数并得出 int 个数 count
    // 2. 用 fseek(fp, (long)idx * (long)sizeof(int), SEEK_SET) 定位并 fread 读出 val

    // <<<EDIT_END>>>

    fclose(fp);
    printf("%ld %d\\n", count, val);
    return 0;
}
""",
            "solution": """#include <stdio.h>

int main(void) {
    int idx;
    scanf("%d", &idx);

    FILE *fp = fopen("arr.bin", "rb");
    if (fp == NULL) return 1;

    long count = 0;
    int val = 0;

    fseek(fp, 0L, SEEK_END);
    long total_bytes = ftell(fp);
    count = total_bytes / (long)sizeof(int);
    fseek(fp, (long)idx * (long)sizeof(int), SEEK_SET);
    fread(&val, sizeof(int), 1, fp);

    fclose(fp);
    printf("%ld %d\\n", count, val);
    return 0;
}
""",
            "code_checks": [
                {"pattern": r"\bfseek\s*\(", "message": "请使用 fseek 定位文件位置。"},
                {"pattern": r"\bftell\s*\(", "message": "请使用 ftell 获取文件大小。"},
            ],
            "tests": [
                {
                    "name": "读取第3项(下标2)",
                    "stdin": "2\n",
                    "input_files": {"arr.bin": {"type": "binary_ints", "values": [11, 22, 33, 44, 55, 66]}},
                    "expected_stdout": "6 33\n",
                },
                {
                    "name": "读取末项(下标5)",
                    "stdin": "5\n",
                    "input_files": {"arr.bin": {"type": "binary_ints", "values": [11, 22, 33, 44, 55, 66]}},
                    "expected_stdout": "6 66\n",
                },
            ],
        },
        {
            "id": "m8_stage",
            "code": "★ 阶段 8",
            "title": "阶段综合：学生成绩文件读取、筛选排序与写回",
            "kind": "stage",
            "editable_default": "full",
            "summary": "综合运用文件流、结构体数组、排序与格式化输出，完成完整的文件数据处理流水线。",
            "goal": "当前目录存在文本文件 \"students.txt\"，每行格式为 `姓名 成绩`（姓名不含空格，不超过 30 字符，总人数不超过 100）。\n请编写程序：\n1. 从 \"students.txt\" 读取全部记录；\n2. 筛选出成绩 >= 60 的及格学生，按成绩降序排列（同分按姓名升序）；\n3. 将及格名单按 `姓名 成绩` 格式逐行写入 \"passed.txt\"，最后一行写入 `PASS_COUNT=及格人数 AVG=及格平均分`（保留 2 位小数；若无人及格则仅写 `PASS_COUNT=0 AVG=0.00`）。",
            "guidance": [
                "1. 定义结构体 typedef struct { char name[32]; int score; } Stu;。",
                "2. 以 \"r\" 打开 \"students.txt\"，用 while (fscanf(in, \"%31s %d\", s.name, &s.score) == 2) 读入，只把 s.score >= 60 的记录存入数组 pass_list[]。",
                "3. 对 pass_list[0..m-1] 按成绩降序、同分按 strcmp 升序排序。",
                "4. 以 \"w\" 打开 \"passed.txt\"，逐行 fprintf 输出名单与汇总行，最后关闭两个文件。",
            ],
            "sidebar_refs": {
                "functions": [
                    {"name": "fopen / fclose", "sig": "FILE *fopen(const char *path, const char *mode);", "desc": "文本读取用 \"r\"，文本写入用 \"w\"，必须判空并在末尾关闭。"},
                    {"name": "fscanf / fprintf", "sig": "while (fscanf(in, \"%31s %d\", name, &score) == 2)", "desc": "判断返回值为成功读取的字段数 2，最安全地读到文件尾。"},
                    {"name": "strcmp", "sig": "#include <string.h>", "desc": "同分时比较姓名字典序。"},
                ],
                "structures": [
                    {
                        "name": "文件安全打开关闭模板",
                        "code": "FILE *in = fopen(\"students.txt\", \"r\");\nif (in == NULL) return 1;\n// 读写逻辑...\nfclose(in);",
                    },
                ],
            },
            "template": """#include <stdio.h>
#include <string.h>

// 请完整编写文件筛选排序程序

int main(void) {
    return 0;
}
""",
            "solution": """#include <stdio.h>
#include <string.h>

typedef struct {
    char name[32];
    int score;
} Stu;

int main(void) {
    FILE *in = fopen("students.txt", "r");
    if (in == NULL) return 1;

    Stu list[100], tmp;
    int m = 0, sum = 0;
    while (m < 100 && fscanf(in, "%31s %d", tmp.name, &tmp.score) == 2) {
        if (tmp.score >= 60) {
            list[m++] = tmp;
            sum += tmp.score;
        }
    }
    fclose(in);

    for (int i = 0; i < m - 1; i++) {
        int best = i;
        for (int j = i + 1; j < m; j++) {
            if (list[j].score > list[best].score ||
                (list[j].score == list[best].score && strcmp(list[j].name, list[best].name) < 0)) {
                best = j;
            }
        }
        Stu t = list[i];
        list[i] = list[best];
        list[best] = t;
    }

    FILE *out = fopen("passed.txt", "w");
    if (out == NULL) return 1;
    for (int i = 0; i < m; i++) {
        fprintf(out, "%s %d\\n", list[i].name, list[i].score);
    }
    double avg = (m > 0) ? ((double)sum / m) : 0.0;
    fprintf(out, "PASS_COUNT=%d AVG=%.2f\\n", m, avg);
    fclose(out);
    return 0;
}
""",
            "tests": [
                {
                    "name": "混合及格与不及格",
                    "input_files": {
                        "students.txt": {
                            "type": "text",
                            "content": "Tom 55\nAlice 92\nBob 78\nCindy 92\nDave 40\n",
                        }
                    },
                    "expected_files": {
                        "passed.txt": {
                            "type": "text",
                            "content": "Alice 92\nCindy 92\nBob 78\nPASS_COUNT=3 AVG=87.33\n",
                        }
                    },
                },
                {
                    "name": "全部不及格边界",
                    "input_files": {
                        "students.txt": {"type": "text", "content": "A 30\nB 59\n"}
                    },
                    "expected_files": {
                        "passed.txt": {"type": "text", "content": "PASS_COUNT=0 AVG=0.00\n"}
                    },
                },
            ],
        },
    ],
}
