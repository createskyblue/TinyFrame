#include <stdio.h>
#include <string.h>
#include "../../TinyFrame.h"
#include "../utils.h"

TinyFrame *demo_tf;

bool do_corrupt = false;

/**
 * 此函数应在应用程序代码中定义。
 * 它实现最底层 - 将字节发送到 UART（或其他）
 */
void TF_WriteImpl(TinyFrame *tf, const uint8_t *buff, uint32_t len)
{
    printf("--------------------\n");
    printf("\033[32mTF_WriteImpl - 发送帧:\033[0m\n");
    
    uint8_t *xbuff = (uint8_t *)buff;    
    if (do_corrupt) {
      printf("(损坏以测试校验和检查...)\n");
      xbuff[8]++;
    }
    
    dumpFrame(xbuff, len);

    // 将其发回，就像我们接收到它一样
    TF_Accept(tf, xbuff, len);
}

/** 示例监听器函数 */
TF_Result myListener(TinyFrame *tf, TF_Msg *msg)
{
    dumpFrameInfo(msg);
    return TF_STAY;
}

TF_Result testIdListener(TinyFrame *tf, TF_Msg *msg)
{
    printf("OK - 消息的 ID 监听器被触发！\n");
    dumpFrameInfo(msg);
    return TF_CLOSE;
}

int main(void)
{
    TF_Msg msg;
    const char *longstr = "Lorem ipsum dolor sit amet.";

    // 设置 TinyFrame 库
    demo_tf = TF_Init(TF_MASTER); // 1 = 主站, 0 = 从站
    TF_AddGenericListener(demo_tf, myListener);

    printf("------ 模拟发送消息 --------\n");

    TF_ClearMsg(&msg);
    msg.type = 0x22;
    msg.data = (pu8) "Hello TinyFrame";
    msg.len = 16;
    TF_Send(demo_tf, &msg);

    msg.type = 0x33;
    msg.data = (pu8) longstr;
    msg.len = (TF_LEN) (strlen(longstr) + 1); // 添加空字节
    TF_Send(demo_tf, &msg);

    msg.type = 0x44;
    msg.data = (pu8) "Hello2";
    msg.len = 7;
    TF_Send(demo_tf, &msg);

    msg.len = 0;
    msg.type = 0x77;
    TF_Query(demo_tf, &msg, testIdListener, NULL, 0);
    
    printf("这应该失败：\n");
    
    // 测试校验和被测试
    do_corrupt = true;    
    msg.type = 0x44;
    msg.data = (pu8) "Hello2";
    msg.len = 7;
    TF_Send(demo_tf, &msg);
    return 0;
}
