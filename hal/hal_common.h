/*
 * @file hal_common.h
 */

#ifndef _HAL_COMMON_H_
#define _HAL_COMMON_H_

#define ON -1
#define OFF 0

#define TRUE -1
#define FALSE 0

#ifndef NULL
#define NULL (void *) 0
#endif

#define BIT10 BITA
#define BIT11 BITB
#define BIT12 BITC
#define BIT13 BITD
#define BIT14 BITE
#define BIT15 BITF

typedef volatile unsigned int *WORD_PTR;

#endif
