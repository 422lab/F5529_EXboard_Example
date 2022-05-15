/*
 * SSD1306_driver.h
 *
 *  Created on: 2021Äê8ÔÂ18ÈÕ
 *      Author: Zuoge
 */

#ifndef SRC_SSD1306_SSD1306_DRIVER_H_
#define SRC_SSD1306_SSD1306_DRIVER_H_

#define SSD1306_MOSI_DIR_OUT()  P2DIR|=BIT3
#define SSD1306_MOSI_HIGH()     P2OUT|=BIT3
#define SSD1306_MOSI_LOW()      P2OUT&=~BIT3
#define SSD1306_CLK_DIR_OUT()   P2DIR|=BIT2
#define SSD1306_CLK_HIGH()      P2OUT|=BIT2
#define SSD1306_CLK_LOW()       P2OUT&=~BIT2

#define SSD1306_CS_DIR_OUT()        P2DIR|=BIT6
#define SSD1306_CS_HIGH()           P2OUT|=BIT6
#define SSD1306_CS_LOW()            P2OUT&=~BIT6
#define SSD1306_DC_DIR_OUT()        P2DIR|=BIT5
#define SSD1306_DC_HIGH()           P2OUT|=BIT5
#define SSD1306_DC_LOW()            P2OUT&=~BIT5
#define SSD1306_RST_DIR_OUT()       P2DIR|=BIT4
#define SSD1306_RST_HIGH()          P2OUT|=BIT4
#define SSD1306_RST_LOW()           P2OUT&=~BIT4

void SSD1306_write_cmd(unsigned char cmd);
void SSD1306_write_data(unsigned char dat);
void SSD1306_color_turn(unsigned i);
void SSD1306_display_turn(unsigned i);
void SSD1306_set_pos(unsigned char x, unsigned char y);
void SSD1306_clear();
void SSD1306_init();

#endif /* SRC_SSD1306_SSD1306_DRIVER_H_ */
