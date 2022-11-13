/*
 *   File:   iopins.h
 *   Author: Matt
 *
 *   Created on 31st July 2020, 07:00
 * 
 *   This is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 2 of the License, or
 *   (at your option) any later version.
 *   This software is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *   You should have received a copy of the GNU General Public License
 *   along with this software.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef __IOPINS_H__
#define __IOPINS_H__

#define IO_INPUT(pin) pin##_DDR &= ~_BV(pin)
#define IO_OUTPUT(pin) pin##_DDR |= _BV(pin)

#define IO_HIGH(pin) pin##_PORT |= _BV(pin)
#define IO_LOW(pin) pin##_PORT &= ~_BV(pin)

#define IO_IN_HIGH(pin) (pin##_PIN & _BV(pin) == _BV(pin))
#define IO_IN_LOW(pin) (pin##_PIN & _BV(pin) == 0x00)

// A neat trick to toggle a pin in a single clock cycle
#define IO_TOGGLE(pin) (pin##_PIN = _BV(pin))

#ifdef _UNO_

#define IO0                PD0
#define IO1                PD1
#define IO2                PD2
#define IO3                PD3
#define IO4                PD4
#define IO5                PD5
#define IO6                PD6
#define IO7                PD7
#define IO8                PB0
#define IO9                PB1
#define IO10               PB2
#define IO11               PB3
#define IO12               PB4
#define IO13               PB5

#define IO0_PIN            PIND
#define IO1_PIN            PIND
#define IO2_PIN            PIND
#define IO3_PIN            PIND
#define IO4_PIN            PIND
#define IO5_PIN            PIND
#define IO6_PIN            PIND
#define IO7_PIN            PIND
#define IO8_PIN            PINB
#define IO9_PIN            PINB
#define IO10_PIN           PINB
#define IO11_PIN           PINB
#define IO12_PIN           PINB
#define IO13_PIN           PINB

#define IO0_PORT           PORTD
#define IO1_PORT           PORTD
#define IO2_PORT           PORTD
#define IO3_PORT           PORTD
#define IO4_PORT           PORTD
#define IO5_PORT           PORTD
#define IO6_PORT           PORTD
#define IO7_PORT           PORTD
#define IO8_PORT           PORTB
#define IO9_PORT           PORTB
#define IO10_PORT          PORTB
#define IO11_PORT          PORTB
#define IO12_PORT          PORTB
#define IO13_PORT          PORTB

#define IO0_DDR            DDRD
#define IO1_DDR            DDRD
#define IO2_DDR            DDRD
#define IO3_DDR            DDRD
#define IO4_DDR            DDRD
#define IO5_DDR            DDRD
#define IO6_DDR            DDRD
#define IO7_DDR            DDRD
#define IO8_DDR            DDRB
#define IO9_DDR            DDRB
#define IO10_DDR           DDRB
#define IO11_DDR           DDRB
#define IO12_DDR           DDRB
#define IO13_DDR           DDRB

#define SDA_DDR            PORTC
#define SDA_PIN            PINC
#define SDA_PORT           PORTC
#define SDA                PC4

#define SCL_DDR            PORTC
#define SCL_PIN            PINC
#define SCL_PORT           PORTC
#define SCL                PC5

#define USART1_DDR         DDRD
#define USART1_TX          PD1
#define USART1_RX          PD0
#define USART1_XCK         PD4

#define SPI_DDR            DDRB
#define SPI_PORT           PORTB
#define SPI_MISO           PB4
#define SPI_MOSI           PB2
#define SPI_SCK            PB5

// This is a pain in the ass. Every AVR I try and use has a completely different set of defines for the first USART.

#ifdef __AVR_ATmega8__

#define UCSRAA             UCSRA
#define UCSRAB             UCSRB
#define UCSRAC             UCSRC
#define UBRRAL             UBRRL
#define UBRRAH             UBRRH
#define UDRIEA             UDRIE
#define UCSZA0             UCSZ0
#define UCSZA1             UCSZ1
#define UCSZA2             UCSZ2
#define RXCIEA             RXCIE
#define RXENA              RXEN
#define TXENA              TXEN
#define UMSELA0            UMSEL
#define UDREA              UDRE
#define UDRA               UDR
#define DORA               DOR
#define FEA                FE

#define USARTA_RX_vect     USART_RXC_vect
#define USARTA_UDRE_vect   USART_UDRE_vect

#endif /* __AVR_ATmega8__ */

