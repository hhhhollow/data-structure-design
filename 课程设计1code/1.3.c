#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHUNKSIZE 5                        // 块大小：每块存5个字符
typedef struct Chunk {
    char ch[CHUNKSIZE];
    struct Chunk *next;
} Chunk;
typedef struct {
    Chunk *head, *tail;                    // 串的头和尾指针
    int curlen;                            // 串的当前长度
} LString;

void StrAssign(LString *S, char *chars) {  // 生成值为chars的串S，末块不满处填'#'
    Chunk *q;
    S->head = S->tail = NULL;
    S->curlen = strlen(chars);
    for (int i = 0; i < S->curlen; ++i) {
        if (i % CHUNKSIZE == 0) {          // 上一块已满，申请新块接到表尾
            q = (Chunk*)malloc(sizeof(Chunk));
            memset(q->ch, '#', CHUNKSIZE);
            q->next = NULL;
            if (S->tail) S->tail->next = q; else S->head = q;
            S->tail = q;
        }
        S->tail->ch[i % CHUNKSIZE] = chars[i];
    }
}

void ToChars(LString S, char *chars) {     // 把串S的字符依次取到chars中
    Chunk *p = S.head;
    for (int i = 0; i < S.curlen; ++i) {
        chars[i] = p->ch[i % CHUNKSIZE];
        if (i % CHUNKSIZE == CHUNKSIZE - 1) p = p->next;  // 本块取完转下一块
    }
    chars[S.curlen] = '\0';
}

void DestroyString(LString *S) {           // 释放串S的所有块
    for (Chunk *p = S->head, *q; p; p = q) {
        q = p->next;
        free(p);
    }
    S->head = S->tail = NULL;
    S->curlen = 0;
}

int StrCompare(LString S, LString T) {     // 若S>T返回值>0，S=T返回0，S<T返回值<0
    Chunk *p = S.head, *q = T.head;
    for (int i = 0; i < S.curlen && i < T.curlen; ++i) {
        if (p->ch[i % CHUNKSIZE] != q->ch[i % CHUNKSIZE])
            return p->ch[i % CHUNKSIZE] - q->ch[i % CHUNKSIZE];
        if (i % CHUNKSIZE == CHUNKSIZE - 1) { p = p->next; q = q->next; }
    }
    return S.curlen - T.curlen;            // 公共部分全相同则比长度
}

int StrInsert(LString *S, int pos, LString T) { // 在串S的第pos个字符之前插入串T
    if (pos < 1 || pos > S->curlen + 1) return 0;   // pos不合法
    char *s = (char*)malloc(S->curlen + 1), *t = (char*)malloc(T.curlen + 1);
    char *r = (char*)malloc(S->curlen + T.curlen + 1);
    ToChars(*S, s);
    ToChars(T, t);
    sprintf(r, "%.*s%s%s", pos - 1, s, t, s + pos - 1); // S前pos-1个 + T + S剩余部分
    DestroyString(S);
    StrAssign(S, r);                       // 按每块5个字符重新分块
    free(s); free(t); free(r);
    return 1;
}

void StrPrint(LString S) {                 // 按块输出串S
    for (Chunk *p = S.head; p; p = p->next)
        printf("[%.*s]", CHUNKSIZE, p->ch);
    printf("  curlen=%d\n", S.curlen);
}

int main() {
    LString S, T;
    int pos = 6;                           // 在第6个字符(空格)前插入
    StrAssign(&S, "aeiou abc");
    StrAssign(&T, "aeiou abd");
    printf("S: "); StrPrint(S);
    printf("T: "); StrPrint(T);
    printf("StrCompare(S,T) = %d\n", StrCompare(S, T));
    DestroyString(&T);
    StrAssign(&T, "xyz");
    printf("T: "); StrPrint(T);
    StrInsert(&S, pos, T);
    printf("StrInsert(&S,%d,T) 后 S: ", pos); StrPrint(S);
    DestroyString(&S); DestroyString(&T);
    return 0;
}
