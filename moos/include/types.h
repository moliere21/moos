#ifndef __TYPES_H_
#define __TYPES_H_


/* 基本类型值   */

#define NULL (void *)0

/*   int 基本类型值   */
typedef char int8_t;
typedef short int16_t;
typedef int int32_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

/*   bool value   */
enum bool_value{
    false   = 0,
    true    = 1
};

typedef enum bool_value bool;


#endif