;
; Actividad_2.asm
;
; Created: 21/11/2024 00:26:51 a. m.
; Author : uriee
; ATmega328P

.include "m328Pdef.inc"       ; Definiciones del microcontrolador ATmega328P

.equ F_CPU = 16000000         ; Frecuencia del cristal en Hz

; --- Segmento de código ---
.cseg
.org 0x00                     ; Vector de reset (dirección 0x00)
    rjmp RESET                ; Saltar a la rutina de inicio

; --- Inicio del programa ---
RESET:
    ; Configuración del stack
    ldi r16, HIGH(RAMEND)     ; Configurar la parte alta del stack
    out SPH, r16
    ldi r16, LOW(RAMEND)      ; Configurar la parte baja del stack
    out SPL, r16

    ; Configuración del puerto B
    ldi r16, 0b00000010       ; PB1 como salida (OC1A)
    out DDRB, r16

    ; Configuración del Timer1 en modo CTC
   ldi r16, 0b00001000       ; WGM12 = 1 (modo CTC)
   sts TCCR1B, r16           ; Configurar registro TCCR1B (STS para registros de memoria alta)
   ldi r16, 0b00000001       ; Sin prescaler (CS10 = 1)
   sts TCCR1B, r16           ; Iniciar el Timer1

    rjmp MAIN                 ; Saltar al programa principal

; --- Programa principal ---
MAIN:
    rcall SELECT_FREQ         ; Llamar a la subrutina de selección de frecuencia
    rjmp MAIN                 ; Repetir continuamente

; --- Subrutinas ---
SELECT_FREQ:
    in r16, PINC              ; Leer selector (entrada en PINC)
    andi r16, 0x07            ; Limitar a 3 bits (valores de 0 a 7)

    ; Acceder a la tabla de valores precalculados
    ldi ZH, HIGH(OCR_VALUES)  ; Cargar dirección alta de la tabla
    ldi ZL, LOW(OCR_VALUES)   ; Cargar dirección baja de la tabla
    add ZL, r16               ; Mover el índice a la tabla
    adc ZH, r1                ; Ajustar si hay desbordamiento (usa r1 como 0)

    lpm r17, Z                ; Cargar el valor OCR1A desde la tabla
    sts OCR1AH, r1            ; Parte alta de OCR1A (usa r1 como 0)
    sts OCR1AL, r17           ; Parte baja de OCR1A (valor de la tabla)
    ret                       ; Regresar al programa principal

; --- Tabla de valores precalculados ---
.cseg
.org 0x0100                   ; Dirección para la tabla de valores
OCR_VALUES:
    .db 79, 52, 39, 31, 25, 22, 19, 17  ; Valores de OCR1A para frecuencias

