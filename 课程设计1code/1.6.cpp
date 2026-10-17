#include <stdio.h>
#include <stdlib.h>

#define LH +1                              // 左高
#define EH 0                               // 等高
#define RH -1                              // 右高
#define TRUE 1
#define FALSE 0
typedef int Boolean;
typedef int ElemType;
typedef struct BSTNode {
    ElemType data;
    int bf;                                // 结点的平衡因子
    struct BSTNode *lchild, *rchild;       // 左、右孩子指针
} BSTNode, *BSTree;

void R_Rotate(BSTree &p) {                 // 对以p为根的子树作右旋
    BSTree lc = p->lchild;
    p->lchild = lc->rchild;
    lc->rchild = p;
    p = lc;                                // p指向新的根结点
}

void L_Rotate(BSTree &p) {                 // 对以p为根的子树作左旋
    BSTree rc = p->rchild;
    p->rchild = rc->lchild;
    rc->lchild = p;
    p = rc;
}

void LeftBalance(BSTree &T) {              // 对以T为根的二叉树作左平衡旋转处理
    BSTree lc = T->lchild, rd;
    switch (lc->bf) {
    case LH:                               // LL型：作单右旋
        T->bf = lc->bf = EH;
        R_Rotate(T);
        break;
    case RH:                               // LR型：作双旋
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

void RightBalance(BSTree &T) {             // 对以T为根的二叉树作右平衡旋转处理
    BSTree rc = T->rchild, ld;
    switch (rc->bf) {
    case RH:                               // RR型：作单左旋
        T->bf = rc->bf = EH;
        L_Rotate(T);
        break;
    case LH:                               // RL型：作双旋
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

int InsertAVL(BSTree &T, ElemType e, Boolean &taller) {
    // 若T中不存在和e相同的结点，则插入e并返回1，否则返回0；taller反映T长高与否
    if (!T) {                              // 插入新结点，树长高
        T = (BSTree)malloc(sizeof(BSTNode));
        T->data = e;
        T->lchild = T->rchild = NULL;
        T->bf = EH;
        taller = TRUE;
        return 1;
    }
    if (e == T->data) { taller = FALSE; return 0; }  // 已存在，不插入
    if (e < T->data) {                     // 在左子树中插入
        if (!InsertAVL(T->lchild, e, taller)) return 0;
        if (taller)
            switch (T->bf) {
            case LH: LeftBalance(T); taller = FALSE; break;  // 原左高，作左平衡处理
            case EH: T->bf = LH; taller = TRUE; break;       // 原等高，现左高
            case RH: T->bf = EH; taller = FALSE; break;      // 原右高，现等高
            }
    } else {                               // 在右子树中插入
        if (!InsertAVL(T->rchild, e, taller)) return 0;
        if (taller)
            switch (T->bf) {
            case LH: T->bf = EH; taller = FALSE; break;
            case EH: T->bf = RH; taller = TRUE; break;
            case RH: RightBalance(T); taller = FALSE; break; // 原右高，作右平衡处理
            }
    }
    return 1;
}

void PrintAVL(BSTree T, int depth) {       // 横向打印树（右子树在上），括号内为平衡因子
    if (!T) return;
    PrintAVL(T->rchild, depth + 1);
    printf("%*s%d(%d)\n", depth * 6, "", T->data, T->bf);
    PrintAVL(T->lchild, depth + 1);
}

int main() {
    BSTree T = NULL;
    Boolean taller;
    ElemType a[] = {5, 3, 7, 6, 9};
    for (int i = 0; i < 5; ++i)
        InsertAVL(T, a[i], taller);
    printf("依次输入{5,3,7,6,9}后：\n");
    PrintAVL(T, 0);
    InsertAVL(T, 2, taller);
    InsertAVL(T, 15, taller);
    printf("插入2和15后：\n");
    PrintAVL(T, 0);
    return 0;
}
