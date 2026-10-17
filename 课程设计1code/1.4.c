#include <stdio.h>
#include <stdlib.h>

typedef int ElemType;
typedef struct {
    int i, j;                              // 该非零元的行下标和列下标
    ElemType e;                            // 非零元
} Triple;                                  // 三元组类型
typedef struct {
    Triple *nzElem;                        // 非零元三元组表，nzElem[0]未用
    int m, n, t;                           // 矩阵的行数、列数和非零元个数
    int size;                              // 三元组表容量
} *TSMat;                                  // 三元组顺序表指针类型

TSMat allocTSMat(int m, int n, int t) {    // 存储分配辅助操作（未存储非零元）
    TSMat M = (TSMat)malloc(sizeof(*M));
    M->nzElem = (Triple*)malloc((t + 1) * sizeof(Triple));
    M->m = m; M->n = n; M->t = t; M->size = t + 1;
    return M;
}

TSMat CreateTSMat(int m, int n, ElemType *a) { // 由m×n二维数组a构造三元组顺序表（行序为主序）
    int t = 0, k = 1;
    for (int p = 0; p < m * n; ++p)
        if (a[p]) ++t;                     // 统计非零元个数
    TSMat M = allocTSMat(m, n, t);
    for (int i = 1; i <= m; ++i)
        for (int j = 1; j <= n; ++j)
            if (a[(i - 1) * n + j - 1]) {
                M->nzElem[k].i = i; M->nzElem[k].j = j;
                M->nzElem[k++].e = a[(i - 1) * n + j - 1];
            }
    return M;
}

TSMat AddSMat(TSMat A, TSMat B) {          // 求C=A+B，结果存入另设的三元组表C
    if (A->m != B->m || A->n != B->n) return NULL;  // 行列数不等不能相加
    TSMat C = allocTSMat(A->m, A->n, A->t + B->t);
    int p = 1, q = 1, k = 1, d;
    while (p <= A->t && q <= B->t) {
        Triple a = A->nzElem[p], b = B->nzElem[q];
        d = (a.i != b.i) ? a.i - b.i : a.j - b.j;   // 按行序为主序比较两元素位置
        if (d < 0) { C->nzElem[k++] = a; ++p; }      // A的元素在前
        else if (d > 0) { C->nzElem[k++] = b; ++q; } // B的元素在前
        else {                                       // 位置相同则相加
            if (a.e + b.e) { C->nzElem[k] = a; C->nzElem[k++].e = a.e + b.e; } // 和为0不存
            ++p; ++q;
        }
    }
    while (p <= A->t) C->nzElem[k++] = A->nzElem[p++];  // 复制A的剩余元素
    while (q <= B->t) C->nzElem[k++] = B->nzElem[q++];  // 复制B的剩余元素
    C->t = k - 1;
    return C;
}

void PrintSMat(TSMat M) {                  // 输出三元组表及对应矩阵
    for (int k = 1; k <= M->t; ++k)
        printf("(%d,%d,%d) ", M->nzElem[k].i, M->nzElem[k].j, M->nzElem[k].e);
    printf("\n");
    for (int i = 1, k = 1; i <= M->m; ++i) {
        for (int j = 1; j <= M->n; ++j)
            if (k <= M->t && M->nzElem[k].i == i && M->nzElem[k].j == j)
                printf("%3d", M->nzElem[k++].e);
            else
                printf("%3d", 0);
        printf("\n");
    }
}

int main() {
    ElemType a[3][3] = {{1, 0, 0}, {0, 2, 0}, {0, 3, 0}};
    ElemType b[3][3] = {{1, 0, 0}, {0, 0, 2}, {0, 3, 0}};
    TSMat A = CreateTSMat(3, 3, &a[0][0]), B = CreateTSMat(3, 3, &b[0][0]);
    TSMat C = AddSMat(A, B);
    printf("A:\n"); PrintSMat(A);
    printf("B:\n"); PrintSMat(B);
    printf("C = A + B:\n"); PrintSMat(C);
    return 0;
}
