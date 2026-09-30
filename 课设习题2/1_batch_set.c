#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OK 1
#define ERROR 0
#define OVERFLOW -2

typedef int Status;
typedef int ElemType;

// 单链表存储结构定义（参考课件第2章）
typedef struct LNode {
    ElemType data;
    struct LNode *next;
} LNode, *LinkList;

// 初始化带头结点的单链表
Status InitList(LinkList *L) {
    *L = (LinkList)malloc(sizeof(LNode));
    if (!*L) exit(OVERFLOW);
    (*L)->next = NULL;
    return OK;
}

// 在单链表中查找元素 e 是否存在，存在返回 1，否则返回 0
int LocateElem(LinkList L, ElemType e) {
    if (!L) return 0;
    LinkList p = L->next;
    while (p) {
        if (p->data == e) return 1;
        p = p->next;
    }
    return 0;
}

// 在单链表尾部插入元素 e
Status ListInsert_Back(LinkList L, ElemType e) {
    LinkList p = L;
    while (p->next) {
        p = p->next;
    }
    LinkList s = (LinkList)malloc(sizeof(LNode));
    if (!s) exit(OVERFLOW);
    s->data = e;
    s->next = NULL;
    p->next = s;
    return OK;
}

// 创建集合 CreateSet(&T, Str)
// 从字符串 Str 中解析整数并建立互异元素集合 T
Status CreateSet(LinkList *T, const char *Str) {
    InitList(T);
    if (!Str || strlen(Str) == 0) return OK;

    // 解析空格或逗号分隔的数值字符串
    const char *p = Str;
    while (*p) {
        // 跳过非数字与非负号字符
        while (*p && *p != '-' && (*p < '0' || *p > '9')) p++;
        if (!*p) break;

        char *endptr;
        long val = strtol(p, &endptr, 10);
        if (p == endptr) break;

        // 集合元素互异性：若集合中不存在该元素，则插入表尾
        if (!LocateElem(*T, (ElemType)val)) {
            ListInsert_Back(*T, (ElemType)val);
        }
        p = endptr;
    }
    return OK;
}

// 求并集 Union(&T, S1, S2)
// 计算 T = S1 ∪ S2（对应原数据集 S1 批量增加数据集 S2）
Status Union(LinkList *T, LinkList S1, LinkList S2) {
    InitList(T);
    // 1. 将 S1 中的所有元素插入 T
    LinkList p = S1 ? S1->next : NULL;
    while (p) {
        ListInsert_Back(*T, p->data);
        p = p->next;
    }
    // 2. 将 S2 中不在 T 中的元素插入 T
    p = S2 ? S2->next : NULL;
    while (p) {
        if (!LocateElem(*T, p->data)) {
            ListInsert_Back(*T, p->data);
        }
        p = p->next;
    }
    return OK;
}

// 求差集 Difference(&T, S1, S2)
// 计算 T = S1 - S2（对应原数据集 S1 批量删除数据集 S2）
Status Difference(LinkList *T, LinkList S1, LinkList S2) {
    InitList(T);
    // 遍历 S1，若元素不在 S2 中，则加入 T
    LinkList p = S1 ? S1->next : NULL;
    while (p) {
        if (!LocateElem(S2, p->data)) {
            ListInsert_Back(*T, p->data);
        }
        p = p->next;
    }
    return OK;
}

// 显示集合 PrintSet(T)
void PrintSet(LinkList T) {
    printf("{ ");
    if (T) {
        LinkList p = T->next;
        while (p) {
            printf("%d", p->data);
            if (p->next) printf(", ");
            p = p->next;
        }
    }
    printf(" }\n");
}

// 释放单链表内存
void DestroySet(LinkList *T) {
    if (!T || !*T) return;
    LinkList p = *T;
    while (p) {
        LinkList q = p->next;
        free(p);
        p = q;
    }
    *T = NULL;
}

int main(void) {
    printf("====================================================\n");
    printf("   题目一：批量增删功能（线性表集合操作）\n");
    printf("====================================================\n\n");

    LinkList A = NULL, B = NULL, C = NULL;
    LinkList T_add = NULL, T_del = NULL;

    // 1. 创建原数据集 A、新增数据集 B、删除数据集 C
    const char *strA = "1 3 5 7 9";
    const char *strB = "2 3 5 8 10";
    const char *strC = "1 5 9 11";

    CreateSet(&A, strA);
    CreateSet(&B, strB);
    CreateSet(&C, strC);

    printf("1. 初始集合建立：\n");
    printf("   原数据集 A:      "); PrintSet(A);
    printf("   新增数据集 B:    "); PrintSet(B);
    printf("   删除数据集 C:    "); PrintSet(C);
    printf("\n");

    // 2. 批量增加功能演示：原数据集 A 与新增数据集 B 求并集
    printf("2. 批量增加功能（求并集 A ∪ B）：\n");
    Union(&T_add, A, B);
    printf("   A ∪ B 结果集:    "); PrintSet(T_add);
    printf("\n");

    // 3. 批量删除功能演示：原数据集 A 删除数据集 C（求差集 A - C）
    printf("3. 批量删除功能（求差集 A - C）：\n");
    Difference(&T_del, A, C);
    printf("   A - C 结果集:    "); PrintSet(T_del);
    printf("\n");

    // 释放动态分配的内存
    DestroySet(&A);
    DestroySet(&B);
    DestroySet(&C);
    DestroySet(&T_add);
    DestroySet(&T_del);

    printf("====================================================\n");
    return 0;
}
