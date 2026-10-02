#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define OK 1
#define ERROR 0
#define OVERFLOW -2
#define TRUE 1
#define FALSE 0

// ============================================================================
//                              模块一：批量增删功能
// ============================================================================
typedef int Status;
typedef int ElemType;
typedef struct LNode {
    ElemType data;
    struct LNode *next;
} LNode, *LinkList;

static Status InitList(LinkList *L) {
    *L = (LinkList)malloc(sizeof(LNode));
    if (!*L) exit(OVERFLOW);
    (*L)->next = NULL;
    return OK;
}

static int LocateElem(LinkList L, ElemType e) {
    if (!L) return 0;
    LinkList p = L->next;
    while (p) {
        if (p->data == e) return 1;
        p = p->next;
    }
    return 0;
}

static Status ListInsert_Back(LinkList L, ElemType e) {
    LinkList p = L;
    while (p->next) p = p->next;
    LinkList s = (LinkList)malloc(sizeof(LNode));
    if (!s) exit(OVERFLOW);
    s->data = e;
    s->next = NULL;
    p->next = s;
    return OK;
}

static Status CreateSet(LinkList *T, const char *Str) {
    InitList(T);
    if (!Str || strlen(Str) == 0) return OK;
    const char *p = Str;
    while (*p) {
        while (*p && *p != '-' && (*p < '0' || *p > '9')) p++;
        if (!*p) break;
        char *endptr;
        long val = strtol(p, &endptr, 10);
        if (p == endptr) {
            p++;
            continue;
        }
        if (!LocateElem(*T, (ElemType)val)) {
            ListInsert_Back(*T, (ElemType)val);
        }
        p = endptr;
    }
    return OK;
}

static Status Union(LinkList *T, LinkList S1, LinkList S2) {
    InitList(T);
    LinkList p = S1 ? S1->next : NULL;
    while (p) {
        ListInsert_Back(*T, p->data);
        p = p->next;
    }
    p = S2 ? S2->next : NULL;
    while (p) {
        if (!LocateElem(*T, p->data)) {
            ListInsert_Back(*T, p->data);
        }
        p = p->next;
    }
    return OK;
}

static Status Difference(LinkList *T, LinkList S1, LinkList S2) {
    InitList(T);
    LinkList p = S1 ? S1->next : NULL;
    while (p) {
        if (!LocateElem(S2, p->data)) {
            ListInsert_Back(*T, p->data);
        }
        p = p->next;
    }
    return OK;
}

static void PrintSet(LinkList T) {
    printf("{ ");
    if (T) {
        LinkList p = T->next;
        while (p) {
            printf("%d", p->data);
            if (p->next) printf(", ");
            p = p->next;
        }
    }
    printf(" }\n");
}

static void DestroySet(LinkList *T) {
    if (!T || !*T) return;
    LinkList p = *T;
    while (p) {
        LinkList q = p->next;
        free(p);
        p = q;
    }
    *T = NULL;
}

static void RunModule1(void) {
    printf("\n======================================================================\n");
    printf("   【模块一：批量增删功能演示（单链表集合操作）】\n");
    printf("======================================================================\n");

    LinkList A = NULL, B = NULL, C = NULL;
    LinkList T_add = NULL, T_del = NULL;

    const char *strA = "1 3 5 7 9";
    const char *strB = "2 3 5 8 10";
    const char *strC = "1 5 9 11";

    CreateSet(&A, strA);
    CreateSet(&B, strB);
    CreateSet(&C, strC);

    printf("1. 初始集合建立（调用 CreateSet 自动去重）：\n");
    printf("   原数据集 A:      "); PrintSet(A);
    printf("   新增数据集 B:    "); PrintSet(B);
    printf("   删除数据集 C:    "); PrintSet(C);
    printf("\n");

    printf("2. 批量增加功能（调用 Union(&T, A, B) 求并集 A ∪ B）：\n");
    Union(&T_add, A, B);
    printf("   A ∪ B 结果集:    "); PrintSet(T_add);
    printf("\n");

    printf("3. 批量删除功能（调用 Difference(&T, A, C) 求差集 A - C）：\n");
    Difference(&T_del, A, C);
    printf("   A - C 结果集:    "); PrintSet(T_del);
    printf("\n");

    DestroySet(&A);
    DestroySet(&B);
    DestroySet(&C);
    DestroySet(&T_add);
    DestroySet(&T_del);
}

