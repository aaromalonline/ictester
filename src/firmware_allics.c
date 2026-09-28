#include <8051.h>

/*
 * ============================================================
 *        AT89S52 74-SERIES LOGIC IC TESTER
 * ============================================================
 *
 * Supported ICs:
 *
 *   74LS00  NAND
 *   74LS02  NOR
 *   74LS04  NOT
 *   74LS08  AND
 *   74LS32  OR
 *   74LS86  XOR
 *
 *
 * ============================================================
 * LCD CONNECTION
 * ============================================================
 *
 * RS -> P3.0  (pin 10)
 * E  -> P3.1  (pin 11)
 * D4 -> P3.2  (pin 12)
 * D5 -> P3.3  (pin 13)
 * D6 -> P3.4  (pin 14)
 * D7 -> P3.5  (pin 15)
 *
 *
 * ============================================================
 * USER BUTTON
 * ============================================================
 *
 * TEST / NEXT -> P3.6 (pin 16)
 *
 * Assumption:
 *     Button produces HIGH when pressed.
 *
 *
 * ============================================================
 * LED CONNECTION
 * ============================================================
 *
 * GREEN -> P0.0 (pin 39)
 * RED   -> P0.1 (pin 38)
 *
 * LEDs are active LOW:
 *
 *     0 = ON
 *     1 = OFF
 *
 *
 * ============================================================
 * ZIF SOCKET -> AT89S52
 * ============================================================
 *
 * ZIF pin 1  -> P1.0
 * ZIF pin 2  -> P1.1
 * ZIF pin 3  -> P1.2
 * ZIF pin 4  -> P1.3
 * ZIF pin 5  -> P1.4
 * ZIF pin 6  -> P1.5
 *
 * ZIF pin 7  -> GND
 *
 * ZIF pin 8  -> P2.6
 * ZIF pin 9  -> P2.5
 * ZIF pin 10 -> P2.4
 * ZIF pin 11 -> P2.3
 * ZIF pin 12 -> P2.2
 * ZIF pin 13 -> P2.1
 *
 * ZIF pin 14 -> VCC
 *
 *
 * ============================================================
 * IMPORTANT
 * ============================================================
 *
 * P2.0 is NOT used because it is not connected to the ZIF
 * socket according to the supplied hardware mapping.
 *
 * The firmware therefore uses only ZIF pins 1-6 and 8-13.
 */


/* ============================================================
 * DELAY
 * ============================================================ */

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


/* ============================================================
 * LCD FUNCTIONS
 * ============================================================ */

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

    lcd_write_nibble(0x03);
    delay_ms(5);

    lcd_write_nibble(0x03);
    delay_ms(1);

    lcd_write_nibble(0x03);
    delay_ms(1);

    lcd_write_nibble(0x02);
    delay_ms(1);

    lcd_cmd(0x28);      /* 4-bit, 2-line, 5x8 */
    lcd_cmd(0x0C);      /* Display ON, cursor OFF */
    lcd_cmd(0x06);      /* Increment cursor */
    lcd_cmd(0x01);      /* Clear display */

    delay_ms(5);
}


void lcd_clear(void)
{
    lcd_cmd(0x01);
    delay_ms(5);
}


void lcd_line2(void)
{
    lcd_cmd(0xC0);
}


/* ============================================================
 * LED FUNCTIONS
 * ============================================================ */

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


/* ============================================================
 * ZIF PIN FUNCTIONS
 *
 * These convert ZIF socket pin numbers into the corresponding
 * AT89S52 port pins.
 *
 * ZIF:
 *
 * 1-6  -> P1.0-P1.5
 * 8-13 -> P2.6-P2.1
 *
 * ZIF pins 7 and 14 are power and are not controlled here.
 * ============================================================ */


/* Set a ZIF pin to logic 0 or 1 */

