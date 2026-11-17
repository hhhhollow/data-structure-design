/* 课程设计 2.2  文本编辑器（块链结构 LString）
 * 每行文本作为一个 Chunk 结点，行间通过指针连接。
 * 支持命令：
 *   i<行号>           行插入：随后输入多行文本，单独一行 . 结束（0 插到最前）
 *   d<行号1>[ <行号2>] 行删除：删除第 行号1 行（到第 行号2 行）
 *   n                 活区切换：写入 editor_out.txt 并从 editor_in.txt 读入下一段
 *   p                 活区显示：遍历链表打印当前活区各行及行号
 *   q                 保存当前活区及输入文件剩余行到 editor_out.txt 并退出
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHUNKSIZE 80 // 可由用户定义的块大小
typedef struct Chunk { // 结点结构
    char ch[CHUNKSIZE];
    struct Chunk *next;
} Chunk;

typedef struct { // 串的链表结构
    Chunk *head, *tail; // 串的头和尾指针
    int curlen;         // 串的当前长度
} LString;

static const int EDITOR_PAGESIZE = 5; // 每次从输入文件读入的最大行数

static void InitLString(LString *S) {
    S->head = S->tail = NULL;
    S->curlen = 0;
}

static void ClearLString(LString *S) {
    Chunk *p = S->head;
    while (p) {
        Chunk *q = p->next;
        free(p);
        p = q;
    }
    S->head = S->tail = NULL;
    S->curlen = 0;
}

static Chunk *CreateChunk(const char *line) {
    Chunk *node = (Chunk *)malloc(sizeof(Chunk));
    if (!node) return NULL;
    strncpy(node->ch, line, CHUNKSIZE - 1);
    node->ch[CHUNKSIZE - 1] = '\0';
    node->next = NULL;
    return node;
}

static void AppendChunk(LString *S, const char *line) {
    Chunk *node = CreateChunk(line);
    if (!node) return;
    if (S->tail) {
        S->tail->next = node;
    } else {
        S->head = node;
    }
    S->tail = node;
    S->curlen++;
}

static int ReadEditorLine(FILE *fp, char *buf, int size) {
    if (!fp || !fgets(buf, size, fp)) return 0;
    if (!strchr(buf, '\n')) {
        int ch;
        while ((ch = fgetc(fp)) != EOF && ch != '\n') {}
    }
    buf[strcspn(buf, "\r\n")] = '\0';
    return 1;
}

/* 从输入文件读入下一段（最多 EDITOR_PAGESIZE 行）作为新活区 */
static int LoadEditorPage(FILE *in, LString *S) {
    char buf[1024];
    ClearLString(S);
    while (in && S->curlen < EDITOR_PAGESIZE && ReadEditorLine(in, buf, sizeof(buf))) {
        AppendChunk(S, buf);
    }
    return S->curlen;
}

/* 将活区各结点写入输出文件 */
static void SaveLString(FILE *out, const LString *S) {
    if (!out) return;
    for (Chunk *p = S->head; p; p = p->next) {
        fprintf(out, "%s\n", p->ch);
    }
}

/* 遍历链表打印当前活区各行及行号 */
static void PrintLString(const LString *S) {
    printf("活区共 %d 行：\n", S->curlen);
    int line_no = 1;
    for (Chunk *p = S->head; p; p = p->next, ++line_no) {
        printf("%3d: %s\n", line_no, p->ch);
    }
}

/* 在第 pos 行之后插入多行文本（pos == 0 插在最前，单独一行 . 结束） */
static int InsertLinesAfter(LString *S, int pos) {
    Chunk *prev = NULL;
    if (pos > 0) {
        prev = S->head;
        for (int i = 1; i < pos; ++i) {
            prev = prev->next;
        }
    }

    char buf[1024];
    int added = 0;
    printf("请输入插入文本（单独一行 . 结束）：\n");
    while (ReadEditorLine(stdin, buf, sizeof(buf)) && strcmp(buf, ".") != 0) {
        Chunk *node = CreateChunk(buf);
        if (!node) break;
        if (prev == NULL) {
            node->next = S->head;
            S->head = node;
            if (S->tail == NULL) S->tail = node;
        } else {
            node->next = prev->next;
            prev->next = node;
            if (prev == S->tail) S->tail = node;
        }
        prev = node;
        S->curlen++;
        added++;
    }
    return added;
}

