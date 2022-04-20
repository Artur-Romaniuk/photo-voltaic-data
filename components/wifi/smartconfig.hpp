#pragma once

/**
 * @brief FreeRTOS task for managing SmartConfig.
 *
 * @param parm
 */
void smartconfig_task(void *parm);
/**
 * @brief Starts SmartConfig.
 *
 */
void smartconfig_start();
/**
 * @brief Stops SmartConfig.
 *
 */
void smartconfig_stop();