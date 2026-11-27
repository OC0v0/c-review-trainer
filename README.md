# C 语言程序设计互动复习助手 (`c-review-trainer`)

一款面向 Windows 的**纯原生单文件独立桌面程序**，依据中国科学技术大学教材《计算机程序设计（C 语言版）》（贾伯琪等编著，机械工业出版社）第 2～11 章及配套课程讲义《程序设计与计算思维：基于 C 语言》核心知识体系设计，内置离线 C99 编译器与代码补全引擎，双击即用，无需配置环境。

> 严格面向 **C 语言（C99）** 备考复习，不包含 C++ 内容。

## 功能亮点

- **系统化复习题库（9 大模块 · 66 道精选题）**
  - **模块 1～8**：覆盖格式化 I/O（含宽度/对齐/补零/进制与 `sizeof`）、流程控制与 `<math.h>`（含辗转相除、一元二次方程求根）、数组与字符串（含 `strncpy`/`strstr`、直接插入/选择/冒泡排序与二分查找）、函数递归与预处理（含二维数组传参、`srand`/`rand`、汉诺塔递归）、指针与动态内存（含行指针 `(*p)[N]`、二级指针二维动态数组、指针返回值与 `trim`）、结构体与单链表（含嵌套结构体、头插法/尾插法）、位运算、文本与二进制文件读写（含 `fgets`/`fputs`/`feof`/`fseek`/`ftell`/`rewind`），共 47 道单点补全题 + 12 道阶段综合题。
  - **模块 9（专项强化）**：7 道跨章节考前综合演练（双指针去重旋转、分词边界处理、递归全排列、链表归并反转、结构体二进制读写与报表输出、自顶向下排序/中位数/二分定位、二维动态矩阵转置与统计）。
- **直白清晰的文档与对照排版**
  - 单点题右侧提供无隐喻、直白易懂的函数原型/参数/返回值/注意点说明；综合题与专项题提供分步指导与常用固定结构速查。
  - 题目描述与「示例输入 / 预期输出」分卡独立展示，输入输出按等宽底色块区分，空格与换行一目了然。
- **内置纯 C99 编译评测引擎（离线可用）**
  - 内置 Tiny C Compiler（TCC C99）内存编译执行，零外部编译器依赖。
  - 支持标准输入输出、多行自定义输入试运行（含一键填入示例）、Windows 文本/二进制文件 I/O 沙盒评测、超时死循环保护。
  - 编译失败自动解析报错行号并在编辑器内高亮跳转；输出不匹配时以绿/红底色块对比预期与实际，并定位首处差异行列。
- **原生 Win32 现代界面 + Scintilla 纯 C 编辑器**
  - 默认「仅可编辑空白区」（模板代码与提示注释灰底锁定，可编辑区蓝色竖条标示），支持一键「解锁全部编辑 / 恢复局部编辑」与「重置代码」。
  - 自研纯 C99 实时补全引擎（`c_intellisense.h`）：作用域感知的局部变量/形参/用户函数/宏/枚举补全、`.` 与 `->` 结构体成员补全（支持 `arr[i].` 与嵌套 `b->data.`）、`#include <...>` 标准 C 头文件补全、模糊/缩写匹配与实时函数参数 CallTip。
  - 浅色 / 深色主题切换、四档全局字号（小 / 默认 / 大 / 超大）、左右栏独立显隐、横纵四向分隔条拖拽、目录树「全部 / 未通过 / 综合专项」筛选、可选「闯关模式」与本地自动保存。

## 快速开始（直接下载使用）

前往本仓库的 **[Releases](https://github.com/OC0v0/c-review-trainer/releases)** 页面下载最新版本的 `c-review-trainer.exe`（约 3 MB），放到任意可读写目录中双击即可运行。

- 进度与草稿自动保存在程序同目录的 `.crev_data/progress.json`（若目录不可写则自动回退到用户目录）。
- 可随时点击 **「查看参考写法」** 对照参考实现并一键复制。

## 常用快捷键

| 快捷键 | 功能 |
|---|---|
| `F5` | 编译并试运行（使用自定义输入或默认第 1 组样例） |
| `Ctrl + Enter` | 提交评测全部测试点 |
| `Ctrl + Space` | 手动触发 C99 代码补全 |
| `Enter` / `Tab` | 采纳当前高亮补全项 |
| `Ctrl + /` | 单行 / 多行注释切换 |
| `Ctrl + 滚轮` / `Ctrl + 0` | 缩放编辑器代码字号 / 恢复默认 |
| `Esc` | 关闭补全弹窗 / 参数提示 / 参考实现窗口 |

## 从源码构建（Windows）

### 前置依赖
- Python 3.10+（仅用于导出题库 JSON 与执行打包脚本）
- [Tiny C Compiler 0.9.27 (win32)](https://download.savannah.gnu.org/releases/tinycc/) 解压至 `vendor/tcc/`
- [Scintilla / Lexilla 4.4.6 (win32)](https://www.scintilla.org/) 的 `Scintilla.dll` 与 `Lexilla.dll` 放至 `vendor/scintilla/`

### 一键编译打包

```powershell
# 使用默认 vendor/ 目录构建，输出 output/c-review-trainer.exe
python src/build_exe.py

# 或通过参数 / 环境变量指定自定义路径
python src/build_exe.py --tcc-dir path\to\tcc --sci-dir path\to\scintilla --out output\c-review-trainer.exe
```

### 运行内置全量题库自检

```powershell
vendor\tcc\tcc.exe -Bvendor\tcc -Ivendor\tcc\include -Ivendor\tcc\include\winapi -Isrc\native -Lvendor\tcc\lib -o build\verify_bundle.exe src\native\verify_bundle.c
build\verify_bundle.exe output\c-review-trainer.exe
```

## 项目结构

```text
├── src/
│   ├── curriculum/          # 9 大模块 66 道 C99 题库与测试用例
│   ├── native/
│   │   ├── gui_app.c        # 纯原生 Win32 GUI、自绘目录树、浮动补全弹窗与 Scintilla 集成
│   │   ├── core.h           # 单文件解包、TCC 内存编译执行、文件 I/O 评测与进度持久化
│   │   ├── c_intellisense.h # 纯 C99 词法/作用域解析、结构体成员补全与 CallTip 引擎
│   │   ├── json.h           # 零依赖轻量 C99 JSON 解析与序列化库
│   │   └── verify_bundle.c  # 无界面全量 50 题 + 报错/超时/持久化自动化校验程序
│   ├── export_data.py       # 题库数据统一导出为 curriculum_all.json
│   └── build_exe.py         # TCC 编译 + 多尺寸图标嵌入 + 单文件载荷打包脚本
├── LICENSE                  # MIT License
└── THIRD_PARTY_LICENSES.md  # 内置第三方组件许可说明
```

## 开源协议

本项目源码采用 **[MIT License](LICENSE)** 开源。

Release 单文件二进制包中附带的第三方组件许可说明详见 [THIRD_PARTY_LICENSES.md](THIRD_PARTY_LICENSES.md)。
