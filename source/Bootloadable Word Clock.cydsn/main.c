/******************************************************************************
* Project Name		: WordClock
* File Name			: main.c
* Version 			: 1.0
* Device Used		: CY8C4245AXI-483
* Software Used		: PSoC Creator 3.0 Service Pack 2
* Compiler    		: ARMGCC 4.7.3, ARM RVDS Generic, ARM MDK Generic
* Related Hardware	: CY8CKIT-049-42xx PSoC 4 Prototyping Kit 
*
*******************************************************************************/

/******************************************************************************
*                           THEORY OF OPERATION
* This is a Word Clock based on a 8x8 matrix LED array. 
* Default UART Port Configuration for bootloading the PSoC 4 in CY8CKIT-049-42xx
* Baud Rate : 115200 bps
* Data Bits : 8
* Stop Bits : 1
* Parity    : None
******************************************************************************/
#include <project.h>

uint16_t millis;
uint8_t seconds;
uint8_t minutes;
uint8_t hours;

uint8_t rows[8];
uint8_t rowIndex;

uint8_t halfSecond;

/*******************************************************************************
* Define Interrupt service routine and allocate an vector to the Interrupt
********************************************************************************/
CY_ISR(InterruptHandler) {
    
    Timer_1_GetInterruptSource();
    
    millis += 1;
    if (millis > 999) {
        millis = 0;
        if (++seconds > 59) {
            seconds = 0;
            if (++minutes > 59) {
                minutes = 0;
                if (++hours > 23) {
                    hours = 0;
                }
            }
        }
    }
    
    rowIndex = rowIndex << 1;
    if (rowIndex == 0) rowIndex = 1;
    switch (rowIndex) {
        case 0x01:
            Source_8_Write(1);
            break;
        case 0x02:
            Source_1_Write(1);
            break;
        case 0x04:
            Source_2_Write(1);
            break;
        case 0x08:
            Source_3_Write(1);
            break;
        case 0x10:
            Source_4_Write(1);
            break;
        case 0x20:
            Source_5_Write(1);
            break;
        case 0x40:
            Source_6_Write(1);
            break;
        default:
            Source_7_Write(1);
            break;
    }
    
    Sink_1_Write((rows[0] & rowIndex) != 0 ? 1 : 0);
    Sink_2_Write((rows[1] & rowIndex) != 0 ? 1 : 0);
    Sink_3_Write((rows[2] & rowIndex) != 0 ? 1 : 0);
    Sink_4_Write((rows[3] & rowIndex) != 0 ? 1 : 0);
    Sink_5_Write((rows[4] & rowIndex) != 0 ? 1 : 0);
    Sink_6_Write((rows[5] & rowIndex) != 0 ? 1 : 0);
    Sink_7_Write((rows[6] & rowIndex) != 0 ? 1 : 0);
    Sink_8_Write((rows[7] & rowIndex) != 0 ? 1 : 0);
    
    switch (rowIndex) {
        case 0x01:
            Source_1_Write(0);
            break;
        case 0x02:
            Source_2_Write(0);
            break;
        case 0x04:
            Source_3_Write(0);
            break;
        case 0x08:
            Source_4_Write(0);
            break;
        case 0x10:
            Source_5_Write(0);
            break;
        case 0x20:
            Source_6_Write(0);
            break;
        case 0x40:
            Source_7_Write(0);
            break;
        default:
            Source_8_Write(0);
            break;
    }
}

CY_ISR(KeyHandler) {
    if (Pin_Button_Read() == 0) {
        seconds = 0;
        minutes += 5;
        if (minutes > 59) {
            minutes -= 60;
            hours++;
            if (hours > 23) {
                hours = 0;
            }
        }
    }
}

void showMinutesFive() {
    rows[1] |= 0x66;
}

void showMinutesTen() {
    rows[0] |= 0x34;
}

