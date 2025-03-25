#include <stdio.h>              //initializing useful libraries
#include "pico/stdlib.h"
#include "hardware/uart.h"

#define UART_ID uart0           //this device used the pico's uart0 (GP0 and GP1)
#define TX_PIN 1                //transmitting pin defined as pin 1 (GP 0)
#define RX_PIN 2                //receiving pin defined as pin 2    (GP 1)
#define DE_RE_PIN 3             //RS-485 control pin (used to switch between transmit and recieve mode) is defined as pin 3 (GP 3)
#define BAUD_RATE 250000        //sets BAUD Rate to 2500,000 baud, the standard for DMX512

void send_dmx_data(uint8_t * dmx_data, int size){
    
    gpio_put(DE_RE_PIN, 1);                 //enables transmit mode
    sleep_us(10);

    uart_set_baudrate(UART_ID, 9600);       //sends DMX BREAK (beginning of DMX packet)
    uart_putc(UART_ID, 0x00);
    sleep_us(100);

    uart_set_baudrate(UART_ID, BAUD_RATE);  //sends DMX MAB (Mark After Break)
    sleep_us(10);

    uart_putc(UART_ID,0x00);                //sends first empty byte of the 513 to be sends

    for (int i = 0; i < size; i++) {        //sends DMX data for each channel, the # of channels is determined by  the variable 'size'
        uart_putc(UART_ID, dmx_data[i]);    //for 6 led's, 18 channels
    }                                       //(end of DMX packet)

    sleep_us(10);                           // disables transmit mode, enable receive mode
    gpio_put(DE_RE_PIN, 0);
}


int main(){

    stdio_init_all();                           //initializes pico's I/O (good practice)

    uart_init(UART_ID, BAUD_RATE);              //initializes pico's uart0, at 250000 baud

    gpio_set_function(TX_PIN, GPIO_FUNC_UART);  //configuring transmiting/receiving pin 
    gpio_set_function(RX_PIN, GPIO_FUNC_UART);

    gpio_init(DE_RE_PIN);                       //initializes pico's GPIO2 for transmiting/receiving pin on RS-485 module, 
    gpio_set_dir(DE_RE_PIN, GPIO_OUT);          //and sets it to receiving mode
    gpio_put(DE_RE_PIN, 0);

    uint8_t dmx_data[18] = {0};                 //DMX Buffer that initializes all color values to 0

    while (true) {                              //while loop that indefinitely cycles LEDs thorugh all the desired colors
        
        for (int i = 0; i < 18; i += 3) {       //for loop that updates all color channels to desired brightness in the 18 channel 'dmx_data' array
            dmx_data[i] = 100;      // Red
            dmx_data[i + 1] = 0;    // Green
            dmx_data[i + 2] = 0;    // Blue
        }
        send_dmx_data(dmx_data, 18);            //sends updated DMX data
        sleep_ms(500);                          //short pause to linger on color

        for (int i = 0; i < 18; i += 3) {
            dmx_data[i] = 0;        // Red
            dmx_data[i + 1] = 100;  // Green
            dmx_data[i + 2] = 0;    // Blue
        }
        send_dmx_data(dmx_data, 18);
        sleep_ms(500);

        for (int i = 0; i < 18; i += 3) {
            dmx_data[i] = 0;        // Red
            dmx_data[i + 1] = 0;    // Green
            dmx_data[i + 2] = 100;  // Blue
        }
        send_dmx_data(dmx_data, 18);
        sleep_ms(500);

        for (int i = 0; i < 18; i += 3) {
            dmx_data[i] = 100;      // Red
            dmx_data[i + 1] = 100;  // Green
            dmx_data[i + 2] = 100;  // Blue
        }
        send_dmx_data(dmx_data, 18);
        sleep_ms(500);

    }
}


// test change