void zif_write(unsigned char pin, unsigned char value)
{
    switch (pin)
    {
        case 1:
            P1_0 = value;
            break;

        case 2:
            P1_1 = value;
            break;

        case 3:
            P1_2 = value;
            break;

        case 4:
            P1_3 = value;
            break;

        case 5:
            P1_4 = value;
            break;

        case 6:
            P1_5 = value;
            break;

        case 8:
            P2_6 = value;
            break;

        case 9:
            P2_5 = value;
            break;

        case 10:
            P2_4 = value;
            break;

        case 11:
            P2_3 = value;
            break;

        case 12:
            P2_2 = value;
            break;

        case 13:
            P2_1 = value;
            break;
    }
}


/* Read a ZIF pin */

unsigned char zif_read(unsigned char pin)
{
    switch (pin)
    {
        case 1:
            return P1_0;

        case 2:
            return P1_1;

        case 3:
            return P1_2;

        case 4:
            return P1_3;

        case 5:
            return P1_4;

        case 6:
            return P1_5;

        case 8:
            return P2_6;

        case 9:
            return P2_5;

        case 10:
            return P2_4;

        case 11:
            return P2_3;

        case 12:
            return P2_2;

        case 13:
            return P2_1;
    }

    return 0;
}


/*
 * Release a ZIF pin for reading.
 *
 * 8051 ports are quasi-bidirectional.
 * Writing 1 releases the pin so the external IC can drive it.
 */

void zif_release(unsigned char pin)
{
    zif_write(pin, 1);
}


/* ============================================================
 * TWO-INPUT LOGIC GATE TEST
 *
 * gate:
 *
 *     input A
 *     input B
 *     output Y
 *
 * operation:
 *
 *     0 = AND
 *     1 = OR
 *     2 = NAND
 *     3 = NOR
 *     4 = XOR
 *
 * ============================================================ */

unsigned char test_2input_gate(
    unsigned char a,
    unsigned char b,
    unsigned char y,
    unsigned char operation)
{
    unsigned char expected;

    /*
     * Release output pin.
     */
    zif_release(y);

    /*
     * Test 00
     */
    zif_write(a, 0);
    zif_write(b, 0);
    delay_ms(2);

    switch (operation)
    {
        case 0:
            expected = 0;          /* AND */
            break;

        case 1:
            expected = 0;          /* OR */
            break;

        case 2:
            expected = 1;          /* NAND */
            break;

        case 3:
            expected = 1;          /* NOR */
            break;

        default:
            expected = 0;          /* XOR */
            break;
    }

    if (zif_read(y) != expected)
        return 0;


    /*
     * Test 01
     */
    zif_write(a, 0);
    zif_write(b, 1);
    delay_ms(2);

    switch (operation)
    {
        case 0:
            expected = 0;
            break;

        case 1:
            expected = 1;
            break;

        case 2:
            expected = 1;
            break;

        case 3:
            expected = 0;
            break;

        default:
            expected = 1;
            break;
    }

    if (zif_read(y) != expected)
        return 0;


    /*
     * Test 10
     */
    zif_write(a, 1);
    zif_write(b, 0);
    delay_ms(2);

    switch (operation)
    {
        case 0:
            expected = 0;
            break;

        case 1:
            expected = 1;
            break;

        case 2:
            expected = 1;
            break;

        case 3:
            expected = 0;
            break;

        default:
            expected = 1;
            break;
    }

    if (zif_read(y) != expected)
        return 0;


    /*
     * Test 11
     */
    zif_write(a, 1);
    zif_write(b, 1);
    delay_ms(2);

    switch (operation)
    {
        case 0:
            expected = 1;
            break;

        case 1:
            expected = 1;
            break;

        case 2:
            expected = 0;
            break;

        case 3:
            expected = 0;
            break;

        default:
            expected = 0;
            break;
    }

    if (zif_read(y) != expected)
        return 0;


    return 1;
}


/* ============================================================
 * 74LS00 NAND
 *
 * Standard 7400 pinout:
 *
 * 1,2 -> 3
 * 4,5 -> 6
 * 9,10 -> 8
 * 12,13 -> 11
 * ============================================================ */

