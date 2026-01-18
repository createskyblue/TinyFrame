//
// 由 MightyPork 于 2017/10/15 创建。
//

#ifndef TF_DEMO_H
#define TF_DEMO_H

#include <stdbool.h>
#include "../TinyFrame.h"
#include "utils.h"

#define PORT 9798

extern TinyFrame *demo_tf;

/** 休眠并等待 ^C */
void demo_sleep(void);

/** 初始化服务端 - 不初始化 TinyFrame！ */
void demo_init(TF_Peer peer);

/** 从服务端断开客户端 - 可以由服务端回调调用 */
void demo_disconn(void);

#endif //TF_DEMO_H
