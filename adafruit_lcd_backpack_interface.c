// LCD backpack is in SPI mode

#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <adafruit_lcd_backpack_interface.h>


/**
 * Sends 8 bits of data through SPI, LSB first
 * Inputs:
 *  - data: data to be sent
 */
void spi_send(const uint8_t data){
    P1OUT ^= BIT5;
    UCA0TXBUF = data;
    __delay_cycles(8);
    P1OUT ^= BIT5;
}


/**
 * Initializes SPI on eUSCI_A0 in 4-wire mode.
 * SIMO (DAT) on 1.4, CLK (CLK) on 1.6, STE (LAT) on 1.7, SOMI not used
 */
void init_spi(void){

    P1DIR |= BIT5;
    P1OUT |= BIT5;

    P1SEL |= BIT2 | BIT4;// | BIT5; // We are using 4-wire spi mode, SIMO on 1.2, CLK on 1.4, STE on 1.5
    P1SEL2 = BIT2 | BIT4;// | BIT5;

    UCA0CTL1=UCSWRST; //disable serial interface
    UCA0CTL0 |= UCCKPH + UCMST + UCSYNC;//+ UCMODE_2;    // data cap at 1st clk edge, MSB first, master mode, synchronous
    UCA0CTL1 |= UCSSEL_2;                           // select SMCLK
    UCA0BR0 = 0;                                    //set frequency
    UCA0BR1 = 0;                                    //
    UCA0CTL1 &= ~UCSWRST;           // Initialize USCI state machine

}


/**
 * Sends a 4-bit command to the backpack
 * Inputs:
 *  - data: 4-bit command to be sent
 */
void command_4bits(uint8_t data) {
    data = (data << 1) | 0x01;
    spi_send(data);     //We must pulse the ENABLE (E) bit while the data is stable for the LCD to register the command
    __delay_cycles(100);
    spi_send(data | ENABLE); //pulse the ENABLE (E) bit
    __delay_cycles(100);
    spi_send(data);
    __delay_cycles(100); //wait some time for spi send to finish
}


/**
 * Sends an 8-bit command to the backpack in two 4-bit chunks
 * Inputs:
 *  - data: 8-bit command to be sent
 */
void command(uint8_t data) {
    command_4bits(data >> 4);
    command_4bits(data & 0x0F);
}


/**
 * Writes a single char to the lcd display
 * Inputs:
 *  - ch: char to write
 */
void write_char(unsigned char ch) {
    uint8_t chLowData = ((ch & 0x0F) << 1) | 0x41;  //Send the character data in two 4-bit chunks
    uint8_t chHighData = ((ch >> 3) & 0x1E) | 0x41;


    spi_send(chHighData);   //We must pulse the ENABLE (E) bit while the data is stable for the LCD to register the data sent
    __delay_cycles(100);
    spi_send(chHighData | ENABLE); //pulse the ENABLE (E) bit
    __delay_cycles(100);
    spi_send(chHighData);
    __delay_cycles(100);    //wait some time for spi send to finish


    spi_send(chLowData);
    __delay_cycles(100);
    spi_send(chLowData | ENABLE); //pulse the ENABLE (E) bit
    __delay_cycles(100);
    spi_send(chLowData);
    __delay_cycles(100);    //wait some time for spi send to finish
}


/**
 * Writes a string to the lcd display
 * Inputs:
 *  - str[]: String to write
 */
void write_string(char str[]) {
    int i;
    for (i = 0; str[i] != '\0'; i++){
      write_char(str[i]);
    }
}


/**
 * Initializes the LCD in 4-bit mode, mostly following the flowchart on page 46 of the HD44780 datasheet
 */
void init_LCD(void) {
    //wait at least 40 ms after VCC reaches 2.7V
    __delay_cycles(60000);

    //follow chart on HD44780 datasheet pg 46 to initialize LCD
    command_4bits(0x03); //x x x x DB7 DB6 DB5 DB4 = 0011, see page 46

    //delay at least 4.1 ms
    __delay_cycles(6000);
    command_4bits(0x03);

    //delay at least 100 microseconds
    __delay_cycles(200);
    command_4bits(0x03);


    command_4bits(0x02); //put lcd in 4-bit mode


    //now we can start issuing commands to configure the lcd
    command(LCD_FUNCTION_SET | LCD_2_LINES); //LCD function set: 4-bits, 2 lines, 5x8 font

    command(LCD_DISPLAY_CTRL | LCD_DISPLAY_ON); //LCD display control: turn display on (we differ from the flowchart here, turn display on instead of off)

    command(LCD_CLEAR_DISPLAY); //clear the display

    __delay_cycles(3000); //The clear command takes a while!

    command(LCD_ENTRYMODE_SET | LCD_CURSOR_INCR); //LCD entry mode set: cursor in increment mode

}


/**
 * Clears the screen
 */
void clear_screen(void) {
    command(LCD_CLEAR_DISPLAY); //clear the display

    __delay_cycles(3000); //The clear command takes a while!

    command(LCD_ENTRYMODE_SET | LCD_CURSOR_INCR); //LCD entry mode set: cursor in increment mode

}

/**
 * Set the cursor position to zero
 */
void home(void) {
  command(LCD_RETURNHOME); // set cursor position to zero
  __delay_cycles(3000); // this command takes a long time!
}

/**
 * Sets the location of the cursor
 * Inputs:
 * - col Column to set the cursor in
 * - row Row to set the cursor in (0 / 1)
 */
void setCursor(uint8_t col, uint8_t row) {
  int row_offsets[] = {0x00, 0x40}; // 0x14, 0x54
  if (row > 2) {
    row = 1; // we count rows starting w/0
  }

  command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turns the underline cursor on/off
void noCursor() {
  command(LCD_DISPLAYCONTROL | LCD_DISPLAY_ON & ~LCD_CURSORON);
}
void cursor() {
  command(LCD_DISPLAYCONTROL | LCD_CURSORON | LCD_DISPLAY_ON);
}

// Turn on and off the blinking cursor
void noBlink() {
  command(LCD_DISPLAYCONTROL | LCD_DISPLAY_ON & ~LCD_BLINKON);
}
void blink() {
  command(LCD_DISPLAYCONTROL | LCD_DISPLAY_ON | LCD_BLINKON);
}
