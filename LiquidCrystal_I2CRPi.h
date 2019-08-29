//Arduino LCD library ported to linux on raspberry pi.
//All unsigned are replaced with unsigned.

//YWROBOT
#ifndef LiquidCrystal_I2C_RPI_h
#define LiquidCrystal_I2C_RPI_h



#include <inttypes.h>
/* original code
 #include "Print.h" //Do not use for now.
#include <Wire.h> //Replace with pigpiod library.
*/
//modidied
#include <pigpiod_if2.h>
//modified end
// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

/*original
#define En B00000100  // Enable bit
#define Rw B00000010  // Read/Write bit
#define Rs B00000001  // Register select bit
 */
//modified
#define En 0b00000100  // Enable bit
#define Rw 0b00000010  // Read/Write bit
#define Rs 0b00000001  // Register select bit
//modified end

//original//class LiquidCrystal_I2C : public Print {
class LiquidCrystal_I2CRPi{
public:
        LiquidCrystal_I2CRPi(int pi,unsigned i2c_bus,unsigned lcd_Addr,unsigned i2c_flags,unsigned lcd_cols,unsigned lcd_rows);
//    LiquidCrystal_I2CRPi(int pi,unsigned i2c_bus,unsigned lcd_Addr,unsigned i2c_flags,unsigned lcd_cols,unsigned lcd_rows)
//    {
//      _Addr = lcd_Addr;
//      _cols = lcd_cols;
//      _rows = lcd_rows;
//      _backlightval = LCD_NOBACKLIGHT;
//        //modidied
//        _pi = pi;
//        _i2c_bus = i2c_bus;
//        _i2c_flags = i2c_flags;
//        //modified end
//    }
  void begin(unsigned cols, unsigned rows, unsigned charsize = LCD_5x8DOTS );
  void clear();
  void home();
  void noDisplay();
  void display();
  void noBlink();
  void blink();
  void noCursor();
  void cursor();
  void scrollDisplayLeft();
  void scrollDisplayRight();
  void printLeft();
  void printRight();
  void leftToRight();
  void rightToLeft();
  void shiftIncrement();
  void shiftDecrement();
  void noBacklight();
  void backlight();
  void autoscroll();
  void noAutoscroll(); 
  //void createChar(unsigned, unsigned[]);
  void setCursor(unsigned, unsigned);
//#if defined(ARDUINO) && ARDUINO >= 100
//  virtual size_t write(unsigned);
//#else
  virtual void write(unsigned);
//#endif
  void command(unsigned);
  void init();

////compatibility API function aliases
void blink_on();						// alias for blink()
void blink_off();       					// alias for noBlink()
void cursor_on();      	 					// alias for cursor()
void cursor_off();      					// alias for noCursor()
void setBacklight(unsigned new_val);				// alias for backlight() and nobacklight()
//void load_custom_character(unsigned char_num, unsigned *rows);	// alias for createChar()
//original//void printstr(const char[]);

////Unsupported API functions (not implemented in this library)
unsigned status();
void setContrast(unsigned new_val);
unsigned keypad();
void setDelay(int,int);
void on();
void off();
unsigned init_bargraph(unsigned graphtype);
void draw_horizontal_graph(unsigned row, unsigned column, unsigned len,  unsigned pixel_col_end);
void draw_vertical_graph(unsigned row, unsigned column, unsigned len,  unsigned pixel_col_end);
	 

private:
    //Modified
    int _pi;
    unsigned _i2c_bus;
    unsigned _i2c_flags;
    //Modified end
  void init_priv();
  void send(unsigned, unsigned);
  void write4bits(unsigned);
  void expanderWrite(unsigned);
  void pulseEnable(unsigned);
  unsigned _Addr;
  unsigned _displayfunction;
  unsigned _displaycontrol;
  unsigned _displaymode;
  unsigned _numlines;
  unsigned _cols;
  unsigned _rows;
  unsigned _backlightval;
};

#endif //LiquidCrystal_I2C_RPI_h
