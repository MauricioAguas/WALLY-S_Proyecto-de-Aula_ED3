/**
 * @file main.c
 * @brief Programa principal del sistema WALLY-S para Raspberry Pi Pico.
 *
 * Este programa implementa un sistema de menús para probar individualmente
 * cada módulo del robot o ejecutar la integración completa de todos los
 * sistemas (magnetómetro, GPS, Bluetooth, motores y control PID).
 * 
 * @author Equipo WALLY-S
 * @date 2025
 * @version 1.0
 */

#include "pico/stdlib.h"
#include "config.h"
#include "magnetometer.h"
#include "gps.h"
#include "bluetooth.h"
#include "motors.h"
#include "pid.h"
#include <stdio.h>

/// @defgroup TEST_MODES Modos de prueba disponibles
/// @{

/// @brief Modo de prueba del magnetómetro
#define TEST_MAGNETOMETER 1

/// @brief Modo de prueba del GPS
#define TEST_GPS 2

/// @brief Modo de prueba del Bluetooth
#define TEST_BLUETOOTH 3

/// @brief Modo de prueba de los motores
#define TEST_MOTORS 4

/// @brief Modo de prueba del controlador PID
#define TEST_PID 5

/// @brief Modo de integración completa
#define TEST_INTEGRATION 6

/// @}

/// @brief Controladores PID globales
static pid_controller_t heading_pid;
static pid_controller_t speed_pid_a;
static pid_controller_t speed_pid_b;

/**
 * @brief Muestra el menú principal de opciones.
 */
void print_menu(void) {
    printf("\n=== MENÚ DE PRUEBAS WALLY-S ===\n");
    printf("1. Probar Magnetómetro QMC5883L\n");
    printf("2. Probar GPS NEO-6M\n");
    printf("3. Probar Bluetooth HC-05\n");
    printf("4. Probar Motores y Encoders\n");
    printf("5. Probar Controlador PID\n");
    printf("6. Integración completa\n");
    printf("Selecciona una opción (1-6): ");
}

/**
 * @brief Ejecuta la prueba de integración completa del sistema.
 * 
 * Inicializa todos los módulos y ejecuta un bucle principal que:
 * - Lee datos de los sensores
 * - Procesa comandos de Bluetooth
 * - Ejecuta control PID para navegación autónoma
 * - Reporta estado del sistema
 */
