/*
 * Actividad_3.c
 *
 * Created: 04/11/2024 02:05:57 p. m.
 * Author : uriee
 */ 

#define F_CPU 16000000UL // Frecuencia del microcontrolador (16 MHz)
#define BAUDRATE 9600    // Baud rate para la comunicación USART
#define UBRR_VALUE ((F_CPU / (BAUDRATE * 8UL)) - 1)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

// Variables globales
volatile char usart_received_char = 0; // Carácter recibido vía USART

// Prototipos de funciones
void init_adc();
uint16_t read_adc(uint8_t channel);
void init_usart(unsigned int baudrate);
void usart_transmit(unsigned int data);
void usart_transmit_string(char s[]);

// Función principal
int main(void)
{
	// Inicializar USART y ADC
	init_usart(UBRR_VALUE); // Iniciar comunicación a 9600 bps 
	init_adc();             // Iniciar el ADC
	sei();                  // interrupciones globales

	// Variables para almacenar el valor ADC y el voltaje
	uint16_t adc_value;
	char buffer[16];

	// Mensaje de inicio
	usart_transmit_string("Ready to receive commands:\r\nSend '1' for POT1 (A0) or '2' for POT2 (A1)\r\n");

	while (1)
	while (1)
	{
		// Procesar el comando recibido
		if (usart_received_char == '1') // Comando para leer POT1 (A0)
		{
			adc_value = read_adc(0); // Leer ADC de A0
			uint16_t voltage = (adc_value * 5000) / 1023; // Convertir a milivoltios (0-5000 mV)
			snprintf(buffer, sizeof(buffer), "POT1: %u.%02uV\r\n", voltage / 1000, (voltage % 1000) / 10); // Mostrar voltaje con dos decimales
			usart_transmit_string(buffer); // Enviar el voltaje vía Bluetooth
		}
		else if (usart_received_char == '2') // Comando para leer POT2 (A1)
		{
			adc_value = read_adc(1); // Leer ADC de A1
			uint16_t voltage = (adc_value * 5000) / 1023; // Convertir a milivoltios (0-5000 mV)
			snprintf(buffer, sizeof(buffer), "POT2: %u.%02uV\r\n", voltage / 1000, (voltage % 1000) / 10); // Mostrar voltaje con dos decimales
			usart_transmit_string(buffer); // Enviar el voltaje vía Bluetooth
		}
		usart_received_char = 0; // Reiniciar la variable para esperar un nuevo comando
		_delay_ms(100); // Retardo para evitar lecturas rápidas
	}
}
// Iniciar el ADC
void init_adc()
{
	ADMUX = (1 << REFS0); // Referencia de voltaje AVcc
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Habilitar ADC y prescaler de 128
}

// Leer el valor del ADC en un canal específico
uint16_t read_adc(uint8_t channel)
{
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F); // Seleccionar el canal
	ADCSRA |= (1 << ADSC); // Iniciar la conversión
	while (ADCSRA & (1 << ADSC)); // Esperar a que la conversión termine
	return ADC;
}

// Implementación de la rutina de interrupción del USART
ISR(USART_RX_vect)
{
	usart_received_char = UDR0; // Leer el carácter recibido
}

// Inicializar el USART
void init_usart(unsigned int baudrate)
{
	UCSR0C &= (~(1 << UMSEL00) & ~(1 << UMSEL01)); // Modo asíncrono
	UCSR0A = (1 << U2X0); // Doble velocidad para UART
	UBRR0H = (unsigned char)(baudrate >> 8);
	UBRR0L = (unsigned char)(baudrate);

	UCSR0C = ((1 << UCSZ00) | (1 << UCSZ01)); // Tamaño de datos: 8 bits
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0); // Habilitar RX, TX e interrupción RX
}

// Transmitir un solo byte vía USART
void usart_transmit(unsigned int data)
{
	while (!(UCSR0A & (1 << UDRE0))); // Esperar hasta que el buffer esté vacío
	UDR0 = data; // Enviar el dato
}

// Transmitir una cadena de caracteres vía USART
void usart_transmit_string(char s[])
{
	int i = 0;
	while (s[i] != '\0')
	{
		usart_transmit(s[i++]); // Enviar cada carácter de la cadena
	}
}


