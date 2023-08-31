/**
 * @file ToText.h
 * @author Mark Tuma (marktuma@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-28
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once
#include <stdint.h>
#include "RGBColour.h"
/**
 * @brief Convert a Boolean value to ON or OFF text
 * 
 * @param bValue 
 * @param uiLength 
 * @return char const* 
 */
char const * ToText(bool bValue, uint8_t & uiLength);
/**
 * @brief Convert a uint32_t to a text representation of a hex code 
 * 
 * @param uiValue 
 * @param uiLength 
 * @return char const* 
 */
char const * ToText(uint32_t uiValue, uint8_t &uiLength);

char const* ToText(RGBColour, uint8_t &uiLength);