void showMinutesFifteen() {
    rows[1] |= 0xde;
    rows[2] |= 0x01;
}

void showMinutesTwenty() {
    rows[0] |= 0xfc;
}

void showMinutesHalf() {
    rows[0] |= 0x03;
    rows[1] |= 0x03;
}

void showPast() {
    rows[2] |= 0x3c;
}

void showTil() {
    rows[2] |= 0xe0;
}

void showHourOne() {
    rows[3] |= 0x1c;
}

void showHourTwo() {
    rows[3] |= 0x07;
}

void showHourThree() {
    rows[3] |= 0x01;
    rows[4] |= 0x0f;
}

void showHourFour() {
    rows[4] |= 0xf0;
}

void showHourFive() {
    rows[6] |= 0x1e;
}

void showHourSix() {
    rows[3] |= 0xe0;
}

void showHourSeven() {
    rows[7] |= 0xf8;
}

void showHourEight() {
    rows[5] |= 0xf8;
}

void showHourNine() {
    rows[5] |= 0x0f;
}

void showHourTen() {
    rows[6] |= 0xe0;
}

void showHourEleven() {
    rows[7] |= 0xf6;
}

void showHourTwelve() {
    rows[6] |= 0x01;
    rows[7] |= 0x67;
}


void clearAll() {
    rows[0] = 0;
    rows[1] = 0;
    rows[2] = 0;
    rows[3] = 0;
    rows[4] = 0;
    rows[5] = 0;
    rows[6] = 0;
    rows[7] = 0;
}

void showTime() {
    clearAll();
    if (halfSecond) {
        rows[2] |= 2;
    }
    uint8_t hourIndex = hours;
    if (minutes < 5) {
    } else if (minutes < 10) {
        showMinutesFive();
        showPast();
    } else if (minutes < 15) {
        showMinutesTen();
        showPast();
    } else if (minutes < 20) {
        showMinutesFifteen();
        showPast();
    } else if (minutes < 25) {
        showMinutesTwenty();
        showPast();
    } else if (minutes < 30) {
        showMinutesTwenty();
        showMinutesFive();
        showPast();
    } else if (minutes < 35) {
        showMinutesHalf();
        showPast();
    } else {
        hourIndex++;
        if (hourIndex > 11) {
            hourIndex = 0;
        }
        
        if (minutes < 40) {
            showMinutesTwenty();
            showMinutesFive();
            showTil();
        } else if (minutes < 45) {
            showMinutesTwenty();
            showTil();
        } else if (minutes < 50) {
            showMinutesFifteen();
            showTil();
        } else if (minutes < 55) {
            showMinutesTen();
            showTil();
        } else {
            showMinutesFive();
            showTil();
        }
    }
    switch (hourIndex) {
        case 0:
            showHourTwelve();
            break;
        case 1:
            showHourOne();
            break;
        case 2:
            showHourTwo();
            break;
        case 3:
            showHourThree();
            break;
        case 4:
            showHourFour();
            break;
        case 5:
            showHourFive();
            break;
        case 6:
            showHourSix();
            break;
        case 7:
            showHourSeven();
            break;
        case 8:
            showHourEight();
            break;
        case 9:
            showHourNine();
            break;
        case 10:
            showHourTen();
            break;
        case 11:
            showHourEleven();
            break;
    }
}


int main()
{   
    /* Enable the global interrupt */
    CyGlobalIntEnable;
    
    /* Enable the Interrupt component connected to interrupt */
    isr_keypress_StartEx(KeyHandler);
    isr_lights_StartEx(InterruptHandler);

	/* Start the components */
    Timer_1_Start();
    debounce_clock_Start();
    
    showTime();
    while (1) {
        if (halfSecond == 0) {
            if (millis > 500) {
                halfSecond = 1;
                showTime();
            }
        } else {
            if (millis <= 500) {
                halfSecond = 0;
                showTime();
            }
        }
    }
    
    return 0;
}
