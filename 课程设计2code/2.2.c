#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHUNKSIZE 80                               // 可由用户定义的块大小
#define MAX_LINE 200                               // 设文件每行不超过200字
#define ACTIVE_CAPACITY 4                          // 活区每次从输入文件读入一段的行数

typedef struct Chunk {                             // 结点结构：同一行的串用定长结构
    char ch[CHUNKSIZE];
    struct Chunk *next;                            // 行和行之间用指针相联接
} Chunk;

typedef struct {                                   // 串的链表结构（整个文本编辑区活区）
    Chunk *head, *tail;                            // 串的头和尾指针
    int curlen;                                    // 串的当前长度（当前活区行数）
} LString;

void InitActive(LString *S) {                      // 初始化空活区
    S->head = S->tail = NULL;
    S->curlen = 0;
}

void InsertLine(LString *S, int lineNo, const char *text) { // 在活区第lineNo行之后插入一行
    if (lineNo < 0) lineNo = 0;
    if (lineNo > S->curlen) lineNo = S->curlen;
    Chunk *q = (Chunk*)malloc(sizeof(Chunk));
    strncpy(q->ch, text, CHUNKSIZE - 1);
    q->ch[CHUNKSIZE - 1] = '\0';
    q->next = NULL;
    if (lineNo == 0) {                             // 插入在活区最前（第0行之后）
        q->next = S->head;
        S->head = q;
        if (!S->tail) S->tail = q;
    } else {
        Chunk *p = S->head;
        for (int i = 1; i < lineNo; ++i) p = p->next;
        q->next = p->next;
        p->next = q;
        if (S->tail == p) S->tail = q;
    }
    S->curlen++;
}

void DeleteLines(LString *S, int line1, int line2) { // (2) 行删除：删除第line1行到第line2行
    if (line1 < 1 || line2 > S->curlen || line1 > line2) return;
    Chunk *prev = NULL, *p = S->head;
    for (int i = 1; i < line1; ++i) { prev = p; p = p->next; }
    for (int i = line1; i <= line2; ++i) {
        Chunk *del = p;
        p = p->next;
        free(del);
        S->curlen--;
    }
    if (prev) prev->next = p; else S->head = p;
    if (!p) S->tail = prev;
}

void PrintActive(LString S) {                      // (4) 活区显示：p<回车>
    printf("--- 当前活区内容 (curlen=%d) ---\n", S.curlen);
    int i = 1;
    for (Chunk *p = S.head; p; p = p->next, ++i)
        printf("%2d: %s\n", i, p->ch);
}

void NextActive(LString *S, FILE *fpIn, FILE *fpOut) { // (3) 活区切换：n<回车>
    for (Chunk *p = S->head, *q; p; p = q) {       // 将当前活区写入输出文件并释放
        q = p->next;
        fprintf(fpOut, "%s\n", p->ch);
        free(p);
    }
    InitActive(S);
    char buf[MAX_LINE + 2];
    while (S->curlen < ACTIVE_CAPACITY && fgets(buf, sizeof(buf), fpIn)) {
        buf[strcspn(buf, "\r\n")] = '\0';
        InsertLine(S, S->curlen, buf);             // 从输入文件读入下一段作为新活区
    }
}

void ExecuteCommands(LString *S, FILE *fpIn, FILE *fpOut, FILE *fpCmd) {
    char cmd[MAX_LINE + 2], buf[MAX_LINE + 2];
    while (fgets(cmd, sizeof(cmd), fpCmd)) {
        cmd[strcspn(cmd, "\r\n")] = '\0';
        if (!cmd[0]) continue;
        printf("\n>>> 执行命令: %s\n", cmd);
        if (cmd[0] == 'i') {                       // (1) 行插入：i<行号><回车><文本> .<回车>
            int lineNo = atoi(cmd + 1);
            while (fgets(buf, sizeof(buf), fpCmd)) {
                buf[strcspn(buf, "\r\n")] = '\0';
                printf("    %s\n", buf);
                if (strcmp(buf, ".") == 0) break;  // 遇 .<回车> 结束插入
                InsertLine(S, lineNo++, buf);
            }
        } else if (cmd[0] == 'd') {                // (2) 行删除：d<行号1>[ <行号2>]<回车>
            int r1, r2;
            if (sscanf(cmd + 1, "%d %d", &r1, &r2) == 1) r2 = r1;
            DeleteLines(S, r1, r2);
        } else if (cmd[0] == 'n') {                // (3) 活区切换：n<回车>
            NextActive(S, fpIn, fpOut);
        } else if (cmd[0] == 'p') {                // (4) 活区显示：p<回车>
            PrintActive(*S);
        }
    }
}

int main() {
    FILE *fp = fopen("editor_in.txt", "w");        // 准备待编辑的源文本文件
    fprintf(fp, "Line 1: Data Structure Course Design\n"
                "Line 2: Linear List Set Operations\n"
                "Line 3: Redundant Line To Be Deleted\n"
                "Line 4: LString Active Buffer Demo\n"
                "Line 5: LSD Radix Sort Dictionary\n"
                "Line 6: Balanced Binary Tree (AVL)\n");
    fclose(fp);

    FILE *fc = fopen("editor_cmd.txt", "w");       // 准备4条基本编辑命令测试序列
    fprintf(fc, "p\n"
                "i2\n"
                "Inserted A: Line after Line 2\n"
                "Inserted B: Second inserted line\n"
                ".\n"
                "p\n"
                "d5\n"
                "p\n"
                "d3 4\n"
                "p\n"
                "n\n"
                "p\n"
                "n\n");
    fclose(fc);

    FILE *fpIn = fopen("editor_in.txt", "r");
    FILE *fpOut = fopen("editor_out.txt", "w");
    FILE *fpCmd = fopen("editor_cmd.txt", "r");
    LString active;
    InitActive(&active);
    NextActive(&active, fpIn, fpOut);              // 读入第一段文本至活区
    ExecuteCommands(&active, fpIn, fpOut, fpCmd);
    fclose(fpIn); fclose(fpOut); fclose(fpCmd);

    printf("\n=== 最终保存到文件 editor_out.txt 的内容 ===\n");
    fpOut = fopen("editor_out.txt", "r");
    char line[MAX_LINE + 2];
    while (fgets(line, sizeof(line), fpOut)) printf("%s", line);
    fclose(fpOut);
    return 0;
}