unsigned char test_7400(void)
{
    if (!test_2input_gate(1, 2, 3, 2))
        return 0;

    if (!test_2input_gate(4, 5, 6, 2))
        return 0;

    if (!test_2input_gate(9, 10, 8, 2))
        return 0;

    if (!test_2input_gate(12, 13, 11, 2))
        return 0;

    return 1;
}


/* ============================================================
 * 74LS08 AND
 *
 * 1,2 -> 3
 * 4,5 -> 6
 * 9,10 -> 8
 * 12,13 -> 11
 * ============================================================ */

unsigned char test_7408(void)
{
    if (!test_2input_gate(1, 2, 3, 0))
        return 0;

    if (!test_2input_gate(4, 5, 6, 0))
        return 0;

    if (!test_2input_gate(9, 10, 8, 0))
        return 0;

    if (!test_2input_gate(12, 13, 11, 0))
        return 0;

    return 1;
}


/* ============================================================
 * 74LS32 OR
 *
 * 1,2 -> 3
 * 4,5 -> 6
 * 9,10 -> 8
 * 12,13 -> 11
 * ============================================================ */

unsigned char test_7432(void)
{
    if (!test_2input_gate(1, 2, 3, 1))
        return 0;

    if (!test_2input_gate(4, 5, 6, 1))
        return 0;

    if (!test_2input_gate(9, 10, 8, 1))
        return 0;

    if (!test_2input_gate(12, 13, 11, 1))
        return 0;

    return 1;
}


/* ============================================================
 * 74LS86 XOR
 *
 * 1,2 -> 3
 * 4,5 -> 6
 * 9,10 -> 8
 * 12,13 -> 11
 * ============================================================ */

unsigned char test_7486(void)
{
    if (!test_2input_gate(1, 2, 3, 4))
        return 0;

    if (!test_2input_gate(4, 5, 6, 4))
        return 0;

    if (!test_2input_gate(9, 10, 8, 4))
        return 0;

    if (!test_2input_gate(12, 13, 11, 4))
        return 0;

    return 1;
}


/* ============================================================
 * 74LS02 NOR
 *
 * IMPORTANT:
 *
 * 7402 has a DIFFERENT pin arrangement from 7400/7408/7432/7486.
 *
 * Gate 1:
 *     2,3 -> 1
 *
 * Gate 2:
 *     5,6 -> 4
 *
 * Gate 3:
 *     8,9 -> 10
 *
 * Gate 4:
 *     11,12 -> 13
 * ============================================================ */

unsigned char test_7402(void)
{
    if (!test_2input_gate(2, 3, 1, 3))
        return 0;

    if (!test_2input_gate(5, 6, 4, 3))
        return 0;

    if (!test_2input_gate(8, 9, 10, 3))
        return 0;

    if (!test_2input_gate(11, 12, 13, 3))
        return 0;

    return 1;
}


/* ============================================================
 * 74LS04 NOT
 *
 * Six independent inverters:
 *
 * 1 -> 2
 * 3 -> 4
 * 5 -> 6
 * 9 -> 8
 * 11 -> 10
 * 13 -> 12
 * ============================================================ */

unsigned char test_7404_gate(
    unsigned char input_pin,
    unsigned char output_pin)
{
    /*
     * Input = 0
     * Expected output = 1
     */

    zif_release(output_pin);

    zif_write(input_pin, 0);
    delay_ms(2);

    if (zif_read(output_pin) != 1)
        return 0;


    /*
     * Input = 1
     * Expected output = 0
     */

    zif_write(input_pin, 1);
    delay_ms(2);

    if (zif_read(output_pin) != 0)
        return 0;


    return 1;
}


unsigned char test_7404(void)
{
    if (!test_7404_gate(1, 2))
        return 0;

    if (!test_7404_gate(3, 4))
        return 0;

    if (!test_7404_gate(5, 6))
        return 0;

    if (!test_7404_gate(9, 8))
        return 0;

    if (!test_7404_gate(11, 10))
        return 0;

    if (!test_7404_gate(13, 12))
        return 0;

    return 1;
}


