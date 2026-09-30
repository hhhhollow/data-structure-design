#include <stdio.h>
#include <stdlib.h>

#define OK 1
#define ERROR 0
#define OVERFLOW -2

typedef int Status;
typedef int ElemType;

// 三元组类型定义（参考课件第5章Slide 11）
typedef struct {
    int i, j;   // 行下标、列下标（从1开始）
    ElemType e; // 非零元素值
} Triple;

// 三元组顺序表类型定义（参考课件第5章Slide 11）
typedef struct {
    Triple *nzElem; // 动态分配的非零元三元组表，nzElem[0]未用
    int     m, n, t;// 矩阵的行数、列数和非零元个数
    int     size;   // 三元组表容量
} *TSMat, TSMatrix;

// 存储分配辅助操作（参考课件Slide 11）
TSMat allocTSMat(int m, int n, int t) {
    if (m < 1 || n < 1 || t < 0) return NULL;
    TSMat M = (TSMat)malloc(sizeof(*M));
    if (!M) exit(OVERFLOW);
    M->nzElem = (Triple *)malloc((t + 1) * sizeof(Triple));
    if (!M->nzElem) exit(OVERFLOW);
    M->m = m;
    M->n = n;
    M->t = t;
    M->size = t + 1;
    return M;
}

// 稀疏矩阵相加算法：C = A + B
Status AddTSMatrix(TSMat A, TSMat B, TSMat *C) {
    if (A->m != B->m || A->n != B->n) return ERROR;

    // 结果矩阵最多含有 A->t + B->t 个非零元
    *C = allocTSMat(A->m, A->n, A->t + B->t);

    int pa = 1, pb = 1, pc = 0;
    while (pa <= A->t && pb <= B->t) {
        // 计算行主序的一维线性位置
        int posA = (A->nzElem[pa].i - 1) * A->n + A->nzElem[pa].j;
        int posB = (B->nzElem[pb].i - 1) * B->n + B->nzElem[pb].j;

        if (posA < posB) {
            (*C)->nzElem[++pc] = A->nzElem[pa++];
        } else if (posA > posB) {
            (*C)->nzElem[++pc] = B->nzElem[pb++];
        } else {
            // 同一位置，元素相加
            ElemType sum = A->nzElem[pa].e + B->nzElem[pb].e;
            if (sum != 0) {
                pc++;
                (*C)->nzElem[pc].i = A->nzElem[pa].i;
                (*C)->nzElem[pc].j = A->nzElem[pa].j;
                (*C)->nzElem[pc].e = sum;
            }
            pa++;
            pb++;
        }
    }

    // 复制A剩余的非零元
    while (pa <= A->t) {
        (*C)->nzElem[++pc] = A->nzElem[pa++];
    }
    // 复制B剩余的非零元
    while (pb <= B->t) {
        (*C)->nzElem[++pc] = B->nzElem[pb++];
    }

    (*C)->t = pc; // 更新实际非零元个数
    return OK;
}

// 打印三元组顺序表
void PrintTripleTable(const char *name, TSMat M) {
    printf("%s 的三元组表 (行数=%d, 列数=%d, 非零元数=%d):\n", name, M->m, M->n, M->t);
    printf("   序号 | 行(i) | 列(j) | 元素值(e)\n");
    printf("  --------------------------------\n");
    for (int k = 1; k <= M->t; k++) {
        printf("    %2d  |   %d   |   %d   |    %d\n", k, M->nzElem[k].i, M->nzElem[k].j, M->nzElem[k].e);
    }
    printf("\n");
}

// 打印稀疏矩阵的二维阵列视图
void PrintMatrixView(const char *name, TSMat M) {
    printf("%s 矩阵视图 (%d x %d):\n", name, M->m, M->n);
    int k = 1;
    for (int r = 1; r <= M->m; r++) {
        printf("  [ ");
        for (int c = 1; c <= M->n; c++) {
            if (k <= M->t && M->nzElem[k].i == r && M->nzElem[k].j == c) {
                printf("%2d ", M->nzElem[k].e);
                k++;
            } else {
                printf("%2d ", 0);
            }
        }
        printf("]\n");
    }
    printf("\n");
}

int main(void) {
    printf("【题目4：三元组顺序表实现稀疏矩阵相加 C = A + B】\n\n");

    // 构造题目指定的 3x3 矩阵 A:
    // [ 1 0 0 ]
    // [ 0 2 0 ]
    // [ 0 3 0 ]
    TSMat A = allocTSMat(3, 3, 3);
    A->nzElem[1] = (Triple){1, 1, 1};
    A->nzElem[2] = (Triple){2, 2, 2};
    A->nzElem[3] = (Triple){3, 2, 3};

    // 构造题目指定的 3x3 矩阵 B:
    // [ 1 0 0 ]
    // [ 0 0 2 ]
    // [ 0 3 0 ]
    TSMat B = allocTSMat(3, 3, 3);
    B->nzElem[1] = (Triple){1, 1, 1};
    B->nzElem[2] = (Triple){2, 3, 2};
    B->nzElem[3] = (Triple){3, 2, 3};

    PrintTripleTable("矩阵 A", A);
    PrintMatrixView("矩阵 A", A);

    PrintTripleTable("矩阵 B", B);
    PrintMatrixView("矩阵 B", B);

    // 矩阵相加 C = A + B
    TSMat C = NULL;
    AddTSMatrix(A, B, &C);

    printf("================ 结果矩阵 C = A + B ================\n");
    PrintTripleTable("矩阵 C", C);
    PrintMatrixView("矩阵 C", C);

    return 0;
}
