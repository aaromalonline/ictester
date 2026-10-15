
#include <REGX51.H>
#include <intrins.h>


// PIN DEFINITION 

sbit LCD_RS = P3^0;
sbit LCD_EN = P3^1;
sbit LCD_D4 = P3^2;
sbit LCD_D5 = P3^3;
sbit LCD_D6 = P3^4;
sbit LCD_D7 = P3^5;

sbit TEST_BUTTON   = P3^7;

sbit LED_GREEN = P0^1;
sbit LED_RED   = P0^0;

sbit ZIF1  = P1^0;
sbit ZIF2  = P1^1;
sbit ZIF3  = P1^2;
sbit ZIF4  = P1^3;
sbit ZIF5  = P1^4;
sbit ZIF6  = P1^5;

sbit ZIF8  = P2^6;
sbit ZIF9  = P2^5;
sbit ZIF10 = P2^4;
sbit ZIF11 = P2^3;
sbit ZIF12 = P2^2;
sbit ZIF13 = P2^1;


// SETTINGS

#define IC_7402 0
#define IC_7404 1
#define IC_7400 2

#define DEBOUNCE_MS   30      
#define LONG_PRESS_MS 1000    
#define SETTLE_MS     2       
#define BTN_NONE     0
#define BTN_PRESSED  1
#define BTN_RELEASED 2


// GLOBALS 

unsigned char selected_ic = IC_7402;


// DELAYS 

void delay_ms(unsigned int ms)
{
    TMOD = (TMOD & 0xF0) | 0x01;

    while (ms != 0)
    {
        TR0 = 0;
        TH0 = 0xFC;         /* 65536 - 1000 = 0xFC18 */
        TL0 = 0x18;
        TF0 = 0;
        TR0 = 1;

        while (TF0 == 0)
        {
            ;
        }

        TR0 = 0;
        ms--;
    }
}

/* Short wait, at least ~80 us (the LCD needs 37 us per instruction). */
void lcd_settle(void)
{
    unsigned char i;

    for (i = 0; i < 20; i++)
    {
        _nop_();
        _nop_();
    }
}


//LCD FUNCTIONS 

void lcd_pulse(void)
{
    LCD_EN = 1;
    _nop_();
    _nop_();
    LCD_EN = 0;
}

void lcd_write_nibble(unsigned char n)
{
    LCD_D4 = (n & 0x01) ? 1 : 0;
    LCD_D5 = (n & 0x02) ? 1 : 0;
    LCD_D6 = (n & 0x04) ? 1 : 0;
    LCD_D7 = (n & 0x08) ? 1 : 0;

    lcd_pulse();
}

void lcd_cmd(unsigned char cmd)
{
    LCD_RS = 0;
    lcd_write_nibble(cmd >> 4);
    lcd_write_nibble(cmd & 0x0F);
    lcd_settle();
}

void lcd_data(unsigned char value)
{
    LCD_RS = 1;
    lcd_write_nibble(value >> 4);
    lcd_write_nibble(value & 0x0F);
    lcd_settle();
}

void lcd_string(char code *s)
{
    while (*s)
    {
        lcd_data(*s);
        s++;
    }
}

void lcd_clear(void)
{
    lcd_cmd(0x01);
    delay_ms(3);            /* clear needs ~1.5 ms */
}

void lcd_goto(unsigned char row, unsigned char col)
{
    if (row == 0)
        lcd_cmd(0x80 + col);
    else
        lcd_cmd(0xC0 + col);
}

void lcd_init(void)
{
    LCD_RS = 0;
    LCD_EN = 0;

    delay_ms(50);

    lcd_write_nibble(0x03);
    delay_ms(5);
    lcd_write_nibble(0x03);
    delay_ms(2);
    lcd_write_nibble(0x03);
    delay_ms(2);
    lcd_write_nibble(0x02);     /* switch to 4-bit mode */
    delay_ms(2);

    lcd_cmd(0x28);              /* 4-bit, 2 lines, 5x8 font */
    lcd_cmd(0x0C);              /* display on, cursor off   */
    lcd_cmd(0x06);              /* cursor increments        */
    lcd_clear();
}


//LED FUNCTIONS (active LOW)
void leds_off(void)
{
    LED_GREEN = 1;
    LED_RED   = 1;
}

void green_on(void)
{
    LED_RED   = 1;
    LED_GREEN = 0;
}

void red_on(void)
{
    LED_GREEN = 1;
    LED_RED   = 0;
}


// ZIF I/O 

/* Writing 1 releases a quasi-bidirectional pin so it can be read. */
void release_zif_pins(void)
{
    P1 = 0xFF;
    P2 = 0xFF;
}


//GATE TEST MACROS 