#ifdef __AVR_ATmega328P__

#define UCSRAA             UCSR0A
#define UCSRAB             UCSR0B
#define UCSRAC             UCSR0C
#define UBRRAL             UBRR0L
#define UBRRAH             UBRR0H
#define UDRIEA             UDRIE0
#define UCSZA0             UCSZ00
#define UCSZA1             UCSZ01
#define UCSZA2             UCSZ02
#define RXCIEA             RXCIE0
#define RXENA              RXEN0
#define TXENA              TXEN0
#define UMSELA0            UMSEL00
#define UDREA              UDRE0
#define UDRA               UDR0
#define DORA               DOR0
#define FEA                FE0

#define USARTA_RX_vect     USART_RX_vect
#define USARTA_UDRE_vect   USART_UDRE_vect

#endif /* __AVR_ATmega328P__ */

#endif

#ifdef _LEONARDO_

#define IO0                PD2
#define IO1                PD3
#define IO2                PD1
#define IO3                PD0
#define IO4                PD4
#define IO5                PC6
#define IO6                PD7
#define IO7                PE6
#define IO8                PB4
#define IO9                PB5
#define IO10               PB6
#define IO11               PB7
#define IO12               PD6
#define IO13               PC7

#define IO0_PIN            PIND
#define IO1_PIN            PIND
#define IO2_PIN            PIND
#define IO3_PIN            PIND
#define IO4_PIN            PIND
#define IO5_PIN            PINC
#define IO6_PIN            PIND
#define IO7_PIN            PINE
#define IO8_PIN            PINB
#define IO9_PIN            PINB
#define IO10_PIN           PINB
#define IO11_PIN           PINB
#define IO12_PIN           PIND
#define IO13_PIN           PINC

#define IO0_PORT           PORTD
#define IO1_PORT           PORTD
#define IO2_PORT           PORTD
#define IO3_PORT           PORTD
#define IO4_PORT           PORTD
#define IO5_PORT           PORTC
#define IO6_PORT           PORTD
#define IO7_PORT           PORTE
#define IO8_PORT           PORTB
#define IO9_PORT           PORTB
#define IO10_PORT          PORTB
#define IO11_PORT          PORTB
#define IO12_PORT          PORTD
#define IO13_PORT          PORTC

#define IO0_DDR            DDRD
#define IO1_DDR            DDRD
#define IO2_DDR            DDRD
#define IO3_DDR            DDRD
#define IO4_DDR            DDRD
#define IO5_DDR            DDRC
#define IO6_DDR            DDRD
#define IO7_DDR            DDRE
#define IO8_DDR            DDRB
#define IO9_DDR            DDRB
#define IO10_DDR           DDRB
#define IO11_DDR           DDRB
#define IO12_DDR           DDRD
#define IO13_DDR           DDRC

#define SDA_DDR            PORTD
#define SDA_PIN            PIND
#define SDA_PORT           PORTD
#define SDA                PD1

#define SCL_DDR            PORTD
#define SCL_PIN            PIND
#define SCL_PORT           PORTD
#define SCL                PD0

#define USART1_DDR         DDRD
#define USART1_TX          PD3
#define USART1_RX          PD2
#define USART1_XCK         PD5

#define SPI_DDR            DDRB
#define SPI_PORT           PORTB
#define SPI_MISO           PB3
#define SPI_MOSI           PB2
#define SPI_SCK            PB1

#define UCSRAA             UCSR1A
#define UCSRAB             UCSR1B
#define UCSRAC             UCSR1C
#define UBRRAL             UBRR1L
#define UBRRAH             UBRR1H
#define UDRIEA             UDRIE1
#define UCSZA0             UCSZ10
#define UCSZA1             UCSZ11
#define UCSZA2             UCSZ12
#define RXCIEA             RXCIE1
#define RXENA              RXEN1
#define TXENA              TXEN1
#define UMSELA0            UMSEL10
#define UDREA              UDRE1
#define UDRA               UDR1
#define DORA               DOR1
#define FEA                FE1

#define USARTA_RX_vect     USART1_RX_vect
#define USARTA_UDRE_vect   USART1_UDRE_vect

#endif

#if defined(_UNO_) || defined(_LEONARDO_)

#define ENA_DDR   IO3_DDR
#define ENA_PORT  IO3_PORT
#define ENA       IO3

