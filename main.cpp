#define RPI 0

#include <stdio.h>
#include <iostream>
using namespace std;
#include <pigpiod_if2.h>
#include "MyNetSerialRPi.h"

#define LED_PIN 5
#define LED_PIN2 10
MyNetSerialRPi mynet(MCID_MASTER_RPI);
int pi;
unsigned handle;

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

    while (1)
    {
        //Serial Process----------------------------------
        if (serial_data_available(pi, handle))
        {
            if (serial_data_available(pi, handle) < 0)
            {
                cout << "serial_data_available returns < 0.check pigpiod." << endl;
            }
            else
            {
                cout << "serial_data_incoming.." << serial_data_available(pi, handle) << "bytes" << endl;
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
