/**
 * @file pid.c
 * @brief Implementación del controlador PID para navegación autónoma.
 *
 * Este módulo implementa un controlador PID genérico que puede ser usado
 * tanto para control de velocidad como de dirección del robot.
 * 
 * @author Equipo WALLY-S
 * @date 2025
 * @version 1.0
 */

#include "pid.h"
#include "config.h"
#include <math.h>

void pid_init(pid_controller_t* pid, double kp, double ki, double kd, 
              double output_min, double output_max) {
    if (!pid) return;
    
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->setpoint = 0.0;
    pid->last_input = 0.0;
    pid->integral_sum = 0.0;
    pid->output_min = output_min;
    pid->output_max = output_max;
    pid->last_time = time_us_32();
    pid->initialized = true;
}

void pid_set_setpoint(pid_controller_t* pid, double setpoint) {
    if (!pid || !pid->initialized) return;
    pid->setpoint = setpoint;
}

double pid_compute(pid_controller_t* pid, double input) {
    if (!pid || !pid->initialized) return 0.0;
    
    uint32_t current_time = time_us_32();
    double dt = (current_time - pid->last_time) / 1000000.0; // Convertir a segundos
    
    if (dt <= 0.0) return 0.0; // Evitar división por cero
    
    // Calcular error
    double error = pid->setpoint - input;
    
    // Término proporcional
    double proportional = pid->kp * error;
    
    // Término integral
    pid->integral_sum += error * dt;
    double integral = pid->ki * pid->integral_sum;
    
    // Término derivativo
    double derivative = pid->kd * (input - pid->last_input) / dt;
    
    // Calcular salida
    double output = proportional + integral - derivative;
    
    // Aplicar límites de saturación
    if (output > pid->output_max) {
        output = pid->output_max;
        // Anti-windup: limitar integral cuando hay saturación
        if (pid->ki != 0.0) {
            double integral_max = (pid->output_max - proportional + derivative) / pid->ki;
            if (pid->integral_sum > integral_max) {
                pid->integral_sum = integral_max;
            }
        }
    } else if (output < pid->output_min) {
        output = pid->output_min;
        // Anti-windup: limitar integral cuando hay saturación
        if (pid->ki != 0.0) {
            double integral_min = (pid->output_min - proportional + derivative) / pid->ki;
            if (pid->integral_sum < integral_min) {
                pid->integral_sum = integral_min;
            }
        }
    }
    
    // Actualizar variables para la próxima iteración
    pid->last_input = input;
    pid->last_time = current_time;
    
    return output;
}

void pid_reset(pid_controller_t* pid) {
    if (!pid || !pid->initialized) return;
    
    pid->integral_sum = 0.0;
    pid->last_input = 0.0;
    pid->last_time = time_us_32();
}

void pid_tune(pid_controller_t* pid, double kp, double ki, double kd) {
    if (!pid || !pid->initialized) return;
    
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    
    // Reiniciar integral para evitar saltos
    pid_reset(pid);
}

void pid_set_output_limits(pid_controller_t* pid, double output_min, double output_max) {
    if (!pid || !pid->initialized) return;
    
    pid->output_min = output_min;
    pid->output_max = output_max;
    
    // Limitar integral acumulada si está fuera de rango
    if (pid->ki != 0.0) {
        double max_integral = pid->output_max / pid->ki;
        double min_integral = pid->output_min / pid->ki;
        
        if (pid->integral_sum > max_integral) {
            pid->integral_sum = max_integral;
        } else if (pid->integral_sum < min_integral) {
            pid->integral_sum = min_integral;
        }
    }
}

double pid_heading_error(double setpoint, double input) {
    double error = setpoint - input;
    
    // Normalizar error a rango [-180, +180]
    while (error > 180.0) {
        error -= 360.0;
    }
    while (error < -180.0) {
        error += 360.0;
    }
    
    return error;
}

void pid_test(void) {
    printf("=== PRUEBA CONTROLADOR PID ===\n");
    
    // Crear controlador PID para prueba
    pid_controller_t test_pid;
    pid_init(&test_pid, 2.0, 0.1, 0.2, -100.0, 100.0);
    
    printf("PID inicializado - Kp:%.1f, Ki:%.1f, Kd:%.1f\n", 
           test_pid.kp, test_pid.ki, test_pid.kd);
    
    // Simular respuesta a escalón
    pid_set_setpoint(&test_pid, 90.0); // Objetivo: 90 grados
    
    printf("Setpoint: %.1f grados\n", test_pid.setpoint);
    printf("Simulando respuesta del sistema:\n");
    printf("Tiempo(s)\tEntrada\t\tSalida PID\n");
    
    double simulated_output = 0.0;
    for (int i = 0; i < 50; i++) {
        // Simular planta de primer orden simple
        simulated_output += pid_compute(&test_pid, simulated_output) * 0.01;
        
        printf("%.1f\t\t%.2f\t\t%.2f\n", 
               i * 0.05, simulated_output, pid_compute(&test_pid, simulated_output));
        
        sleep_ms(50);
    }
    
    // Prueba de control de rumbo
    printf("\n--- Prueba de control de rumbo ---\n");
    pid_controller_t heading_pid;
    pid_init(&heading_pid, KP_DIR, KI_DIR, KD_DIR, -50.0, 50.0);
    
    double test_headings[] = {10, 350, 180, 0, 270};
    double target = 0;
    
    for (int i = 0; i < 5; i++) {
        double error = pid_heading_error(target, test_headings[i]);
        double correction = pid_compute(&heading_pid, test_headings[i]);
        
        printf("Rumbo: %.0f°, Error: %.1f°, Corrección: %.1f\n", 
               test_headings[i], error, correction);
    }
    
    printf("Prueba PID completada\n");
}