// ============================================================================
//                              模块二：文本编辑器
// ============================================================================
#define CHUNKSIZE 80
#define MAX_LINE_LEN 200

typedef struct Chunk {
    char ch[CHUNKSIZE];
    struct Chunk *next;
} Chunk;

typedef struct {
    Chunk *head, *tail;
    int curlen;
} LString;

typedef struct LineNode {
    LString text;
    struct LineNode *next;
} LineNode;

typedef struct {
    LineNode *head;
    int lineCount;
} EditorArea;

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

static int StrAssignLString(LString *S, const char *chars) {
    ClearLString(S);
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
        for (int i = copyLen; i < CHUNKSIZE; i++) chk->ch[i] = ' ';
        if (!S->head) S->head = S->tail = chk;
        else { S->tail->next = chk; S->tail = chk; }
        pos += copyLen;
    }
    return OK;
}

static void LStringToCString(LString S, char *buf) {
    int idx = 0;
    Chunk *p = S.head;
    while (p && idx < S.curlen) {
        int remain = S.curlen - idx;
        int take = remain < CHUNKSIZE ? remain : CHUNKSIZE;
        for (int i = 0; i < take; i++) buf[idx++] = p->ch[i];
        p = p->next;
    }
    buf[idx] = '\0';
}

static void InitEditorArea(EditorArea *ed) {
    ed->head = (LineNode *)malloc(sizeof(LineNode));
    if (!ed->head) exit(OVERFLOW);
    InitLString(&(ed->head->text));
    ed->head->next = NULL;
    ed->lineCount = 0;
}

static void CommandP(EditorArea *ed) {
    printf("--- [活区当前内容 (共 %d 行)] ---\n", ed->lineCount);
    if (ed->lineCount == 0) {
        printf("    (活区为空)\n");
    } else {
        LineNode *curr = ed->head->next;
        int lineNo = 1;
        char buf[MAX_LINE_LEN + 1];
        while (curr) {
            LStringToCString(curr->text, buf);
            printf("%3d: %s\n", lineNo++, buf);
            curr = curr->next;
        }
    }
    printf("----------------------------------\n");
}

static void CommandI(EditorArea *ed, int lineNo, const char *lines[], int numLines) {
    if (lineNo < 0 || lineNo > ed->lineCount) lineNo = ed->lineCount;
    LineNode *p = ed->head;
    for (int i = 0; i < lineNo && p; i++) p = p->next;
    LineNode *after = p->next;
    for (int i = 0; i < numLines; i++) {
        LineNode *newNode = (LineNode *)malloc(sizeof(LineNode));
        if (!newNode) exit(OVERFLOW);
        InitLString(&(newNode->text));
        StrAssignLString(&(newNode->text), lines[i]);
        p->next = newNode;
        p = newNode;
        ed->lineCount++;
    }
    p->next = after;
    printf("成功在第 %d 行后插入 %d 行文本。\n", lineNo, numLines);
}

static void CommandD(EditorArea *ed, int line1, int line2) {
    if (line1 < 1 || line1 > ed->lineCount) return;
    if (line2 < line1) line2 = line1;
    if (line2 > ed->lineCount) line2 = ed->lineCount;
    LineNode *prev = ed->head;
    for (int i = 1; i < line1; i++) prev = prev->next;
    int delCount = line2 - line1 + 1;
    for (int i = 0; i < delCount; i++) {
        LineNode *toDelete = prev->next;
        prev->next = toDelete->next;
        ClearLString(&(toDelete->text));
        free(toDelete);
        ed->lineCount--;
    }
    printf("成功删除第 %d 行至第 %d 行 (共删除 %d 行)。\n", line1, line2, delCount);
}

