/**
 * @file bluetooth.h
 * @brief Header del driver para Bluetooth HC-05.
 *
 * Define las funciones para comunicación UART con el módulo Bluetooth,
 * incluyendo envío y recepción de datos, y parsing de comandos.
 * 
 * @author Equipo WALLY-S
 * @date 2025
 * @version 1.0
 */

#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "pico/stdlib.h"
#include <stdint.h>

/// @defgroup BT_FUNCTIONS Funciones de Bluetooth
/// @{

/**
 * @brief Inicializa la comunicación UART con el módulo Bluetooth.
 * 
 * Configura los pines UART, velocidad de comunicación y formato
 * para comunicarse con el módulo HC-05.
 * 
 * @return true si la inicialización fue exitosa
 */
bool bluetooth_init(void);

/**
 * @brief Envía una cadena de texto por Bluetooth.
 * 
 * @param str Cadena terminada en null a enviar
 * @return true si se envió correctamente
 */
bool bluetooth_send_string(const char* str);

/**
 * @brief Envía datos binarios por Bluetooth.
 * 
 * @param data Puntero a los datos a enviar
 * @param length Número de bytes a enviar
 * @return true si se envió correctamente
 */
bool bluetooth_send_data(const uint8_t* data, size_t length);

/**
 * @brief Verifica si hay datos disponibles para leer.
 * 
 * @return true si hay datos disponibles en el buffer UART
 */
bool bluetooth_available(void);

/**
 * @brief Lee una línea completa desde Bluetooth.
 * 
 * Lee caracteres hasta encontrar un salto de línea o alcanzar
 * el límite del buffer.
 * 
 * @param[out] buffer Buffer donde almacenar la línea leída
 * @param max_length Tamaño máximo del buffer
 * @return Número de caracteres leídos, 0 si no hay datos, -1 si error
 */
int bluetooth_read_line(char* buffer, int max_length);

/**
 * @brief Procesa un comando para extraer coordenadas.
 * 
 * Espera formato "latitud,longitud" y valida que sean coordenadas válidas.
 * 
 * @param command Comando recibido
 * @param[out] lat Puntero donde almacenar la latitud
 * @param[out] lng Puntero donde almacenar la longitud
 * @return true si las coordenadas son válidas
 */
bool bluetooth_parse_coordinates(const char* command, double* lat, double* lng);

/**
 * @brief Envía el estado actual del sistema por Bluetooth.
 * 
 * @param heading Rumbo actual en grados
 * @param target_heading Rumbo objetivo en grados
 * @param distance Distancia al objetivo en metros
 * @param gps_fix true si hay fix GPS válido
 */
void bluetooth_send_status(double heading, double target_heading, double distance, bool gps_fix);

/**
 * @brief Envía información de navegación completa.
 * 
 * @param current_lat Latitud actual
 * @param current_lng Longitud actual
 * @param target_lat Latitud objetivo
 * @param target_lng Longitud objetivo
 * @param distance Distancia al objetivo en metros
 * @param bearing Rumbo al objetivo en grados
 */
void bluetooth_send_navigation_info(double current_lat, double current_lng, 
                                   double target_lat, double target_lng, 
                                   double distance, double bearing);

/**
 * @brief Función de prueba independiente del Bluetooth.
 * 
 * Ejecuta comandos de prueba como encender/apagar LED y
 * procesar coordenadas enviadas desde la app móvil.
 */
void bluetooth_test(void);

/// @}

#endif // BLUETOOTH_H