#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OK 1
#define ERROR 0
#define OVERFLOW -2

#define CHUNKSIZE 80       // 块大小（课件指定定长80字符）
#define MAX_LINE_LEN 200   // 每行不超过200字
#define PAGE_LINES 10      // 活区每次从输入文件读取的最大行数

// 块链结点结构定义（题设指定结构）
typedef struct Chunk {
    char ch[CHUNKSIZE];
    struct Chunk *next;
} Chunk;

// 串的链表结构定义（题设指定结构）
typedef struct {
    Chunk *head, *tail; // 串的头和尾指针
    int curlen;         // 串的当前长度
} LString;

// 行结点结构定义（行和行之间用指针相联接）
typedef struct LineNode {
    LString text;           // 行文本（采用块链串存储）
    struct LineNode *next;  // 指向下一行
} LineNode;

// 文本编辑器活区上下文结构
typedef struct {
    LineNode *head; // 活区链表头结点
    int lineCount;  // 活区当前行数
    char inFilename[64];
    char outFilename[64];
    FILE *inFp;
    FILE *outFp;
} Editor;

// 初始化块链串
void InitString(LString *S) {
    S->head = S->tail = NULL;
    S->curlen = 0;
}

// 清空并释放块链串
void ClearString(LString *S) {
    Chunk *p = S->head;
    while (p) {
        Chunk *q = p->next;
        free(p);
        p = q;
    }
    S->head = S->tail = NULL;
    S->curlen = 0;
}

// 用 C 字符串为块链串赋值
int StrAssign(LString *S, const char *chars) {
    ClearString(S);
    int len = (int)strlen(chars);
    if (len == 0) return OK;

    S->curlen = len;
    int pos = 0;
    while (pos < len) {
        Chunk *chk = (Chunk *)malloc(sizeof(Chunk));
        if (!chk) exit(OVERFLOW);
        chk->next = NULL;

        int copyLen = len - pos;
        if (copyLen > CHUNKSIZE) copyLen = CHUNKSIZE;

        memcpy(chk->ch, chars + pos, copyLen);
        // 未填满部分补空格
        for (int i = copyLen; i < CHUNKSIZE; i++) {
            chk->ch[i] = ' ';
        }

        if (!S->head) {
            S->head = S->tail = chk;
        } else {
            S->tail->next = chk;
            S->tail = chk;
        }
        pos += copyLen;
    }
    return OK;
}

// 提取块链串中的有效字符到普通缓冲区中
void StringToCString(LString S, char *buf) {
    int idx = 0;
    Chunk *p = S.head;
    while (p && idx < S.curlen) {
        int remain = S.curlen - idx;
        int take = remain < CHUNKSIZE ? remain : CHUNKSIZE;
        for (int i = 0; i < take; i++) {
            buf[idx++] = p->ch[i];
        }
        p = p->next;
    }
    buf[idx] = '\0';
}

// 初始化编辑器活区
void InitEditor(Editor *ed, const char *inFile, const char *outFile) {
    ed->head = (LineNode *)malloc(sizeof(LineNode));
    if (!ed->head) exit(OVERFLOW);
    InitString(&(ed->head->text));
    ed->head->next = NULL;
    ed->lineCount = 0;

    strncpy(ed->inFilename, inFile, sizeof(ed->inFilename) - 1);
    strncpy(ed->outFilename, outFile, sizeof(ed->outFilename) - 1);

    ed->inFp = fopen(inFile, "r");
    ed->outFp = fopen(outFile, "w");
}

// (4) 活区显示命令: p<回车>
void Command_p(Editor *ed) {
    printf("--- [活区当前内容 (共 %d 行)] ---\n", ed->lineCount);
    if (ed->lineCount == 0) {
        printf("    (活区为空)\n");
    } else {
        LineNode *curr = ed->head->next;
        int lineNo = 1;
        char buf[MAX_LINE_LEN + 1];
        while (curr) {
            StringToCString(curr->text, buf);
            printf("%3d: %s\n", lineNo++, buf);
            curr = curr->next;
        }
    }
    printf("----------------------------------\n");
}

