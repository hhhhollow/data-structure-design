#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OK 1
#define ERROR 0
#define OVERFLOW -2
#define TRUE 1
#define FALSE 0

#define KEY_NUM 5    // 每个术语前5个汉字对应5个关键字
#define RADIX 256    // 基数（ASCII 0~255 及补0）

// 平衡因子定义（参考课件第9章）
#define LH +1 // 左高
#define EH  0 // 等高
#define RH -1 // 右高

// ==================== 1. 术语词典链表结点定义 ====================
// 每个节点包含：术语、5个关键字、术语解释（题设要求）
typedef struct TermNode {
    char term[32];               // 术语名称（如"图书"）
    unsigned char keys[KEY_NUM]; // 5个关键字（如 {'t', 's', 0, 0, 0}）
    char explanation[128];       // 术语解释
    struct TermNode *next;       // 链表指针
} TermNode, *TermList;

// 关键字比较函数：从第 0 位到第 4 位依次比较
int KeyCompare(const unsigned char k1[KEY_NUM], const unsigned char k2[KEY_NUM]) {
    for (int i = 0; i < KEY_NUM; i++) {
        if (k1[i] != k2[i]) return (int)k1[i] - (int)k2[i];
    }
    return 0;
}

// 打印单个术语的 5 个关键字
void PrintKeys(const unsigned char keys[KEY_NUM]) {
    printf("[");
    for (int i = 0; i < KEY_NUM; i++) {
        if (keys[i] >= 'a' && keys[i] <= 'z') {
            printf("'%c'", keys[i]);
        } else {
            printf("%d", keys[i]);
        }
        if (i < KEY_NUM - 1) printf(", ");
    }
    printf("]");
}

// 尾插法向词典链表追加结点
void AppendTerm(TermList *head, const char *term, const char *pinyinKeys, const char *exp) {
    TermNode *s = (TermNode *)malloc(sizeof(TermNode));
    if (!s) exit(OVERFLOW);
    strncpy(s->term, term, sizeof(s->term) - 1);
    s->term[sizeof(s->term) - 1] = '\0';
    strncpy(s->explanation, exp, sizeof(s->explanation) - 1);
    s->explanation[sizeof(s->explanation) - 1] = '\0';

    // 填充 5 个关键字：汉字拼音小写字符，不足 5 个补数字 0
    int len = (int)strlen(pinyinKeys);
    for (int i = 0; i < KEY_NUM; i++) {
        s->keys[i] = (i < len) ? (unsigned char)pinyinKeys[i] : 0;
    }
    s->next = NULL;

    if (!*head) {
        *head = s;
    } else {
        TermNode *p = *head;
        while (p->next) p = p->next;
        p->next = s;
    }
}

// 打印词典链表
void PrintTermList(TermList L) {
    TermNode *p = L;
    int idx = 1;
    while (p) {
        printf("  %d. 术语: %-12s 关键字: ", idx++, p->term);
        PrintKeys(p->keys);
        printf("  释义: %s\n", p->explanation);
        p = p->next;
    }
}

// ==================== 2. 基于 LSD 的链式基数排序 ====================
// 参考课件第10章 算法10.13 Distribute, 算法10.14 Collect, 算法10.15 RadixSort

// 按第 keyIdx 位关键字进行分配
void Distribute(TermNode *head, int keyIdx, TermNode *front[], TermNode *rear[]) {
    for (int j = 0; j < RADIX; j++) {
        front[j] = rear[j] = NULL;
    }
    TermNode *p = head;
    while (p) {
        TermNode *next = p->next;
        p->next = NULL;
        unsigned char k = p->keys[keyIdx];

        if (!front[k]) {
            front[k] = p;
            rear[k] = p;
        } else {
            rear[k]->next = p;
            rear[k] = p;
        }
        p = next;
    }
}

// 收集各队列成一个单链表
void Collect(TermList *head, TermNode *front[], TermNode *rear[]) {
    *head = NULL;
    TermNode *last = NULL;
    for (int j = 0; j < RADIX; j++) {
        if (front[j]) {
            if (!*head) {
                *head = front[j];
            } else {
                last->next = front[j];
            }
            last = rear[j];
        }
    }
    if (last) last->next = NULL;
}

// LSD 链式基数排序（从最低位 keys[4] 到最高位 keys[0] 进行 5 趟分配与收集）
void RadixSort(TermList *head) {
    TermNode *front[RADIX];
    TermNode *rear[RADIX];

    for (int i = KEY_NUM - 1; i >= 0; i--) {
        Distribute(*head, i, front, rear);
        Collect(head, front, rear);
    }
}

// ==================== 3. 二叉平衡树 (AVL Tree) 结构与查找 ====================
// 参考课件第9章 算法9.9 LeftBalance, 算法9.10 RightBalance, 算法9.11 InsertAVL

typedef struct AVLTreeNode {
    char term[32];
    unsigned char keys[KEY_NUM];
    char explanation[128];
    int bf; // 平衡因子
    struct AVLTreeNode *lchild, *rchild;
} AVLTreeNode, *AVLTree;

// LL 右单旋
void R_Rotate(AVLTree *p) {
    AVLTree lc = (*p)->lchild;
    (*p)->lchild = lc->rchild;
    lc->rchild = *p;
    *p = lc;
}

// RR 左单旋
void L_Rotate(AVLTree *p) {
    AVLTree rc = (*p)->rchild;
    (*p)->rchild = rc->lchild;
    rc->lchild = *p;
    *p = rc;
}