static void CommandN(EditorArea *ed, FILE *inFp, FILE *outFp) {
    if (outFp) {
        LineNode *curr = ed->head->next;
        char buf[MAX_LINE_LEN + 1];
        while (curr) {
            LStringToCString(curr->text, buf);
            fprintf(outFp, "%s\n", buf);
            curr = curr->next;
        }
        fflush(outFp);
        printf("已将当前活区 (%d 行) 写入输出文件。\n", ed->lineCount);
    }
    LineNode *p = ed->head->next;
    while (p) {
        LineNode *q = p->next;
        ClearLString(&(p->text));
        free(p);
        p = q;
    }
    ed->head->next = NULL;
    ed->lineCount = 0;

    if (inFp && !feof(inFp)) {
        char lineBuf[MAX_LINE_LEN + 1];
        int loaded = 0;
        LineNode *tail = ed->head;
        while (loaded < 10 && fgets(lineBuf, sizeof(lineBuf), inFp)) {
            size_t len = strlen(lineBuf);
            while (len > 0 && (lineBuf[len - 1] == '\r' || lineBuf[len - 1] == '\n')) {
                lineBuf[--len] = '\0';
            }
            LineNode *newNode = (LineNode *)malloc(sizeof(LineNode));
            if (!newNode) exit(OVERFLOW);
            InitLString(&(newNode->text));
            StrAssignLString(&(newNode->text), lineBuf);
            newNode->next = NULL;
            tail->next = newNode;
            tail = newNode;
            ed->lineCount++;
            loaded++;
        }
        printf("成功从输入文件读入下一段 (%d 行) 作为新活区。\n", loaded);
    }
}

static void DestroyEditorArea(EditorArea *ed) {
    LineNode *p = ed->head;
    while (p) {
        LineNode *q = p->next;
        ClearLString(&(p->text));
        free(p);
        p = q;
    }
    ed->head = NULL;
    ed->lineCount = 0;
}

static void RunModule2(void) {
    printf("\n======================================================================\n");
    printf("   【模块二：文本编辑器演示（基于LString块链存储）】\n");
    printf("======================================================================\n");

    const char *inFile = "temp_editor_in.txt";
    const char *outFile = "temp_editor_out.txt";

    FILE *fp = fopen(inFile, "w");
    if (fp) {
        fprintf(fp, "Line 1: Computer data structures organize information efficiently.\n");
        fprintf(fp, "Line 2: Single linked list provides dynamic allocation.\n");
        fprintf(fp, "Line 3: Stack is Last-In-First-Out linear structure.\n");
        fprintf(fp, "Line 4: Queue is First-In-First-Out linear structure.\n");
        fclose(fp);
    }

    FILE *inFp = fopen(inFile, "r");
    FILE *outFp = fopen(outFile, "w");
    EditorArea ed;
    InitEditorArea(&ed);

    printf("1. 活区切换命令 n（从输入文件载入首段文本）：\n");
    CommandN(&ed, inFp, outFp);
    printf("\n2. 活区显示命令 p：\n");
    CommandP(&ed);

    printf("\n3. 行插入命令 i（在第 2 行后插入新文本）：\n");
    const char *newLines[] = {
        "[Inserted] LString block size is 80 characters.",
        "[Inserted] Multiple chunks link together seamlessly."
    };
    CommandI(&ed, 2, newLines, 2);
    CommandP(&ed);

    printf("\n4. 行删除命令 d（删除第 1 行到第 2 行）：\n");
    CommandD(&ed, 1, 2);
    CommandP(&ed);

    printf("\n5. 保存活区并写入文件...\n");
    CommandN(&ed, inFp, outFp);

    if (inFp) fclose(inFp);
    if (outFp) fclose(outFp);
    DestroyEditorArea(&ed);

    printf("输出文件内容展示：\n");
    FILE *chk = fopen(outFile, "r");
    if (chk) {
        char ch;
        while ((ch = fgetc(chk)) != EOF) putchar(ch);
        fclose(chk);
    }
    remove(inFile);
    remove(outFile);
}