#define ENB_DDR   IO11_DDR
#define ENB_PORT  IO11_PORT
#define ENB       IO11

#define DIRA_DDR  IO12_DDR
#define DIRA_PORT IO12_PORT
#define DIRA      IO12

#define DIRB_DDR  IO13_DDR
#define DIRB_PORT IO13_PORT
#define DIRB      IO13

#endif /* defined(_UNO_) || defined(_LEONARDO_) */

#ifdef _AVR_FANSPEED_BOARD_

#define FHDR_SPARE1        PC2
#define FHDR_SPARE2        PC3
#define PWM0A              PD6
#define PWM0B              PD5
#define PWM1A              PB1
#define PWM1B              PB2
#define DIRA               PD7
#define DIRB               PD4
#define ONEWIRE            PC1
#define START              PB0 //PCINT0
#define SP2                PD5 //PCINT21
#define SP3                PD6 //PCINT22
#define BOTTOM             PC0 //PCINT8
#define TOP                PD3 //PCINT19
#define SP6                PD2 //PCINT18

#define FHDR_SPARE1_PIN    PINC
#define FHDR_SPARE2_PIN    PINC
#define PWM0A_PIN          PIND
#define PWM0B_PIN          PIND
#define PWM1A_PIN          PINB
#define PWM1B_PIN          PINB
#define DIRA_PIN           PIND
#define DIRB_PIN           PIND
#define ONEWIRE_PIN        PINC
#define START_PIN          PINB
#define SP2_PIN            PIND
#define SP3_PIN            PIND
#define BOTTOM_PIN         PINC
#define TOP_PIN            PIND
#define SP6_PIN            PIND

#define FHDR_SPARE1_PORT   PORTC
#define FHDR_SPARE2_PORT   PORTC
#define PWM0A_PORT         PORTD
#define PWM0B_PORT         PORTD
#define PWM1A_PORT         PORTB
#define PWM1B_PORT         PORTB
#define DIRA_PORT          PORTD
#define DIRB_PORT          PORTD
#define ONEWIRE_PORT       PORTC
#define START_PORT         PORTB
#define SP2_PORT           PORTD
#define SP3_PORT           PORTD
#define BOTTOM_PORT        PORTC
#define TOP_PORT           PORTD
#define SP6_PORT           PORTD

#define FHDR_SPARE1_DDR    DDRC
#define FHDR_SPARE2_DDR    DDRC
#define PWM0A_DDR          DDRD
#define PWM0B_DDR          DDRD
#define PWM1A_DDR          DDRB
#define PWM1B_DDR          DDRB
#define DIRA_DDR           DDRD
#define DIRB_DDR           DDRD
#define ONEWIRE_DDR        DDRC
#define START_DDR          DDRB
#define SP2_DDR            DDRD
#define SP3_DDR            DDRD
#define BOTTOM_DDR         DDRC
#define TOP_DDR            DDRD
#define SP6_DDR            DDRD

#define SDA_DDR            PORTC
#define SDA_PIN            PINC
#define SDA_PORT           PORTC
#define SDA                PC4

#define SCL_DDR            PORTC
#define SCL_PIN            PINC
#define SCL_PORT           PORTC
#define SCL                PC5

#define USART1_DDR         DDRD
#define USART1_TX          PD1
#define USART1_RX          PD0
#define USART1_XCK         PD4

#define SPI_DDR            DDRB
#define SPI_PORT           PORTB
#define SPI_MISO           PB4
#define SPI_MOSI           PB2
#define SPI_SCK            PB5

#if defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)

#define UCSRAA             UCSR0A
#define UCSRAB             UCSR0B
#define UCSRAC             UCSR0C
#define UBRRAL             UBRR0L
#define UBRRAH             UBRR0H
#define UDRIEA             UDRIE0
#define UCSZA0             UCSZ00
#define UCSZA1             UCSZ01
#define UCSZA2             UCSZ02
#define RXCIEA             RXCIE0
#define RXENA              RXEN0
#define TXENA              TXEN0
#define UMSELA0            UMSEL00
#define UDREA              UDRE0
#define UDRA               UDR0
#define DORA               DOR0
#define FEA                FE0

#define USARTA_RX_vect     USART_RX_vect
#define USARTA_UDRE_vect   USART_UDRE_vect

#endif /* __AVR_ATmega328P__ */

#endif /* _AVR_FANSPEED_BOARD_ */

#endif /* __IOPINS_H__ */