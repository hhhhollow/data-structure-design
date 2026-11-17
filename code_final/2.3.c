/* 课程设计 2.3  术语词典
 * 1. 链表结点 TermNode, *TermList 含术语、5 个 int 关键字、术语解释；
 * 2. 基于 LSD 的链式基数排序（从 key[4] 到 key[0] 共 5 趟分配与收集）；
 * 3. 由排好序的链表建立二叉平衡树（AVL），实现 find(key) 查找功能。
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define KEY_NUM 5
#define RADIX   128
#define LH      1
#define EH      0
#define RH      -1

typedef struct TermNode {
    char term[20];
    int key[5];
    char explain[100];
    struct TermNode *next;
} TermNode, *TermList;

typedef struct BSTNode {
    char term[20];
    int key[5];
    char explain[100];
    int bf;
    struct BSTNode *lchild, *rchild;
} BSTNode, *BSTree;

static BSTree g_avl = NULL;

/* 将拼音首字母串（如 "ts" 或 "ts000"）转为 5 个小写 ASCII 码，'0' 或不足补 0 */
static void MakeKeys(const char *s, int key[KEY_NUM]) {
    int i = 0;
    for (; s[i] && i < KEY_NUM; ++i)
        key[i] = (s[i] == '0') ? 0 : tolower((unsigned char)s[i]);
    for (; i < KEY_NUM; ++i)
        key[i] = 0;
}

static int CmpKeys(const int a[KEY_NUM], const int b[KEY_NUM]) {
    for (int i = 0; i < KEY_NUM; ++i)
        if (a[i] != b[i]) return a[i] - b[i];
    return 0;
}

static void PrintKey(const int key[KEY_NUM]) {
    for (int i = 0; i < KEY_NUM; ++i)
        putchar(key[i] ? key[i] : '0');
}

/* ---------- 术语链表建立、打印与释放 ---------- */
static TermList CreateTermList(void) {
    static const char *DICT[][3] = {
        {"图书", "ts", "书籍等出版物的统称"},
        {"最小生成树", "zxscs", "连通网中各边权值之和最小的生成树"},
        {"数据库", "sjk", "按一定结构组织和存储数据的仓库"},
        {"线性表", "xxb", "由同类型数据元素组成的有限序列"},
        {"栈", "z", "只允许在一端插入和删除的线性表"},
        {"数据结构", "sjjg", "研究数据的逻辑结构、存储结构及运算"},
        {"平衡二叉树", "phecs", "左右子树高度差绝对值不超过1的查找树"},
        {"图", "t", "由顶点集合和边集合组成的非线性结构"},
        {"算法", "sf", "解决特定问题的有限指令序列"},
        {"二叉树", "ecs", "每个结点至多有两棵子树的树形结构"},
        {"队列", "dl", "队尾插入队头删除的先进先出线性表"},
        {"查找", "cz", "在数据集合中寻找满足条件的记录"}
    };
    TermList head = NULL, tail = NULL;
    int n = (int)(sizeof(DICT) / sizeof(DICT[0]));
    for (int i = 0; i < n; ++i) {
        TermNode *p = (TermNode *)malloc(sizeof(TermNode));
        strcpy(p->term, DICT[i][0]);
        MakeKeys(DICT[i][1], p->key);
        strcpy(p->explain, DICT[i][2]);
        p->next = NULL;
        if (tail) tail->next = p;
        else head = p;
        tail = p;
    }
    return head;
}

static void PrintTermList(TermList L) {
    int i = 0;
    for (; L; L = L->next) {
        printf("  %2d. ", ++i);
        PrintKey(L->key);
        printf(" [%3d,%3d,%3d,%3d,%3d]  %s : %s\n",
               L->key[0], L->key[1], L->key[2], L->key[3], L->key[4],
               L->term, L->explain);
    }
}

static void FreeTermList(TermList *L) {
    TermList p = *L;
    while (p) {
        TermList q = p->next;
        free(p);
        p = q;
    }
    *L = NULL;
}

/* ---------- 基于 LSD 的链式基数排序 ---------- */
static void RadixSort(TermList *L) {
    TermList front[RADIX], rear[RADIX];
    for (int pass = KEY_NUM - 1; pass >= 0; --pass) {
        for (int i = 0; i < RADIX; ++i)
            front[i] = rear[i] = NULL;
        for (TermList p = *L, next; p; p = next) {      /* 分配 */
            next = p->next;
            int d = p->key[pass];
            p->next = NULL;
            if (front[d]) rear[d]->next = p;
            else front[d] = p;
            rear[d] = p;
        }
        TermList tail = NULL;                           /* 收集 */
        *L = NULL;
        for (int i = 0; i < RADIX; ++i) {
            if (!front[i]) continue;
            if (tail) tail->next = front[i];
            else *L = front[i];
            tail = rear[i];
        }
        printf("  第 %d 趟 (key[%d]): ", KEY_NUM - pass, pass);
        for (TermList p = *L; p; p = p->next) {
            PrintKey(p->key);
            putchar(' ');
        }
        putchar('\n');
    }
}

/* ---------- 二叉平衡树（AVL） ---------- */
static void R_Rotate(BSTree *p) {                       /* 右旋 */
    BSTree lc = (*p)->lchild;
    (*p)->lchild = lc->rchild;
    lc->rchild = *p;
    *p = lc;
}