static void RunModule2Interactive(void) {
    printf("\n======================================================================\n");
    printf("   【模块二：简易文本编辑器（键盘交互编辑模式）】\n");
    printf("======================================================================\n");
    const char *inFile = "temp_interactive_in.txt";
    const char *outFile = "temp_interactive_out.txt";

    FILE *fp = fopen(inFile, "w");
    if (fp) {
        fprintf(fp, "Section 1: Data structures provide structured data storage.\n");
        fprintf(fp, "Section 1: Linked list is a dynamic linear data structure.\n");
        fprintf(fp, "Section 2: Stack and Queue represent LIFO and FIFO policies.\n");
        fprintf(fp, "Section 2: Block-chain string improves storage utilization.\n");
        fclose(fp);
    }

    FILE *inFp = fopen(inFile, "r");
    FILE *outFp = fopen(outFile, "w");
    EditorArea ed;
    InitEditorArea(&ed);

    // 首先读入首段
    CommandN(&ed, inFp, outFp);

    printf("\n=== 键盘交互命令说明（严格符合题设）===\n");
    printf("  i<行号>           : 在指定行后插入文本，随后逐行输入内容，单起一行输入 \".\" 结束输入\n");
    printf("  d<行1> [<行2>]   : 删除指定行（或行区间，例如 d 2 或 d 2 4）\n");
    printf("  n                 : 活区切换（写出当前活区并读入下一段）\n");
    printf("  p                 : 活区显示（打印当前活区所有行文本）\n");
    printf("  q                 : 保存并退出编辑器\n");
    printf("=========================================\n");

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
            CommandP(&ed);
        } else if (op == 'n' || op == 'N') {
            CommandN(&ed, inFp, outFp);
        } else if (op == 'd' || op == 'D') {
            int line1 = 0, line2 = 0;
            int matched = sscanf(cmd + 1, "%d %d", &line1, &line2);
            if (matched == 1) {
                CommandD(&ed, line1, line1);
            } else if (matched == 2) {
                CommandD(&ed, line1, line2);
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
                CommandI(&ed, lineNo, linesPtr, inputCount);
            }
        } else if (op == 'q' || op == 'Q' || op == 'e' || op == 'E') {
            printf("保存活区并退出编辑器...\n");
            break;
        } else {
            printf("未知命令: '%c'，请输入 i, d, n, p 或 q\n", op);
        }
    }

    // 退出前将活区内容写出
    if (outFp) {
        LineNode *curr = ed.head->next;
        char buf[MAX_LINE_LEN + 1];
        while (curr) {
            LStringToCString(curr->text, buf);
            fprintf(outFp, "%s\n", buf);
            curr = curr->next;
        }
        if (inFp && !feof(inFp)) {
            char lineBuf[MAX_LINE_LEN + 1];
            while (fgets(lineBuf, sizeof(lineBuf), inFp)) {
                fputs(lineBuf, outFp);
            }
        }
    }
    if (inFp) fclose(inFp);
    if (outFp) fclose(outFp);
    DestroyEditorArea(&ed);

    printf("最终保存文件内容如下：\n");
    FILE *savedChk = fopen(outFile, "r");
    if (savedChk) {
        char ch;
        while ((ch = fgetc(savedChk)) != EOF) putchar(ch);
        fclose(savedChk);
    }
    remove(inFile);
    remove(outFile);
}

// ============================================================================
//                   模块三：术语词典（LSD链式基数排序 + AVL树查找）
// ============================================================================
#define KEY_NUM 5
#define RADIX 256
#define LH +1
#define EH  0
#define RH -1

typedef struct TermNode {
    char term[32];
    unsigned char keys[KEY_NUM];
    char explanation[128];
    struct TermNode *next;
} TermNode, *TermList;

typedef struct AVLTreeNode {
    char term[32];
    unsigned char keys[KEY_NUM];
    char explanation[128];
    int bf;
    struct AVLTreeNode *lchild, *rchild;
} AVLTreeNode, *AVLTree;

