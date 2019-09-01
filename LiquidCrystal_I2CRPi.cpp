//Arduino LCD library ported to linux on raspberry pi.
//All unsigned are replaced with unsigned.


// Based on the work by DFRobot

#include "LiquidCrystal_I2CRPi.h"
#include <inttypes.h>


//  #if defined(ARDUINO) && ARDUINO >= 100

// #include "Arduino.h"

// #define printIIC(args)    Wire.write(args)
inline size_t LiquidCrystal_I2CRPi::write(unsigned char value) {
    send(value, Rs);
    return 1;
}

// #else
// #include "WProgram.h"

// #define printIIC(args)	Wire.send(args)
// inline void LiquidCrystal_I2CRPi::write(unsigned value) {
// 	send(value, Rs);
// }

// //#endif
// #include "Wire.h"



// inline void LiquidCrystal_I2CRPi::write(unsigned value) {
//     send(value, Rs);
// }

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1
//    S = 0; No shift 
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal constructor is called).

LiquidCrystal_I2CRPi::LiquidCrystal_I2CRPi(int pi,unsigned i2c_bus,unsigned lcd_Addr,unsigned i2c_flags,unsigned lcd_cols,unsigned lcd_rows)
{
  _Addr = lcd_Addr;
  _cols = lcd_cols;
  _rows = lcd_rows;
  _backlightval = LCD_NOBACKLIGHT;
    //modidied
    _pi = pi;
    _i2c_bus = i2c_bus;
    _i2c_flags = i2c_flags;
    //modified end
}
//LiquidCrystal_I2CRPi::LiquidCrystal_I2CRPi(){}


void LiquidCrystal_I2CRPi::init(){
	init_priv();
}

void LiquidCrystal_I2CRPi::init_priv()
{
	//original//Wire.begin();
	_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
	begin(_cols, _rows);  
}

void LiquidCrystal_I2CRPi::begin(unsigned cols, unsigned lines, unsigned dotsize) {
	if (lines > 1) {
		_displayfunction |= LCD_2LINE;
	}
	_numlines = lines;

	// for some 1 line displays you can select a 10 pixel high font
	if ((dotsize != 0) && (lines == 1)) {
		_displayfunction |= LCD_5x10DOTS;
	}

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	//delay(50);
    time_sleep(0.05);
  
	// Now we pull both RS and R/W low to begin commands
	expanderWrite(_backlightval);	// reset expanderand turn backlight off (Bit 8 =1)
	//delay(1000);
    time_sleep(1);
  	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46
	
	  // we start in 8bit mode, try to set 4 bit mode
   write4bits(0x03 << 4);
   //delayMicroseconds(4500); // wait min 4.1ms
    time_sleep(0.0045);
    
   // second try
   write4bits(0x03 << 4);
   //delayMicroseconds(4500); // wait min 4.1ms
   time_sleep(0.0045);
    
   // third go!
   write4bits(0x03 << 4); 
   //delayMicroseconds(150);
   time_sleep(0.00015);
    
   // finally, set to 4-bit interface
   write4bits(0x02 << 4); 


	// set # lines, font size, etc.
	command(LCD_FUNCTIONSET | _displayfunction);  
	
	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	display();
	
	// clear it off
	clear();
	
	// Initialize to default text direction (for roman languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	
	// set the entry mode
	command(LCD_ENTRYMODESET | _displaymode);
	
	home();
  
}

/********** high level commands, for the user! */
void LiquidCrystal_I2CRPi::clear(){
	command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	//delayMicroseconds(2000);  // this command takes a long time!
    time_sleep(0.002);
}

void LiquidCrystal_I2CRPi::home(){
	command(LCD_RETURNHOME);  // set cursor position to zero
	//delayMicroseconds(2000);  // this command takes a long time!
    time_sleep(0.002);
}

