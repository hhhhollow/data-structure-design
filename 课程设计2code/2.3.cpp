#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NUM_OF_KEY 5                           // 设一个术语不超过5个字，5个关键字
#define RADIX 128                                  // ASCII基数（含数字0与小写字母'a'~'z'）
#define LH +1                                      // 左高
#define EH 0                                       // 等高
#define RH -1                                      // 右高
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
    WordNode data;                                 // 术语记录
    int bf;                                        // 结点的平衡因子
    struct BSTNode *lchild, *rchild;
} BSTNode, *BSTree;

BSTree dictTree = NULL;                            // 全局术语词典二叉平衡树根指针

void SetKeys(int keys[MAX_NUM_OF_KEY], const char *py) { // 生成5个关键字，不足补0
    int len = (int)strlen(py);
    for (int i = 0; i < MAX_NUM_OF_KEY; ++i)
        keys[i] = (i < len) ? (unsigned char)py[i] : 0;
}

int CmpKeys(const int k1[MAX_NUM_OF_KEY], const int k2[MAX_NUM_OF_KEY]) { // 多关键字比较
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

void PrintWordList(WordList L) {                   // 打印术语链表及5个关键字
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
    for (int i = MAX_NUM_OF_KEY - 1; i >= 0; --i) { // 按最低位优先(LSD)依次对各关键字分配和收集
        Distribute(L, i, f, e);
        Collect(L, i, f, e);
    }
}

void R_Rotate(BSTree &p) {                         // 对以p为根的子树作右旋
    BSTree lc = p->lchild;
    p->lchild = lc->rchild;
    lc->rchild = p;
    p = lc;
}

void L_Rotate(BSTree &p) {                         // 对以p为根的子树作左旋
    BSTree rc = p->rchild;
    p->rchild = rc->lchild;
    rc->lchild = p;
    p = rc;
}

void LeftBalance(BSTree &T) {                      // 对以T为根的二叉树作左平衡旋转处理
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

void RightBalance(BSTree &T) {                     // 对以T为根的二叉树作右平衡旋转处理
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

int InsertAVL(BSTree &T, WordNode e, Boolean &taller) { // 动态平衡插入结点e
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

void PrintAVL(BSTree T, int depth) {               // 横向打印二叉平衡树结构
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

int main() {
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
    return 0;
}
