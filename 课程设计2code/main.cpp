#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/* ===================== 一、批量增删功能（单链表集合运算） ===================== */
typedef char ElemType;
typedef struct LNode {
    ElemType data;
    struct LNode *next;
} LNode, *LinkList;

int LocateElem(LinkList L, ElemType e) {           // 若L中存在e返回1，否则返回0
    for (LinkList p = L->next; p; p = p->next)
        if (p->data == e) return 1;
    return 0;
}

void AppendElem(LinkList *tail, ElemType e) {      // 在表尾指针*tail后追加元素e
    LinkList s = (LinkList)malloc(sizeof(LNode));
    s->data = e;
    s->next = NULL;
    (*tail)->next = s;
    *tail = s;
}

void CreateSet(LinkList *T, const char *Str) {     // 算法2.11&例2-2：由字符串Str建立去重集合链表T
    LinkList tail = *T = (LinkList)malloc(sizeof(LNode));
    (*T)->next = NULL;
    for (int i = 0; Str[i]; ++i)
        if (!LocateElem(*T, Str[i]))
            AppendElem(&tail, Str[i]);
}

void Union(LinkList *T, LinkList S1, LinkList S2) { // 算法2.1：求并集 T = S1 ∪ S2（批量新增）
    LinkList tail = *T = (LinkList)malloc(sizeof(LNode));
    (*T)->next = NULL;
    for (LinkList p = S1->next; p; p = p->next)
        AppendElem(&tail, p->data);
    for (LinkList p = S2->next; p; p = p->next)
        if (!LocateElem(*T, p->data))
            AppendElem(&tail, p->data);
}

void Difference(LinkList *T, LinkList S1, LinkList S2) { // 求差集 T = S1 - S2（批量删除）
    LinkList tail = *T = (LinkList)malloc(sizeof(LNode));
    (*T)->next = NULL;
    for (LinkList p = S1->next; p; p = p->next)
        if (!LocateElem(S2, p->data))
            AppendElem(&tail, p->data);
}

void PrintSet(LinkList T) {                        // 显示集合T中的元素
    printf("{ ");
    for (LinkList p = T->next; p; p = p->next)
        printf("%c ", p->data);
    printf("}\n");
}

/* ===================== 二、文本编辑器（LString块链存储活区） ===================== */
#define CHUNKSIZE 80                               // 可由用户定义的块大小
#define MAX_LINE 200                               // 设文件每行不超过200字
#define ACTIVE_CAPACITY 4                          // 活区每次读入一段的行数

typedef struct Chunk {                             // 结点结构：同一行的串用定长结构
    char ch[CHUNKSIZE];
    struct Chunk *next;                            // 行和行之间用指针相联接
} Chunk;

typedef struct {                                   // 串的链表结构（整个文本编辑区活区）
    Chunk *head, *tail;                            // 串的头和尾指针
    int curlen;                                    // 串的当前长度（当前活区行数）
} LString;

