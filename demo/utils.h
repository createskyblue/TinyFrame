//
// 由 MightyPork 于 2017/10/15 创建。
//

#ifndef TF_UTILS_H
#define TF_UTILS_H

#include <stdio.h>
#include "../TinyFrame.h"

/** 指向 unsigned char 的指针 */
typedef unsigned char* pu8;

/**
 * 以十六进制、十进制和 ASCII 格式转储二进制帧
 */
void dumpFrame(const uint8_t *buff, size_t len);

/**
 * 转储消息元数据（不是内容）
 *
 * @param msg
 */
void dumpFrameInfo(TF_Msg *msg);

#endif //TF_UTILS_H
