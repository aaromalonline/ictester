#include <8051.h>

/*
 * AT89S52 74LS00 IC TESTER
 *
 * MCU:
 *   AT89S52
 *   12 MHz crystal
 *
 * LCD (4-bit):
 *   RS -> P3.0 (pin 10)
 *   E  -> P3.1 (pin 11)
 *   D4 -> P3.2 (pin 12)
 *   D5 -> P3.3 (pin 13)
 *   D6 -> P3.4 (pin 14)
 *   D7 -> P3.5 (pin 15)
 *
 * TEST BUTTON:
 *   P3.6 (pin 16)
 *
 * LEDs (active LOW):
 *   GREEN -> P0.0 (pin 39)
 *   RED   -> P0.1 (pin 38)
 *
 * 14-pin ZIF SOCKET:
 *
 *   74LS00 pin    AT89S52 pin
 *       1             1  (P1.0)
 *       2             2  (P1.1)
 *       3             3  (P1.2)
 *       4             4  (P1.3)
 *       5             5  (P1.4)
 *       6             6  (P1.5)
 *       7            GND
 *       8            27  (P2.6)
 *       9            26  (P2.5)
 *      10            25  (P2.4)
 *      11            24  (P2.3)
 *      12            23  (P2.2)
 *      13            22  (P2.1)
 *      14            VCC
 *
 * IMPORTANT:
 * P2.0 is NOT connected to the ZIF according to the
 * provided hardware mapping, so it is NOT used below.
 */


/* -------------------------------------------------------
 * Delay
 * ------------------------------------------------------- */

void delay_ms(unsigned int t)
{
    unsigned int i;
    unsigned int j;

    for (i = 0; i < t; i++)
    {
        for (j = 0; j < 120; j++)
        {
            ;
        }
    }
}


/* -------------------------------------------------------
 * LCD
 * ------------------------------------------------------- */

void lcd_pulse(void)
{
    P3_1 = 1;
    delay_ms(1);
    P3_1 = 0;
    delay_ms(1);
}


void lcd_write_nibble(unsigned char n)
{
    P3_2 = (n >> 0) & 1;
    P3_3 = (n >> 1) & 1;
    P3_4 = (n >> 2) & 1;
    P3_5 = (n >> 3) & 1;

    lcd_pulse();
}


void lcd_cmd(unsigned char c)
{
    P3_0 = 0;

    lcd_write_nibble(c >> 4);
    lcd_write_nibble(c & 0x0F);

    delay_ms(2);
}


void lcd_data(unsigned char c)
{
    P3_0 = 1;

    lcd_write_nibble(c >> 4);
    lcd_write_nibble(c & 0x0F);

    delay_ms(1);
}


void lcd_string(char *s)
{
    while (*s)
    {
        lcd_data(*s);
        s++;
    }
}


void lcd_init(void)
{
    P3_0 = 0;
    P3_1 = 0;

    delay_ms(50);

    /*
     * Standard HD44780 4-bit initialization.
     */

    lcd_write_nibble(0x03);
    delay_ms(5);

    lcd_write_nibble(0x03);
    delay_ms(1);

    lcd_write_nibble(0x03);
    delay_ms(1);

    lcd_write_nibble(0x02);
    delay_ms(1);

    lcd_cmd(0x28);   /* 4-bit, 2-line, 5x8 */
    lcd_cmd(0x0C);   /* Display ON, cursor OFF */
    lcd_cmd(0x06);   /* Entry mode */
    lcd_cmd(0x01);   /* Clear */

    delay_ms(5);
}


void lcd_clear(void)
{
    lcd_cmd(0x01);
    delay_ms(5);
}


/* -------------------------------------------------------
 * LEDs
 *
 * LEDs are connected in pull-up configuration.
 *
 * LOW  = ON
 * HIGH = OFF
 * ------------------------------------------------------- */

void leds_off(void)
{
    P0_0 = 1;
    P0_1 = 1;
}


void green_on(void)
{
    P0_0 = 0;
    P0_1 = 1;
}


void red_on(void)
{
    P0_0 = 1;
    P0_1 = 0;
}


/* -------------------------------------------------------
 * 74LS00 TEST
 *
 * 74LS00 contains four 2-input NAND gates.
 *
 * Truth table:
 *
 * A B | Y
 * ----+---
 * 0 0 | 1
 * 0 1 | 1
 * 1 0 | 1
 * 1 1 | 0
 *
 * ZIF mapping:
 *
 * Gate 1:
 *   IC pin 1 -> P1.0
 *   IC pin 2 -> P1.1
 *   IC pin 3 -> P1.2
 *
 * Gate 2:
 *   IC pin 4 -> P1.3
 *   IC pin 5 -> P1.4
 *   IC pin 6 -> P1.5
 *
 * Gate 3:
 *   IC pin 9  -> P2.5
 *   IC pin 10 -> P2.4
 *   IC pin 8  -> P2.6
 *
 * Gate 4:
 *   IC pin 12 -> P2.2
 *   IC pin 13 -> P2.1
 *   IC pin 11 -> P2.3
 * ------------------------------------------------------- */