#define TEST_NOR(A, B, Y)                           \
do {                                                \
    Y = 1;                                          \
    A = 0; B = 0; delay_ms(SETTLE_MS);              \
    if (Y != 1) return 0;                           \
    A = 0; B = 1; delay_ms(SETTLE_MS);              \
    if (Y != 0) return 0;                           \
    A = 1; B = 0; delay_ms(SETTLE_MS);              \
    if (Y != 0) return 0;                           \
    A = 1; B = 1; delay_ms(SETTLE_MS);              \
    if (Y != 0) return 0;                           \
    A = 1; B = 1;                                   \
} while (0)

#define TEST_NAND(A, B, Y)                          \
do {                                                \
    Y = 1;                                          \
    A = 0; B = 0; delay_ms(SETTLE_MS);              \
    if (Y != 1) return 0;                           \
    A = 0; B = 1; delay_ms(SETTLE_MS);              \
    if (Y != 1) return 0;                           \
    A = 1; B = 0; delay_ms(SETTLE_MS);              \
    if (Y != 1) return 0;                           \
    A = 1; B = 1; delay_ms(SETTLE_MS);              \
    if (Y != 0) return 0;                           \
    A = 1; B = 1;                                   \
} while (0)

#define TEST_INV(A, Y)                              \
do {                                                \
    Y = 1;                                          \
    A = 0; delay_ms(SETTLE_MS);                     \
    if (Y != 1) return 0;                           \
    A = 1; delay_ms(SETTLE_MS);                     \
    if (Y != 0) return 0;                           \
} while (0)


/* ================= 7402 QUAD NOR ================= */
/* gate1 A=2 B=3 Y=1 | gate2 A=5 B=6 Y=4 | gate3 A=8 B=9 Y=10 | gate4 A=11 B=12 Y=13 */

unsigned char test_7402_gate1(void) { TEST_NOR(ZIF2,  ZIF3,  ZIF1);  return 1; }
unsigned char test_7402_gate2(void) { TEST_NOR(ZIF5,  ZIF6,  ZIF4);  return 1; }
unsigned char test_7402_gate3(void) { TEST_NOR(ZIF8,  ZIF9,  ZIF10); return 1; }
unsigned char test_7402_gate4(void) { TEST_NOR(ZIF11, ZIF12, ZIF13); return 1; }

/* Returns 0 for PASS, otherwise the number of the first failed gate. */
unsigned char test_7402(void)
{
    if (!test_7402_gate1()) return 1;
    if (!test_7402_gate2()) return 2;
    if (!test_7402_gate3()) return 3;
    if (!test_7402_gate4()) return 4;
    return 0;
}


// 7400 QUAD NAND 
/* gate1 A=1 B=2 Y=3 | gate2 A=4 B=5 Y=6 | gate3 A=9 B=10 Y=8 | gate4 A=12 B=13 Y=11 */

unsigned char test_7400_gate1(void) { TEST_NAND(ZIF1,  ZIF2,  ZIF3);  return 1; }
unsigned char test_7400_gate2(void) { TEST_NAND(ZIF4,  ZIF5,  ZIF6);  return 1; }
unsigned char test_7400_gate3(void) { TEST_NAND(ZIF9,  ZIF10, ZIF8);  return 1; }
unsigned char test_7400_gate4(void) { TEST_NAND(ZIF12, ZIF13, ZIF11); return 1; }

unsigned char test_7400(void)
{
    if (!test_7400_gate1()) return 1;
    if (!test_7400_gate2()) return 2;
    if (!test_7400_gate3()) return 3;
    if (!test_7400_gate4()) return 4;
    return 0;
}


// 7404 HEX INVERTER 
/* 1->2, 3->4, 5->6, 9->8, 11->10, 13->12   (input -> output) */

unsigned char test_7404_gate1(void) { TEST_INV(ZIF1,  ZIF2);  return 1; }
unsigned char test_7404_gate2(void) { TEST_INV(ZIF3,  ZIF4);  return 1; }
unsigned char test_7404_gate3(void) { TEST_INV(ZIF5,  ZIF6);  return 1; }
unsigned char test_7404_gate4(void) { TEST_INV(ZIF9,  ZIF8);  return 1; }
unsigned char test_7404_gate5(void) { TEST_INV(ZIF11, ZIF10); return 1; }
unsigned char test_7404_gate6(void) { TEST_INV(ZIF13, ZIF12); return 1; }

unsigned char test_7404(void)
{
    if (!test_7404_gate1()) return 1;
    if (!test_7404_gate2()) return 2;
    if (!test_7404_gate3()) return 3;
    if (!test_7404_gate4()) return 4;
    if (!test_7404_gate5()) return 5;
    if (!test_7404_gate6()) return 6;
    return 0;
}


// USER INTERFACE 

void show_selection(void)
{
    lcd_clear();
    lcd_goto(0, 0);

    if (selected_ic == IC_7402)
        lcd_string("IC: 7402 NOR");
    else if (selected_ic == IC_7404)
        lcd_string("IC: 7404 INV");
    else
        lcd_string("IC: 7400 NAND");

    lcd_goto(1, 0);
    lcd_string("TAP=TEST HOLD=IC");
}

