/*
 * lcd.h
 *
 * Created: 31/10/2019
 *  Author: Oskar
 */ 

#ifndef LCD_H_
#define LCD_H_

void porta_lcd(char data_display);
void lcd_comando(char data_display);
void lcdWritechar(char data_display);
void clear_display();
void lcd_config();
void setCursor(unsigned int coluna, unsigned int linha);
void lcdWrite(char string1[16]);

#endif /* LCD_H_ */