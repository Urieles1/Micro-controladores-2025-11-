/*
 * Final.c
 *
 * Created: 04/12/2024 04:38:03 p. m.
 * Author : uriee
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#define RTC_ADDRESS 0x68
#define LCD_I2C_ADDRESS 0x3F

// Inicialización del I2C
void I2C_Init(void) {
	TWSR = 0x00; // Prescaler en 1
	TWBR = 100;   // Configura TWBR para 100 kHz
	TWCR = (1 << TWEN); // Habilitar TWI
}

// Enviar Start
void I2C_Start(void) {
	TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT); // Inicia condición START
	while (!(TWCR & (1 << TWINT))); // Espera a que se complete
}

// Enviar Stop
void I2C_Stop(void) {
	TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT); // Condición STOP
	while (TWCR & (1 << TWSTO)); // Espera a que se complete
}

// Enviar un byte
void I2C_Write(uint8_t data) {
	TWDR = data; // Carga el dato
	TWCR = (1 << TWEN) | (1 << TWINT); // Enviar
	while (!(TWCR & (1 << TWINT))); // Espera a que termine
}

// Leer un byte con ACK o NACK
uint8_t I2C_Read(uint8_t ack) {
	TWCR = (1 << TWEN) | (1 << TWINT) | (ack << TWEA); // ACK o NACK
	while (!(TWCR & (1 << TWINT))); // Espera a que termine
	return TWDR; // Devuelve el dato recibido
}

// Leer un registro del RTC
uint8_t RTC_ReadRegister(uint8_t reg) {
	I2C_Start();
	I2C_Write((RTC_ADDRESS << 1) | 0); // Dirección del DS3231 en modo escritura
	I2C_Write(reg); // Registro que queremos leer
	I2C_Start(); // Restart
	I2C_Write((RTC_ADDRESS << 1) | 1); // Dirección en modo lectura
	uint8_t data = I2C_Read(0); // Leer con NACK
	I2C_Stop();
	return data;
}

// Leer tiempo del RTC
void RTC_GetTime(uint8_t* seconds, uint8_t* minutes, uint8_t* hours) {
	*seconds = RTC_ReadRegister(0x00); // Segundos
	*minutes = RTC_ReadRegister(0x01); // Minutos
	*hours = RTC_ReadRegister(0x02); // Horas
}

// Enviar un nibble de datos al LCD
void LCD_I2C_SendNibble(uint8_t data, uint8_t control) {
	uint8_t payload = (data & 0xF0) | control;
	I2C_Start();
	I2C_Write((LCD_I2C_ADDRESS << 1) | 0);
	I2C_Write(payload | 0x04); // EN = 1
	_delay_us(1);
	I2C_Write(payload & ~0x04); // EN = 0
	_delay_us(50);
	I2C_Stop();
}

// Enviar un comando al LCD
void LCD_I2C_Command(uint8_t cmd) {
	LCD_I2C_SendNibble(cmd & 0xF0, 0x00);
	LCD_I2C_SendNibble((cmd << 4), 0x00);
	_delay_ms(2);
}

// Enviar datos al LCD
void LCD_I2C_Data(uint8_t data) {
	LCD_I2C_SendNibble(data & 0xF0, 0x01);
	LCD_I2C_SendNibble((data << 4), 0x01);
	_delay_ms(2);
}

// Inicializar LCD
void LCD_I2C_Init(void) {
	_delay_ms(50);
	LCD_I2C_Command(0x30);
	_delay_ms(5);
	LCD_I2C_Command(0x30);
	_delay_us(100);
	LCD_I2C_Command(0x30);
	LCD_I2C_Command(0x20);
	LCD_I2C_Command(0x28);
	LCD_I2C_Command(0x08);
	LCD_I2C_Command(0x01);
	_delay_ms(2);
	LCD_I2C_Command(0x06);
	LCD_I2C_Command(0x0C);
}

// Imprimir en LCD
void LCD_I2C_Print(char* str) {
	while (*str) {
		LCD_I2C_Data(*str++);
	}
}

int main(void) {
	uint8_t seconds, minutes, hours;
	char buffer[16];

	I2C_Init();
	LCD_I2C_Init();

	while (1) {
		RTC_GetTime(&seconds, &minutes, &hours);
		sprintf(buffer, "%02d:%02d:%02d", hours, minutes, seconds);
		LCD_I2C_Command(0x80); // Mover a primera línea
		LCD_I2C_Print(buffer);
		_delay_ms(1000);
	}
	return 0;
}