static int KeyCompare(const unsigned char k1[KEY_NUM], const unsigned char k2[KEY_NUM]) {
    for (int i = 0; i < KEY_NUM; i++) {
        if (k1[i] != k2[i]) return (int)k1[i] - (int)k2[i];
    }
    return 0;
}

static void PrintKeys(const unsigned char keys[KEY_NUM]) {
    printf("[");
    for (int i = 0; i < KEY_NUM; i++) {
        if (keys[i] >= 'a' && keys[i] <= 'z') printf("'%c'", keys[i]);
        else printf("%d", keys[i]);
        if (i < KEY_NUM - 1) printf(", ");
    }
    printf("]");
}

static void AppendTerm(TermList *head, const char *term, const char *pinyin, const char *exp) {
    TermNode *s = (TermNode *)malloc(sizeof(TermNode));
    if (!s) exit(OVERFLOW);
    strncpy(s->term, term, sizeof(s->term) - 1);
    s->term[sizeof(s->term) - 1] = '\0';
    strncpy(s->explanation, exp, sizeof(s->explanation) - 1);
    s->explanation[sizeof(s->explanation) - 1] = '\0';
    int len = (int)strlen(pinyin);
    for (int i = 0; i < KEY_NUM; i++) {
        s->keys[i] = (i < len) ? (unsigned char)pinyin[i] : 0;
    }
    s->next = NULL;
    if (!*head) *head = s;
    else {
        TermNode *p = *head;
        while (p->next) p = p->next;
        p->next = s;
    }
}

static void PrintTermList(TermList L) {
    TermNode *p = L;
    int idx = 1;
    while (p) {
        printf("  %d. 术语: %-12s 关键字: ", idx++, p->term);
        PrintKeys(p->keys);
        printf("  释义: %s\n", p->explanation);
        p = p->next;
    }
}

static void Distribute(TermNode *head, int keyIdx, TermNode *front[], TermNode *rear[]) {
    for (int j = 0; j < RADIX; j++) front[j] = rear[j] = NULL;
    TermNode *p = head;
    while (p) {
        TermNode *next = p->next;
        p->next = NULL;
        unsigned char k = p->keys[keyIdx];
        if (!front[k]) {
            front[k] = rear[k] = p;
        } else {
            rear[k]->next = p;
            rear[k] = p;
        }
        p = next;
    }
}

static void Collect(TermList *head, TermNode *front[], TermNode *rear[]) {
    *head = NULL;
    TermNode *last = NULL;
    for (int j = 0; j < RADIX; j++) {
        if (front[j]) {
            if (!*head) *head = front[j];
            else last->next = front[j];
            last = rear[j];
        }
    }
    if (last) last->next = NULL;
}

static void RadixSort(TermList *head) {
    TermNode *front[RADIX];
    TermNode *rear[RADIX];
    for (int i = KEY_NUM - 1; i >= 0; i--) {
        Distribute(*head, i, front, rear);
        Collect(head, front, rear);
    }
}

static void R_Rotate(AVLTree *p) {
    AVLTree lc = (*p)->lchild;
    (*p)->lchild = lc->rchild;
    lc->rchild = *p;
    *p = lc;
}

static void L_Rotate(AVLTree *p) {
    AVLTree rc = (*p)->rchild;
    (*p)->rchild = rc->lchild;
    rc->lchild = *p;
    *p = rc;
}

static void LeftBalance(AVLTree *T) {
    AVLTree lc = (*T)->lchild;
    switch (lc->bf) {
        case LH:
            (*T)->bf = lc->bf = EH;
            R_Rotate(T);
            break;
        case RH: {
            AVLTree rd = lc->rchild;
            switch (rd->bf) {
                case LH: (*T)->bf = RH; lc->bf = EH; break;
                case EH: (*T)->bf = lc->bf = EH; break;
                case RH: (*T)->bf = EH; lc->bf = LH; break;
            }
            rd->bf = EH;
            L_Rotate(&((*T)->lchild));
            R_Rotate(T);
            break;
        }
    }
}

