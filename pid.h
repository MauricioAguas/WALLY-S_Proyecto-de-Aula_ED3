/**
 * @file pid.h
 * @brief Header del controlador PID para navegación autónoma.
 *
 * Define la estructura y funciones para implementar controladores PID
 * tanto para velocidad como para dirección del robot.
 * 
 * @author Equipo WALLY-S
 * @date 2025
 * @version 1.0
 */

#ifndef PID_H
#define PID_H

#include "pico/stdlib.h"

/// @defgroup PID_STRUCTURES Estructuras PID
/// @{

/**
 * @brief Estructura que contiene los parámetros y estado de un controlador PID.
 */
typedef struct {
    double kp;              ///< Ganancia proporcional
    double ki;              ///< Ganancia integral  
    double kd;              ///< Ganancia derivativa
    double setpoint;        ///< Valor objetivo
    double last_input;      ///< Último valor de entrada
    double integral_sum;    ///< Suma del término integral
    double output_min;      ///< Límite mínimo de salida
    double output_max;      ///< Límite máximo de salida
    uint32_t last_time;     ///< Último tiempo de cálculo en us
    bool initialized;       ///< true si el PID está inicializado
} pid_controller_t;

/// @}

/// @defgroup PID_FUNCTIONS Funciones PID
/// @{

/**
 * @brief Inicializa un controlador PID.
 * 
 * @param[out] pid Puntero al controlador PID a inicializar
 * @param kp Ganancia proporcional
 * @param ki Ganancia integral
 * @param kd Ganancia derivativa
 * @param output_min Límite mínimo de salida
 * @param output_max Límite máximo de salida
 */
void pid_init(pid_controller_t* pid, double kp, double ki, double kd, 
              double output_min, double output_max);

/**
 * @brief Establece el punto de referencia (setpoint) del PID.
 * 
 * @param pid Puntero al controlador PID
 * @param setpoint Nuevo valor objetivo
 */
void pid_set_setpoint(pid_controller_t* pid, double setpoint);

/**
 * @brief Calcula la salida del controlador PID.
 * 
 * @param pid Puntero al controlador PID
 * @param input Valor actual de la variable controlada
 * @return Salida del controlador PID
 */
double pid_compute(pid_controller_t* pid, double input);

/**
 * @brief Reinicia el estado interno del controlador PID.
 * 
 * Pone a cero la suma integral y actualiza el tiempo base.
 * 
 * @param pid Puntero al controlador PID
 */
void pid_reset(pid_controller_t* pid);

/**
 * @brief Ajusta los parámetros del controlador PID.
 * 
 * @param pid Puntero al controlador PID
 * @param kp Nueva ganancia proporcional
 * @param ki Nueva ganancia integral
 * @param kd Nueva ganancia derivativa
 */
void pid_tune(pid_controller_t* pid, double kp, double ki, double kd);

/**
 * @brief Establece los límites de salida del controlador.
 * 
 * @param pid Puntero al controlador PID
 * @param output_min Nuevo límite mínimo
 * @param output_max Nuevo límite máximo
 */
void pid_set_output_limits(pid_controller_t* pid, double output_min, double output_max);

/**
 * @brief Maneja el cruce de 0/360 grados para control de rumbo.
 * 
 * Calcula el error más corto entre el setpoint y la entrada,
 * considerando que 359° y 1° están separados por 2°.
 * 
 * @param setpoint Rumbo objetivo (0-360°)
 * @param input Rumbo actual (0-360°)
 * @return Error con signo (-180° a +180°)
 */
double pid_heading_error(double setpoint, double input);

/**
 * @brief Función de prueba para el controlador PID.
 * 
 * Ejecuta una simulación del controlador con diferentes setpoints
 * y muestra la respuesta del sistema.
 */
void pid_test(void);

/// @}

#endif // PID_H/**
 * @file pid.h
 * @brief Header del controlador PID para navegación autónoma.
 *
 * Define la estructura y funciones para implementar controladores PID
 * tanto para velocidad como para dirección del robot.
 * 
 * @author Equipo WALLY-S
 * @date 2025
 * @version 1.0
 */

#ifndef PID_H
#define PID_H

#include "pico/stdlib.h"

/// @defgroup PID_STRUCTURES Estructuras PID
/// @{

/**
 * @brief Estructura que contiene los parámetros y estado de un controlador PID.
 */
typedef struct {
    double kp;              ///< Ganancia proporcional
    double ki;              ///< Ganancia integral  
    double kd;              ///< Ganancia derivativa
    double setpoint;        ///< Valor objetivo
    double last_input;      ///< Último valor de entrada
    double integral_sum;    ///< Suma del término integral
    double output_min;      ///< Límite mínimo de salida
    double output_max;      ///< Límite máximo de salida
    uint32_t last_time;     ///< Último tiempo de cálculo en us
    bool initialized;       ///< true si el PID está inicializado
} pid_controller_t;

/// @}

/// @defgroup PID_FUNCTIONS Funciones PID
/// @{

/**
 * @brief Inicializa un controlador PID.
 * 
 * @param[out] pid Puntero al controlador PID a inicializar
 * @param kp Ganancia proporcional
 * @param ki Ganancia integral
 * @param kd Ganancia derivativa
 * @param output_min Límite mínimo de salida
 * @param output_max Límite máximo de salida
 */
void pid_init(pid_controller_t* pid, double kp, double ki, double kd, 
              double output_min, double output_max);

/**
 * @brief Establece el punto de referencia (setpoint) del PID.
 * 
 * @param pid Puntero al controlador PID
 * @param setpoint Nuevo valor objetivo
 */
void pid_set_setpoint(pid_controller_t* pid, double setpoint);

/**
 * @brief Calcula la salida del controlador PID.
 * 
 * @param pid Puntero al controlador PID
 * @param input Valor actual de la variable controlada
 * @return Salida del controlador PID
 */
double pid_compute(pid_controller_t* pid, double input);

/**
 * @brief Reinicia el estado interno del controlador PID.
 * 
 * Pone a cero la suma integral y actualiza el tiempo base.
 * 
 * @param pid Puntero al controlador PID
 */
void pid_reset(pid_controller_t* pid);

/**
 * @brief Ajusta los parámetros del controlador PID.
 * 
 * @param pid Puntero al controlador PID
 * @param kp Nueva ganancia proporcional
 * @param ki Nueva ganancia integral
 * @param kd Nueva ganancia derivativa
 */
void pid_tune(pid_controller_t* pid, double kp, double ki, double kd);

/**
 * @brief Establece los límites de salida del controlador.
 * 
 * @param pid Puntero al controlador PID
 * @param output_min Nuevo límite mínimo
 * @param output_max Nuevo límite máximo
 */
void pid_set_output_limits(pid_controller_t* pid, double output_min, double output_max);

/**
 * @brief Maneja el cruce de 0/360 grados para control de rumbo.
 * 
 * Calcula el error más corto entre el setpoint y la entrada,
 * considerando que 359° y 1° están separados por 2°.
 * 
 * @param setpoint Rumbo objetivo (0-360°)
 * @param input Rumbo actual (0-360°)
 * @return Error con signo (-180° a +180°)
 */
double pid_heading_error(double setpoint, double input);

/**
 * @brief Función de prueba para el controlador PID.
 * 
 * Ejecuta una simulación del controlador con diferentes setpoints
 * y muestra la respuesta del sistema.
 */
void pid_test(void);

/// @}

#endif // PID_H