/**
 * @file gps.h
 * @brief Header del driver para GPS NEO-6M.
 *
 * Define las estructuras de datos y funciones públicas para manejar
 * la comunicación UART con el módulo GPS y procesar sentencias NMEA.
 * 
 * @author Equipo WALLY-S
 * @date 2025
 * @version 1.0
 */

#ifndef GPS_H
#define GPS_H

#include "pico/stdlib.h"
#include <stdint.h>

/// @defgroup GPS_STRUCTURES Estructuras de datos GPS
/// @{

/**
 * @brief Estructura que contiene los datos actuales del GPS.
 */
typedef struct {
    double latitude;     ///< Latitud en grados decimales
    double longitude;    ///< Longitud en grados decimales  
    double altitude;     ///< Altitud en metros
    int satellites;      ///< Número de satélites en uso
    int fix_quality;     ///< Calidad del fix (0=sin fix, 1=GPS, 2=DGPS)
    bool fix_valid;      ///< true si el fix GPS es válido
    char time[8];        ///< Tiempo UTC en formato HHMMSS
} gps_data_t;

/**
 * @brief Estructura que contiene las coordenadas objetivo.
 */
typedef struct {
    double latitude;     ///< Latitud objetivo en grados decimales
    double longitude;    ///< Longitud objetivo en grados decimales
    bool target_set;     ///< true si se ha establecido un objetivo
} target_data_t;

/// @}

/// @defgroup GPS_FUNCTIONS Funciones del GPS
/// @{

/**
 * @brief Inicializa la comunicación UART con el módulo GPS.
 * 
 * Configura los pines UART, velocidad de comunicación y formato.
 * 
 * @return true si la inicialización fue exitosa
 */
bool gps_init(void);

/**
 * @brief Actualiza los datos GPS leyendo desde UART.
 * 
 * Lee los datos disponibles en el buffer UART y procesa las
 * sentencias NMEA GPGGA para actualizar la información de posición.
 * 
 * @return true si se procesaron datos correctamente
 */
bool gps_update(void);

/**
 * @brief Obtiene la estructura con los datos GPS actuales.
 * 
 * @return Estructura gps_data_t con los datos más recientes
 */
gps_data_t gps_get_data(void);

/**
 * @brief Establece las coordenadas objetivo para navegación.
 * 
 * @param lat Latitud objetivo en grados decimales
 * @param lng Longitud objetivo en grados decimales
 */
void gps_set_target(double lat, double lng);

/**
 * @brief Verifica si hay un objetivo establecido.
 * 
 * @return true si hay coordenadas objetivo configuradas
 */
bool gps_has_target(void);

/**
 * @brief Calcula la distancia al objetivo usando fórmula haversine.
 * 
 * @return Distancia al objetivo en metros
 */
double gps_distance_to_target(void);

/**
 * @brief Calcula el rumbo al objetivo.
 * 
 * @return Rumbo al objetivo en grados (0-360)
 */
double gps_bearing_to_target(void);

/**
 * @brief Verifica si se ha alcanzado el objetivo.
 * 
 * @return true si la distancia al objetivo es menor a 2 metros
 */
bool gps_target_reached(void);

/**
 * @brief Función de prueba independiente del GPS.
 * 
 * Ejecuta un bucle que muestra continuamente los datos GPS
 * hasta obtener un fix válido.
 */
void gps_test(void);

/// @}

#endif // GPS_H