#include <stdio.h>
#include <stdlib.h>

#define MAX_VEX_NUM 20
#define INFINITY 32767

typedef int VertexType;

// ================= 1. 邻接表结构定义（满足题设“存入邻接表”） =================
// 参考课件第7章Slide 8
typedef struct ArcNode {
    int adjvex;              // 该弧所指向的顶点位置
    int weight;              // 弧权值
    struct ArcNode *nextarc; // 指向下一弧结点的指针
} ArcNode;

typedef struct VNode {
    VertexType data;         // 顶点数据
    ArcNode *firstarc;       // 指向第一条依附该顶点的弧结点
} VNode, AdjList[MAX_VEX_NUM];

typedef struct {
    AdjList vertices;        // 顶点表
    int vexnum, arcnum;      // 图的顶点数与弧数
} ALGraph;

// ================= 2. 邻接矩阵结构定义（满足题设MiniSpanTree_P签名） =================
// 参考课件第7章Slide 5与Slide 20-22
typedef struct {
    VertexType vexs[MAX_VEX_NUM];             // 顶点数组
    int arcs[MAX_VEX_NUM][MAX_VEX_NUM];       // 邻接矩阵
    int vexnum, arcnum;                       // 顶点数与边数
} MGraph;

// 辅助数组元素定义（参考课件Slide 20：closedge数组）
typedef struct {
    VertexType adjvex; // U中顶点的名称
    int lowcost;       // 边权值，0表示已并入集合U
} ClosedgeInfo;

// 边信息结构
typedef struct {
    VertexType u, v;
    int w;
} EdgeInfo;

// 题目给定的边和权值列表
static EdgeInfo edgeList[] = {
    {1, 2, 2}, {1, 3, 3}, {5, 6, 6}, {1, 5, 5}, {1, 6, 6},
    {2, 3, 3}, {2, 4, 4}, {3, 4, 3}, {4, 5, 5}, {4, 6, 6}
};
static int totalEdges = 10;
static int totalVex = 6;

// 构造邻接表 ALGraph
void CreateALGraph(ALGraph *G) {
    G->vexnum = totalVex;
    G->arcnum = totalEdges;
    for (int i = 0; i < G->vexnum; i++) {
        G->vertices[i].data = i + 1; // 顶点标号 1~6
        G->vertices[i].firstarc = NULL;
    }

    // 插入无向边（头插法）
    for (int k = 0; k < totalEdges; k++) {
        int u = edgeList[k].u - 1;
        int v = edgeList[k].v - 1;
        int w = edgeList[k].w;

        // 插入边 u -> v
        ArcNode *node1 = (ArcNode *)malloc(sizeof(ArcNode));
        node1->adjvex = v;
        node1->weight = w;
        node1->nextarc = G->vertices[u].firstarc;
        G->vertices[u].firstarc = node1;

        // 插入边 v -> u
        ArcNode *node2 = (ArcNode *)malloc(sizeof(ArcNode));
        node2->adjvex = u;
        node2->weight = w;
        node2->nextarc = G->vertices[v].firstarc;
        G->vertices[v].firstarc = node2;
    }
}

// 打印邻接表
void PrintALGraph(ALGraph G) {
    printf("--- 图的邻接表存储表示 ---\n");
    for (int i = 0; i < G.vexnum; i++) {
        printf("顶点 [%d]", G.vertices[i].data);
        ArcNode *p = G.vertices[i].firstarc;
        while (p) {
            printf(" -> (至顶点%d, 权值:%d)", G.vertices[p->adjvex].data, p->weight);
            p = p->nextarc;
        }
        printf(" -> NULL\n");
    }
    printf("\n");
}

// 构造邻接矩阵 MGraph
void CreateMGraph(MGraph *G) {
    G->vexnum = totalVex;
    G->arcnum = totalEdges;
    for (int i = 0; i < G->vexnum; i++) {
        G->vexs[i] = i + 1;
        for (int j = 0; j < G->vexnum; j++) {
            G->arcs[i][j] = (i == j) ? 0 : INFINITY;
        }
    }
    for (int k = 0; k < totalEdges; k++) {
        int i = edgeList[k].u - 1;
        int j = edgeList[k].v - 1;
        int w = edgeList[k].w;
        G->arcs[i][j] = G->arcs[j][i] = w;
    }
}

