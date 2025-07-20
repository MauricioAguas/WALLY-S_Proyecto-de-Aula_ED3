/**
 * @file magnetometer.h
 * @brief Header del driver para magnetómetro QMC5883L.
 *
 * Define las funciones públicas para inicializar, leer y procesar datos
 * del magnetómetro QMC5883L conectado por I2C.
 * 
 * @author Equipo WALLY-S
 * @date 2025
 * @version 1.0
 */

#ifndef MAGNETOMETER_H
#define MAGNETOMETER_H

#include "pico/stdlib.h"
#include <stdint.h>

/// @defgroup MAG_FUNCTIONS Funciones del magnetómetro
/// @{

/**
 * @brief Inicializa la interfaz I2C y configura el magnetómetro QMC5883L.
 * 
 * Configura los pines I2C, establece la velocidad de comunicación a 400kHz
 * y envía los comandos de configuración al sensor.
 * 
 * @return true si la inicialización fue exitosa, false en caso contrario
 */
bool magnetometer_init(void);

/**
 * @brief Lee los valores crudos del magnetómetro.
 * 
 * @param[out] x Puntero donde almacenar el valor X
 * @param[out] y Puntero donde almacenar el valor Y  
 * @param[out] z Puntero donde almacenar el valor Z
 * @return true si la lectura fue exitosa, false en caso contrario
 */
bool magnetometer_read_raw(int16_t *x, int16_t *y, int16_t *z);

/**
 * @brief Calcula el rumbo magnético en grados.
 * 
 * @param x Componente X del campo magnético
 * @param y Componente Y del campo magnético
 * @return Rumbo en grados (0-360)
 */
double magnetometer_calculate_heading(int16_t x, int16_t y);

/**
 * @brief Obtiene el rumbo filtrado para reducir ruido.
 * 
 * Aplica un filtro paso bajo para estabilizar las lecturas y maneja
 * correctamente el cruce de 0/360 grados.
 * 
 * @return Rumbo filtrado en grados (0-360)
 */
double magnetometer_get_filtered_heading(void);

/**
 * @brief Verifica si el magnetómetro está inicializado.
 * 
 * @return true si está inicializado, false en caso contrario
 */
bool magnetometer_is_initialized(void);

/**
 * @brief Establece la declinación magnética local.
 * 
 * @param declination_rad Declinación magnética en radianes
 */
void magnetometer_set_declination(double declination_rad);

/**
 * @brief Función de prueba independiente del magnetómetro.
 * 
 * Ejecuta un bucle de prueba que muestra continuamente los valores
 * leídos y el rumbo calculado.
 */
void magnetometer_test(void);

/// @}

#endif // MAGNETOMETER_H