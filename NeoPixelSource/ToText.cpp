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
#include <string.h>
#include <sstream>

char const * ToText(bool bValue, uint8_t & uiLength)
{
    uiLength = bValue ? 2 : 3;
    return bValue ? "ON" : "OFF";
}
static char cText[255];
char const * ToText(uint32_t uiValue, uint8_t &uiLength)
{
    std::stringstream stream;
    stream << "0x" << std::hex << uiValue;
    std::string hexstring(stream.str());
    strncpy (cText, hexstring.c_str(), hexstring.length());
    cText[hexstring.length()+1]=0; //Null terminator
    uiLength = hexstring.length();
    return cText;
}

char const * ToText(RGBColour Colour, uint8_t &uiLength)
{
    Colour.vGetRGBText(cText,uiLength);
    return cText;
}