unsigned char test_gate1(void)
{
    /* 00 -> 1 */

    P1_0 = 0;
    P1_1 = 0;
    delay_ms(2);

    if (P1_2 != 1)
        return 0;


    /* 01 -> 1 */

    P1_0 = 0;
    P1_1 = 1;
    delay_ms(2);

    if (P1_2 != 1)
        return 0;


    /* 10 -> 1 */

    P1_0 = 1;
    P1_1 = 0;
    delay_ms(2);

    if (P1_2 != 1)
        return 0;


    /* 11 -> 0 */

    P1_0 = 1;
    P1_1 = 1;
    delay_ms(2);

    if (P1_2 != 0)
        return 0;


    return 1;
}


unsigned char test_gate2(void)
{
    /* 00 -> 1 */

    P1_3 = 0;
    P1_4 = 0;
    delay_ms(2);

    if (P1_5 != 1)
        return 0;


    /* 01 -> 1 */

    P1_3 = 0;
    P1_4 = 1;
    delay_ms(2);

    if (P1_5 != 1)
        return 0;


    /* 10 -> 1 */

    P1_3 = 1;
    P1_4 = 0;
    delay_ms(2);

    if (P1_5 != 1)
        return 0;


    /* 11 -> 0 */

    P1_3 = 1;
    P1_4 = 1;
    delay_ms(2);

    if (P1_5 != 0)
        return 0;


    return 1;
}


unsigned char test_gate3(void)
{
    /*
     * IC pin 9  -> P2.5
     * IC pin 10 -> P2.4
     * IC pin 8  -> P2.6
     */

    /* 00 -> 1 */

    P2_5 = 0;
    P2_4 = 0;
    delay_ms(2);

    if (P2_6 != 1)
        return 0;


    /* 01 -> 1 */

    P2_5 = 0;
    P2_4 = 1;
    delay_ms(2);

    if (P2_6 != 1)
        return 0;


    /* 10 -> 1 */

    P2_5 = 1;
    P2_4 = 0;
    delay_ms(2);

    if (P2_6 != 1)
        return 0;


    /* 11 -> 0 */

    P2_5 = 1;
    P2_4 = 1;
    delay_ms(2);

    if (P2_6 != 0)
        return 0;


    return 1;
}


unsigned char test_gate4(void)
{
    /*
     * IC pin 12 -> P2.2
     * IC pin 13 -> P2.1
     * IC pin 11 -> P2.3
     */

    /* 00 -> 1 */

    P2_2 = 0;
    P2_1 = 0;
    delay_ms(2);

    if (P2_3 != 1)
        return 0;


    /* 01 -> 1 */

    P2_2 = 0;
    P2_1 = 1;
    delay_ms(2);

    if (P2_3 != 1)
        return 0;


    /* 10 -> 1 */

    P2_2 = 1;
    P2_1 = 0;
    delay_ms(2);

    if (P2_3 != 1)
        return 0;


    /* 11 -> 0 */

    P2_2 = 1;
    P2_1 = 1;
    delay_ms(2);

    if (P2_3 != 0)
        return 0;


    return 1;
}


unsigned char nand_test(void)
{
    if (!test_gate1())
        return 0;

    if (!test_gate2())
        return 0;

    if (!test_gate3())
        return 0;

    if (!test_gate4())
        return 0;

    return 1;
}


/* -------------------------------------------------------
 * Main
 * ------------------------------------------------------- */

void main(void)
{
    unsigned char result;

    /*
     * Initial states.
     */

    leds_off();

    P3_6 = 1;       /* Button input */
    P3_7 = 1;       /* Unused P3.7 */

    lcd_init();

    lcd_clear();
    lcd_string("IC TESTER");

    lcd_cmd(0xC0);
    lcd_string("74LS00 READY");

    while (1)
    {
        /*
         * Button connected to P3.6.
         *
         * Change this to P3.7 only if the physical
         * button is actually connected to pin 17.
         */

        if (P3_6 == 1)
        {
            delay_ms(20);

            if (P3_6 == 1)
            {
                lcd_clear();
                lcd_string("TESTING 74LS00");

                leds_off();

                result = nand_test();

                if (result)
                {
                    lcd_clear();
                    lcd_string("74LS00 PASS");

                    green_on();
                }
                else
                {
                    lcd_clear();
                    lcd_string("74LS00 FAIL");

                    red_on();
                }

                /*
                 * Wait until button is released.
                 */

                while (P3_6 == 1)
                {
                    ;
                }

                delay_ms(20);
            }
        }
    }
}
```
