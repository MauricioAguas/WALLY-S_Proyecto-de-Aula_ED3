/**
 * @file bluetooth.c
 * @brief Implementación del driver para Bluetooth HC-05.
 *
 * Este módulo maneja la comunicación UART con el módulo Bluetooth,
 * procesa comandos recibidos y envía información de estado.
 * 
 * @author Equipo WALLY-S
 * @date 2025
 * @version 1.0
 */

#include "bluetooth.h"
#include "config.h"
#include <string.h>
#include <stdlib.h>

/// @brief Estado de inicialización del Bluetooth
static bool initialized = false;

bool bluetooth_init(void) {
    // Inicializar UART para Bluetooth
    uart_init(BT_UART_ID, BT_BAUD_RATE);
    gpio_set_function(BT_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(BT_RX_PIN, GPIO_FUNC_UART);
    
    // Configurar formato UART
    uart_set_format(BT_UART_ID, 8, 1, UART_PARITY_NONE);
    uart_set_hw_flow(BT_UART_ID, false, false);
    uart_set_fifo_enabled(BT_UART_ID, false);
    
    initialized = true;
    return true;
}

bool bluetooth_send_string(const char* str) {
    if (!initialized || !str) return false;
    
    uart_puts(BT_UART_ID, str);
    return true;
}

bool bluetooth_send_data(const uint8_t* data, size_t length) {
    if (!initialized || !data) return false;
    
    uart_write_blocking(BT_UART_ID, data, length);
    return true;
}

bool bluetooth_available(void) {
    return initialized && uart_is_readable(BT_UART_ID);
}

int bluetooth_read_line(char* buffer, int max_length) {
    if (!initialized || !buffer || max_length <= 0) return -1;
    
    int index = 0;
    absolute_time_t timeout = make_timeout_time_ms(100); // 100ms timeout
    
    while (index < max_length - 1 && !time_reached(timeout)) {
        if (uart_is_readable(BT_UART_ID)) {
            char c = uart_getc(BT_UART_ID);
            
            if (c == '\n' || c == '\r') {
                if (index > 0) {
                    buffer[index] = '\0';
                    return index;
                }
            } else if (c >= ' ' && c <= '~') { // Solo caracteres imprimibles
                buffer[index++] = c;
            }
        }
    }
    
    if (index > 0) {
        buffer[index] = '\0';
        return index;
    }
    
    return 0;
}

bool bluetooth_parse_coordinates(const char* command, double* lat, double* lng) {
    if (!command || !lat || !lng) return false;
    
    // Buscar coma separando latitud y longitud
    char* comma = strchr(command, ',');
    if (!comma) return false;
    
    // Crear copias temporales para parsear
    char lat_str[32], lng_str[32];
    int lat_len = comma - command;
    
    if (lat_len >= sizeof(lat_str)) return false;
    
    strncpy(lat_str, command, lat_len);
    lat_str[lat_len] = '\0';
    strcpy(lng_str, comma + 1);
    
    // Convertir a double
    *lat = atof(lat_str);
    *lng = atof(lng_str);
    
    // Validar que son coordenadas válidas
    return (*lat >= -90.0 && *lat <= 90.0 && *lng >= -180.0 && *lng <= 180.0);
}

void bluetooth_send_status(double heading, double target_heading, double distance, bool gps_fix) {
    if (!initialized) return;
    
    char buffer[256];
    snprintf(buffer, sizeof(buffer), 
             "STATUS,%.1f,%.1f,%.1f,%d\n", 
             heading, target_heading, distance, gps_fix ? 1 : 0);
    
    bluetooth_send_string(buffer);
}

void bluetooth_send_navigation_info(double current_lat, double current_lng, 
                                   double target_lat, double target_lng, 
                                   double distance, double bearing) {
    if (!initialized) return;
    
    char buffer[256];
    snprintf(buffer, sizeof(buffer), 
             "NAV,%.6f,%.6f,%.6f,%.6f,%.1f,%.1f\n", 
             current_lat, current_lng, target_lat, target_lng, distance, bearing);
    
    bluetooth_send_string(buffer);
}

void bluetooth_test(void) {
    printf("=== PRUEBA BLUETOOTH ===\n");
    
    if (!bluetooth_init()) {
        printf("ERROR: No se pudo inicializar Bluetooth\n");
        return;
    }
    
    printf("Bluetooth inicializado correctamente\n");
    printf("Envía comandos desde la app Bluetooth:\n");
    printf("- 'LED_ON' para encender LED\n");
    printf("- 'LED_OFF' para apagar LED\n");
    printf("- 'LAT,LONG' para establecer coordenadas\n");
    
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    char buffer[128];
    while (true) {
        int bytes_read = bluetooth_read_line(buffer, sizeof(buffer));
        
        if (bytes_read > 0) {
            printf("Recibido: %s\n", buffer);
            
            if (strcmp(buffer, "LED_ON") == 0) {
                gpio_put(LED_PIN, 1);
                bluetooth_send_string("LED encendido\n");
            } else if (strcmp(buffer, "LED_OFF") == 0) {
                gpio_put(LED_PIN, 0);
                bluetooth_send_string("LED apagado\n");
            } else {
                double lat, lng;
                if (bluetooth_parse_coordinates(buffer, &lat, &lng)) {
                    printf("Coordenadas recibidas: %.6f, %.6f\n", lat, lng);
                    bluetooth_send_string("Coordenadas válidas\n");
                } else {
                    bluetooth_send_string("Formato inválido\n");
                }
            }
        }
        
        sleep_ms(50);
    }
}