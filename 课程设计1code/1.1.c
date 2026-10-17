#include <stdio.h>
#include <stdlib.h>

typedef int ElemType;
typedef struct LNode
{
    ElemType data;
    struct LNode *next;
} LNode, *LinkList;

LinkList CreateList(int n, ElemType s[])
{ // 算法2.11 尾插法建立带头结点的单链表
    LinkList L, p;
    L = p = (LinkList)malloc(sizeof(LNode)); // 建头结点，p指向表尾
    for (int i = 0; i < n; ++i)
    {
        p->next = (LinkList)malloc(sizeof(LNode));
        p = p->next;
        p->data = s[i]; // p指向新表尾，存入元素
    }
    p->next = NULL;
    return L;
}

int LocateElem(LinkList L, ElemType e)
{ // 若L中存在e返回1，否则返回0
    for (LinkList p = L->next; p; p = p->next)
        if (p->data == e)
            return 1;
    return 0;
}

void Union(LinkList LA, LinkList LB)
{ // 算法2.1 将在LB中但不在LA中的元素插入LA
    LinkList tail = LA, p, s;
    while (tail->next)
        tail = tail->next;             // tail指向LA表尾
    for (p = LB->next; p; p = p->next) // 依次取LB中的元素
        if (!LocateElem(LA, p->data))
        { // 若LA中不存在，则插入LA表尾
            s = (LinkList)malloc(sizeof(LNode));
            s->data = p->data;
            s->next = NULL;
            tail->next = s;
            tail = s;
        }
}

int main()
{
    ElemType a[] = {1, 3, 5}, b[] = {1, 4, 6, 2};
    LinkList LA = CreateList(3, a), LB = CreateList(4, b);
    Union(LA, LB);
    for (LinkList p = LA->next; p; p = p->next)
        printf("%d ", p->data);
    return 0;
}