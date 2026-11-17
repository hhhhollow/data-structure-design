/* 课程设计 2.1  批量增删功能（有序单链表集合运算）
 * 数据结构：带头结点的递增有序单链表 LNode, *LinkList
 * 基本操作：CreateSet(&T, Str)、Union(&T, S1, S2)、Difference(&T, S1, S2)、PrintSet(T)
 */
#include <stdio.h>
#include <stdlib.h>

typedef struct LNode {
    char data;
    struct LNode *next;
} LNode, *LinkList;

/* 在链尾追加值为 e 的新结点，返回新链尾指针 */
static LNode *AppendNode(LNode *tail, char e) {
    LNode *s = (LNode *)malloc(sizeof(LNode));
    s->data = e;
    s->next = NULL;
    tail->next = s;
    return s;
}

/* 向有序集合 T 中插入元素 e（保持递增有序，若已存在则去重） */
static void InsertElem(LinkList T, char e) {
    LNode *p = T;
    while (p->next && p->next->data < e) p = p->next;
    if (p->next && p->next->data == e) return;
    LNode *s = (LNode *)malloc(sizeof(LNode));
    s->data = e;
    s->next = p->next;
    p->next = s;
}

/* 释放集合 T 的所有结点（含头结点） */
static void DestroySet(LinkList *T) {
    LNode *p = *T;
    while (p) {
        LNode *q = p->next;
        free(p);
        p = q;
    }
    *T = NULL;
}

/* 创建集合：由字符串 Str 创建带头结点的递增有序单链表集合 T */
void CreateSet(LinkList *T, const char *Str) {
    *T = (LinkList)malloc(sizeof(LNode));
    (*T)->next = NULL;
    for (; *Str; ++Str) InsertElem(*T, *Str);
}

/* 求并集：有序链表二路归并求 T = S1 ∪ S2 */
void Union(LinkList *T, LinkList S1, LinkList S2) {
    CreateSet(T, "");
    LNode *tail = *T, *p = S1->next, *q = S2->next;
    while (p && q) {
        if (p->data < q->data) {
            tail = AppendNode(tail, p->data);
            p = p->next;
        } else if (q->data < p->data) {
            tail = AppendNode(tail, q->data);
            q = q->next;
        } else {
            tail = AppendNode(tail, p->data);
            p = p->next;
            q = q->next;
        }
    }
    while (p) { tail = AppendNode(tail, p->data); p = p->next; }
    while (q) { tail = AppendNode(tail, q->data); q = q->next; }
}

/* 求差集：有序链表求 T = S1 - S2 */
void Difference(LinkList *T, LinkList S1, LinkList S2) {
    CreateSet(T, "");
    LNode *tail = *T, *p = S1->next, *q = S2->next;
    while (p && q) {
        if (p->data < q->data) {
            tail = AppendNode(tail, p->data);
            p = p->next;
        } else if (q->data < p->data) {
            q = q->next;
        } else {
            p = p->next;
            q = q->next;
        }
    }
    while (p) { tail = AppendNode(tail, p->data); p = p->next; }
}

/* 显示集合 T 的元素 */
void PrintSet(LinkList T) {
    printf("{ ");
    for (LNode *p = T->next; p; p = p->next) printf("%c ", p->data);
    printf("}\n");
}

int run_2_1(void) {
    LinkList A, B, C, U, D;

    printf("===== 一、批量增删功能（有序单链表集合运算） =====\n");
    CreateSet(&A, "513973");    /* 含乱序与重复字符 '3'，验证排序与去重 */
    CreateSet(&B, "6273");
    CreateSet(&C, "738");
    printf("原数据集 A   = "); PrintSet(A);
    printf("新增数据集 B = "); PrintSet(B);
    printf("删除数据集 C = "); PrintSet(C);

    Union(&U, A, B);
    printf("(1) 批量增加 A ∪ B       = "); PrintSet(U);
    Difference(&D, U, C);
    printf("(2) 批量删除 (A ∪ B) - C = "); PrintSet(D);
    DestroySet(&D);
    Difference(&D, A, C);
    printf("(3) 直接删除 A - C       = "); PrintSet(D);

    DestroySet(&A);
    DestroySet(&B);
    DestroySet(&C);
    DestroySet(&U);
    DestroySet(&D);
    return 0;
}

#ifndef NO_MAIN
int main(void) { return run_2_1(); }
#endif