void show_result(unsigned char failed_gate)
{
    lcd_clear();
    lcd_goto(0, 0);

    if (failed_gate == 0)
    {
        green_on();

        if (selected_ic == IC_7402)
            lcd_string("7402 PASS");
        else if (selected_ic == IC_7404)
            lcd_string("7404 PASS");
        else
            lcd_string("7400 PASS");

        lcd_goto(1, 0);

        if (selected_ic == IC_7404)
            lcd_string("ALL 6 GATES OK");
        else
            lcd_string("ALL 4 GATES OK");
    }
    else
    {
        red_on();

        if (selected_ic == IC_7402)
            lcd_string("7402 FAIL");
        else if (selected_ic == IC_7404)
            lcd_string("7404 FAIL");
        else
            lcd_string("7400 FAIL");

        lcd_goto(1, 0);
        lcd_string("GATE ");
        lcd_data('0' + failed_gate);
        lcd_string(" FAILED");
    }
}

void run_selected_test(void)
{
    unsigned char failed_gate;

    leds_off();
    lcd_clear();
    release_zif_pins();

    if (selected_ic == IC_7402)
    {
        lcd_string("TESTING 7402");
        delay_ms(100);
        failed_gate = test_7402();
    }
    else if (selected_ic == IC_7404)
    {
        lcd_string("TESTING 7404");
        delay_ms(100);
        failed_gate = test_7404();
    }
    else
    {
        lcd_string("TESTING 7400");
        delay_ms(100);
        failed_gate = test_7400();
    }

    release_zif_pins();
    show_result(failed_gate);
}


// SINGLE-BUTTON PRESS HANDLING 
typedef struct
{
    unsigned char stable;       /* debounced level: 1=released, 0=pressed */
    unsigned char count;        /* consecutive ms that raw differs from stable */
    unsigned int  held_ms;      /* debounced press duration in ms */
    unsigned char armed;        /* set after pin has been seen HIGH */
    unsigned char press_active; /* true only for a valid press begun after HIGH */
} BUTTON;

BUTTON main_button;

/*
 * Do not interpret a pin that is already LOW at startup as a new press.
 * It must first be released HIGH, then pressed again.
 */
void button_init(BUTTON *b, unsigned char raw)
{
    b->stable = raw ? 1 : 0;
    b->count = 0;
    b->held_ms = 0;
    b->armed = raw ? 1 : 0;
    b->press_active = 0;
}

/* Call every ~1 ms. Reports one event per debounced press/release. */
unsigned char button_update(BUTTON *b, unsigned char raw)
{
    unsigned char event;

    event = BTN_NONE;
    raw = raw ? 1 : 0;

    if (raw == b->stable)
    {
        b->count = 0;
    }
    else
    {
        b->count++;

        if (b->count >= DEBOUNCE_MS)
        {
            b->count = 0;
            b->stable = raw;

            if (raw == 0)
            {
                if (b->armed)
                {
                    b->press_active = 1;
                    b->held_ms = 0;
                    event = BTN_PRESSED;
                }
            }
            else
            {
                /* HIGH: release; arm for the next deliberate press. */
                if (b->press_active)
                {
                    b->press_active = 0;
                    event = BTN_RELEASED;
                }
                b->armed = 1;
                b->held_ms = 0;
            }
        }
    }

    if (b->press_active && b->stable == 0 && b->held_ms < 0xFFFF)
        b->held_ms++;

    return event;
}

/* ================= MAIN ================= */

void main(void)
{
    unsigned char event;
    unsigned char long_action_done;

    leds_off();

    /* P3.7 is the only button used by this firmware. */
    TEST_BUTTON = 1;            /* release latch to use pin as input */

    release_zif_pins();

    lcd_init();
    show_selection();           /* starts on 7402 */

    delay_ms(50);               /* let supply and input settle */
    button_init(&main_button, TEST_BUTTON);
    long_action_done = 0;

    while (1)
    {
        delay_ms(1);
        event = button_update(&main_button, TEST_BUTTON);

        if (event == BTN_PRESSED)
        {
            long_action_done = 0;
        }

        /*
         * Long press: advance one IC once while the button is held.
         * Releasing afterwards will NOT run a test for this same press.
         */
        if (main_button.press_active &&
            !long_action_done &&
            main_button.held_ms >= LONG_PRESS_MS)
        {
            selected_ic++;
            if (selected_ic > IC_7400)
                selected_ic = IC_7402;

            long_action_done = 1;
            leds_off();
            show_selection();
        }

        /*
         * Short press: test only when the button is released, so the
         * firmware can distinguish a tap from a long hold.
         */
        if (event == BTN_RELEASED)
        {
            if (!long_action_done)
                run_selected_test();

            long_action_done = 0;
        }
    }
}
