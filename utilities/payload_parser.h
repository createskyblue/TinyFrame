#ifndef PAYLOAD_PARSER_H
#define PAYLOAD_PARSER_H

/**
 * PayloadParser，TinyFrame 工具集合的一部分
 * 
 * (c) Ondřej Hruška, 2016-2017. MIT 许可证。
 * 
 * 此模块帮助你解析负载（不仅来自 TinyFrame）。
 * 
 * 解析器支持在初始化时选择或通过访问
 * bigendian 结构字段选择的大端和小端序。
 *
 * 解析器执行边界检查，当请求的读取
 * 没有足够数据时调用提供的处理程序。使用处理程序
 * 采取适当的操作，例如报告错误。
 * 
 * 如果未定义处理程序函数，则将 pb->ok 标志设置为 false
 * （使用它来检查成功），并且进一步的读取不会产生任何效果
 * 并且总是返回 0 或空数组。
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "type_coerce.h"

typedef struct PayloadParser_ PayloadParser;

/**
 * 空缓冲区处理程序。 
 * 
 * 'needed' 应该读取更多字节但已达到末尾。
 * 
 * 如果问题已解决则返回 true（例如：新数据已加载到
 * 缓冲区中，并且 'current' 指针移动到开头）。
 *  
 * 如果返回 false，则将结构体上的 'ok' 标志设置为 false
 * 并且所有后续的读取都将失败 / 返回 0。
 */
typedef bool (*pp_empty_handler)(PayloadParser *pp, uint32_t needed);

struct PayloadParser_ {
    const uint8_t *start;   //!< 指向缓冲区开头的指针
    const uint8_t *current; //!< 指向下一个要读取的字节的指针
    const uint8_t *end;     //!< 指向缓冲区末尾的指针（start + length）
    pp_empty_handler empty_handler; //!< 缓冲区不足的回调
    bool bigendian;   //!< 使用大端序解析的标志
    bool ok;          //!< 表示所有读取都成功
};

// --- 初始化辅助宏 ---

/** 启动解析器。 */
#define pp_start_e(buf, length, bigendian, empty_handler) ((PayloadParser){buf, buf, (buf)+(length), empty_handler, bigendian,1})

/** 以大端序模式启动解析器 */
#define pp_start_be(buf, length, empty_handler) pp_start_e(buf, length,1, empty_handler)

/** 以小端序模式启动解析器 */
#define pp_start_le(buf, length, empty_handler) pp_start_e(buf, length,0, empty_handler)

/** 以小端序模式启动解析器（默认） */
#define pp_start(buf, length, empty_handler) pp_start_le(buf, length, empty_handler)

// --- 工具函数 ---

/** 获取剩余长度 */
#define pp_length(pp) ((pp)->end - (pp)->current)

/** 将当前指针重置为开头 */
#define pp_rewind(pp) do { pp->current = pp->start; } while (0)


/**
 * @brief 获取缓冲区的剩余部分。
 *
 * 如果没有剩余字节，则返回 NULL 并将 'length' 设置为 0。
 *
 * @param pp
 * @param length ：此处将存储缓冲区长度。NULL 表示不存储。
 * @return 输入缓冲区的剩余部分
 */
const uint8_t *pp_tail(PayloadParser *pp, uint32_t *length);

/** 从负载中读取 uint8_t。 */
uint8_t pp_u8(PayloadParser *pp);

/** 从负载中读取布尔值。 */
static inline int8_t pp_bool(PayloadParser *pp)
{
    return pp_u8(pp) != 0;
}

/** 跳过字节 */
void pp_skip(PayloadParser *pp, uint32_t num);

/** 从负载中读取 uint16_t。 */
uint16_t pp_u16(PayloadParser *pp);

/** 从负载中读取 uint32_t。 */
uint32_t pp_u32(PayloadParser *pp);

/** 从负载中读取 int8_t。 */
int8_t pp_i8(PayloadParser *pp);

/** 从负载中读取 char（int8_t）。 */
static inline int8_t pp_char(PayloadParser *pp)
{
    return pp_i8(pp);
}

/** 从负载中读取 int16_t。 */
int16_t pp_i16(PayloadParser *pp);

/** 从负载中读取 int32_t。 */
int32_t pp_i32(PayloadParser *pp);

/** 从负载中读取 4 字节浮点数。 */
float pp_float(PayloadParser *pp);

/**
 * 解析零终止字符串
 *
 * @param pp - 解析器
 * @param buffer - 目标缓冲区
 * @param maxlen - 缓冲区大小
 * @return 实际字节数，不包括终止符
 */
uint32_t pp_string(PayloadParser *pp, char *buffer, uint32_t maxlen);

/**
 * 解析缓冲区
 *
 * @param pp - 解析器
 * @param buffer - 目标缓冲区
 * @param maxlen - 缓冲区大小
 * @return 实际字节数，不包括终止符
 */
uint32_t pp_buf(PayloadParser *pp, uint8_t *buffer, uint32_t maxlen);


#endif // PAYLOAD_PARSER_H