void LiquidCrystal_I2CRPi::setCursor(unsigned col, unsigned row){
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if ( row > _numlines ) {
		row = _numlines-1;    // we count rows starting w/0
	}
	command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void LiquidCrystal_I2CRPi::noDisplay() {
	_displaycontrol &= ~LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal_I2CRPi::display() {
	_displaycontrol |= LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void LiquidCrystal_I2CRPi::noCursor() {
	_displaycontrol &= ~LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal_I2CRPi::cursor() {
	_displaycontrol |= LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void LiquidCrystal_I2CRPi::noBlink() {
	_displaycontrol &= ~LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal_I2CRPi::blink() {
	_displaycontrol |= LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void LiquidCrystal_I2CRPi::scrollDisplayLeft(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LiquidCrystal_I2CRPi::scrollDisplayRight(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LiquidCrystal_I2CRPi::leftToRight(void) {
	_displaymode |= LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void LiquidCrystal_I2CRPi::rightToLeft(void) {
	_displaymode &= ~LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void LiquidCrystal_I2CRPi::autoscroll(void) {
	_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void LiquidCrystal_I2CRPi::noAutoscroll(void) {
	_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
//void LiquidCrystal_I2CRPi::createChar(unsigned location, unsigned charmap[]) {
//    location &= 0x7; // we only have 8 locations 0-7
//    command(LCD_SETCGRAMADDR | (location << 3));
//    for (int i=0; i<8; i++) {
//        write(charmap[i]);
//    }
//}

// Turn the (optional) backlight off/on
void LiquidCrystal_I2CRPi::noBacklight(void) {
	_backlightval=LCD_NOBACKLIGHT;
	expanderWrite(0);
}

void LiquidCrystal_I2CRPi::backlight(void) {
	_backlightval=LCD_BACKLIGHT;
	expanderWrite(0);
}



/*********** mid level commands, for sending data/cmds */

inline void LiquidCrystal_I2CRPi::command(unsigned value) {
	send(value, 0);
}


/************ low level data pushing commands **********/

// write either command or data
void LiquidCrystal_I2CRPi::send(unsigned value, unsigned mode) {
	unsigned highnib=value&0xf0;
	unsigned lownib=(value<<4)&0xf0;
       write4bits((highnib)|mode);
	write4bits((lownib)|mode); 
}

void LiquidCrystal_I2CRPi::write4bits(unsigned value) {
	expanderWrite(value);
	pulseEnable(value);
}

void LiquidCrystal_I2CRPi::expanderWrite(unsigned _data){
    /*original
	Wire.beginTransmission(_Addr);
	printIIC((int)(_data) | _backlightval);
	Wire.endTransmission();
     */
    //////Modified code
    unsigned handle = i2c_open(_pi,_i2c_bus,_Addr,_i2c_flags);
    i2c_write_byte(_pi,handle,(int)(_data) | _backlightval);
    i2c_close(_pi,handle);
}

void LiquidCrystal_I2CRPi::pulseEnable(unsigned _data){
	expanderWrite(_data | En);	// En high
	//delayMicroseconds(1);		// enable pulse must be >450ns
    time_sleep(0.000001);
	expanderWrite(_data & ~En);	// En low
	//delayMicroseconds(50);		// commands need > 37us to settle
    time_sleep(0.00005);
} 


// Alias functions

void LiquidCrystal_I2CRPi::cursor_on(){
	cursor();
}

void LiquidCrystal_I2CRPi::cursor_off(){
	noCursor();
}

void LiquidCrystal_I2CRPi::blink_on(){
	blink();
}

void LiquidCrystal_I2CRPi::blink_off(){
	noBlink();
}

//void LiquidCrystal_I2CRPi::load_custom_character(unsigned char_num, unsigned *rows){
//        createChar(char_num, rows);
//}

void LiquidCrystal_I2CRPi::setBacklight(unsigned new_val){
	if(new_val){
		backlight();		// turn backlight on
	}else{
		noBacklight();		// turn backlight off
	}
}

/* original
void LiquidCrystal_I2CRPi::printstr(const char c[]){
	//This function is not identical to the function used for "real" I2C displays
	//it's here so the user sketch doesn't have to be changed 
	print(c);
}
*/

// unsupported API functions
void LiquidCrystal_I2CRPi::off(){}
void LiquidCrystal_I2CRPi::on(){}
void LiquidCrystal_I2CRPi::setDelay (int cmdDelay,int charDelay) {}
unsigned LiquidCrystal_I2CRPi::status(){return 0;}
unsigned LiquidCrystal_I2CRPi::keypad (){return 0;}
unsigned LiquidCrystal_I2CRPi::init_bargraph(unsigned graphtype){return 0;}
void LiquidCrystal_I2CRPi::draw_horizontal_graph(unsigned row, unsigned column, unsigned len,  unsigned pixel_col_end){}
void LiquidCrystal_I2CRPi::draw_vertical_graph(unsigned row, unsigned column, unsigned len,  unsigned pixel_row_end){}
void LiquidCrystal_I2CRPi::setContrast(unsigned new_val){}

	