static void RightBalance(AVLTree *T) {
    AVLTree rc = (*T)->rchild;
    switch (rc->bf) {
        case RH:
            (*T)->bf = rc->bf = EH;
            L_Rotate(T);
            break;
        case LH: {
            AVLTree ld = rc->lchild;
            switch (ld->bf) {
                case LH: (*T)->bf = EH; rc->bf = RH; break;
                case EH: (*T)->bf = rc->bf = EH; break;
                case RH: (*T)->bf = LH; rc->bf = EH; break;
            }
            ld->bf = EH;
            R_Rotate(&((*T)->rchild));
            L_Rotate(T);
            break;
        }
    }
}

static int InsertAVL(AVLTree *T, TermNode node, int *taller) {
    if (!*T) {
        *T = (AVLTree)malloc(sizeof(AVLTreeNode));
        if (!*T) exit(OVERFLOW);
        strcpy((*T)->term, node.term);
        memcpy((*T)->keys, node.keys, KEY_NUM);
        strcpy((*T)->explanation, node.explanation);
        (*T)->lchild = (*T)->rchild = NULL;
        (*T)->bf = EH;
        *taller = TRUE;
        return 1;
    }
    int cmp = KeyCompare(node.keys, (*T)->keys);
    if (cmp == 0) {
        *taller = FALSE;
        return 0;
    }
    if (cmp < 0) {
        if (!InsertAVL(&((*T)->lchild), node, taller)) return 0;
        if (*taller) {
            switch ((*T)->bf) {
                case LH: LeftBalance(T); *taller = FALSE; break;
                case EH: (*T)->bf = LH; *taller = TRUE; break;
                case RH: (*T)->bf = EH; *taller = FALSE; break;
            }
        }
    } else {
        if (!InsertAVL(&((*T)->rchild), node, taller)) return 0;
        if (*taller) {
            switch ((*T)->bf) {
                case LH: (*T)->bf = EH; *taller = FALSE; break;
                case EH: (*T)->bf = RH; *taller = TRUE; break;
                case RH: RightBalance(T); *taller = FALSE; break;
            }
        }
    }
    return 1;
}

static int find(AVLTree T, const char *keyStr) {
    unsigned char targetKeys[KEY_NUM] = {0};
    int len = (int)strlen(keyStr);
    for (int i = 0; i < KEY_NUM; i++) {
        targetKeys[i] = (i < len) ? (unsigned char)tolower((unsigned char)keyStr[i]) : 0;
    }
    printf("--> 查找关键字: ");
    PrintKeys(targetKeys);
    printf(" (拼音: \"%s\")\n", keyStr);
    AVLTree p = T;
    while (p) {
        int cmp = KeyCompare(targetKeys, p->keys);
        if (cmp == 0) {
            printf("    【查找成功】术语: %s | 释义: %s\n", p->term, p->explanation);
            return TRUE;
        } else if (cmp < 0) p = p->lchild;
        else p = p->rchild;
    }
    printf("    【查找失败】未找到关键字为 \"%s\" 的术语。\n", keyStr);
    return FALSE;
}

static void InOrderTraverse(AVLTree T) {
    if (T) {
        InOrderTraverse(T->lchild);
        printf("    %-10s 关键字: ", T->term);
        PrintKeys(T->keys);
        printf("  [bf=%2d]\n", T->bf);
        InOrderTraverse(T->rchild);
    }
}

static void DestroyTermList(TermList *head) {
    TermNode *p = *head;
    while (p) {
        TermNode *q = p->next;
        free(p);
        p = q;
    }
    *head = NULL;
}

static void DestroyAVLTree(AVLTree *T) {
    if (*T) {
        DestroyAVLTree(&((*T)->lchild));
        DestroyAVLTree(&((*T)->rchild));
        free(*T);
        *T = NULL;
    }
}