void integration_test(void) {
    printf("=== INTEGRACIÓN COMPLETA WALLY-S ===\n");
    
    // Inicializar todos los módulos
    bool mag_ok = magnetometer_init();
    bool gps_ok = gps_init();
    bool bt_ok = bluetooth_init();
    bool motors_ok = motors_init();
    
    printf("Estado de inicialización:\n");
    printf("  Magnetómetro: %s\n", mag_ok ? "✓ OK" : "✗ ERROR");
    printf("  GPS: %s\n", gps_ok ? "✓ OK" : "✗ ERROR");
    printf("  Bluetooth: %s\n", bt_ok ? "✓ OK" : "✗ ERROR");
    printf("  Motores: %s\n", motors_ok ? "✓ OK" : "✗ ERROR");
    
    if (!mag_ok || !gps_ok || !bt_ok || !motors_ok) {
        printf("\nERROR: Falló la inicialización. Verifica las conexiones.\n");
        return;
    }
    
    // Inicializar controladores PID
    pid_init(&heading_pid, KP_DIR, KI_DIR, KD_DIR, -50.0, 50.0);
    pid_init(&speed_pid_a, KP_RPM, KI_RPM, KD_RPM, MIN_SPEED, MAX_SPEED);
    pid_init(&speed_pid_b, KP_RPM, KI_RPM, KD_RPM, MIN_SPEED, MAX_SPEED);
    
    pid_set_setpoint(&speed_pid_a, BASE_SPEED_A);
    pid_set_setpoint(&speed_pid_b, BASE_SPEED_B);
    
    printf("\nSistema inicializado correctamente\n");
    printf("Enviando datos por Bluetooth cada segundo...\n");
    printf("Comandos disponibles por Bluetooth:\n");
    printf("  - 'LAT,LNG' para establecer objetivo\n");
    printf("  - 'STOP' para detener navegación\n");
    
    // Configurar LED de estado
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    char bt_buffer[128];
    int loop_counter = 0;
    bool navigation_active = false;
    
    while (true) {
        // Leer sensores
        double heading = magnetometer_get_filtered_heading();
        gps_update();
        gps_data_t gps_data = gps_get_data();
        
        // Procesar comandos Bluetooth
        int bt_bytes = bluetooth_read_line(bt_buffer, sizeof(bt_buffer));
        if (bt_bytes > 0) {
            printf("BT << %s\n", bt_buffer);
            
            if (strcmp(bt_buffer, "STOP") == 0) {
                navigation_active = false;
                motors_stop_all();
                bluetooth_send_string("Navegación detenida\n");
                printf("Navegación manual detenida\n");
            } else {
                double lat, lng;
                if (bluetooth_parse_coordinates(bt_buffer, &lat, &lng)) {
                    gps_set_target(lat, lng);
                    navigation_active = true;
                    pid_reset(&heading_pid);
                    printf("Nuevo objetivo: %.6f, %.6f\n", lat, lng);
                    bluetooth_send_string("Objetivo establecido\n");
                } else {
                    bluetooth_send_string("Formato inválido. Usar: LAT,LNG\n");
                }
            }
        }
        
        // Control de navegación autónoma
        if (navigation_active && gps_data.fix_valid && gps_has_target()) {
            double target_bearing = gps_bearing_to_target();
            double distance = gps_distance_to_target();
            
            if (gps_target_reached()) {
                navigation_active = false;
                motors_stop_all();
                bluetooth_send_string("Objetivo alcanzado!\n");
                printf("¡Objetivo alcanzado!\n");
            } else {
                // Calcular corrección de rumbo
                pid_set_setpoint(&heading_pid, target_bearing);
                double heading_correction = pid_compute(&heading_pid, heading);
                
                // Aplicar corrección diferencial a los motores
                int speed_a = BASE_SPEED_A - (int)heading_correction;
                int speed_b = BASE_SPEED_B + (int)heading_correction;
                
                // Limitar velocidades
                speed_a = (speed_a < MIN_SPEED) ? MIN_SPEED : 
                         (speed_a > MAX_SPEED) ? MAX_SPEED : speed_a;
                speed_b = (speed_b < MIN_SPEED) ? MIN_SPEED : 
                         (speed_b > MAX_SPEED) ? MAX_SPEED : speed_b;
                
                motors_set_both_motors(MOTOR_FORWARD, speed_a, MOTOR_FORWARD, speed_b);
                
                printf("Nav: H=%.1f° T=%.1f° D=%.1fm SpA=%d SpB=%d\n", 
                       heading, target_bearing, distance, speed_a, speed_b);
            }
        }
        
        // Enviar estado por Bluetooth (cada segundo)
        if (++loop_counter >= 20) { // 50ms * 20 = 1 segundo
            gpio_put(LED_PIN, !gpio_get(LED_PIN)); // Parpadear LED
            
            if (gps_has_target() && gps_data.fix_valid) {
                double distance = gps_distance_to_target();
                double bearing = gps_bearing_to_target();
                
                bluetooth_send_navigation_info(
                    gps_data.latitude, gps_data.longitude,
                    0, 0, // Las coordenadas objetivo se guardarían por separado
                    distance, bearing
                );
            } else {
                bluetooth_send_status(heading, 0, 0, gps_data.fix_valid);
            }
            
            printf("Estado: H=%.1f° GPS=%s Sats=%d Nav=%s\n",
                   heading, gps_data.fix_valid ? "OK" : "NO", 
                   gps_data.satellites, navigation_active ? "SI" : "NO");
            
            loop_counter = 0;
        }
        
        sleep_ms(LOOP_INTERVAL_MS);
    }
}

/**
 * @brief Función principal del programa.
 * 
 * Inicializa el sistema y presenta un menú interactivo para seleccionar
 * el modo de prueba deseado.
 * 
 * @return 0 si el programa termina correctamente
 */
int main(void) {
    // Inicializar comunicación serie
    stdio_init_all();
    
    // Esperar conexión serial
    sleep_ms(2000);
    
    printf("=====================================\n");
    printf("    WALLY-S - SISTEMA DE PRUEBAS    \n");
    printf("  Raspberry Pi Pico - Versión 1.0   \n");
    printf("=====================================\n");
    
    while (true) {
        print_menu();
        
        // Leer opción del usuario
        int option = 0;
        scanf("%d", &option);
        
        printf("\n");
        
        switch (option) {
            case TEST_MAGNETOMETER:
                magnetometer_test();
                break;
                
            case TEST_GPS:
                gps_test();
                break;
                
            case TEST_BLUETOOTH:
                bluetooth_test();
                break;
                
            case TEST_MOTORS:
                motors_test();
                break;
                
            case TEST_PID:
                pid_test();
                break;
                
            case TEST_INTEGRATION:
                integration_test();
                break;
                
            default:
                printf("⚠️  Opción inválida. Selecciona 1-6.\n");
                break;
        }
        
        printf("\nPresiona Enter para continuar...");
        getchar(); // Limpiar buffer
        getchar(); // Esperar Enter
    }
    
    return 0;
}