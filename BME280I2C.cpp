//Based on Tyler Glenn's library for Arduino.

/*
BME280I2CI2C.cpp
This code records data from the BME280I2C sensor and provides an API.
This file is part of the Arduino BME280I2C library.
Copyright (C) 2016  Tyler Glenn

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Written: Dec 30 2015.
Last Updated: Jan 1 2016. - Happy New year!

This header must be included in any derived code or copies of the code.

Based on the data sheet provided by Bosch for the BME280I2C environmental sensor,
calibration code based on algorithms providedBosch, some unit conversations courtesy
of www.endmemo.com, altitude equation courtesy of NOAA, and dew point equation
courtesy of Brian McNoldy at http://andrew.rsmas.miami.edu.
 */

#include <Wire.h>

#include "BME280I2C.h"


/****************************************************************/
BME280I2C::BME280I2C
(
  const Settings& settings
):BME280(settings),
  m_bme_280_addr(settings.bme280Addr)
{
}


/****************************************************************/
bool BME280I2C::WriteRegister
(
  uint8_t addr,
  uint8_t data
)
{
  Wire.beginTransmission(m_bme_280_addr);
  Wire.write(addr);
  Wire.write(data);
  Wire.endTransmission();

  return true; // TODO: Chech return values from wire calls.
}


/****************************************************************/
bool BME280I2C::ReadRegister
(
  uint8_t addr,//BME280のレジスタのアドレス
  uint8_t data[],
  uint8_t length//BME280のレジスタの範囲（データのビット数）
)
{
  uint8_t ord(0);

  Wire.beginTransmission(m_bme_280_addr);//i2c_open()
  Wire.write(addr);//i2c_write_byte
  Wire.endTransmission();//i2c_close

  Wire.requestFrom(m_bme_280_addr, length);//→Wire.cpp twi_readFrom(,,,)
  //→twi.c TWCR TWBR TWAR....   →<avr/io.h> 
  //→ <iom328p.h>etc..  #define TWCR _SFR_MEM8(0xBC)
  //→　avr/#define _SFR_MEM8(mem_addr) (mem_addr)



//RPIでWireのBufferIndexとBufferLengthに当たるものがなんなのか調べないといけない
  while(Wire.available())//問題個所
  {
    data[ord++] = Wire.read();
  }

  return ord == length;
}