// (1) 行插入命令: i<行号><回车><文本*>.<回车>
// 将 <文本> 插入活区中第 <行号> 行之后（行号为0表示插入在首行之前）
void Command_i(Editor *ed, int lineNo, const char *lines[], int numLines) {
    if (lineNo < 0 || lineNo > ed->lineCount) {
        printf("提示: 行号 %d 超出范围[0, %d]，自动调整为在尾部追加\n", lineNo, ed->lineCount);
        lineNo = ed->lineCount;
    }

    // 寻找到第 lineNo 个结点
    LineNode *p = ed->head;
    for (int i = 0; i < lineNo && p; i++) {
        p = p->next;
    }

    LineNode *after = p->next;
    for (int i = 0; i < numLines; i++) {
        LineNode *newNode = (LineNode *)malloc(sizeof(LineNode));
        if (!newNode) exit(OVERFLOW);
        InitString(&(newNode->text));
        StrAssign(&(newNode->text), lines[i]);

        p->next = newNode;
        p = newNode;
        ed->lineCount++;
    }
    p->next = after;
    printf("成功在第 %d 行后插入 %d 行文本。\n", lineNo, numLines);
}

// (2) 行删除命令: d<行号1>[<空格><行号2>]<回车>
// 删除活区中第 <行号1> 行（到第 <行号2> 行）
void Command_d(Editor *ed, int line1, int line2) {
    if (line1 < 1 || line1 > ed->lineCount) {
        printf("错误: 起始行号 %d 无效 (当前有效行 1~%d)\n", line1, ed->lineCount);
        return;
    }
    if (line2 < line1) line2 = line1;
    if (line2 > ed->lineCount) line2 = ed->lineCount;

    // 寻找第 line1 - 1 个结点
    LineNode *prev = ed->head;
    for (int i = 1; i < line1; i++) {
        prev = prev->next;
    }

    int delCount = line2 - line1 + 1;
    for (int i = 0; i < delCount; i++) {
        LineNode *toDelete = prev->next;
        prev->next = toDelete->next;
        ClearString(&(toDelete->text));
        free(toDelete);
        ed->lineCount--;
    }
    printf("成功删除第 %d 行至第 %d 行 (共删除 %d 行)。\n", line1, line2, delCount);
}

// (3) 活区切换命令: n<回车>
// 将活区写入输出文件，并从输入文件中读入下一段，作为新的活区
void Command_n(Editor *ed) {
    // 1. 将当前活区写入输出文件
    if (ed->outFp) {
        LineNode *curr = ed->head->next;
        char buf[MAX_LINE_LEN + 1];
        while (curr) {
            StringToCString(curr->text, buf);
            fprintf(ed->outFp, "%s\n", buf);
            curr = curr->next;
        }
        fflush(ed->outFp);
        printf("已将当前活区 (%d 行) 写入输出文件 \"%s\"。\n", ed->lineCount, ed->outFilename);
    }

    // 2. 清空活区
    LineNode *p = ed->head->next;
    while (p) {
        LineNode *q = p->next;
        ClearString(&(p->text));
        free(p);
        p = q;
    }
    ed->head->next = NULL;
    ed->lineCount = 0;

    // 3. 从输入文件中读取下一段作为新活区
    if (!ed->inFp || feof(ed->inFp)) {
        printf("输入文件已无更多内容可读，活区已清空。\n");
        return;
    }

    char lineBuf[MAX_LINE_LEN + 1];
    int loaded = 0;
    LineNode *tail = ed->head;
    while (loaded < PAGE_LINES && fgets(lineBuf, sizeof(lineBuf), ed->inFp)) {
        // 去除末尾换行符
        size_t len = strlen(lineBuf);
        while (len > 0 && (lineBuf[len - 1] == '\r' || lineBuf[len - 1] == '\n')) {
            lineBuf[--len] = '\0';
        }

        LineNode *newNode = (LineNode *)malloc(sizeof(LineNode));
        if (!newNode) exit(OVERFLOW);
        InitString(&(newNode->text));
        StrAssign(&(newNode->text), lineBuf);
        newNode->next = NULL;

        tail->next = newNode;
        tail = newNode;
        ed->lineCount++;
        loaded++;
    }
    printf("成功从输入文件读入下一段 (%d 行) 作为新活区。\n", loaded);
}

