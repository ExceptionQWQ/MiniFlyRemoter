/**
 * @file drv_oled.h
 *
 * @brief 0.96寸OLED SPI驱动程序
 * @author BusyBox (busybox177634@gmail.com)
 * @date 2024/9/2
 *
 */

#pragma once

#include "stdint.h"


enum OLED_FONT_SIZE
{
    OLED_FONT_SIZE_0608 = 1,
    OLED_FONT_SIZE_0612 = 2,
};

typedef void(*oled_spi_tx_callback_t)(uint8_t *data, uint32_t len);
typedef void(*oled_gpio_rst_callback_t)(uint8_t level);
typedef void(*oled_gpio_dc_callback_t)(uint8_t level);
typedef void(*oled_delay_callback_t)(uint32_t ms);

/**!
 * @brief oled驱动初始化
 *
 * @param[in] tx_callback    spi 数据发送回调
 * @param[in] rst_callback   rst 引脚控制回调
 * @param[in] dc_callback    dc  引脚控制回调
 * @param[in] delay_callback 延时回调
 * @return 0 初始化成功 -1 回调函数空指针错误
 */
int drv_oled_init(oled_spi_tx_callback_t   tx_callback,
                  oled_gpio_rst_callback_t rst_callback,
                  oled_gpio_dc_callback_t  dc_callback,
                  oled_delay_callback_t    delay_callback);

/**!
 * @brief 将内存缓冲区内的图像写入oled设备
 */
void oled_refresh_gram();

/**!
 * @brief 开启OLED显示
 */
void oled_display_on();

/**!
 * @brief 关闭OLED显示
 */
void oled_display_off();

/**!
 * @brief OLED清屏
 */
void oled_clear();

/**!
 * @brief 在内存缓冲区内画点
 *
 * @param[in] x
 * @param[in] y
 * @param[in] value 1 显示 0 不显示
 */
void oled_draw_point(uint8_t x, uint8_t y, uint8_t value);

/**!
 * @brief 以矩形方式填充内存缓冲区
 *
 * @param[in] x1 左上角x坐标
 * @param[in] y1 左上角y坐标
 * @param[in] x2 右下角x坐标
 * @param[in] y2 右下角y坐标
 * @param value 1 显示 0 不显示
 */
void oled_fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t value);

/**!
 * @brief 在内存缓冲区内写字符
 *
 * @param[in] x
 * @param[in] y
 * @param[in] ch
 * @param[in] font_size 一共支持3种 OLED_FONT_SIZE_0608, OLED_FONT_SIZE_0612, OLED_FONT_SIZE_1224
 */
void oled_draw_char(uint8_t x, uint8_t y, uint8_t ch, enum OLED_FONT_SIZE font_size);

/**!
 * @brief 在内存缓冲区中写数字字符串
 *
 * @param[in] x
 * @param[in] y
 * @param[in] number
 * @param[in] len 字符串长度，数字位数不足len的前面补0
 * @param[in] font_size 一共支持2种 OLED_FONT_SIZE_0608, OLED_FONT_SIZE_0612
 */
void oled_draw_number(uint8_t x, uint8_t y, uint32_t number, uint8_t len, enum OLED_FONT_SIZE font_size);

/**!
 * @brief 在内存缓冲区中写字符串
 *
 * @param[in] x
 * @param[in] y
 * @param[in] str
 * @param[in] font_size 一共支持2种 OLED_FONT_SIZE_0608, OLED_FONT_SIZE_0612
 */
void oled_draw_string(uint8_t x, uint8_t y, const uint8_t *str, enum OLED_FONT_SIZE font_size);
