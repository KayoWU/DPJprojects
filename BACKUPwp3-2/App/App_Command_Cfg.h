/**
 * @file    App_Command_Cfg.h
 * @brief   Command protocol configuration — codes and output levels
 *
 * Changing these constants changes command behaviour without touching
 * the command-processing logic in App_Command.c.
 */

#ifndef APP_COMMAND_CFG_H
#define APP_COMMAND_CFG_H

/* ---- 命令码（串口接收到的字节值） ---- */
#define APP_COMMAND_PORT_OFF        1u      /**< 命令1：关端口输出   */
#define APP_COMMAND_CHANNEL_OFF     2u      /**< 命令2：关通道输出   */
#define APP_COMMAND_OUTPUT_RECOVER  3u      /**< 命令3：恢复端口+通道 */
#define APP_COMMAND_SERIAL_ECHO     4u      /**< 命令4：回发+清缓存   */

/* ---- 各命令对应的输出电平 ---- */
#define APP_COMMAND_PORT_OFF_LEVEL        0x00u   /**< 端口全低(全关)     */
#define APP_COMMAND_PORT_RECOVER_LEVEL    0xffu   /**< 端口全高(恢复)     */
#define APP_COMMAND_CHANNEL_OFF_LEVEL     0u      /**< 通道低电平         */
#define APP_COMMAND_CHANNEL_RECOVER_LEVEL 1u      /**< 通道高电平(恢复)   */
#define APP_COMMAND_DISPLAY_CLEAR_VALUE   0u      /**< 数码管显示"000"    */

#endif
