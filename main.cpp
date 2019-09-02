#define RPI 0

#include <stdio.h>
#include <iostream>
using namespace std;
#include <pigpiod_if2.h>
#include "MyNetSerialRPi.h"
#include "LiquidCrystal_I2CRPi.h"
#define LED_PIN 5
#define LED_PIN2 10
#define BME280_ADDR 0x76//tes

int pi = pigpio_start(0,0);
MyNetSerialRPi mynet(pi,MCID_MASTER_RPI);
LiquidCrystal_I2CRPi lcd(pi,1,0x25,0,16,2);

int bmehandle;//tes

//argument :ID of the data array(mydata) to be displayed
void dataToMe(SerialDataRPi *sd)
{
    switch (sd->sdpp())
    {
    case SDPP_PERIODIC_CONFIRMATION:
        cout << "received periodic confirmation.." << endl;
        mynet.send(MCID_ESP32_C, SDPP_PERIODIC_CONFIRMATION);
        break;
    }
}
void processMyData()
{
    SerialDataRPi *sd = mynet.containedPick();
    while (sd != NULL)
    {

        dataToMe(sd);

        sd = mynet.containedPick();
    }
}

int main()
{
    mynet.debugSerial();
    set_mode(pi, LED_PIN, PI_OUTPUT);
    set_mode(pi, LED_PIN2, PI_OUTPUT);
    char wc[1];
bmehandle = i2c_open(pi,1,BME280_ADDR,0);//tes

    
    lcd.init();
    lcd.backlight();
    lcd.setCursor(2, 0);
    const char cc[10] = "Bye!!";
    lcd.print(cc);
    lcd.print("tes");
    
    
    while (1)
    {
        //Serial Process----------------------------------
        if (mynet.serialDataAvailable())
        {
            if (mynet.serialDataAvailable() < 0)
            {
                cout << "serial_data_available returns < 0.check pigpiod." << endl;
            }
            else
            {
                cout << "serial_data_incoming.." << mynet.serialDataAvailable() << "bytes" << endl;
                mynet.receive();
            }
        }
        processMyData();
        //Serial Process---end----------------------------

        // wc[0] = 'a';
        // serial_write(pi, handle, wc, 1);
        // cout << "Kitty on your a";
        // gpio_write(pi, LED_PIN, 1);
        // gpio_write(pi, LED_PIN2, 1);
        // wc[0] = 'b';
        // serial_write(pi, handle, wc, 1);
        // cout << "Kitty on your b" << endl;
        // gpio_write(pi, LED_PIN, 0);
        // gpio_write(pi, LED_PIN2, 0);

        time_sleep(1);

    } //while(1)

} //main
