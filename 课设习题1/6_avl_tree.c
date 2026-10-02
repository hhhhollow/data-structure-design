#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
typedef int Boolean;
typedef int ElemType;

// 平衡因子定义（参考严蔚敏《数据结构》第9章平衡二叉树）
#define LH +1 // 左高
#define EH  0 // 等高
#define RH -1 // 右高

// 二叉平衡树结点结构定义
typedef struct BSTNode {
    ElemType data;                  // 结点数据域
    int bf;                         // 结点的平衡因子 (balance factor)
    struct BSTNode *lchild, *rchild;// 左右孩子指针
} BSTNode, *BSTree;

// 对以 *p 为根的二叉树作右单旋转处理（LL型旋转）
void R_Rotate(BSTree *p) {
    BSTree lc = (*p)->lchild;
    (*p)->lchild = lc->rchild;
    lc->rchild = *p;
    *p = lc;
}

// 对以 *p 为根的二叉树作左单旋转处理（RR型旋转）
void L_Rotate(BSTree *p) {
    BSTree rc = (*p)->rchild;
    (*p)->rchild = rc->lchild;
    rc->lchild = *p;
    *p = rc;
}

// 题目要求操作：左旋/左平衡旋转处理 LeftBalance(BSTree *T)
// 对以指针 *T 所指结点为根的二叉树作左平衡旋转处理（本树左子树过高导致失衡）
// 包含 LL 单旋转 或 LR 双旋转
void LeftBalance(BSTree *T) {
    BSTree lc = (*T)->lchild; // lc 指向 *T 的左孩子
    switch (lc->bf) {
        case LH: // 新结点插入在 *T 左孩子的左子树上，作单右旋处理（LL型）
            (*T)->bf = lc->bf = EH;
            R_Rotate(T);
            break;
        case RH: { // 新结点插入在 *T 左孩子的右子树上，作双旋处理（LR型）
            BSTree rd = lc->rchild; // rd 指向 *T 左孩子的右子树根
            switch (rd->bf) { // 修改 *T 及其左孩子的平衡因子
                case LH:
                    (*T)->bf = RH;
                    lc->bf = EH;
                    break;
                case EH:
                    (*T)->bf = lc->bf = EH;
                    break;
                case RH:
                    (*T)->bf = EH;
                    lc->bf = LH;
                    break;
            }
            rd->bf = EH;
            L_Rotate(&((*T)->lchild)); // 对 *T 的左子树作左旋平衡处理
            R_Rotate(T);               // 对 *T 作右旋平衡处理
            break;
        }
    }
}

// 右平衡旋转处理 RightBalance(BSTree *T)
// 对以指针 *T 所指结点为根的二叉树作右平衡旋转处理（本树右子树过高导致失衡）
// 包含 RR 单旋转 或 RL 双旋转
void RightBalance(BSTree *T) {
    BSTree rc = (*T)->rchild; // rc 指向 *T 的右孩子
    switch (rc->bf) {
        case RH: // 新结点插入在 *T 右孩子的右子树上，作单左旋处理（RR型）
            (*T)->bf = rc->bf = EH;
            L_Rotate(T);
            break;
        case LH: { // 新结点插入在 *T 右孩子的左子树上，作双旋处理（RL型）
            BSTree ld = rc->lchild; // ld 指向 *T 右孩子的左子树根
            switch (ld->bf) { // 修改 *T 及其右孩子的平衡因子
                case LH:
                    (*T)->bf = EH;
                    rc->bf = RH;
                    break;
                case EH:
                    (*T)->bf = rc->bf = EH;
                    break;
                case RH:
                    (*T)->bf = LH;
                    rc->bf = EH;
                    break;
            }
            ld->bf = EH;
            R_Rotate(&((*T)->rchild)); // 对 *T 的右子树作右旋平衡处理
            L_Rotate(T);               // 对 *T 作左旋平衡处理
            break;
        }
    }
}

