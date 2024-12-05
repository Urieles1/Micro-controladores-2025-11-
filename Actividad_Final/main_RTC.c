/*
 * Actividad_Final.c
 *
 * Created: 28/11/2024 07:20:38 p. m.
 * Author : uriee
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>

#define F_CPU 16000000UL  // Definir la frecuencia de la CPU para delay

// Definir direcciones I2C
#define RTC_ADDR 0xD0  // Dirección del DS3231 en I2C (Escribir)
#define LCD_ADDR 0x27  // Dirección del LCD en I2C (Asegúrate de que sea la correcta para tu LCD)

void I2C_Init(void) {
	// Inicializar I2C (TWI) a 100kHz
	TWBR = 32;  // Configura el baud rate del bus I2C
	TWSR = 0;   // Preescalador a 1
}

void I2C_Start(uint8_t address) {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);  // Generar start condition
	while (!(TWCR & (1 << TWINT)));  // Esperar hasta que se complete la transmisión
	TWDR = address;  // Cargar la dirección del esclavo
	TWCR = (1 << TWINT) | (1 << TWEN);  // Iniciar la transmisión
	while (!(TWCR & (1 << TWINT)));  // Esperar hasta que se complete
}

void I2C_Stop(void) {
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);  // Generar stop condition
	while (TWCR & (1 << TWSTO));  // Esperar hasta que se complete el stop
}

void I2C_Write(uint8_t data) {
	TWDR = data;  // Cargar dato a enviar
	TWCR = (1 << TWINT) | (1 << TWEN);  // Iniciar la transmisión
	while (!(TWCR & (1 << TWINT)));  // Esperar hasta que se complete
}

uint8_t I2C_ReadAck(void) {
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);  // Enviar ACK después de leer
	while (!(TWCR & (1 << TWINT)));  // Esperar hasta que se complete
	return TWDR;  // Retornar dato recibido
}

uint8_t I2C_ReadNack(void) {
	TWCR = (1 << TWINT) | (1 << TWEN);  // Enviar NACK después de leer
	while (!(TWCR & (1 << TWINT)));  // Esperar hasta que se complete
	return TWDR;  // Retornar dato recibido
}

// Función para leer la hora y fecha del RTC
void RTC_ReadTime(uint8_t* hour, uint8_t* minute, uint8_t* second) {
	I2C_Start(RTC_ADDR);  // Iniciar comunicación con el RTC
	I2C_Write(0x00);  // Dirección del registro de segundos
	I2C_Start(RTC_ADDR + 1);  // Repetir inicio para leer
	*second = I2C_ReadAck();
	*minute = I2C_ReadAck();
	*hour = I2C_ReadNack();
	I2C_Stop();  // Detener la comunicación
}

// Función para mostrar los datos en el LCD
void LCD_SendCommand(uint8_t command) {
	I2C_Start(LCD_ADDR);
	I2C_Write(0x00);  // Comando para el LCD
	I2C_Write(command);
	I2C_Stop();
}

void LCD_SendData(uint8_t data) {
	I2C_Start(LCD_ADDR);
	I2C_Write(0x40);  // Datos para el LCD
	I2C_Write(data);
	I2C_Stop();
}

void LCD_Init(void) {
	_delay_ms(20);  // Esperar 20ms
	LCD_SendCommand(0x38);  // Configuración en modo de 8 bits
	LCD_SendCommand(0x0C);  // Encender pantalla y cursor
	LCD_SendCommand(0x01);  // Limpiar pantalla
	LCD_SendCommand(0x06);  // Modo de incremento
}

void LCD_Print(char* str) {
	while (*str) {
		LCD_SendData(*str++);
	}
}

int main(void) {
	I2C_Init();  // Inicializar I2C
	LCD_Init();  // Inicializar LCD

	uint8_t hour, minute, second;

	while (1) {
		RTC_ReadTime(&hour, &minute, &second);  // Leer hora del RTC

		char buffer[20];
		snprintf(buffer, sizeof(buffer), "Time: %02d:%02d:%02d", hour, minute, second);

		LCD_SendCommand(0x01);  // Limpiar pantalla
		LCD_Print(buffer);  // Mostrar hora en el LCD

		_delay_ms(1000);  // Esperar 1 segundo antes de actualizar
	}

	return 0;
}