static void RunModule3(void) {
    printf("\n======================================================================\n");
    printf("   【模块三：术语词典系统（LSD链式基数排序 + 平衡二叉树查找）】\n");
    printf("======================================================================\n");

    TermList dictList = NULL;
    AppendTerm(&dictList, "图书",     "ts",    "由纸张、油墨结合的出版物，用于记录与传承信息");
    AppendTerm(&dictList, "数据结构", "sjjg",  "计算机存储、组织数据的方式，相互之间存在特定关系");
    AppendTerm(&dictList, "二叉树",   "ecs",   "每个节点最多有两个子树的树形数据结构");
    AppendTerm(&dictList, "线性表",   "xxb",   "由相同特性的数据元素构成的有限序列");
    AppendTerm(&dictList, "栈",       "z",     "限定仅在表尾进行插入和删除操作的线性表(LIFO)");
    AppendTerm(&dictList, "队列",     "dl",    "只允许在一端插入另一端删除的线性表(FIFO)");
    AppendTerm(&dictList, "平衡树",   "phs",   "任意结点的左右两棵子树高度差绝对值不超过1的树");

    printf("1. 初始未排序的术语词典链表：\n");
    PrintTermList(dictList);

    printf("\n2. 执行基于 LSD 的链式基数排序 (由最低位 keys[4] 到最高位 keys[0])...\n");
    RadixSort(&dictList);
    printf("   排序完成后的有序术语词典链表：\n");
    PrintTermList(dictList);

    printf("\n3. 在排序成果基础上构建二叉平衡树 (AVL Tree)...\n");
    AVLTree dictTree = NULL;
    int taller = FALSE;
    TermNode *p = dictList;
    while (p) {
        InsertAVL(&dictTree, *p, &taller);
        p = p->next;
    }
    printf("   平衡二叉树建立完成。中序遍历验证有序性与平衡因子：\n");
    InOrderTraverse(dictTree);

    printf("\n4. 执行 find(key) 查找功能测试：\n");
    find(dictTree, "ts");
    find(dictTree, "sjjg");
    find(dictTree, "ecs");
    find(dictTree, "xyz");

    DestroyTermList(&dictList);
    DestroyAVLTree(&dictTree);
}

// ============================================================================
//                   模块四：图的使用（迪杰斯特拉单源最短路径）
// ============================================================================
#define MAX_VEX_NUM 20
#define INFINITY 32767

typedef struct {
    char vexs[MAX_VEX_NUM][32];
    int arcs[MAX_VEX_NUM][MAX_VEX_NUM];
    int vexnum, arcnum;
} GraphMatrix;

typedef struct {
    int prev;
    int lowcost;
} PathDist;

static void InitGraph(GraphMatrix *G, int vexnum) {
    G->vexnum = vexnum;
    G->arcnum = 0;
    for (int i = 0; i < vexnum; i++) {
        for (int j = 0; j < vexnum; j++) {
            G->arcs[i][j] = (i == j) ? 0 : INFINITY;
        }
    }
}

static void AddEdge(GraphMatrix *G, int u, int v, int w) {
    G->arcs[u][v] = w;
    G->arcs[v][u] = w;
    G->arcnum++;
}

static void Dijkstra(GraphMatrix G, int v0, PathDist Dist[]) {
    int final[MAX_VEX_NUM];
    for (int v = 0; v < G.vexnum; v++) {
        final[v] = 0;
        Dist[v].lowcost = G.arcs[v0][v];
        Dist[v].prev = (Dist[v].lowcost < INFINITY && v != v0) ? v0 : -1;
    }
    Dist[v0].lowcost = 0;
    Dist[v0].prev = -1;
    final[v0] = 1;

    for (int i = 1; i < G.vexnum; i++) {
        int minCost = INFINITY, k = -1;
        for (int w = 0; w < G.vexnum; w++) {
            if (!final[w] && Dist[w].lowcost < minCost) {
                k = w;
                minCost = Dist[w].lowcost;
            }
        }
        if (k == -1) break;
        final[k] = 1;
        for (int w = 0; w < G.vexnum; w++) {
            if (!final[w] && G.arcs[k][w] < INFINITY) {
                if (Dist[k].lowcost + G.arcs[k][w] < Dist[w].lowcost) {
                    Dist[w].lowcost = Dist[k].lowcost + G.arcs[k][w];
                    Dist[w].prev = k;
                }
            }
        }
    }
}