// 左平衡旋转处理 LeftBalance
void LeftBalance(AVLTree *T) {
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

// 右平衡旋转处理 RightBalance
void RightBalance(AVLTree *T) {
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

// 向二叉平衡树插入结点
int InsertAVL(AVLTree *T, TermNode node, int *taller) {
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
        return 0; // 树中已有相同关键字结点
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

// 从已排序链表构建 AVL 树
AVLTree BuildAVLFromSortedList(TermList L) {
    AVLTree T = NULL;
    int taller = FALSE;
    TermNode *p = L;
    while (p) {
        InsertAVL(&T, *p, &taller);
        p = p->next;
    }
    return T;
}

// 术语词典查找函数 find(key)
// 若找到关键字 key 的术语，则打印出该术语和该术语的解释并返回 true, 否则返回 false
int find(AVLTree T, const char *keyStr) {
    unsigned char targetKeys[KEY_NUM] = {0};
    int len = (int)strlen(keyStr);
    for (int i = 0; i < KEY_NUM; i++) {
        targetKeys[i] = (i < len) ? (unsigned char)keyStr[i] : 0;
    }

    printf("--> 正在词典树中查找关键字: ");
    PrintKeys(targetKeys);
    printf(" (输入串: \"%s\")\n", keyStr);

    AVLTree p = T;
    int compareSteps = 0;
    while (p) {
        compareSteps++;
        int cmp = KeyCompare(targetKeys, p->keys);
        if (cmp == 0) {
            printf("    【查找成功】(比较 %d 次)\n", compareSteps);
            printf("    术语名称: %s\n", p->term);
            printf("    术语解释: %s\n", p->explanation);
            return TRUE;
        } else if (cmp < 0) {
            p = p->lchild;
        } else {
            p = p->rchild;
        }
    }
    printf("    【查找失败】未找到关键字为 \"%s\" 的术语。\n", keyStr);
    return FALSE;
}

// 中序遍历平衡二叉树，验证树结构按关键字有序
void InOrderTraverse(AVLTree T) {
    if (T) {
        InOrderTraverse(T->lchild);
        printf("    %-10s 关键字: ", T->term);
        PrintKeys(T->keys);
        printf("  [bf=%2d]\n", T->bf);
        InOrderTraverse(T->rchild);
    }
}

// 释放链表内存
void DestroyTermList(TermList *head) {
    TermNode *p = *head;
    while (p) {
        TermNode *q = p->next;
        free(p);
        p = q;
    }
    *head = NULL;
}

// 释放二叉平衡树内存
void DestroyAVLTree(AVLTree *T) {
    if (*T) {
        DestroyAVLTree(&((*T)->lchild));
        DestroyAVLTree(&((*T)->rchild));
        free(*T);
        *T = NULL;
    }
}

int main(void) {
    printf("======================================================================\n");
    printf("   题目三：术语词典（LSD链式基数排序 + 平衡二叉树构建与find查找）\n");
    printf("======================================================================\n\n");

    TermList dictList = NULL;

    // 1. 建立初始术语词典链表
    // 术语不超过5个汉字，按前5个汉字首个拼音字符（小写）作为多关键字，不足补0
    AppendTerm(&dictList, "图书",     "ts",    "由纸张、油墨结合的出版物，用于记录与传承信息");
    AppendTerm(&dictList, "数据结构", "sjjg",  "计算机存储、组织数据的方式，相互之间存在特定关系");
    AppendTerm(&dictList, "二叉树",   "ecs",   "每个节点最多有两个子树的树形数据结构");
    AppendTerm(&dictList, "线性表",   "xxb",   "由相同特性的数据元素构成的有限序列");
    AppendTerm(&dictList, "栈",       "z",     "限定仅在表尾进行插入和删除操作的线性表(LIFO)");
    AppendTerm(&dictList, "队列",     "dl",    "只允许在一端插入另一端删除的线性表(FIFO)");
    AppendTerm(&dictList, "平衡树",   "phs",   "任意结点的左右两棵子树高度差绝对值不超过1的树");

    printf("1. 初始未排序的术语词典链表：\n");
    PrintTermList(dictList);
    printf("\n");

    // 2. 对词典链表执行基于 LSD 的 5 位链式基数排序
    printf("2. 执行基于 LSD 的链式基数排序 (从最低位 keys[4] 到最高位 keys[0])...\n");
    RadixSort(&dictList);
    printf("   排序完成后的有序术语词典链表：\n");
    PrintTermList(dictList);
    printf("\n");

    // 3. 在此排序的基础上构建二叉平衡树 (AVL Tree)
    printf("3. 在基数排序成果的基础上，构建二叉平衡树 (AVL Tree)...\n");
    AVLTree dictTree = BuildAVLFromSortedList(dictList);
    printf("   平衡二叉树中序遍历结果（验证关键字升序）：\n");
    InOrderTraverse(dictTree);
    printf("\n");

    // 4. 调用 find(key) 查找功能
    printf("4. 调用 find(key) 函数执行检索测试：\n\n");

    // 测试 1：查找 "图书" (key = "ts")
    int res1 = find(dictTree, "ts");
    printf("   返回状态值: %s\n\n", res1 ? "true (1)" : "false (0)");

    // 测试 2：查找 "数据结构" (key = "sjjg")
    int res2 = find(dictTree, "sjjg");
    printf("   返回状态值: %s\n\n", res2 ? "true (1)" : "false (0)");

    // 测试 3：查找 "二叉树" (key = "ecs")
    int res3 = find(dictTree, "ecs");
    printf("   返回状态值: %s\n\n", res3 ? "true (1)" : "false (0)");

    // 测试 4：查找不存在的术语 (key = "xyz")
    int res4 = find(dictTree, "xyz");
    printf("   返回状态值: %s\n\n", res4 ? "true (1)" : "false (0)");

    // 释放资源
    DestroyTermList(&dictList);
    DestroyAVLTree(&dictTree);

    printf("======================================================================\n");
    return 0;
}