static void L_Rotate(BSTree *p) {                       /* 左旋 */
    BSTree rc = (*p)->rchild;
    (*p)->rchild = rc->lchild;
    rc->lchild = *p;
    *p = rc;
}

static void LeftBalance(BSTree *T) {                    /* 左平衡旋转处理 */
    BSTree lc = (*T)->lchild, rd;
    switch (lc->bf) {
    case LH:                                            /* LL 型：单右旋 */
        (*T)->bf = lc->bf = EH;
        R_Rotate(T);
        break;
    case RH:                                            /* LR 型：先左旋后右旋 */
        rd = lc->rchild;
        switch (rd->bf) {
        case LH: (*T)->bf = RH; lc->bf = EH; break;
        case EH: (*T)->bf = lc->bf = EH; break;
        case RH: (*T)->bf = EH; lc->bf = LH; break;
        }
        rd->bf = EH;
        L_Rotate(&(*T)->lchild);
        R_Rotate(T);
        break;
    }
}

static void RightBalance(BSTree *T) {                   /* 右平衡旋转处理 */
    BSTree rc = (*T)->rchild, ld;
    switch (rc->bf) {
    case RH:                                            /* RR 型：单左旋 */
        (*T)->bf = rc->bf = EH;
        L_Rotate(T);
        break;
    case LH:                                            /* RL 型：先右旋后左旋 */
        ld = rc->lchild;
        switch (ld->bf) {
        case LH: (*T)->bf = EH; rc->bf = RH; break;
        case EH: (*T)->bf = rc->bf = EH; break;
        case RH: (*T)->bf = LH; rc->bf = EH; break;
        }
        ld->bf = EH;
        R_Rotate(&(*T)->rchild);
        L_Rotate(T);
        break;
    }
}

static int InsertAVL(BSTree *T, const TermNode *e, bool *taller) {
    if (!*T) {
        *T = (BSTree)malloc(sizeof(BSTNode));
        strcpy((*T)->term, e->term);
        memcpy((*T)->key, e->key, sizeof((*T)->key));
        strcpy((*T)->explain, e->explain);
        (*T)->lchild = (*T)->rchild = NULL;
        (*T)->bf = EH;
        *taller = true;
        return 1;
    }
    int cmp = CmpKeys(e->key, (*T)->key);
    if (cmp == 0) {
        *taller = false;
        return 0;
    }
    if (cmp < 0) {
        if (!InsertAVL(&(*T)->lchild, e, taller)) return 0;
        if (*taller) {
            switch ((*T)->bf) {
            case LH: LeftBalance(T); *taller = false; break;
            case EH: (*T)->bf = LH; *taller = true; break;
            case RH: (*T)->bf = EH; *taller = false; break;
            }
        }
    } else {
        if (!InsertAVL(&(*T)->rchild, e, taller)) return 0;
        if (*taller) {
            switch ((*T)->bf) {
            case LH: (*T)->bf = EH; *taller = false; break;
            case EH: (*T)->bf = RH; *taller = true; break;
            case RH: RightBalance(T); *taller = false; break;
            }
        }
    }
    return 1;
}

static void PrintAVL(BSTree T, int depth) {             /* 横向打印 AVL 树（右子树在上） */
    if (!T) return;
    PrintAVL(T->rchild, depth + 1);
    printf("  %*s", depth * 4, "");
    PrintKey(T->key);
    printf("(bf=%+d) %s\n", T->bf, T->term);
    PrintAVL(T->lchild, depth + 1);
}

static void FreeAVL(BSTree *T) {
    if (!*T) return;
    FreeAVL(&(*T)->lchild);
    FreeAVL(&(*T)->rchild);
    free(*T);
    *T = NULL;
}

/* 在已建立的二叉平衡树中按关键字查找，找到打印术语与解释并返回 true，否则返回 false */
bool find(const char *key) {
    int k[KEY_NUM];
    MakeKeys(key, k);
    BSTree p = g_avl;
    while (p) {
        int cmp = CmpKeys(k, p->key);
        if (cmp == 0) {
            printf("  find(\"%s\") -> true  | 术语：%s | 解释：%s\n",
                   key, p->term, p->explain);
            return true;
        }
        p = (cmp < 0) ? p->lchild : p->rchild;
    }
    printf("  find(\"%s\") -> false | 未找到关键字对应的术语\n", key);
    return false;
}

int run_2_3(void) {
    printf("===== 三、术语词典（LSD 链式基数排序 + 二叉平衡树） =====\n");

    TermList list = CreateTermList();
    printf("(1) 排序前的术语链表：\n");
    PrintTermList(list);

    printf("\n(2) LSD 链式基数排序过程（key[4] -> key[0]）：\n");
    RadixSort(&list);

    printf("\n(3) 排序后的术语链表：\n");
    PrintTermList(list);

    FreeAVL(&g_avl);
    for (TermList p = list; p; p = p->next) {
        bool taller = false;
        InsertAVL(&g_avl, p, &taller);
    }
    printf("\n(4) 由有序链表建立的二叉平衡树（右子树在上，括号内为平衡因子）：\n");
    PrintAVL(g_avl, 0);

    printf("\n(5) find(key) 查找测试：\n");
    find("ts");
    find("ts000");
    find("sjjg");
    find("phecs");
    find("t");
    find("abc");
    find("sjj");

    FreeTermList(&list);
    FreeAVL(&g_avl);
    return 0;
}

#ifndef NO_MAIN
int main(void) { return run_2_3(); }
#endif