static void PrintPath(GraphMatrix G, PathDist Dist[], int dest) {
    if (Dist[dest].lowcost == INFINITY) {
        printf("  目标: %-10s 最短距离: 无穷大(不可达)\n", G.vexs[dest]);
        return;
    }
    int path[MAX_VEX_NUM];
    int count = 0, curr = dest;
    while (curr != -1) {
        path[count++] = curr;
        curr = Dist[curr].prev;
    }
    printf("  目标: %-10s 距离: %3d 米  路径: ", G.vexs[dest], Dist[dest].lowcost);
    for (int i = count - 1; i >= 0; i--) {
        printf("%s", G.vexs[path[i]]);
        if (i > 0) printf(" -> ");
    }
    printf("\n");
}

static void RunModule4(void) {
    printf("\n======================================================================\n");
    printf("   【模块四：图的使用——迪杰斯特拉（Dijkstra）算法求解单源最短路径】\n");
    printf("======================================================================\n");

    GraphMatrix G;
    InitGraph(&G, 6);
    strcpy(G.vexs[0], "校门");
    strcpy(G.vexs[1], "图书馆");
    strcpy(G.vexs[2], "教学楼");
    strcpy(G.vexs[3], "实验中心");
    strcpy(G.vexs[4], "学生宿舍");
    strcpy(G.vexs[5], "体育馆");

    AddEdge(&G, 0, 1, 300);
    AddEdge(&G, 0, 2, 200);
    AddEdge(&G, 1, 3, 250);
    AddEdge(&G, 1, 4, 400);
    AddEdge(&G, 2, 3, 150);
    AddEdge(&G, 2, 5, 350);
    AddEdge(&G, 3, 4, 100);
    AddEdge(&G, 4, 5, 120);

    printf("1. 校园导游图构建（6个景点，8条双向道路）\n");
    PathDist Dist[MAX_VEX_NUM];
    int v0 = 0; // 源点：校门
    Dijkstra(G, v0, Dist);

    printf("2. 规划以【%s】为源点的全校各景点最短导航路径：\n", G.vexs[v0]);
    for (int i = 0; i < G.vexnum; i++) {
        PrintPath(G, Dist, i);
    }
}

// ============================================================================
//                               主菜单与交互
// ============================================================================
int main(void) {
    int choice = -1;
    while (choice != 0) {
        printf("\n");
        printf("======================================================================\n");
        printf("              数据结构课程设计（二）综合运行系统                      \n");
        printf("======================================================================\n");
        printf("   [1] 模块一：批量增删功能演示（单链表集合运算）\n");
        printf("   [2] 模块二：简易文本编辑器演示（LString块链存储 - 自动演示）\n");
        printf("   [3] 模块三：术语词典系统（LSD链式基数排序 + AVL树检索）\n");
        printf("   [4] 模块四：图的使用（迪杰斯特拉单源最短路径规划）\n");
        printf("   [5] 一键连续演示全部模块（生成完整课设运行输出）\n");
        printf("   [6] 模块二：简易文本编辑器（键盘交互编辑模式：i/d/n/p/q）\n");
        printf("   [0] 退出系统\n");
        printf("======================================================================\n");
        printf("请输入功能选项 [0-6]: ");

        if (scanf("%d", &choice) != 1) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
            printf("输入非法，请输入数字选项 [0-6]！\n");
            continue;
        }

        switch (choice) {
            case 1: RunModule1(); break;
            case 2: RunModule2(); break;
            case 3: RunModule3(); break;
            case 4: RunModule4(); break;
            case 5:
                RunModule1();
                RunModule2();
                RunModule3();
                RunModule4();
                break;
            case 6: RunModule2Interactive(); break;
            case 0:
                printf("\n退出数据结构课程设计系统。再见！\n");
                break;
            default:
                printf("无效输入，请重新选择。\n");
                break;
        }
    }
    return 0;
}