/* 删除第 a 行到第 b 行对应的结点并释放 */
static void DeleteLines(LString *S, int a, int b) {
    Chunk *prev = NULL;
    Chunk *cur = S->head;
    for (int i = 1; i < a; ++i) {
        prev = cur;
        cur = cur->next;
    }

    int count = b - a + 1;
    for (int i = 0; cur && i < count; ++i) {
        Chunk *next = cur->next;
        free(cur);
        cur = next;
        S->curlen--;
    }

    if (prev) {
        prev->next = cur;
    } else {
        S->head = cur;
    }
    if (!cur) {
        S->tail = prev;
    }
}

int run_2_2(void) {
    FILE *in = fopen("editor_in.txt", "r");
    FILE *out = fopen("editor_out.txt", "w");
    if (!out) {
        printf("无法创建输出文件 editor_out.txt。\n");
        if (in) fclose(in);
        return 1;
    }

    LString S;
    InitLString(&S);

    printf("===== 二、文本编辑器（块链结构 LString） =====\n");
    printf("命令：i<行号> 插入 | d<行号1>[ <行号2>] 删除 | n 切换活区 | p 显示 | q 退出\n");
    if (!in) {
        printf("提示：未找到 editor_in.txt，当前活区为空。\n");
    } else {
        int got = LoadEditorPage(in, &S);
        printf("已从 editor_in.txt 读入第 1 段：%d 行。\n", got);
    }

    char cmd[1024];
    while (printf("> "), fflush(stdout), ReadEditorLine(stdin, cmd, sizeof(cmd))) {
        char *p = cmd;
        while (*p == ' ' || *p == '\t') ++p;
        if (*p == '\0') continue;

        if (*p == 'i') {
            int k = 0;
            char extra;
            if (sscanf(p + 1, "%d %c", &k, &extra) != 1 || k < 0 || k > S.curlen) {
                printf("插入行号无效（有效范围 0~%d）。\n", S.curlen);
                continue;
            }
            int added = InsertLinesAfter(&S, k);
            printf("已插入 %d 行，活区现有 %d 行。\n", added, S.curlen);
        } else if (*p == 'd') {
            int a = 0, b = 0;
            char extra;
            int cnt = sscanf(p + 1, "%d %d %c", &a, &b, &extra);
            if (cnt != 1 && cnt != 2) {
                printf("删除命令格式错误，请使用 d<行号1>[ <行号2>]。\n");
                continue;
            }
            if (cnt == 1) b = a;
            if (S.curlen == 0 || a < 1 || b > S.curlen || a > b) {
                printf("删除行号无效（当前共 %d 行）。\n", S.curlen);
                continue;
            }
            DeleteLines(&S, a, b);
            printf("已删除第 %d 行到第 %d 行，活区现有 %d 行。\n", a, b, S.curlen);
        } else if (*p == 'n') {
            SaveLString(out, &S);
            int got = LoadEditorPage(in, &S);
            printf("已将活区写入 editor_out.txt，并读入下一段：%d 行。\n", got);
        } else if (*p == 'p') {
            PrintLString(&S);
        } else if (*p == 'q') {
            break;
        } else {
            printf("未知命令，请使用 i/d/n/p/q。\n");
        }
    }

    SaveLString(out, &S);
    while (in && ReadEditorLine(in, cmd, sizeof(cmd))) {
        fprintf(out, "%s\n", cmd);
    }
    ClearLString(&S);
    if (in) fclose(in);
    fclose(out);
    printf("已保存到 editor_out.txt，编辑器退出。\n");
    return 0;
}

#ifndef NO_MAIN
int main(void) { return run_2_2(); }
#endif
