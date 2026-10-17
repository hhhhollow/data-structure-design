#include <stdio.h>
#include <stdlib.h>

#define STACK_SIZE 32                      // unsigned int 转八进制最多11位，足够用
typedef int ElemType;
typedef struct {
    ElemType *base;                        // 栈底指针
    ElemType *top;                         // 栈顶指针
} sqStack;

void InitStack(sqStack *S) {               // 构造空栈
    S->base = S->top = (ElemType*)malloc(STACK_SIZE * sizeof(ElemType));
}

int StackEmpty(sqStack S) {                // 栈空返回1，否则返回0
    return S.top == S.base;
}

void Push(sqStack *S, ElemType e) {        // 元素e入栈，top增1
    *S->top++ = e;
}

ElemType Pop(sqStack *S) {                 // top减1，返回栈顶元素
    return *--S->top;
}

void conversion(unsigned int N) {          // 算法3.1 输出N等值的八进制数
    sqStack S;
    InitStack(&S);
    do {
        Push(&S, N % 8);                   // 8除N，余数入栈
        N = N / 8;                         // 商替代原N值
    } while (N);                           // 用do-while保证N=0时也输出0
    while (!StackEmpty(S))
        printf("%d", Pop(&S));             // 出栈即由高位到低位输出
    printf("\n");
}

int main() {
    unsigned int N;
    printf("请输入非负十进制整数：");
    scanf("%u", &N);
    conversion(N);
    return 0;
}