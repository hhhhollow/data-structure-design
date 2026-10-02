#include <stdio.h>
#include <stdlib.h>

#define OK 1
#define ERROR 0
#define OVERFLOW -2

typedef int Status;
typedef int ElemType;

// 顺序栈类型定义（参考课件第3章Slide 5，提示命名：sqStack）
typedef struct {
    ElemType *base; // 栈底指针
    ElemType *top;  // 栈顶指针
    int       size; // 栈容量
} SqStack, *sqStack;

// 栈初始化操作（参考课件Slide 6）
sqStack InitStack(int size) {
    sqStack S = (sqStack)malloc(sizeof(SqStack));
    if (!S) exit(OVERFLOW);
    S->base = (ElemType*)malloc(size * sizeof(ElemType));
    if (!S->base) exit(OVERFLOW);
    S->top = S->base;
    S->size = size;
    return S;
}

// 判栈空操作
int StackEmpty(sqStack S) {
    return S->top == S->base;
}

// 入栈操作（参考课件Slide 7）
Status Push(sqStack S, ElemType e) {
    if (!S) return ERROR;
    if (S->top - S->base >= S->size) { // 栈满扩容
        ElemType *newBase = (ElemType*)realloc(S->base, S->size * 2 * sizeof(ElemType));
        if (!newBase) return ERROR;
        S->base = newBase;
        S->top = S->base + S->size;
        S->size *= 2;
    }
    *S->top++ = e;
    return OK;
}

// 出栈操作（参考课件Slide 7）
ElemType Pop(sqStack S) {
    if (!S || S->top == S->base) return -1;
    return *--S->top;
}

// 数制转换：对于非负十进制整数N，打印输出等值的八进制数（参考课件算法3.1 conversion）
void conversion(unsigned int N) {
    sqStack S = InitStack(6);
    if (N == 0) {
        Push(S, 0);
    } else {
        while (N) {
            Push(S, N % 8);
            N = N / 8;
        }
    }
    while (!StackEmpty(S)) {
        printf("%d", Pop(S));
    }
    printf("\n");
    free(S->base);
    free(S);
}

int main(void) {
    printf("【题目2：顺序栈实现十进制转换为八进制】\n");

    // 1. 课件示例验证：1348 转换为八进制应为 2504
    unsigned int test1 = 1348;
    printf("示例测试1：十进制 %u 转换为八进制为：", test1);
    conversion(test1);

    // 2. 边界测试：0
    unsigned int test2 = 0;
    printf("示例测试2：十进制 %u 转换为八进制为：", test2);
    conversion(test2);

    // 3. 用户输入交互测试
    unsigned int n;
    printf("\n请输入一个非负十进制整数: ");
    if (scanf("%u", &n) == 1) {
        printf("十进制 %u 转换为八进制为：", n);
        conversion(n);
    }

    return 0;
}