// 关闭并保存编辑器文件
void CloseEditor(Editor *ed) {
    // 退出前将活区剩余内容追加至输出文件
    if (ed->outFp) {
        LineNode *curr = ed->head->next;
        char buf[MAX_LINE_LEN + 1];
        while (curr) {
            StringToCString(curr->text, buf);
            fprintf(ed->outFp, "%s\n", buf);
            curr = curr->next;
        }
        // 若输入文件中仍有未读完的后续内容，全部顺次拷贝至输出文件（防止数据丢失）
        if (ed->inFp && !feof(ed->inFp)) {
            char lineBuf[MAX_LINE_LEN + 1];
            while (fgets(lineBuf, sizeof(lineBuf), ed->inFp)) {
                fputs(lineBuf, ed->outFp);
            }
        }
        fclose(ed->outFp);
        ed->outFp = NULL;
    }
    if (ed->inFp) {
        fclose(ed->inFp);
        ed->inFp = NULL;
    }

    // 释放活区内存
    LineNode *p = ed->head;
    while (p) {
        LineNode *q = p->next;
        ClearString(&(p->text));
        free(p);
        p = q;
    }
    ed->head = NULL;
    ed->lineCount = 0;
}

// 交互式命令解析执行循环（支持题目规定的 i, d, n, p 及退出保存命令 q）
void InteractiveEditor(Editor *ed) {
    printf("\n=== 进入简易文本编辑器交互模式 ===\n");
    printf("支持命令说明（格式严格符合题设）：\n");
    printf("  i<行号>           : 在指定行后插入文本，随后逐行输入内容，单起一行输入 \".\" 结束输入\n");
    printf("  d<行1> [<行2>]   : 删除指定行（或行区间，例如 d 2 或 d 2 4）\n");
    printf("  n                 : 活区切换（写出当前活区并读入下一段）\n");
    printf("  p                 : 活区显示（打印当前活区所有行文本）\n");
    printf("  q                 : 保存并退出编辑器\n");
    printf("==================================\n");

    char cmdLine[MAX_LINE_LEN + 1];
    while (1) {
        printf("Editor> ");
        if (!fgets(cmdLine, sizeof(cmdLine), stdin)) break;

        char *cmd = cmdLine;
        while (*cmd == ' ' || *cmd == '\t') cmd++;
        size_t len = strlen(cmd);
        while (len > 0 && (cmd[len - 1] == '\r' || cmd[len - 1] == '\n')) cmd[--len] = '\0';
        if (len == 0) continue;

        char op = cmd[0];
        if (op == 'p' || op == 'P') {
            Command_p(ed);
        } else if (op == 'n' || op == 'N') {
            Command_n(ed);
        } else if (op == 'd' || op == 'D') {
            int line1 = 0, line2 = 0;
            int matched = sscanf(cmd + 1, "%d %d", &line1, &line2);
            if (matched == 1) {
                Command_d(ed, line1, line1);
            } else if (matched == 2) {
                Command_d(ed, line1, line2);
            } else {
                printf("命令格式错误！示例: d 2 或 d 2 4\n");
            }
        } else if (op == 'i' || op == 'I') {
            int lineNo = 0;
            if (sscanf(cmd + 1, "%d", &lineNo) != 1) {
                printf("命令格式错误！示例: i 2\n");
                continue;
            }
            printf("请输入待插入文本（每行一条，单起一行输入 \".\" 结束输入）：\n");
            char textBuf[50][MAX_LINE_LEN + 1];
            const char *linesPtr[50];
            int inputCount = 0;
            char inLine[MAX_LINE_LEN + 1];
            while (inputCount < 50 && fgets(inLine, sizeof(inLine), stdin)) {
                size_t l = strlen(inLine);
                while (l > 0 && (inLine[l - 1] == '\r' || inLine[l - 1] == '\n')) inLine[--l] = '\0';
                if (strcmp(inLine, ".") == 0) break;
                strncpy(textBuf[inputCount], inLine, MAX_LINE_LEN);
                textBuf[inputCount][MAX_LINE_LEN] = '\0';
                linesPtr[inputCount] = textBuf[inputCount];
                inputCount++;
            }
            if (inputCount > 0) {
                Command_i(ed, lineNo, linesPtr, inputCount);
            }
        } else if (op == 'q' || op == 'Q' || op == 'e' || op == 'E') {
            printf("保存活区并退出编辑器...\n");
            break;
        } else {
            printf("未知命令: '%c'，请输入 i, d, n, p 或 q\n", op);
        }
    }
}