void InitActive(LString *S) {
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
    if (lineNo == 0) {
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
    for (Chunk *p = S->head, *q; p; p = q) {
        q = p->next;
        fprintf(fpOut, "%s\n", p->ch);
        free(p);
    }
    InitActive(S);
    char buf[MAX_LINE + 2];
    while (S->curlen < ACTIVE_CAPACITY && fgets(buf, sizeof(buf), fpIn)) {
        buf[strcspn(buf, "\r\n")] = '\0';
        InsertLine(S, S->curlen, buf);
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
                if (strcmp(buf, ".") == 0) break;
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

/* ===================== 三、术语词典（LSD链式基数排序 + 二叉平衡树AVL） ===================== */
#define MAX_NUM_OF_KEY 5
#define RADIX 128
#define LH +1
#define EH 0
#define RH -1
#define TRUE 1
#define FALSE 0
typedef int Boolean;

typedef struct WordNode {                          // 术语词典链表结点
    char term[32];                                 // 术语
    int keys[MAX_NUM_OF_KEY];                      // 5个关键字（首拼音小写字母ASCII码，不足补0）
    char desc[128];                                // 术语解释
    struct WordNode *next;
} WordNode, *WordList;

typedef struct BSTNode {                           // 二叉平衡树（AVL）结点
    WordNode data;
    int bf;
    struct BSTNode *lchild, *rchild;
} BSTNode, *BSTree;

BSTree dictTree = NULL;

void SetKeys(int keys[MAX_NUM_OF_KEY], const char *py) {
    int len = (int)strlen(py);
    for (int i = 0; i < MAX_NUM_OF_KEY; ++i)
        keys[i] = (i < len) ? (unsigned char)py[i] : 0;
}

int CmpKeys(const int k1[MAX_NUM_OF_KEY], const int k2[MAX_NUM_OF_KEY]) {
    for (int i = 0; i < MAX_NUM_OF_KEY; ++i)
        if (k1[i] != k2[i]) return k1[i] - k2[i];
    return 0;
}

void AppendWord(WordList L, const char *term, const char *py, const char *desc) {
    WordNode *p = L;
    while (p->next) p = p->next;
    WordNode *s = (WordNode*)malloc(sizeof(WordNode));
    strcpy(s->term, term);
    SetKeys(s->keys, py);
    strcpy(s->desc, desc);
    s->next = NULL;
    p->next = s;
}

void PrintWordList(WordList L) {
    for (WordNode *p = L->next; p; p = p->next) {
        printf("  %-12s keys=[", p->term);
        for (int i = 0; i < MAX_NUM_OF_KEY; ++i)
            printf("%d%s", p->keys[i], (i < MAX_NUM_OF_KEY - 1) ? "," : "");
        printf("]  解释：%s\n", p->desc);
    }
}

void Distribute(WordList L, int i, WordList f[], WordList e[]) { // 算法10.13：按第i位关键字分配
    for (int j = 0; j < RADIX; ++j) f[j] = e[j] = NULL;
    for (WordNode *p = L->next; p; p = p->next) {
        int j = p->keys[i];
        if (!f[j]) f[j] = p;
        else e[j]->next = p;
        e[j] = p;
    }
}

void Collect(WordList L, int i, WordList f[], WordList e[]) {    // 算法10.14：按第i位关键字收集
    int j = 0; (void)i;
    while (j < RADIX && !f[j]) ++j;
    L->next = f[j];
    WordNode *t = e[j];
    while (j < RADIX) {
        for (++j; j < RADIX && !f[j]; ++j);
        if (j < RADIX) { t->next = f[j]; t = e[j]; }
    }
    if (t) t->next = NULL;
}

void RadixSort(WordList L) {                       // 算法10.15：基于LSD的链式基数排序
    WordList f[RADIX], e[RADIX];
    for (int i = MAX_NUM_OF_KEY - 1; i >= 0; --i) {
        Distribute(L, i, f, e);
        Collect(L, i, f, e);
    }
}

void R_Rotate(BSTree &p) {
    BSTree lc = p->lchild;
    p->lchild = lc->rchild;
    lc->rchild = p;
    p = lc;
}

void L_Rotate(BSTree &p) {
    BSTree rc = p->rchild;
    p->rchild = rc->lchild;
    rc->lchild = p;
    p = rc;
}

void LeftBalance(BSTree &T) {
    BSTree lc = T->lchild, rd;
    switch (lc->bf) {
    case LH: T->bf = lc->bf = EH; R_Rotate(T); break;
    case RH:
        rd = lc->rchild;
        switch (rd->bf) {
        case LH: T->bf = RH; lc->bf = EH; break;
        case EH: T->bf = lc->bf = EH; break;
        case RH: T->bf = EH; lc->bf = LH; break;
        }
        rd->bf = EH;
        L_Rotate(T->lchild);
        R_Rotate(T);
    }
}

void RightBalance(BSTree &T) {
    BSTree rc = T->rchild, ld;
    switch (rc->bf) {
    case RH: T->bf = rc->bf = EH; L_Rotate(T); break;
    case LH:
        ld = rc->lchild;
        switch (ld->bf) {
        case LH: T->bf = EH; rc->bf = RH; break;
        case EH: T->bf = rc->bf = EH; break;
        case RH: T->bf = LH; rc->bf = EH; break;
        }
        ld->bf = EH;
        R_Rotate(T->rchild);
        L_Rotate(T);
    }
}

int InsertAVL(BSTree &T, WordNode e, Boolean &taller) {
    if (!T) {
        T = (BSTree)malloc(sizeof(BSTNode));
        T->data = e; T->data.next = NULL;
        T->lchild = T->rchild = NULL;
        T->bf = EH; taller = TRUE;
        return 1;
    }
    int cmp = CmpKeys(e.keys, T->data.keys);
    if (cmp == 0) { taller = FALSE; return 0; }
    if (cmp < 0) {
        if (!InsertAVL(T->lchild, e, taller)) return 0;
        if (taller)
            switch (T->bf) {
            case LH: LeftBalance(T); taller = FALSE; break;
            case EH: T->bf = LH; taller = TRUE; break;
            case RH: T->bf = EH; taller = FALSE; break;
            }
    } else {
        if (!InsertAVL(T->rchild, e, taller)) return 0;
        if (taller)
            switch (T->bf) {
            case LH: T->bf = EH; taller = FALSE; break;
            case EH: T->bf = RH; taller = TRUE; break;
            case RH: RightBalance(T); taller = FALSE; break;
            }
    }
    return 1;
}

void PrintAVL(BSTree T, int depth) {
    if (!T) return;
    PrintAVL(T->rchild, depth + 1);
    printf("%*s%s(bf=%d)\n", depth * 8, "", T->data.term, T->bf);
    PrintAVL(T->lchild, depth + 1);
}

bool find(const char *key) {                       // 术语词典查找功能 find(key)
    int k[MAX_NUM_OF_KEY];
    SetKeys(k, key);
    for (BSTree p = dictTree; p; ) {
        int cmp = CmpKeys(k, p->data.keys);
        if (cmp == 0) {
            printf("查找 \"%s\" 成功 -> 术语：%s | 解释：%s\n", key, p->data.term, p->data.desc);
            return true;
        }
        p = (cmp < 0) ? p->lchild : p->rchild;
    }
    printf("查找 \"%s\" 失败 -> 词典中不存在该术语\n", key);
    return false;
}

/* ===================== 四、图的使用（迪杰斯特拉算法） ===================== */
#define MAX_VEX_NUM 10
#define INFINITY INT_MAX
typedef int VertexType;
typedef struct { VertexType v1, v2; int weight; } ArcInfo;

typedef struct {
    VertexType vexs[MAX_VEX_NUM];
    int arcs[MAX_VEX_NUM][MAX_VEX_NUM];
    int n, a;
} MGraph;

typedef struct {
    int prev;                                      // 当前最短路径上该顶点的前驱顶点位序
    int lowcost;                                   // 当前最短路径的长度
} DistInfo;
DistInfo Dist[MAX_VEX_NUM];

int LocateVex(MGraph G, VertexType v) {
    for (int i = 0; i < G.n; ++i)
        if (G.vexs[i] == v) return i;
    return -1;
}

void CreateUDN(MGraph *G, VertexType *V, int n, ArcInfo *A, int e) { // 算法7.2：构造无向网G
    G->n = n; G->a = e;
    for (int i = 0; i < n; ++i) {
        G->vexs[i] = V[i];
        for (int j = 0; j < n; ++j)
            G->arcs[i][j] = (i == j) ? 0 : INFINITY;
    }
    for (int k = 0; k < e; ++k) {
        int i = LocateVex(*G, A[k].v1), j = LocateVex(*G, A[k].v2);
        G->arcs[i][j] = G->arcs[j][i] = A[k].weight;
    }
}

void ShortestPath_DIJ(MGraph G, int v0) {          // 算法7.12：迪杰斯特拉算法求单源最短路径
    int final[MAX_VEX_NUM] = {0};
    for (int j = 0; j < G.n; ++j) {
        Dist[j].lowcost = G.arcs[v0][j];
        Dist[j].prev = (j != v0 && Dist[j].lowcost < INFINITY) ? v0 : -1;
    }
    Dist[v0].lowcost = 0;
    final[v0] = 1;
    for (int m = 1; m < G.n; ++m) {
        int k = -1;
        for (int j = 0; j < G.n; ++j)
            if (!final[j] && (k < 0 || Dist[j].lowcost < Dist[k].lowcost))
                k = j;
        if (k < 0 || Dist[k].lowcost == INFINITY) break;
        final[k] = 1;
        for (int j = 0; j < G.n; ++j)
            if (!final[j] && G.arcs[k][j] < INFINITY &&
                Dist[k].lowcost + G.arcs[k][j] < Dist[j].lowcost) {
                Dist[j].lowcost = Dist[k].lowcost + G.arcs[k][j];
                Dist[j].prev = k;
            }
    }
}

void OutputPath(MGraph G, DistInfo Dist[], int k) { // 课件第7章第29页：递归输出最短路径
    if (k == -1) return;
    OutputPath(G, Dist, Dist[k].prev);
    printf("%d ", G.vexs[k]);
}

int main() {
    printf("================ 一、批量增删功能 ================\n");
    LinkList A, B, C, T1, T2;
    CreateSet(&A, "abcdef");
    CreateSet(&B, "efghij");
    CreateSet(&C, "ace");
    printf("原数据集 A = "); PrintSet(A);
    printf("新增数据集 B = "); PrintSet(B);
    printf("删除数据集 C = "); PrintSet(C);
    Union(&T1, A, B);
    printf("批量新增 Union(A, B) = "); PrintSet(T1);
    Difference(&T2, T1, C);
    printf("批量删除 Difference(A∪B, C) = "); PrintSet(T2);

    printf("\n================ 二、文本编辑器 ================\n");
    FILE *fp = fopen("editor_in.txt", "w");
    fprintf(fp, "Line 1: Data Structure Course Design\n"
                "Line 2: Linear List Set Operations\n"
                "Line 3: Redundant Line To Be Deleted\n"
                "Line 4: LString Active Buffer Demo\n"
                "Line 5: LSD Radix Sort Dictionary\n"
                "Line 6: Balanced Binary Tree (AVL)\n");
    fclose(fp);
    FILE *fc = fopen("editor_cmd.txt", "w");
    fprintf(fc, "p\ni2\nInserted A: Line after Line 2\nInserted B: Second inserted line\n.\np\nd5\np\nd3 4\np\nn\np\nn\n");
    fclose(fc);
    FILE *fpIn = fopen("editor_in.txt", "r");
    FILE *fpOut = fopen("editor_out.txt", "w");
    FILE *fpCmd = fopen("editor_cmd.txt", "r");
    LString active;
    InitActive(&active);
    NextActive(&active, fpIn, fpOut);
    ExecuteCommands(&active, fpIn, fpOut, fpCmd);
    fclose(fpIn); fclose(fpOut); fclose(fpCmd);
    printf("\n=== 最终保存到文件 editor_out.txt 的内容 ===\n");
    fpOut = fopen("editor_out.txt", "r");
    char line[MAX_LINE + 2];
    while (fgets(line, sizeof(line), fpOut)) printf("%s", line);
    fclose(fpOut);

    printf("\n================ 三、术语词典 ================\n");
    WordList L = (WordList)malloc(sizeof(WordNode));
    L->next = NULL;
    AppendWord(L, "图书",     "ts",   "装订成册的著作或出版物");
    AppendWord(L, "数据结构", "sjjg", "相互之间存在一种或多种特定关系的数据元素的集合");
    AppendWord(L, "二叉树",   "ecs",  "每个结点最多有两个子树的树结构");
    AppendWord(L, "基数排序", "jspx", "借助分配和收集进行排序的多关键字排序方法");
    AppendWord(L, "线性表",   "xxb",  "具有相同特性的数据元素的有限序列");
    AppendWord(L, "平衡树",   "phs",  "左右子树高度差绝对值不超过1的二叉排序树");
    AppendWord(L, "图",       "t",    "由顶点的有穷非空集合和顶点之间边的集合组成");
    printf("1. 初始术语词典链表：\n");
    PrintWordList(L);
    RadixSort(L);
    printf("\n2. 基于LSD的链式基数排序后：\n");
    PrintWordList(L);
    Boolean taller;
    for (WordNode *p = L->next; p; p = p->next)
        InsertAVL(dictTree, *p, taller);
    printf("\n3. 在排序基础上建立的二叉平衡树（AVL）：\n");
    PrintAVL(dictTree, 0);
    printf("\n4. 调用 find(key) 查找测试：\n");
    printf("返回值：%s\n", find("ts") ? "true" : "false");
    printf("返回值：%s\n", find("sjjg") ? "true" : "false");
    printf("返回值：%s\n", find("abc") ? "true" : "false");

    printf("\n================ 四、图的使用 ================\n");
    VertexType V[] = {1, 2, 3, 4, 5, 6};
    ArcInfo Arc[] = {{1, 2, 2}, {1, 3, 5}, {1, 5, 5}, {2, 3, 2}, {2, 4, 4},
                     {3, 4, 1}, {3, 6, 5}, {4, 5, 2}, {4, 6, 3}, {5, 6, 6}};
    MGraph G;
    CreateUDN(&G, V, 6, Arc, 10);
    ShortestPath_DIJ(G, 0);
    printf("迪杰斯特拉算法求得从顶点 %d 到其余各顶点的最短路径：\n", G.vexs[0]);
    for (int k = 1; k < G.n; ++k) {
        printf("  %d -> %d (最短距离 = %d)，路径：", G.vexs[0], G.vexs[k], Dist[k].lowcost);
        OutputPath(G, Dist, k);
        printf("\n");
    }
    return 0;
}
