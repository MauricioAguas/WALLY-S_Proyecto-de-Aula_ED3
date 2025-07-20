/**
 * @file config.h
 * @brief Configuración central del sistema WALLY-S para Raspberry Pi Pico.
 *
 * Este archivo contiene todas las definiciones de pines, constantes del sistema,
 * parámetros PID y configuraciones de comunicación para el robot autónomo.
 * 
 * @author Equipo WALLY-S
 * @date 2025
 * @version 1.0
 */

#ifndef CONFIG_H
#define CONFIG_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"

/// @defgroup SYSTEM_PINS Definición de pines del sistema
/// @{

/// @brief Pin del LED integrado para depuración visual
#define LED_PIN 25

/// @}

/// @defgroup I2C_CONFIG Configuración I2C para magnetómetro
/// @{

/// @brief Pin SDA para comunicación I2C
#define I2C_SDA_PIN 4
/// @brief Pin SCL para comunicación I2C  
#define I2C_SCL_PIN 5
/// @brief Puerto I2C utilizado
#define I2C_PORT i2c0
/// @brief Dirección I2C del magnetómetro QMC5883L
#define QMC5883L_ADDR 0x0D

/// @}

/// @defgroup GPS_CONFIG Configuración UART para GPS NEO-6M
/// @{

/// @brief UART utilizada para GPS
#define GPS_UART_ID uart0
/// @brief Pin TX para GPS
#define GPS_TX_PIN 0
/// @brief Pin RX para GPS
#define GPS_RX_PIN 1
/// @brief Velocidad de comunicación GPS
#define GPS_BAUD_RATE 9600

/// @}

/// @defgroup BT_CONFIG Configuración UART para Bluetooth HC-05
/// @{

/// @brief UART utilizada para Bluetooth
#define BT_UART_ID uart1
/// @brief Pin TX para Bluetooth
#define BT_TX_PIN 8
/// @brief Pin RX para Bluetooth
#define BT_RX_PIN 9
/// @brief Velocidad de comunicación Bluetooth
#define BT_BAUD_RATE 9600

/// @}

/// @defgroup MOTOR_CONFIG Configuración de motores L298N
/// @{

/// @brief Pin Enable A (PWM Motor A)
#define MOTOR_ENA_PIN 6
/// @brief Pin Enable B (PWM Motor B)
#define MOTOR_ENB_PIN 7
/// @brief Pin Input 1 (Dirección Motor A)
#define MOTOR_IN1_PIN 10
/// @brief Pin Input 2 (Dirección Motor A)
#define MOTOR_IN2_PIN 11
/// @brief Pin Input 3 (Dirección Motor B)
#define MOTOR_IN3_PIN 12
/// @brief Pin Input 4 (Dirección Motor B)
#define MOTOR_IN4_PIN 13

/// @}

/// @defgroup ENCODER_CONFIG Configuración de encoders
/// @{

/// @brief Pin encoder A (rueda izquierda)
#define ENCODER_A_PIN 2
/// @brief Pin encoder B (rueda derecha)
#define ENCODER_B_PIN 3

/// @}

/// @defgroup PID_PARAMS Parámetros PID del sistema
/// @{

/// @brief Ganancia proporcional para control de velocidad RPM
#define KP_RPM 3.0
/// @brief Ganancia integral para control de velocidad RPM
#define KI_RPM 0.2
/// @brief Ganancia derivativa para control de velocidad RPM
#define KD_RPM 0.1

/// @brief Ganancia proporcional para control de dirección
#define KP_DIR 2.0
/// @brief Ganancia integral para control de dirección
#define KI_DIR 0.1
/// @brief Ganancia derivativa para control de dirección
#define KD_DIR 0.2

/// @}

/// @defgroup SYSTEM_CONSTANTS Constantes del sistema
/// @{

/// @brief Intervalo del bucle principal en milisegundos
#define LOOP_INTERVAL_MS 50
/// @brief Pulsos por revolución del encoder
#define PULSES_PER_REV 20
/// @brief Tolerancia de rumbo en grados
#define HEADING_TOLERANCE 2.0
/// @brief Umbral de corrección extrema en grados
#define EXTREME_CORRECTION_THRESHOLD 15.0

/// @}

/// @defgroup MOTOR_SPEEDS Velocidades de los motores
/// @{

/// @brief Velocidad base del motor A
#define BASE_SPEED_A 180
/// @brief Velocidad base del motor B
#define BASE_SPEED_B 190
/// @brief Velocidad mínima de los motores
#define MIN_SPEED 30
/// @brief Velocidad máxima de los motores
#define MAX_SPEED 255

/// @}

#endif // CONFIG_H