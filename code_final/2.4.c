#include <stdio.h>

#define MAX_VERTEX_NUM 20
#define GRAPH_INF 99999

typedef struct {
    char vexs[MAX_VERTEX_NUM];                  /* 顶点向量 */
    int arcs[MAX_VERTEX_NUM][MAX_VERTEX_NUM];   /* 邻接矩阵 */
    int vexnum, arcnum;                         /* 顶点数和边数 */
} MGraph;

static struct {
    int adjvex;                                 /* U 集中的顶点下标 */
    int lowcost;                                /* 边的权值 */
} closedge[MAX_VERTEX_NUM];                     /* 记录 V-U 到 U 的最小边信息 */

/* 构造示例带权无向网（课件第07章 P61 示例图） */
static void CreateUDN(MGraph *G) {
    const char vexs[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g'};
    const int edges[][3] = {
        {0, 1, 19}, {0, 4, 14}, {0, 6, 18},
        {1, 2, 5},  {1, 3, 7},  {1, 4, 12},
        {2, 3, 3},  {3, 4, 8},  {3, 5, 21},
        {4, 6, 16}, {5, 6, 27}
    };
    int i, j;

    G->vexnum = 7;
    G->arcnum = 11;
    for (i = 0; i < G->vexnum; ++i) {
        G->vexs[i] = vexs[i];
        for (j = 0; j < G->vexnum; ++j) {
            G->arcs[i][j] = GRAPH_INF;
        }
    }
    for (i = 0; i < G->arcnum; ++i) {
        int u = edges[i][0];
        int v = edges[i][1];
        int w = edges[i][2];
        G->arcs[u][v] = w;
        G->arcs[v][u] = w;
    }
}

/* 打印邻接矩阵 */
static void PrintGraph(MGraph G) {
    int i, j;
    printf("示例无向网邻接矩阵（顶点数 = %d，边数 = %d）：\n    ", G.vexnum, G.arcnum);
    for (i = 0; i < G.vexnum; ++i) {
        printf("%4c", G.vexs[i]);
    }
    printf("\n");
    for (i = 0; i < G.vexnum; ++i) {
        printf("  %c ", G.vexs[i]);
        for (j = 0; j < G.vexnum; ++j) {
            if (G.arcs[i][j] >= GRAPH_INF) {
                printf("   ∞");
            } else {
                printf("%4d", G.arcs[i][j]);
            }
        }
        printf("\n");
    }
}

/* 在 closedge 中找出 lowcost 最小且未并入 U 的顶点下标 */
static int minimum(int n) {
    int i, k = -1;
    int min = GRAPH_INF;
    for (i = 0; i < n; ++i) {
        if (closedge[i].lowcost > 0 && closedge[i].lowcost < min) {
            min = closedge[i].lowcost;
            k = i;
        }
    }
    return k;
}

/* 算法 7.6 普里姆算法：从下标为 u 的顶点出发构造 G 的最小生成树并输出 */
void MiniSpanTree_P(MGraph G, int u) {
    int i, j, k, sum = 0;
    if (u < 0 || u >= G.vexnum) {
        return;
    }
    for (j = 0; j < G.vexnum; ++j) {            /* 辅助数组初始化 */
        if (j != u) {
            closedge[j].adjvex = u;
            closedge[j].lowcost = G.arcs[u][j];
        }
    }
    closedge[u].adjvex = u;
    closedge[u].lowcost = 0;                    /* 初始，U = {u} */

    printf("普里姆算法求最小生成树（起点 %c）：\n", G.vexs[u]);
    for (i = 1; i < G.vexnum; ++i) {            /* 选择其余 vexnum - 1 个顶点 */
        k = minimum(G.vexnum);                  /* 求出加入生成树的下一个顶点 k */
        if (k == -1) {
            printf("图不连通，无法生成完整最小生成树。\n");
            return;
        }
        printf("  边 (%c, %c) 权值 = %d\n", G.vexs[closedge[k].adjvex], G.vexs[k], closedge[k].lowcost);
        sum += closedge[k].lowcost;
        closedge[k].lowcost = 0;                /* 第 k 顶点并入 U 集 */
        for (j = 0; j < G.vexnum; ++j) {        /* 修改其它顶点的最小边 */
            if (closedge[j].lowcost > 0 && G.arcs[k][j] < closedge[j].lowcost) {
                closedge[j].adjvex = k;
                closedge[j].lowcost = G.arcs[k][j];
            }
        }
    }
    printf("最小生成树总权值 = %d\n", sum);
}

/* 算法 7.8 迪杰斯特拉算法：求源点 v0 到其余各顶点的最短路径及距离 */
void ShortestPath_DIJ(MGraph G, int v0) {
    int final[MAX_VERTEX_NUM];
    int Dist[MAX_VERTEX_NUM];
    int path[MAX_VERTEX_NUM];                   /* 记录最短路径上的前驱顶点下标 */
    int i, v, w, min;

    if (v0 < 0 || v0 >= G.vexnum) {
        return;
    }
    for (v = 0; v < G.vexnum; ++v) {            /* 初始化 */
        final[v] = 0;
        Dist[v] = G.arcs[v0][v];
        path[v] = (Dist[v] < GRAPH_INF) ? v0 : -1;
    }
    Dist[v0] = 0;
    final[v0] = 1;                              /* v0 并入已求得最短路径的顶点集 S */
    path[v0] = -1;

    for (i = 1; i < G.vexnum; ++i) {            /* 其余 vexnum - 1 个顶点 */
        min = GRAPH_INF;
        v = -1;
        for (w = 0; w < G.vexnum; ++w) {
            if (!final[w] && Dist[w] < min) {
                v = w;
                min = Dist[w];
            }
        }
        if (v == -1) {
            break;
        }
        final[v] = 1;                           /* 当前 Dist 最小者并入 S */
        for (w = 0; w < G.vexnum; ++w) {        /* 修改其它顶点的 Dist 值 */
            if (!final[w] && G.arcs[v][w] < GRAPH_INF && min + G.arcs[v][w] < Dist[w]) {
                Dist[w] = min + G.arcs[v][w];
                path[w] = v;
            }
        }
    }

    printf("迪杰斯特拉算法求最短路径（源点 %c）：\n", G.vexs[v0]);
    for (v = 0; v < G.vexnum; ++v) {
        if (v == v0) {
            continue;
        }
        if (Dist[v] >= GRAPH_INF) {
            printf("  %c -> %c : 不可达\n", G.vexs[v0], G.vexs[v]);
        } else {
            int stack[MAX_VERTEX_NUM], top = 0;
            int cur = v;
            while (cur != -1) {
                stack[top++] = cur;
                if (cur == v0) {
                    break;
                }
                cur = path[cur];
            }
            printf("  %c -> %c : 最短距离 = %2d，路径：", G.vexs[v0], G.vexs[v], Dist[v]);
            while (top > 0) {
                printf("%c", G.vexs[stack[--top]]);
                if (top > 0) {
                    printf(" -> ");
                }
            }
            printf("\n");
        }
    }
}

int run_2_4(void) {
    MGraph G;
    CreateUDN(&G);
    PrintGraph(G);
    printf("\n");
    MiniSpanTree_P(G, 0);
    printf("\n");
    ShortestPath_DIJ(G, 0);
    return 0;
}

#ifndef NO_MAIN
int main(void) {
    return run_2_4();
}
#endif
