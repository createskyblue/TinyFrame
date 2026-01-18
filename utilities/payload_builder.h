#ifndef PAYLOAD_BUILDER_H
#define PAYLOAD_BUILDER_H

/**
 * PayloadBuilder，TinyFrame 工具集合的一部分
 * 
 * (c) Ondřej Hruška, 2014-2017. MIT 许可证。
 * 
 * 构建器支持大端和小端序，在初始化时选择
 * 或通过访问 bigendian 结构字段。
 * 
 * 该模块帮助你构建负载（不仅用于 TinyFrame）
 *
 * 构建器执行边界检查，当请求的写入无法容纳时调用
 * 提供的处理程序。使用处理程序重新分配/刷新缓冲区
 * 或报告错误。
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "type_coerce.h"

typedef struct PayloadBuilder_ PayloadBuilder;

/**
 * 满缓冲区处理程序。 
 * 
 * 'needed' 更多字节应该写入但已达到缓冲区末尾。
 * 
 * 如果问题已解决（例如：缓冲区已刷新且
 * 'current' 指针移动到开头），则返回 true。
 * 
 * 如果返回 false，则结构体上的 'ok' 标志设置为 false
 * 并且所有后续的写入将被丢弃。
 */
typedef bool (*pb_full_handler)(PayloadBuilder *pb, uint32_t needed);

struct PayloadBuilder_ {
    uint8_t *start;   //!< 指向缓冲区开头的指针
    uint8_t *current; //!< 指向下一个要读取的字节的指针
    uint8_t *end;     //!< 指向缓冲区末尾的指针（start + length）
    pb_full_handler full_handler; //!< 缓冲区溢出的回调
    bool bigendian;   //!< 使用大端序解析的标志
    bool ok;          //!< 表示所有读取都成功
};

// --- 初始化辅助宏 ---

/** 启动构建器。 */
#define pb_start_e(buf, capacity, bigendian, full_handler) ((PayloadBuilder){buf, buf, (buf)+(capacity), full_handler, bigendian,1})

/** 以大端序模式启动构建器 */
#define pb_start_be(buf, capacity, full_handler) pb_start_e(buf, capacity,1, full_handler)

/** 以小端序模式启动构建器 */
#define pb_start_le(buf, capacity, full_handler) pb_start_e(buf, capacity,0, full_handler)

/** 以小端序模式启动解析器（默认） */
#define pb_start(buf, capacity, full_handler) pb_start_le(buf, capacity, full_handler)

// --- 工具函数 ---

/** 获取已使用的字节数 */
#define pb_length(pb) ((pb)->current - (pb)->start)

/** 将当前指针重置为开头 */
#define pb_rewind(pb) do { pb->current = pb->start; } while (0)


/** 从缓冲区写入 */
bool pb_buf(PayloadBuilder *pb, const uint8_t *buf, uint32_t len);

/** 写入零终止字符串 */
bool pb_string(PayloadBuilder *pb, const char *str);

/** 将 uint8_t 写入缓冲区 */
bool pb_u8(PayloadBuilder *pb, uint8_t byte);

/** 将布尔值写入缓冲区。 */
static inline bool pb_bool(PayloadBuilder *pb, bool b)
{
    return pb_u8(pb, (uint8_t) b);
}

/** 将 uint16_t 写入缓冲区。 */
bool pb_u16(PayloadBuilder *pb, uint16_t word);

/** 将 uint32_t 写入缓冲区。 */
bool pb_u32(PayloadBuilder *pb, uint32_t word);

/** 将 int8_t 写入缓冲区。 */
bool pb_i8(PayloadBuilder *pb, int8_t byte);

/** 将 char（int8_t）写入缓冲区。 */
static inline bool pb_char(PayloadBuilder *pb, char c)
{
    return pb_i8(pb, c);
}

/** 将 int16_t 写入缓冲区。 */
bool pb_i16(PayloadBuilder *pb, int16_t word);

/** 将 int32_t 写入缓冲区。 */
bool pb_i32(PayloadBuilder *pb, int32_t word);

/** 将 4 字节浮点数写入缓冲区。 */
bool pb_float(PayloadBuilder *pb, float f);

#endif // PAYLOAD_BUILDER_H
