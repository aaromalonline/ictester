#include <8051.h>

#define LCD P3

void delay(unsigned int t)
{
    unsigned int i,j;
    for(i=0;i<t;i++)
        for(j=0;j<120;j++);
}

void lcd_pulse(void)
{
    P3_1 = 1;
    delay(2);
    P3_1 = 0;
    delay(2);
}

void lcd_cmd(unsigned char c)
{
    P3_0 = 0;

    P3_2 = (c >> 4) & 1;
    P3_3 = (c >> 5) & 1;
    P3_4 = (c >> 6) & 1;
    P3_5 = (c >> 7) & 1;
    lcd_pulse();

    P3_2 = c & 1;
    P3_3 = (c >> 1) & 1;
    P3_4 = (c >> 2) & 1;
    P3_5 = (c >> 3) & 1;
    lcd_pulse();

    delay(5);
}

void lcd_data(unsigned char c)
{
    P3_0 = 1;

    P3_2 = (c >> 4) & 1;
    P3_3 = (c >> 5) & 1;
    P3_4 = (c >> 6) & 1;
    P3_5 = (c >> 7) & 1;
    lcd_pulse();

    P3_2 = c & 1;
    P3_3 = (c >> 1) & 1;
    P3_4 = (c >> 2) & 1;
    P3_5 = (c >> 3) & 1;
    lcd_pulse();

    delay(5);
}

void lcd_string(char *s)
{
    while(*s)
        lcd_data(*s++);
}

void lcd_init(void)
{
    P3_0 = 0;
    P3_1 = 0;

    delay(50);

    lcd_cmd(0x02);
    delay(10);

    lcd_cmd(0x28);
    delay(5);

    lcd_cmd(0x0C);
    delay(5);

    lcd_cmd(0x06);
    delay(5);

    lcd_cmd(0x01);
    delay(10);
}

void lcd_clear(void)
{
    lcd_cmd(0x01);
    delay(10);
}

unsigned char nand_test(void)
{
    unsigned char pass = 1;

    /* Gate 1: P1.0, P1.1 -> P1.2 */

    P1_2 = 1;

    P1_0 = 0;
    P1_1 = 0;
    delay(2);
    if(P1_2 != 1)
        pass = 0;

    P1_0 = 0;
    P1_1 = 1;
    delay(2);
    if(P1_2 != 1)
        pass = 0;

    P1_0 = 1;
    P1_1 = 0;
    delay(2);
    if(P1_2 != 1)
        pass = 0;

    P1_0 = 1;
    P1_1 = 1;
    delay(2);
    if(P1_2 != 0)
        pass = 0;


    /* Gate 2: P1.3, P1.4 -> P1.5 */

    P1_5 = 1;

    P1_3 = 0;
    P1_4 = 0;
    delay(2);
    if(P1_5 != 1)
        pass = 0;

    P1_3 = 0;
    P1_4 = 1;
    delay(2);
    if(P1_5 != 1)
        pass = 0;

    P1_3 = 1;
    P1_4 = 0;
    delay(2);
    if(P1_5 != 1)
        pass = 0;

    P1_3 = 1;
    P1_4 = 1;
    delay(2);
    if(P1_5 != 0)
        pass = 0;


    /* Gate 3: P2.1, P2.2 -> P2.0 */

    P2_0 = 1;

    P2_1 = 0;
    P2_2 = 0;
    delay(2);
    if(P2_0 != 1)
        pass = 0;

    P2_1 = 0;
    P2_2 = 1;
    delay(2);
    if(P2_0 != 1)
        pass = 0;

    P2_1 = 1;
    P2_2 = 0;
    delay(2);
    if(P2_0 != 1)
        pass = 0;

    P2_1 = 1;
    P2_2 = 1;
    delay(2);
    if(P2_0 != 0)
        pass = 0;


    /* Gate 4: P2.4, P2.5 -> P2.3 */

    P2_3 = 1;

    P2_4 = 0;
    P2_5 = 0;
    delay(2);
    if(P2_3 != 1)
        pass = 0;

    P2_4 = 0;
    P2_5 = 1;
    delay(2);
    if(P2_3 != 1)
        pass = 0;

    P2_4 = 1;
    P2_5 = 0;
    delay(2);
    if(P2_3 != 1)
        pass = 0;

    P2_4 = 1;
    P2_5 = 1;
    delay(2);
    if(P2_3 != 0)
        pass = 0;

    return pass;
}

void main(void)
{
    lcd_init();

    lcd_string("IC TESTER");
    lcd_cmd(0xC0);
    lcd_string("74LS00 READY");

    while(1)
    {
        if(P3_7 == 1)
        {
            delay(20);

            lcd_clear();
            lcd_string("TESTING 74LS00");

            if(nand_test())
            {
                lcd_clear();
                lcd_string("74LS00 PASS");
            }
            else
            {
                lcd_clear();
                lcd_string("74LS00 FAIL");
            }

            while(P3_7 == 1);

            delay(20);
        }
    }
}
