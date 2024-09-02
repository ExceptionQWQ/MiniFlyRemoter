/**
 * @file drv_oled.h
 *
 * @brief 0.96寸OLED SPI驱动程序
 * @author BusyBox (busybox177634@gmail.com)
 * @date 2024/9/2
 *
 */

#include "math.h"

#include "oled_font.h"

#include "drv_oled.h"


#define OLED_DC_CMD  0
#define OLED_DC_DATA 1


static oled_spi_tx_callback_t   oled_spi_tx_callback_   = 0;
static oled_gpio_rst_callback_t oled_gpio_rst_callback_ = 0;
static oled_gpio_dc_callback_t  oled_gpio_dc_callback_  = 0;
static oled_delay_callback_t    oled_delay_callback_    = 0;

volatile static uint8_t oled_gram_[128][8];


static void oled_write_byte(uint8_t data, uint8_t cmd)
{
    oled_gpio_dc_callback_(cmd);
    oled_spi_tx_callback_(&data, 1);
    oled_gpio_dc_callback_(OLED_DC_DATA);
}

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
                  oled_delay_callback_t    delay_callback)
{
    oled_spi_tx_callback_   = tx_callback;
    oled_gpio_rst_callback_ = rst_callback;
    oled_gpio_dc_callback_  = dc_callback;
    oled_delay_callback_    = delay_callback;

    if (!tx_callback)    return -1;
    if (!rst_callback)   return -1;
    if (!dc_callback)    return -1;
    if (!delay_callback) return -1;


    oled_gpio_dc_callback_(OLED_DC_DATA);

    oled_gpio_rst_callback_(0);
    oled_delay_callback_(100);
    oled_gpio_rst_callback_(1);


    oled_write_byte(0xAE, OLED_DC_CMD);
    oled_write_byte(0xD5, OLED_DC_CMD);
    oled_write_byte(0x80, OLED_DC_CMD);
    oled_write_byte(0xA8, OLED_DC_CMD);
    oled_write_byte(0X3F, OLED_DC_CMD);
    oled_write_byte(0xD3, OLED_DC_CMD);
    oled_write_byte(0X00, OLED_DC_CMD);

    oled_write_byte(0x40, OLED_DC_CMD);

    oled_write_byte(0x8D, OLED_DC_CMD);
    oled_write_byte(0x14, OLED_DC_CMD);
    oled_write_byte(0x20, OLED_DC_CMD);
    oled_write_byte(0x02, OLED_DC_CMD);
    oled_write_byte(0xA1, OLED_DC_CMD);
    oled_write_byte(0xC0, OLED_DC_CMD);
    oled_write_byte(0xDA, OLED_DC_CMD);
    oled_write_byte(0x12, OLED_DC_CMD);

    oled_write_byte(0x81, OLED_DC_CMD);
    oled_write_byte(0xEF, OLED_DC_CMD);
    oled_write_byte(0xD9, OLED_DC_CMD);
    oled_write_byte(0xf1, OLED_DC_CMD);
    oled_write_byte(0xDB, OLED_DC_CMD);
    oled_write_byte(0x30, OLED_DC_CMD);

    oled_write_byte(0xA4, OLED_DC_CMD);
    oled_write_byte(0xA6, OLED_DC_CMD);
    oled_write_byte(0xAF, OLED_DC_CMD);
    oled_clear();

}

/**!
 * @brief 将内存缓冲区内的图像写入oled设备
 */
void oled_refresh_gram()
{
    for (uint8_t i = 0; i < 8; i++) {
        oled_write_byte(0xb0 + i, OLED_DC_CMD);
        oled_write_byte(0x00, OLED_DC_CMD);
        oled_write_byte(0x10, OLED_DC_CMD);

        for (uint8_t n = 0; n < 128; n++) {
            oled_write_byte(oled_gram_[n][i], OLED_DC_DATA);
        }
    }
}

/**!
 * @brief 开启OLED显示
 */
void oled_display_on()
{
    oled_write_byte(0X8D, OLED_DC_CMD);
    oled_write_byte(0X14, OLED_DC_CMD);
    oled_write_byte(0XAF, OLED_DC_CMD);
}

/**!
 * @brief 关闭OLED显示
 */
void oled_display_off()
{
    oled_write_byte(0X8D, OLED_DC_CMD);
    oled_write_byte(0X10, OLED_DC_CMD);
    oled_write_byte(0XAE, OLED_DC_CMD);
}

/**!
 * @brief OLED清屏
 */
void oled_clear()
{
    for (uint8_t i = 0; i < 8; i++)
        for (uint8_t n = 0; n < 128; n++)
            oled_gram_[n][i] = 0;

    oled_refresh_gram();
}

