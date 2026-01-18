#ifndef TYPE_COERCE_H
#define TYPE_COERCE_H

/**
 * 用于类型转换的结构体，
 * TinyFrame 工具集合的一部分
 * 
 * (c) Ondřej Hruška, 2016-2017. MIT 许可证。
 * 
 * 这是 PayloadParser 和 PayloadBuilder 的支持头文件。
 */

#include <stdint.h>
#include <stddef.h>

union conv8 {
    uint8_t u8;
    int8_t i8;
};

union conv16 {
    uint16_t u16;
    int16_t i16;
};

union conv32 {
    uint32_t u32;
    int32_t i32;
    float f32;
};

#endif // TYPE_COERCE_H
