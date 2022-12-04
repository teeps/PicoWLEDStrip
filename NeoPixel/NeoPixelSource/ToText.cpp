/**
 * @file ToText.cpp
 * @author Mark Tuma (marktuma@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-28
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "ToText.h"
#include <stdint.h>
#include <iomanip>
#include <string>
#include <sstream>

char const * ToText(bool bValue, uint8_t & uiLength)
{
    uiLength = bValue ? 2 : 3;
    return bValue ? "ON" : "OFF";
}

char const * ToText(uint32_t uiValue, uint8_t &uiLength)
{
    std::stringstream stream;
    stream << "0x" << std::hex << uiValue;
    std::string hexstring(stream.str());
    return hexstring.c_str();
}
static char cText[11];
char const * ToText(RGBColour Colour, uint8_t &uiLength)
{
    Colour.vGetRGBText(cText,uiLength);
    return cText;
}