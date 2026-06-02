/* 数码管配置：定义要显示的三位位置和硬件上限。 */

#ifndef DISPLAY_CFG_H
#define DISPLAY_CFG_H

/* 三位十进制显示使用的位号。 */
#define DISPLAY_DIGIT_HUNDREDS  0u  /* 百位 */
#define DISPLAY_DIGIT_TENS      1u  /* 十位 */
#define DISPLAY_DIGIT_ONES      2u  /* 个位 */

/* 硬件范围：板上6位，段码表支持0~F共16个值。 */
#define DISPLAY_DIGIT_COUNT  6u
#define DISPLAY_VALUE_COUNT  16u

#endif