/* ============================================================
 * DISPLAY IC NAME
 * ============================================================ */

void display_ic(unsigned char ic)
{
    lcd_clear();

    switch (ic)
    {
        case 0:
            lcd_string("74LS00 NAND");
            break;

        case 1:
            lcd_string("74LS02 NOR");
            break;

        case 2:
            lcd_string("74LS04 NOT");
            break;

        case 3:
            lcd_string("74LS08 AND");
            break;

        case 4:
            lcd_string("74LS32 OR");
            break;

        case 5:
            lcd_string("74LS86 XOR");
            break;
    }
}


/* ============================================================
 * TEST SELECTED IC
 * ============================================================ */

unsigned char test_selected_ic(unsigned char ic)
{
    switch (ic)
    {
        case 0:
            return test_7400();

        case 1:
            return test_7402();

        case 2:
            return test_7404();

        case 3:
            return test_7408();

        case 4:
            return test_7432();

        case 5:
            return test_7486();
    }

    return 0;
}


/* ============================================================
 * MAIN
 * ============================================================ */

void main(void)
{
    unsigned char selected_ic;
    unsigned char result;

    /*
     * Initial port states.
     */

    leds_off();

    /*
     * Button input.
     */
    P3_6 = 1;

    /*
     * Unused P3.7 released.
     */
    P3_7 = 1;

    /*
     * Initialize LCD.
     */
    lcd_init();

    /*
     * Initial screen.
     */
    lcd_clear();
    lcd_string("IC TESTER");

    lcd_line2();
    lcd_string("74LS00 READY");

    /*
     * Start with 74LS00.
     */
    selected_ic = 0;


    while (1)
    {
        /*
         * Wait for button press.
         *
         * This assumes P3.6 becomes HIGH when the
         * button is pressed.
         */

        if (P3_6 == 1)
        {
            delay_ms(20);

            if (P3_6 == 1)
            {
                /*
                 * Display selected IC.
                 */
                display_ic(selected_ic);

                lcd_line2();
                lcd_string("TESTING...");

                leds_off();

                delay_ms(100);

                /*
                 * Run complete truth-table test.
                 */
                result = test_selected_ic(selected_ic);


                /*
                 * Display result.
                 */

                if (result)
                {
                    lcd_clear();

                    switch (selected_ic)
                    {
                        case 0:
                            lcd_string("74LS00 PASS");
                            break;

                        case 1:
                            lcd_string("74LS02 PASS");
                            break;

                        case 2:
                            lcd_string("74LS04 PASS");
                            break;

                        case 3:
                            lcd_string("74LS08 PASS");
                            break;

                        case 4:
                            lcd_string("74LS32 PASS");
                            break;

                        case 5:
                            lcd_string("74LS86 PASS");
                            break;
                    }

                    green_on();
                }
                else
                {
                    lcd_clear();

                    switch (selected_ic)
                    {
                        case 0:
                            lcd_string("74LS00 FAIL");
                            break;

                        case 1:
                            lcd_string("74LS02 FAIL");
                            break;

                        case 2:
                            lcd_string("74LS04 FAIL");
                            break;

                        case 3:
                            lcd_string("74LS08 FAIL");
                            break;

                        case 4:
                            lcd_string("74LS32 FAIL");
                            break;

                        case 5:
                            lcd_string("74LS86 FAIL");
                            break;
                    }

                    red_on();
                }


                /*
                 * Move to next IC.
                 *
                 * Sequence:
                 *
                 * 74LS00
                 * 74LS02
                 * 74LS04
                 * 74LS08
                 * 74LS32
                 * 74LS86
                 * 74LS00 ...
                 */

                selected_ic++;

                if (selected_ic >= 6)
                    selected_ic = 0;


                /*
                 * Wait for button release.
                 */

                while (P3_6 == 1)
                {
                    ;
                }

                delay_ms(50);
            }
        }
    }
}