int main(int argc, char *argv[]) {
    printf("====================================================\n");
    printf("   题目二：简易文本编辑器（基于LString块链存储）\n");
    printf("====================================================\n\n");

    const char *inFile = "test_input.txt";
    const char *outFile = "test_output.txt";

    // 准备一段测试输入文件
    FILE *fp = fopen(inFile, "w");
    if (fp) {
        fprintf(fp, "Section 1: Data structures provide structured data storage.\n");
        fprintf(fp, "Section 1: Linked list is a dynamic linear data structure.\n");
        fprintf(fp, "Section 2: Stack and Queue represent LIFO and FIFO policies.\n");
        fprintf(fp, "Section 2: Block-chain string improves storage utilization.\n");
        fclose(fp);
    }

    Editor ed;
    InitEditor(&ed, inFile, outFile);

    if (argc > 1 && (strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "--interactive") == 0)) {
        // 键盘实时交互编辑模式
        Command_n(&ed);
        InteractiveEditor(&ed);
        CloseEditor(&ed);
    } else {
        // 默认自动化脚本演示模式（生成符合报告规范的标准输出）
        // 1. 演示活区切换 n（从输入文件加载第 1 段内容）
        printf("1. 执行活区切换命令: n (从输入文件读入初始文本段)\n");
        Command_n(&ed);
        printf("\n");

        // 2. 演示活区显示命令: p
        printf("2. 执行活区显示命令: p\n");
        Command_p(&ed);
        printf("\n");

        // 3. 演示行插入命令: i<行号> (在第 2 行后插入新文本)
        printf("3. 执行行插入命令: i 2 (在第 2 行之后插入 2 行新内容)\n");
        const char *insertText[] = {
            "[Inserted Line A] This line is stored using LString block chain.",
            "[Inserted Line B] Chunk size is 80 characters."
        };
        Command_i(&ed, 2, insertText, 2);
        printf("插入后查看活区内容:\n");
        Command_p(&ed);
        printf("\n");

        // 4. 演示行删除命令: d<行号1> <行号2>
        printf("4. 执行行删除命令: d 1 2 (删除第 1 行到第 2 行)\n");
        Command_d(&ed, 1, 2);
        printf("删除后查看活区内容:\n");
        Command_p(&ed);
        printf("\n");

        // 5. 再次演示行删除单行: d 2
        printf("5. 执行单行删除命令: d 2 (删除第 2 行)\n");
        Command_d(&ed, 2, 2);
        printf("删除后查看活区内容:\n");
        Command_p(&ed);
        printf("\n");

        // 6. 最终文本保存至文件
        printf("6. 结束编辑并保存最终文本到输出文件 \"%s\"...\n", outFile);
        CloseEditor(&ed);
        printf("文件保存完成。输出文件内容如下：\n");

        FILE *outCheck = fopen(outFile, "r");
        if (outCheck) {
            char ch;
            while ((ch = fgetc(outCheck)) != EOF) {
                putchar(ch);
            }
            fclose(outCheck);
        }
        printf("\n提示：运行程序添加参数 -i (如: ./2_text_editor -i) 可启动键盘实时交互编辑模式。\n");
    }

    printf("\n====================================================\n");
    return 0;
}
