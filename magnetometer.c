/**
 * @file magnetometer.c
 * @brief Implementación del driver para magnetómetro QMC5883L.
 *
 * Este módulo maneja toda la comunicación I2C con el magnetómetro,
 * incluyendo inicialización, lectura de datos y cálculo de rumbo
 * con filtrado paso bajo para estabilidad.
 * 
 * @author Equipo WALLY-S
 * @date 2025
 * @version 1.0
 */

#include "magnetometer.h"
#include "config.h"
#include <math.h>

/// @brief Estado de inicialización del magnetómetro
static bool initialized = false;

/// @brief Último rumbo filtrado calculado
static double filtered_heading = 0.0;

/// @brief Constante del filtro paso bajo (0.0 - 1.0)
static const double ALPHA = 0.6;

/// @brief Declinación magnética local en radianes
static double declination_angle = 0.0404;

bool magnetometer_init(void) {
    // Inicializar I2C a 400kHz
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
    
    // Configurar QMC5883L: modo continuo, 200Hz, 8G, 512 OSR
    uint8_t config_data[] = {0x09, 0x1D};
    int result = i2c_write_blocking(I2C_PORT, QMC5883L_ADDR, config_data, 2, false);
    
    if (result == PICO_ERROR_GENERIC) {
        return false;
    }
    
    // Configurar período SET/RESET
    uint8_t period_data[] = {0x0B, 0x01};
    i2c_write_blocking(I2C_PORT, QMC5883L_ADDR, period_data, 2, false);
    
    initialized = true;
    return true;
}

bool magnetometer_read_raw(int16_t *x, int16_t *y, int16_t *z) {
    if (!initialized) return false;
    
    uint8_t buffer[6];
    uint8_t reg = 0x00; // Registro X LSB
    
    // Leer 6 bytes desde X LSB
    i2c_write_blocking(I2C_PORT, QMC5883L_ADDR, &reg, 1, true);
    int result = i2c_read_blocking(I2C_PORT, QMC5883L_ADDR, buffer, 6, false);
    
    if (result == PICO_ERROR_GENERIC) {
        return false;
    }
    
    // Ensamblar datos (little endian)
    *x = (int16_t)(buffer[1] << 8 | buffer[0]);
    *y = (int16_t)(buffer[3] << 8 | buffer[2]);
    *z = (int16_t)(buffer[5] << 8 | buffer[4]);
    
    return true;
}

double magnetometer_calculate_heading(int16_t x, int16_t y) {
    // Calcular rumbo usando atan2
    double heading = atan2(y, x) * 180.0 ;
    
    // Ajustar declinación magnética
    heading += declination_angle * 180.0 ;
    
    // Normalizar a 0-360 grados
    if (heading < 0) {
        heading += 360;
    } else if (heading >= 360) {
        heading -= 360;
    }
    
    return heading;
}

double magnetometer_get_filtered_heading(void) {
    int16_t x, y, z;
    if (!magnetometer_read_raw(&x, &y, &z)) {
        return filtered_heading; // Retornar último valor válido
    }
    
    double new_heading = magnetometer_calculate_heading(x, y);
    
    // Aplicar filtro paso bajo
    double difference = new_heading - filtered_heading;
    
    // Manejar cruce de 0/360 grados
    if (difference > 180) {
        difference -= 360;
    } else if (difference < -180) {
        difference += 360;
    }
    
    filtered_heading += ALPHA * difference;
    
    // Normalizar resultado
    if (filtered_heading < 0) {
        filtered_heading += 360;
    } else if (filtered_heading >= 360) {
        filtered_heading -= 360;
    }
    
    return filtered_heading;
}

bool magnetometer_is_initialized(void) {
    return initialized;
}

void magnetometer_set_declination(double declination_rad) {
    declination_angle = declination_rad;
}

void magnetometer_test(void) {
    printf("=== PRUEBA MAGNETÓMETRO ===\n");
    
    if (!magnetometer_init()) {
        printf("ERROR: No se pudo inicializar el magnetómetro\n");
        return;
    }
    
    printf("Magnetómetro inicializado correctamente\n");
    printf("Leyendo datos cada 500ms (Ctrl+C para salir):\n");
    
    while (true) {
        int16_t x, y, z;
        if (magnetometer_read_raw(&x, &y, &z)) {
            double heading = magnetometer_get_filtered_heading();
            printf("X: %d, Y: %d, Z: %d, Rumbo: %.1f°\n", x, y, z, heading);
        } else {
            printf("Error leyendo magnetómetro\n");
        }
        
        sleep_ms(500);
    }
}