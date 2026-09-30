#include <stdio.h>
#include <stdlib.h>

#define OK 1
#define ERROR 0
#define OVERFLOW -2

typedef int Status;
typedef int ElemType;

// 单链表存储结构定义（参考课件第2章算法2.11）
typedef struct LNode {
    ElemType data;
    struct LNode *next;
} LNode, *LinkList;

// 由数组元素构建带头结点的单链表（参考课件算法2.11）
LinkList CreateList(int n, ElemType s[]) {
    LinkList L, p;
    if (!(L = p = (LinkList)calloc(1, sizeof(LNode)))) exit(OVERFLOW);
    for (int i = 0; i < n; ++i) {
        if (!(p->next = (LinkList)malloc(sizeof(LNode)))) exit(OVERFLOW);
        p = p->next;
        p->data = s[i];
    }
    p->next = NULL;
    return L;
}

// 在单链表中查找元素e是否存在，存在返回1，不存在返回0
int LocateElem(LinkList L, ElemType e) {
    LinkList p = L->next;
    while (p) {
        if (p->data == e) return 1;
        p = p->next;
    }
    return 0;
}

// 在单链表尾部追加元素e
void ListInsert_Back(LinkList L, ElemType e) {
    LinkList p = L;
    while (p->next) {
        p = p->next;
    }
    LinkList s = (LinkList)malloc(sizeof(LNode));
    if (!s) exit(OVERFLOW);
    s->data = e;
    s->next = NULL;
    p->next = s;
}

// 集合求并集 A = A ∪ B（参考课件算法2.1）
// 将所有在LB中但不在LA中的数据元素插入到LA中
void Union(LinkList LA, LinkList LB) {
    LinkList pb = LB->next;
    while (pb) {
        ElemType e = pb->data;
        if (!LocateElem(LA, e)) {
            ListInsert_Back(LA, e);
        }
        pb = pb->next;
    }
}

// 打印单链表元素
void PrintList(const char *name, LinkList L) {
    printf("%s = { ", name);
    LinkList p = L->next;
    while (p) {
        printf("%d", p->data);
        if (p->next) printf(", ");
        p = p->next;
    }
    printf(" }\n");
}

int main(void) {
    // 集合 A = {1, 3, 5}，集合 B = {1, 4, 6, 2}
    ElemType a[] = {1, 3, 5};
    ElemType b[] = {1, 4, 6, 2};

    LinkList LA = CreateList(3, a);
    LinkList LB = CreateList(4, b);

    printf("【题目1：单链表实现集合并集 A = A U B】\n");
    printf("初始集合：\n");
    PrintList("LA", LA);
    PrintList("LB", LB);

    // 求并集 A = A ∪ B
    Union(LA, LB);

    printf("\n执行并集操作后：\n");
    PrintList("LA (A = A U B)", LA);

    return 0;
}
