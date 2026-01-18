//
// 由 MightyPork 于 2017/10/15 创建。
//

#include "utils.h"
#include <stdio.h>

// 用于测试的辅助函数
void dumpFrame(const uint8_t *buff, size_t len)
{
    size_t i;
    for (i = 0; i < len; i++) {
        printf("%04d | %3u \033[94m%02X\033[0m", i, buff[i], buff[i]);
        if (buff[i] >= 0x20 && buff[i] < 127) {
            printf(" %c", buff[i]);
        }
        else {
            printf(" \033[31m.\033[0m");
        }
        printf("\n");
    }
    printf("--- 帧结束 ---\n\n");
}

void dumpFrameInfo(TF_Msg *msg)
{
    printf("\033[33m帧信息\n"
               "  类型: %02Xh\n"
               "  数据: \"%.*s\"\n"
               "   长度: %u\n"
               "    ID: %Xh\033[0m\n\n",
           msg->type, msg->len, msg->data, msg->len, msg->frame_id);
}