/**!
 * @brief 在内存缓冲区内画点
 *
 * @param[in] x
 * @param[in] y
 * @param[in] value 1 显示 0 不显示
 */
void oled_draw_point(uint8_t x, uint8_t y, uint8_t value)
{
    if (x > 127 || y > 63) return ;

    uint8_t pos  = 7 - y / 8;
    uint8_t bx   = y % 8;
    uint8_t temp = 1 << (7 - bx);

    if (value) oled_gram_[x][pos] |=  temp;
    else       oled_gram_[x][pos] &= ~temp;
}

/**!
 * @brief 以矩形方式填充内存缓冲区
 *
 * @param[in] x1 左上角x坐标
 * @param[in] y1 左上角y坐标
 * @param[in] x2 右下角x坐标
 * @param[in] y2 右下角y坐标
 * @param value 1 显示 0 不显示
 */
void oled_fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t value)
{
    for (uint8_t x = x1; x <= x2; x++)
        for (uint8_t y = y1; y <= y2; y++)
            oled_draw_point(x, y, value);
}


/**!
 * @brief 在内存缓冲区内写字符
 *
 * @param[in] x
 * @param[in] y
 * @param[in] ch
 * @param[in] font_size 一共支持2种 OLED_FONT_SIZE_0608, OLED_FONT_SIZE_0612
 */
void oled_draw_char(uint8_t x, uint8_t y, uint8_t ch, enum OLED_FONT_SIZE font_size)
{
    uint8_t height, width;
    switch (font_size) {
        case OLED_FONT_SIZE_0608:
            width  = 6;
            height = 8;
            break;
        case OLED_FONT_SIZE_0612:
            width  = 6;
            height = 12;
            break;
        default:
            return;
    }

    uint8_t y0 = y;
    uint8_t csize = (height / 8 + ((height% 8 ) ? 1 : 0)) * width;
    ch = ch- ' ';

    for (uint8_t t = 0; t < csize; t++) {
        uint8_t temp;
        switch (font_size) {
            case OLED_FONT_SIZE_0608:
                temp = asc2_0608[ch][t];
                break;
            case OLED_FONT_SIZE_0612:
                temp = asc2_0612[ch][t];
                break;
        }

        for (uint8_t t1 = 0; t1 < 8; t1++) {
            if (temp & 0x80) oled_draw_point(x, y, 1);
            else oled_draw_point(x, y, 0);
            temp <<= 1;
            y++;
            if ((y - y0) == height) {
                y = y0;
                x++;
                break;
            }
        }
    }
}

/**!
 * @brief 在内存缓冲区中写数字字符串
 *
 * @param[in] x
 * @param[in] y
 * @param[in] number
 * @param[in] len 字符串长度，数字位数不足len的前面补0
 * @param[in] font_size 一共支持2种 OLED_FONT_SIZE_0608, OLED_FONT_SIZE_0612
 */
void oled_draw_number(uint8_t x, uint8_t y, uint32_t number, uint8_t len, enum OLED_FONT_SIZE font_size)
{
    uint8_t width;
    switch (font_size) {
        case OLED_FONT_SIZE_0608:
            width  = 6;
            break;
        case OLED_FONT_SIZE_0612:
            width  = 6;
            break;
        default:
            return;
    }

    uint8_t enshow = 0;
    for (uint8_t t = 0; t < len; t++) {
        uint8_t temp = (number / (int)pow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1)) {
            if(temp == 0) {
                oled_draw_char(x + width * t, y, '0', font_size);
                continue;
            } else {
                enshow = 1;
            }
        }
        oled_draw_char(x + width * t, y, temp + '0', font_size);
    }
}

/**!
 * @brief 在内存缓冲区中写字符串
 *
 * @param[in] x
 * @param[in] y
 * @param[in] str
 * @param[in] font_size 一共支持2种 OLED_FONT_SIZE_0608, OLED_FONT_SIZE_0612
 */
void oled_draw_string(uint8_t x, uint8_t y, const uint8_t *str, enum OLED_FONT_SIZE font_size)
{
    uint8_t width, height;
    switch (font_size) {
        case OLED_FONT_SIZE_0608:
            width  =  6;
            height =  8;
            break;
        case OLED_FONT_SIZE_0612:
            width  =  6;
            height = 12;
            break;
        default:
            return;
    }

    while ((*str <= '~') && (*str >= ' ')) {
        if (x > (128 - width)) {
            x = 0;
            y += height;
        }
        if (y > (64 - height)) {
            y = x = 0;
            oled_clear();
        }
        oled_draw_char(x, y, *str, font_size);
        x += width;
        str++;
    }
}