// 题目要求操作：插入 InsertAVL(BSTree *T, ElemType e, Boolean *taller)
// 若在二叉平衡树 *T 中不存在和 e 相同关键字的结点，则插入新结点并返回 1，否则返回 0
// 若因插入使二叉树长高，则置 *taller 为 TRUE，否则为 FALSE
// 注：纯C语言使用指针模拟引用传参
int InsertAVL(BSTree *T, ElemType e, Boolean *taller) {
    if (!*T) {
        // 插入新结点，树长高，置 *taller 为 TRUE
        *T = (BSTree)malloc(sizeof(BSTNode));
        (*T)->data = e;
        (*T)->lchild = (*T)->rchild = NULL;
        (*T)->bf = EH;
        *taller = TRUE;
        return 1;
    }

    if (e == (*T)->data) {
        // 树中已存在相同关键字的结点，不再插入
        *taller = FALSE;
        return 0;
    }

    if (e < (*T)->data) {
        // 应在 *T 的左子树中继续搜索插入
        if (!InsertAVL(&((*T)->lchild), e, taller)) return 0;
        if (*taller) { // 已插入到 *T 的左子树中且左子树长高
            switch ((*T)->bf) {
                case LH: // 原本左子树比右子树高，需要作左平衡旋转
                    LeftBalance(T);
                    *taller = FALSE;
                    break;
                case EH: // 原本左右子树等高，现左子树长高
                    (*T)->bf = LH;
                    *taller = TRUE;
                    break;
                case RH: // 原本右子树比左子树高，现左右子树等高
                    (*T)->bf = EH;
                    *taller = FALSE;
                    break;
            }
        }
    } else {
        // 应在 *T 的右子树中继续搜索插入
        if (!InsertAVL(&((*T)->rchild), e, taller)) return 0;
        if (*taller) { // 已插入到 *T 的右子树中且右子树长高
            switch ((*T)->bf) {
                case LH: // 原本左子树比右子树高，现左右子树等高
                    (*T)->bf = EH;
                    *taller = FALSE;
                    break;
                case EH: // 原本左右子树等高，现右子树长高
                    (*T)->bf = RH;
                    *taller = TRUE;
                    break;
                case RH: // 原本右子树比左子树高，需要作右平衡旋转
                    RightBalance(T);
                    *taller = FALSE;
                    break;
            }
        }
    }
    return 1;
}

// 中序遍历打印（验证排序二叉树递增性）
void InOrderTraverse(BSTree T) {
    if (T) {
        InOrderTraverse(T->lchild);
        printf("%d(bf=%d) ", T->data, T->bf);
        InOrderTraverse(T->rchild);
    }
}

// 打印树的直观层次结构（带缩进）
void PrintTreeIndented(BSTree T, int space) {
    if (T == NULL) return;
    space += 6;
    PrintTreeIndented(T->rchild, space);
    printf("\n");
    for (int i = 6; i < space; i++) printf(" ");
    printf("%d[bf:%d]\n", T->data, T->bf);
    PrintTreeIndented(T->lchild, space);
}

// 递归后序释放二叉平衡树存储空间
void DestroyAVLTree(BSTree *T) {
    if (*T) {
        DestroyAVLTree(&((*T)->lchild));
        DestroyAVLTree(&((*T)->rchild));
        free(*T);
        *T = NULL;
    }
}

int main(void) {
    printf("【题目6：二叉平衡树的构造、LeftBalance旋转与InsertAVL插入】\n\n");

    BSTree T = NULL;
    Boolean taller = FALSE;

    // 1. 依次输入 {5, 3, 7, 6, 9} 构造一棵二叉平衡树
    ElemType initialKeys[] = {5, 3, 7, 6, 9};
    int n = sizeof(initialKeys) / sizeof(initialKeys[0]);

    printf("1. 依次插入初始序列 {5, 3, 7, 6, 9}：\n");
    for (int i = 0; i < n; i++) {
        InsertAVL(&T, initialKeys[i], &taller);
        printf("   插入节点 %d 成功\n", initialKeys[i]);
    }

    printf("\n当前平衡二叉树形态（横向树状图，右子树在上，左子树在下）：\n");
    PrintTreeIndented(T, 0);
    printf("中序遍历序列: ");
    InOrderTraverse(T);
    printf("\n\n");

    // 2. 使用 InsertAVL 插入新的节点 2
    printf("2. 调用 InsertAVL 插入新节点 2：\n");
    InsertAVL(&T, 2, &taller);
    printf("插入节点 2 后的平衡二叉树形态：\n");
    PrintTreeIndented(T, 0);
    printf("中序遍历序列: ");
    InOrderTraverse(T);
    printf("\n\n");

    // 3. 使用 InsertAVL 插入新的节点 15
    printf("3. 调用 InsertAVL 插入新节点 15：\n");
    InsertAVL(&T, 15, &taller);
    printf("插入节点 15 后的平衡二叉树形态：\n");
    PrintTreeIndented(T, 0);
    printf("中序遍历序列: ");
    InOrderTraverse(T);
    printf("\n\n");

    // 释放二叉平衡树内存
    DestroyAVLTree(&T);

    return 0;
}
