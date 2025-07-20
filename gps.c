/**
 * @file gps.c
 * @brief Implementación del driver para GPS NEO-6M.
 *
 * Este módulo maneja la comunicación UART con el GPS, procesa sentencias
 * NMEA GPGGA y proporciona funciones de navegación básica.
 * 
 * @author Equipo WALLY-S
 * @date 2025
 * @version 1.0
 */

#include "gps.h"
#include "config.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

/// @brief Estado de inicialización del GPS
static bool initialized = false;

/// @brief Datos GPS actuales
static gps_data_t current_gps_data = {0};

/// @brief Coordenadas objetivo
static target_data_t target_data = {0};

/**
 * @brief Convierte coordenadas en formato DDMM.MMMM a grados decimales.
 * 
 * @param raw_degrees Cadena con coordenadas en formato NMEA
 * @return Coordenadas en grados decimales
 */
static double convert_to_degrees(const char* raw_degrees) {
    if (!raw_degrees || strlen(raw_degrees) < 4) return 0.0;
    
    double raw = atof(raw_degrees);
    int degrees = (int)(raw / 100);
    double minutes = raw - (degrees * 100);
    return degrees + (minutes / 60.0);
}

/**
 * @brief Procesa una sentencia GPGGA y actualiza los datos GPS.
 * 
 * @param sentence Sentencia NMEA a procesar
 * @return true si se procesó correctamente y hay fix válido
 */
static bool parse_gga_sentence(char* sentence) {
    char* token = strtok(sentence, ",");
    int field = 0;
    
    while (token != NULL && field < 15) {
        switch (field) {
            case 1: // Tiempo
                if (strlen(token) >= 6) {
                    strncpy(current_gps_data.time, token, 6);
                    current_gps_data.time[6] = '\0';
                }
                break;
            case 2: // Latitud
                if (strlen(token) > 0) {
                    current_gps_data.latitude = convert_to_degrees(token);
                }
                break;
            case 3: // N/S
                if (token[0] == 'S') {
                    current_gps_data.latitude = -current_gps_data.latitude;
                }
                break;
            case 4: // Longitud
                if (strlen(token) > 0) {
                    current_gps_data.longitude = convert_to_degrees(token);
                }
                break;
            case 5: // E/W
                if (token[0] == 'W') {
                    current_gps_data.longitude = -current_gps_data.longitude;
                }
                break;
            case 6: // Fix quality
                current_gps_data.fix_quality = atoi(token);
                break;
            case 7: // Satellites
                current_gps_data.satellites = atoi(token);
                break;
            case 9: // Altitude
                current_gps_data.altitude = atof(token);
                break;
        }
        token = strtok(NULL, ",");
        field++;
    }
    
    // Considerar fix válido si hay al menos 4 satélites y fix quality > 0
    current_gps_data.fix_valid = (current_gps_data.satellites >= 4 && current_gps_data.fix_quality > 0);
    
    return current_gps_data.fix_valid;
}

bool gps_init(void) {
    // Inicializar UART para GPS
    uart_init(GPS_UART_ID, GPS_BAUD_RATE);
    gpio_set_function(GPS_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(GPS_RX_PIN, GPIO_FUNC_UART);
    
    // Configurar formato UART
    uart_set_format(GPS_UART_ID, 8, 1, UART_PARITY_NONE);
    uart_set_hw_flow(GPS_UART_ID, false, false);
    uart_set_fifo_enabled(GPS_UART_ID, false);
    
    initialized = true;
    return true;
}

bool gps_update(void) {
    if (!initialized) return false;
    
    static char buffer[256];
    static int buffer_index = 0;
    
    // Leer datos disponibles
    while (uart_is_readable(GPS_UART_ID)) {
        char c = uart_getc(GPS_UART_ID);
        
        if (c == '\n' || c == '\r') {
            if (buffer_index > 0) {
                buffer[buffer_index] = '\0';
                
                // Procesar solo sentencias GPGGA
                if (strncmp(buffer, "$GPGGA", 6) == 0) {
                    parse_gga_sentence(buffer);
                }
                
                buffer_index = 0;
            }
        } else if (buffer_index < sizeof(buffer) - 1) {
            buffer[buffer_index++] = c;
        }
    }
    
    return true;
}

gps_data_t gps_get_data(void) {
    return current_gps_data;
}

void gps_set_target(double lat, double lng) {
    target_data.latitude = lat;
    target_data.longitude = lng;
    target_data.target_set = true;
}

bool gps_has_target(void) {
    return target_data.target_set;
}

double gps_distance_to_target(void) {
    if (!current_gps_data.fix_valid || !target_data.target_set) {
        return 0.0;
    }
    
    // Fórmula haversine simplificada
    double lat1_rad = current_gps_data.latitude * M_PI / 180.0;
    double lat2_rad = target_data.latitude * M_PI / 180.0;
    double delta_lat = (target_data.latitude - current_gps_data.latitude) * M_PI / 180.0;
    double delta_lng = (target_data.longitude - current_gps_data.longitude) * M_PI / 180.0;
    
    double a = sin(delta_lat/2) * sin(delta_lat/2) + 
               cos(lat1_rad) * cos(lat2_rad) * sin(delta_lng/2) * sin(delta_lng/2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    
    return 6371000 * c; // Distancia en metros
}

double gps_bearing_to_target(void) {
    if (!current_gps_data.fix_valid || !target_data.target_set) {
        return 0.0;
    }
    
    double lat1_rad = current_gps_data.latitude * M_PI / 180.0;
    double lat2_rad = target_data.latitude * M_PI / 180.0;
    double delta_lng = (target_data.longitude - current_gps_data.longitude) * M_PI / 180.0;
    
    double y = sin(delta_lng) * cos(lat2_rad);
    double x = cos(lat1_rad) * sin(lat2_rad) - sin(lat1_rad) * cos(lat2_rad) * cos(delta_lng);
    
    double bearing = atan2(y, x) * 180.0 / M_PI;
    
    // Normalizar a 0-360 grados
    if (bearing < 0) {
        bearing += 360;
    }
    
    return bearing;
}

bool gps_target_reached(void) {
    return gps_distance_to_target() < 2.0; // Menos de 2 metros
}

void gps_test(void) {
    printf("=== PRUEBA GPS ===\n");
    
    if (!gps_init()) {
        printf("ERROR: No se pudo inicializar el GPS\n");
        return;
    }
    
    printf("GPS inicializado correctamente\n");
    printf("Esperando señal GPS (puede tomar varios minutos)...\n");
    
    while (true) {
        gps_update();
        gps_data_t data = gps_get_data();
        
        if (data.fix_valid) {
            printf("GPS Fix válido:\n");
            printf("  Lat: %.6f, Lng: %.6f\n", data.latitude, data.longitude);
            printf("  Satélites: %d, Altitud: %.1f m\n", data.satellites, data.altitude);
            printf("  Tiempo: %s\n", data.time);
        } else {
            printf("Sin fix GPS - Satélites: %d\n", data.satellites);
        }
        
        sleep_ms(1000);
    }
}