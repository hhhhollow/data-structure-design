#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OK 1
#define ERROR 0
#define OVERFLOW -2

typedef int Status;

// 块大小定义为5：字符串"aeiou abc"长度为9，分为2块存储（5 + 4，最后补'#'）
#define CHUNKSIZE 5
#define BLANK '#' // 填充字符

// 块链结点结构
typedef struct Chunk {
    char ch[CHUNKSIZE];
    struct Chunk *next;
} Chunk;

// 块链串结构
typedef struct {
    Chunk *head; // 头指针
    Chunk *tail; // 尾指针
    int curlen;  // 串的当前长度
} LString;

// 初始化块链串
void InitString(LString *S) {
    S->head = NULL;
    S->tail = NULL;
    S->curlen = 0;
}

// 释放块链串内存
void ClearString(LString *S) {
    Chunk *p = S->head;
    while (p) {
        Chunk *q = p->next;
        free(p);
        p = q;
    }
    S->head = S->tail = NULL;
    S->curlen = 0;
}

// 生成一个其值等于字符串常量chars的块链串S
Status StrAssign(LString *S, const char *chars) {
    ClearString(S);
    int len = (int)strlen(chars);
    if (len == 0) return OK;

    S->curlen = len;
    int numChunks = (len + CHUNKSIZE - 1) / CHUNKSIZE;
    int charIdx = 0;

    for (int i = 0; i < numChunks; i++) {
        Chunk *p = (Chunk *)malloc(sizeof(Chunk));
        if (!p) exit(OVERFLOW);
        p->next = NULL;

        for (int j = 0; j < CHUNKSIZE; j++) {
            if (charIdx < len) {
                p->ch[j] = chars[charIdx++];
            } else {
                p->ch[j] = BLANK; // 未用空间填充'#'
            }
        }

        if (!S->head) {
            S->head = S->tail = p;
        } else {
            S->tail->next = p;
            S->tail = p;
        }
    }
    return OK;
}

// 提取块链串S中的实际字符到目标字符串数组（便于操作）
void ToCString(LString S, char *str) {
    int idx = 0;
    Chunk *p = S.head;
    while (p && idx < S.curlen) {
        for (int j = 0; j < CHUNKSIZE && idx < S.curlen; j++) {
            str[idx++] = p->ch[j];
        }
        p = p->next;
    }
    str[idx] = '\0';
}

// 比较操作：若 S > T 返回正数；S == T 返回 0；S < T 返回负数
int StrCompare(LString S, LString T) {
    int minLen = S.curlen < T.curlen ? S.curlen : T.curlen;
    Chunk *ps = S.head;
    Chunk *pt = T.head;
    int is = 0, it = 0;

    for (int k = 0; k < minLen; k++) {
        char cs = ps->ch[is];
        char ct = pt->ch[it];
        if (cs != ct) {
            return cs - ct;
        }
        is++;
        if (is == CHUNKSIZE) {
            is = 0;
            ps = ps->next;
        }
        it++;
        if (it == CHUNKSIZE) {
            it = 0;
            pt = pt->next;
        }
    }
    return S.curlen - T.curlen;
}

// 串插入操作：在串S的第pos个字符之前插入串T
Status StrInsert(LString *S, int pos, LString T) {
    if (pos < 1 || pos > S->curlen + 1) return ERROR;
    if (T.curlen == 0) return OK;

    // 提取S和T的字符，完成插入并重新构建块链
    int totalLen = S->curlen + T.curlen;
    char *bufS = (char *)malloc(S->curlen + 1);
    char *bufT = (char *)malloc(T.curlen + 1);
    char *newBuf = (char *)malloc(totalLen + 1);
    if (!bufS || !bufT || !newBuf) exit(OVERFLOW);

    ToCString(*S, bufS);
    ToCString(T, bufT);

    // 拼接：前 pos - 1 个字符来自 S，接着 T，最后 S 剩余字符
    strncpy(newBuf, bufS, pos - 1);
    newBuf[pos - 1] = '\0';
    strcat(newBuf, bufT);
    strcat(newBuf, bufS + (pos - 1));

    // 用拼接后的字符串重新为S赋值
    StrAssign(S, newBuf);

    free(bufS);
    free(bufT);
    free(newBuf);
    return OK;
}

// 打印块链串的直观块结构
void PrintStringBlocks(const char *name, LString S) {
    printf("%s (长度: %d): ", name, S.curlen);
    Chunk *p = S.head;
    int chunkIndex = 1;
    while (p) {
        printf("[块%d: ", chunkIndex++);
        for (int j = 0; j < CHUNKSIZE; j++) {
            printf("'%c' ", p->ch[j]);
        }
        printf("]");
        if (p->next) printf(" -> ");
        p = p->next;
    }
    printf(" -> NULL\n");
}

int main(void) {
    printf("【题目3：串以块链结构存储（分2块）及插入、比较操作】\n\n");

    LString S, T1, T2;
    InitString(&S);
    InitString(&T1);
    InitString(&T2);

    // 1. 将串"aeiou abc"以块链结构存储（分2块）
    const char *initialStr = "aeiou abc";
    StrAssign(&S, initialStr);
    printf("1. 初始存储串 \"%s\" 为块链结构：\n", initialStr);
    PrintStringBlocks("串 S", S);

    // 2. 比较操作测试
    StrAssign(&T1, "aeiou 123");
    StrAssign(&T2, "aeiou abc");
    printf("\n2. 比较操作 StrCompare 测试：\n");
    printf("   StrCompare(S, \"aeiou 123\") = %d (S更大多以正数返回)\n", StrCompare(S, T1));
    printf("   StrCompare(S, \"aeiou abc\") = %d (相等返回0)\n", StrCompare(S, T2));

    // 3. 串插入操作测试 StrInsert(&S, pos, T)
    LString insertStr;
    InitString(&insertStr);
    StrAssign(&insertStr, "xyz");
    printf("\n3. 串插入操作 StrInsert(&S, 6, \"xyz\") 测试：\n");
    printf("   在 S 的第 6 个字符前插入 \"xyz\"：\n");
    StrInsert(&S, 6, insertStr);
    PrintStringBlocks("插入后的串 S", S);

    char finalChars[100];
    ToCString(S, finalChars);
    printf("   展开字符值: \"%s\"\n", finalChars);

    return 0;
}
