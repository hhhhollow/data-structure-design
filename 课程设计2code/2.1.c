#include <stdio.h>
#include <stdlib.h>

typedef char ElemType;
typedef struct LNode
{
    ElemType data;
    struct LNode *next;
} LNode, *LinkList;

int LocateElem(LinkList L, ElemType e)
{ // 若L中存在e返回1，否则返回0
    for (LinkList p = L->next; p; p = p->next)
        if (p->data == e)
            return 1;
    return 0;
}

void AppendElem(LinkList *tail, ElemType e)
{ // 在表尾指针*tail后追加元素e
    LinkList s = (LinkList)malloc(sizeof(LNode));
    s->data = e;
    s->next = NULL;
    (*tail)->next = s;
    *tail = s;
}

void CreateSet(LinkList *T, const char *Str)
{ // 算法2.11&例2-2：由字符串Str建立去重集合链表T
    LinkList tail = *T = (LinkList)malloc(sizeof(LNode));
    (*T)->next = NULL;
    for (int i = 0; Str[i]; ++i)
        if (!LocateElem(*T, Str[i])) // 集合元素互异，去重插入表尾
            AppendElem(&tail, Str[i]);
}

void Union(LinkList *T, LinkList S1, LinkList S2)
{ // 算法2.1：求并集 T = S1 ∪ S2（批量新增）
    LinkList tail = *T = (LinkList)malloc(sizeof(LNode));
    (*T)->next = NULL;
    for (LinkList p = S1->next; p; p = p->next)
        AppendElem(&tail, p->data);
    for (LinkList p = S2->next; p; p = p->next)
        if (!LocateElem(*T, p->data)) // 将在S2中但不在S1中的元素插入T
            AppendElem(&tail, p->data);
}

void Difference(LinkList *T, LinkList S1, LinkList S2)
{ // 求差集 T = S1 - S2（批量删除）
    LinkList tail = *T = (LinkList)malloc(sizeof(LNode));
    (*T)->next = NULL;
    for (LinkList p = S1->next; p; p = p->next)
        if (!LocateElem(S2, p->data)) // 保留在S1中但不在S2中的元素
            AppendElem(&tail, p->data);
}

void PrintSet(LinkList T)
{ // 显示集合T中的元素
    printf("{ ");
    for (LinkList p = T->next; p; p = p->next)
        printf("%c ", p->data);
    printf("}\n");
}

int main()
{
    LinkList A, B, C, T1, T2;
    CreateSet(&A, "abcdef"); // 创建原数据集A
    CreateSet(&B, "efghij"); // 创建新增数据集B
    CreateSet(&C, "ace");    // 创建删除数据集C
    printf("原数据集 A = ");
    PrintSet(A);
    printf("新增数据集 B = ");
    PrintSet(B);
    printf("删除数据集 C = ");
    PrintSet(C);
    Union(&T1, A, B); // 批量新增：求并集 A ∪ B
    printf("批量新增 Union(A, B) = ");
    PrintSet(T1);
    Difference(&T2, T1, C); // 批量删除：求差集 (A ∪ B) - C
    printf("批量删除 Difference(A∪B, C) = ");
    PrintSet(T2);
    return 0;
}
