#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_VEX_NUM 10
#define INFINITY INT_MAX                           // 权值的最大值∞
typedef int VertexType;
typedef struct { VertexType v1, v2; int weight; } ArcInfo;

typedef struct {                                   // 邻接矩阵存储结构（课件第7章）
    VertexType vexs[MAX_VEX_NUM];                  // 顶点向量
    int arcs[MAX_VEX_NUM][MAX_VEX_NUM];            // 邻接矩阵
    int n, a;                                      // 顶点数和边（弧）数
} MGraph;

typedef struct {                                   // 课件第7章第28页：V-U中顶点的当前最短路径信息
    int prev;                                      // 当前最短路径上该顶点的前驱顶点位序
    int lowcost;                                   // 当前最短路径的长度
} DistInfo;
DistInfo Dist[MAX_VEX_NUM];                        // 全局辅助数组

int LocateVex(MGraph G, VertexType v) {            // 返回顶点v在G中的位序
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

void ShortestPath_DIJ(MGraph G, int v0) {          // 算法7.12：迪杰斯特拉算法求源点v0到其余顶点的最短路径
    int final[MAX_VEX_NUM] = {0};                  // final[j]=1表示顶点j已并入集合U
    for (int j = 0; j < G.n; ++j) {                // Dist数组初始化
        Dist[j].lowcost = G.arcs[v0][j];
        Dist[j].prev = (j != v0 && Dist[j].lowcost < INFINITY) ? v0 : -1;
    }
    Dist[v0].lowcost = 0;
    final[v0] = 1;                                 // 初始U={v0}
    for (int m = 1; m < G.n; ++m) {                // 按路径长度升序依次求得其余n-1个顶点的最短路径
        int k = -1;
        for (int j = 0; j < G.n; ++j)              // 在V-U中找lowcost最小的顶点k
            if (!final[j] && (k < 0 || Dist[j].lowcost < Dist[k].lowcost))
                k = j;
        if (k < 0 || Dist[k].lowcost == INFINITY) break;
        final[k] = 1;                              // 顶点k并入集合U
        for (int j = 0; j < G.n; ++j)              // 更新V-U中各顶点的最短路径信息Dist
            if (!final[j] && G.arcs[k][j] < INFINITY &&
                Dist[k].lowcost + G.arcs[k][j] < Dist[j].lowcost) {
                Dist[j].lowcost = Dist[k].lowcost + G.arcs[k][j];
                Dist[j].prev = k;
            }
    }
}

void OutputPath(MGraph G, DistInfo Dist[], int k) { // 课件第7章第29页：递归输出源点到第k顶点的最短路径
    if (k == -1) return;
    OutputPath(G, Dist, Dist[k].prev);
    printf("%d ", G.vexs[k]);
}

int main() {
    VertexType V[] = {1, 2, 3, 4, 5, 6};
    ArcInfo A[] = {{1, 2, 2}, {1, 3, 5}, {1, 5, 5}, {2, 3, 2}, {2, 4, 4},
                   {3, 4, 1}, {3, 6, 5}, {4, 5, 2}, {4, 6, 3}, {5, 6, 6}};
    MGraph G;
    CreateUDN(&G, V, 6, A, 10);
    int v0 = 0;                                    // 从顶点1（下标0）出发
    ShortestPath_DIJ(G, v0);
    printf("迪杰斯特拉算法求得从顶点 %d 到其余各顶点的最短路径：\n", G.vexs[v0]);
    for (int k = 1; k < G.n; ++k) {
        printf("  %d -> %d (最短距离 = %d)，路径：", G.vexs[v0], G.vexs[k], Dist[k].lowcost);
        OutputPath(G, Dist, k);
        printf("\n");
    }
    return 0;
}