// 定位顶点在数组中的下标
int LocateVex(MGraph G, VertexType u) {
    for (int i = 0; i < G.vexnum; i++) {
        if (G.vexs[i] == u) return i;
    }
    return -1;
}

// 普利姆算法求解最小生成树（题目要求实现的操作）
// 参考课件第7章Slide 20-22
void MiniSpanTree_P(MGraph G, VertexType u) {
    int k = LocateVex(G, u);
    if (k == -1) {
        printf("起始顶点不存在！\n");
        return;
    }

    ClosedgeInfo closedge[MAX_VEX_NUM];

    // 初始化 closedge 数组
    for (int j = 0; j < G.vexnum; ++j) {
        if (j != k) {
            closedge[j].adjvex = u;
            closedge[j].lowcost = G.arcs[k][j];
        }
    }
    closedge[k].adjvex = u;
    closedge[k].lowcost = 0; // 起始顶点u并入U集合

    printf("--- 普利姆算法求解最小生成树（从顶点 %d 出发）---\n", u);
    int totalWeight = 0;

    for (int i = 1; i < G.vexnum; ++i) {
        // 在 closedge 数组中选择 lowcost 最小的顶点
        int minCost = INFINITY;
        int minIdx = -1;
        for (int j = 0; j < G.vexnum; ++j) {
            if (closedge[j].lowcost != 0 && closedge[j].lowcost < minCost) {
                minCost = closedge[j].lowcost;
                minIdx = j;
            }
        }

        if (minIdx == -1) {
            printf("图不连通，无法生成完整最小生成树！\n");
            return;
        }

        // 输出选取的最小生成树边
        printf("  第 %d 步选取边: (%d, %d), 权值: %d\n", i, closedge[minIdx].adjvex, G.vexs[minIdx], minCost);
        totalWeight += minCost;
        closedge[minIdx].lowcost = 0; // 将选中的顶点并入集合U

        // 更新 closedge 数组中各顶点的最小边
        for (int j = 0; j < G.vexnum; ++j) {
            if (closedge[j].lowcost != 0 && G.arcs[minIdx][j] < closedge[j].lowcost) {
                closedge[j].adjvex = G.vexs[minIdx];
                closedge[j].lowcost = G.arcs[minIdx][j];
            }
        }
    }

    printf("最小生成树的总权值为: %d\n\n", totalWeight);
}

// 将邻接表转换为邻接矩阵（实现题设“存入邻接表”与“MiniSpanTree_P(MGraph G, VertexType u)”的衔接）
void ALGraphToMGraph(const ALGraph *alg, MGraph *mg) {
    mg->vexnum = alg->vexnum;
    mg->arcnum = alg->arcnum;
    for (int i = 0; i < mg->vexnum; i++) {
        mg->vexs[i] = alg->vertices[i].data;
        for (int j = 0; j < mg->vexnum; j++) {
            mg->arcs[i][j] = (i == j) ? 0 : INFINITY;
        }
    }
    for (int i = 0; i < alg->vexnum; i++) {
        ArcNode *p = alg->vertices[i].firstarc;
        while (p) {
            mg->arcs[i][p->adjvex] = p->weight;
            p = p->nextarc;
        }
    }
}

// 释放邻接表内存空间
void DestroyALGraph(ALGraph *G) {
    for (int i = 0; i < G->vexnum; i++) {
        ArcNode *p = G->vertices[i].firstarc;
        while (p) {
            ArcNode *q = p->nextarc;
            free(p);
            p = q;
        }
        G->vertices[i].firstarc = NULL;
    }
    G->vexnum = G->arcnum = 0;
}

int main(void) {
    printf("【题目5：图的邻接表存储与普利姆算法求解最小生成树】\n\n");

    // 1. 将题目图存入邻接表并打印验证（满足题设“将右图存入邻接表”）
    ALGraph alg;
    CreateALGraph(&alg);
    PrintALGraph(alg);

    // 2. 将邻接表转换为邻接矩阵，并执行题目要求的操作 MiniSpanTree_P(MGraph G, VertexType u)
    MGraph mg;
    ALGraphToMGraph(&alg, &mg);
    MiniSpanTree_P(mg, 1);

    // 3. 释放邻接表动态分配的内存
    DestroyALGraph(&alg);

    return 0;
}
