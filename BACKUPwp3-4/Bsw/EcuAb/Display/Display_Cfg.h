/**
 * @file    Display_Cfg.h
 * @brief   7-segment display configuration
 *
 * Defines digit positions and hardware limits.
 * The board has 6 digits; the segment table covers 16 values (0-F).
 */

#ifndef DISPLAY_CFG_H
#define DISPLAY_CFG_H

/** Digit positions (0-indexed, board-specific layout) */
#define DISPLAY_DIGIT_HUNDREDS  0u  /**< Hundreds place  */
#define DISPLAY_DIGIT_TENS      1u  /**< Tens place      */
#define DISPLAY_DIGIT_ONES      2u  /**< Ones place      */

/** Hardware limits */
#define DISPLAY_DIGIT_COUNT  6u     /**< Total digits on board  */
#define DISPLAY_VALUE_COUNT  16u    /**< Segment table entries  */

#endif
