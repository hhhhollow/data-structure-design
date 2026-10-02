#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VEX_NUM 20
#define INFINITY 32767

// 顶点信息类型
typedef struct {
    int id;             // 顶点编号
    char name[32];      // 地点名称（例如校园建筑）
} VertexType;

// 图的邻接矩阵存储结构（参考课件第7章）
typedef struct {
    VertexType vexs[MAX_VEX_NUM];        // 顶点表
    int arcs[MAX_VEX_NUM][MAX_VEX_NUM];  // 邻接矩阵（弧权值）
    int vexnum, arcnum;                  // 顶点数与弧数
} MGraph;

// 最短路径辅助结构（参考课件第7章Slide 27-28 DistInfo定义）
typedef struct {
    int prev;    // 当前最短路径上该顶点的前驱顶点的位序
    int lowcost; // 当前最短路径的长度（若不可达为INFINITY）
} DistInfo;

// 初始化邻接矩阵图
void InitGraph(MGraph *G, int vexnum) {
    G->vexnum = vexnum;
    G->arcnum = 0;
    for (int i = 0; i < vexnum; i++) {
        for (int j = 0; j < vexnum; j++) {
            G->arcs[i][j] = (i == j) ? 0 : INFINITY;
        }
    }
}

// 设置顶点信息
void SetVertex(MGraph *G, int idx, int id, const char *name) {
    if (idx >= 0 && idx < G->vexnum) {
        G->vexs[idx].id = id;
        strncpy(G->vexs[idx].name, name, sizeof(G->vexs[idx].name) - 1);
        G->vexs[idx].name[sizeof(G->vexs[idx].name) - 1] = '\0';
    }
}

// 添加带权有向边/双向边
void AddEdge(MGraph *G, int u, int v, int w) {
    if (u >= 0 && u < G->vexnum && v >= 0 && v < G->vexnum) {
        G->arcs[u][v] = w;
        G->arcs[v][u] = w; // 校园道路为双向连通图
        G->arcnum++;
    }
}

// 迪杰斯特拉单源最短路径算法 ShortestPath_DIJ
// 参考课件第7章 算法7.15及Slide 26-29
void ShortestPath_DIJ(MGraph G, int v0, DistInfo Dist[]) {
    int final[MAX_VEX_NUM]; // final[i]=1 表示已求得从源点 v0 到顶点 i 的最短路径

    // 1. 初始化各顶点的最短路径长度与前驱顶点
    for (int v = 0; v < G.vexnum; v++) {
        final[v] = 0;
        Dist[v].lowcost = G.arcs[v0][v];
        if (Dist[v].lowcost < INFINITY && v != v0) {
            Dist[v].prev = v0;
        } else {
            Dist[v].prev = -1;
        }
    }

    Dist[v0].lowcost = 0;
    Dist[v0].prev = -1;
    final[v0] = 1; // 将源点 v0 并入集合 U

    // 2. 主循环：逐个求解其余 n-1 个顶点的最短路径
    for (int i = 1; i < G.vexnum; i++) {
        int minCost = INFINITY;
        int k = -1;

        // 在未确定最短路径的顶点集中寻找 lowcost 最小的顶点 k
        for (int w = 0; w < G.vexnum; w++) {
            if (!final[w] && Dist[w].lowcost < minCost) {
                k = w;
                minCost = Dist[w].lowcost;
            }
        }

        if (k == -1) break; // 其余顶点均不可达
        final[k] = 1;       // 将顶点 k 加入已确定集合 U

        // 3. 更新集合 V - U 中各顶点的最短路径及前驱顶点
        for (int w = 0; w < G.vexnum; w++) {
            if (!final[w] && G.arcs[k][w] < INFINITY) {
                if (Dist[k].lowcost + G.arcs[k][w] < Dist[w].lowcost) {
                    Dist[w].lowcost = Dist[k].lowcost + G.arcs[k][w];
                    Dist[w].prev = k;
                }
            }
        }
    }
}

// 格式化输出最短路径
void PrintShortestPath(MGraph G, DistInfo Dist[], int dest) {
    if (Dist[dest].lowcost == INFINITY) {
        printf("  目标: %-10s 最短距离: 无穷大(不可达)\n", G.vexs[dest].name);
        return;
    }

    // 回溯路径到临时数组中
    int path[MAX_VEX_NUM];
    int count = 0;
    int curr = dest;
    while (curr != -1) {
        path[count++] = curr;
        curr = Dist[curr].prev;
    }

    printf("  目标: %-10s 距离: %3d 米  路径: ", G.vexs[dest].name, Dist[dest].lowcost);
    for (int i = count - 1; i >= 0; i--) {
        printf("%s", G.vexs[path[i]].name);
        if (i > 0) printf(" -> ");
    }
    printf("\n");
}

int main(void) {
    printf("======================================================================\n");
    printf("   题目四：图的使用——迪杰斯特拉（Dijkstra）算法求解单源最短路径\n");
    printf("======================================================================\n\n");

    MGraph G;
    int n = 6;
    InitGraph(&G, n);

    // 设定实际应用场景：高校校园导游/路径规划网
    SetVertex(&G, 0, 0, "校门");
    SetVertex(&G, 1, 1, "图书馆");
    SetVertex(&G, 2, 2, "教学楼");
    SetVertex(&G, 3, 3, "实验中心");
    SetVertex(&G, 4, 4, "学生宿舍");
    SetVertex(&G, 5, 5, "体育馆");

    // 添加校园道路与权值（米）
    AddEdge(&G, 0, 1, 300); // 校门 <-> 图书馆 300米
    AddEdge(&G, 0, 2, 200); // 校门 <-> 教学楼 200米
    AddEdge(&G, 1, 3, 250); // 图书馆 <-> 实验中心 250米
    AddEdge(&G, 1, 4, 400); // 图书馆 <-> 学生宿舍 400米
    AddEdge(&G, 2, 3, 150); // 教学楼 <-> 实验中心 150米
    AddEdge(&G, 2, 5, 350); // 教学楼 <-> 体育馆 350米
    AddEdge(&G, 3, 4, 100); // 实验中心 <-> 学生宿舍 100米
    AddEdge(&G, 4, 5, 120); // 学生宿舍 <-> 体育馆 120米

    printf("1. 校园导游图构建完成（共 %d 个景点，%d 条道路）\n", G.vexnum, G.arcnum);
    printf("   景点列表：0:校门, 1:图书馆, 2:教学楼, 3:实验中心, 4:学生宿舍, 5:体育馆\n\n");

    int v0 = 0; // 以“校门”作为出发源点
    DistInfo Dist[MAX_VEX_NUM];

    printf("2. 调用 ShortestPath_DIJ 算法，求解以【%s】为源点的单源最短路径：\n\n", G.vexs[v0].name);
    ShortestPath_DIJ(G, v0, Dist);

    printf("3. 规划结果输出（由近及远的最优导航路线）：\n");
    for (int i = 0; i < G.vexnum; i++) {
        PrintShortestPath(G, Dist, i);
    }

    printf("\n======================================================================\n");
    return 0;
}
