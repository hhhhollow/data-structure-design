/* 数据结构课程设计（二）整合主程序 */
#include <stdio.h>

#define NO_MAIN
#include "2.1.c"
#include "2.2.c"
#include "2.3.c"
#include "2.4.c"

int main(void) {
    char buf[64];
    int choice;

    while (1) {
        printf("\n===== 数据结构课程设计（二） =====\n");
        printf("1. 批量增删功能\n");
        printf("2. 文本编辑器\n");
        printf("3. 术语词典\n");
        printf("4. 图的使用\n");
        printf("0. 退出\n");
        printf("请选择：");
        fflush(stdout);

        if (!fgets(buf, sizeof(buf), stdin)) {
            break;
        }
        if (sscanf(buf, "%d", &choice) != 1) {
            printf("输入无效，请输入 0-4。\n");
            continue;
        }
        if (choice == 0) {
            break;
        }

        switch (choice) {
            case 1: run_2_1(); break;
            case 2: run_2_2(); break;
            case 3: run_2_3(); break;
            case 4: run_2_4(); break;
            default: printf("无效选择，请输入 0-4。\n"); break;
        }
    }
    return 0;
}
