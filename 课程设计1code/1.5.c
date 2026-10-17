#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_VEX_NUM 10
#define INFINITY INT_MAX                   // 权值的最大值∞
typedef int VertexType;
typedef struct { VertexType v1, v2; int weight; } ArcInfo;  // 边的信息：两个顶点和权值

typedef struct ArcNode {                   // ---- 邻接表 ----
    int adjvex;                            // 该弧所指向的顶点的位置
    struct ArcNode *nextarc;               // 指向下一弧结点的指针
    int weight;                            // 弧权值
} ArcNode;
typedef struct {
    VertexType data;                       // 顶点数据
    ArcNode *firstarc;                     // 指向第一条依附该顶点的弧结点
} VNode;
typedef struct {
    VNode vexs[MAX_VEX_NUM];               // 顶点数组
    int n, a;                              // 顶点数和边数
} ALGraph;

typedef struct {                           // ---- 邻接矩阵 ----
    VertexType vexs[MAX_VEX_NUM];          // 顶点向量
    int arcs[MAX_VEX_NUM][MAX_VEX_NUM];    // 邻接矩阵
    int n, a;                              // 顶点数和边数
} MGraph;

struct {
    int adjIdx;                            // U中顶点的位序
    int lowcost;                           // 边的权值
} closedge[MAX_VEX_NUM];                   // V-U的顶点和当前U之间的最小边信息

int LocateVex(ALGraph G, VertexType v) {   // 返回顶点v在G中的位序
    for (int i = 0; i < G.n; ++i)
        if (G.vexs[i].data == v) return i;
    return -1;
}

void InsertArc(ALGraph *G, int i, int j, int w) {  // 在第i个顶点的链表头插入弧<i,j>
    ArcNode *p = (ArcNode*)malloc(sizeof(ArcNode));
    p->adjvex = j; p->weight = w;
    p->nextarc = G->vexs[i].firstarc;
    G->vexs[i].firstarc = p;
}

void CreateALGraph(ALGraph *G, VertexType *V, int n, ArcInfo *A, int e) { // 构造无向网的邻接表
    G->n = n; G->a = e;
    for (int i = 0; i < n; ++i) {
        G->vexs[i].data = V[i];
        G->vexs[i].firstarc = NULL;
    }
    for (int k = 0; k < e; ++k) {
        int i = LocateVex(*G, A[k].v1), j = LocateVex(*G, A[k].v2);
        InsertArc(G, i, j, A[k].weight);   // 无向网的一条边对应两条对称弧
        InsertArc(G, j, i, A[k].weight);
    }
}

void ALToMGraph(ALGraph G, MGraph *M) {    // 由邻接表G得到邻接矩阵M
    M->n = G.n; M->a = G.a;
    for (int i = 0; i < G.n; ++i) {
        M->vexs[i] = G.vexs[i].data;
        for (int j = 0; j < G.n; ++j) M->arcs[i][j] = INFINITY;
    }
    for (int i = 0; i < G.n; ++i)
        for (ArcNode *p = G.vexs[i].firstarc; p; p = p->nextarc)
            M->arcs[i][p->adjvex] = p->weight;
}

void MiniSpanTree_P(MGraph G, VertexType u) { // 普里姆算法：从顶点u出发构造G的最小生成树，输出各边
    int i, j, k, sum = 0;
    for (k = 0; G.vexs[k] != u; ++k);      // 确定起始顶点u的位序
    for (j = 0; j < G.n; ++j) {            // 辅助数组初始化
        closedge[j].adjIdx = k;
        closedge[j].lowcost = G.arcs[k][j];
    }
    closedge[k].lowcost = 0;               // 初始U={u}
    for (i = 1; i < G.n; ++i) {            // 选择其余n-1个顶点
        k = -1;
        for (j = 0; j < G.n; ++j)          // 在V-U中找lowcost最小的顶点k
            if (closedge[j].lowcost && (k < 0 || closedge[j].lowcost < closedge[k].lowcost))
                k = j;
        printf("(%d,%d) = %d\n", G.vexs[closedge[k].adjIdx], G.vexs[k], closedge[k].lowcost);
        sum += closedge[k].lowcost;
        closedge[k].lowcost = 0;           // 第k顶点并入U
        for (j = 0; j < G.n; ++j)          // 新顶点并入U后更新closedge
            if (G.arcs[k][j] < closedge[j].lowcost) {
                closedge[j].adjIdx = k;
                closedge[j].lowcost = G.arcs[k][j];
            }
    }
    printf("最小生成树总权值 = %d\n", sum);
}

int main() {
    VertexType V[] = {1, 2, 3, 4, 5, 6};
    ArcInfo A[] = {{1, 2, 2}, {1, 3, 3}, {5, 6, 6}, {1, 5, 5}, {1, 6, 6},
                   {2, 3, 3}, {2, 4, 4}, {3, 4, 3}, {4, 5, 5}, {4, 6, 6}};
    ALGraph AG;
    MGraph G;
    CreateALGraph(&AG, V, 6, A, 10);
    printf("邻接表：\n");
    for (int i = 0; i < AG.n; ++i) {
        printf("%d", AG.vexs[i].data);
        for (ArcNode *p = AG.vexs[i].firstarc; p; p = p->nextarc)
            printf(" -> %d(%d)", AG.vexs[p->adjvex].data, p->weight);
        printf("\n");
    }
    ALToMGraph(AG, &G);
    printf("普利姆算法求得的最小生成树：\n");
    MiniSpanTree_P(G, 1);
    return 0;
}
