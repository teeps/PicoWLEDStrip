/**
 * @file RGBColour.cpp
 * @author Mark Tuma (marktuma@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "RGBColour.h"
#include <stdint.h>
#include <string>
/**
 * @brief Search a string for a character, starting from a given position, and copy all characters until finding it to a destination.
 * 
 * @param[in] char const * cSource - source string
 * @param[in] uint8_t uiLength - source length
 * @param[in] uint8_t uiStartPosition - start position for search
 * @param[out] char * cDest - Destination string
 * @param[in] uint8_t uiDestLen - length of destination string
 * @param[in] char cSplitCharacter - Character to split on
 * @param[out] uint8_t & uiSplitPosition - position of found character
 */
static void vStringSearch(char const *  cSource, uint8_t uiLength, uint8_t uiStartPosition, char * cDest, uint8_t uiDestLen, char cSplitCharacter, uint8_t & uiSplitPosition)
{
  uint8_t uiDestPosition=0;
  for (uint8_t i=uiStartPosition; i< uiLength; i++)
  {
    if (cSource[i] == cSplitCharacter)
    {
      uiSplitPosition = i;
      break;
    } else
    cDest[uiDestPosition++] = cSource[i];
  }
  for (uint8_t j=uiSplitPosition; j< uiDestLen; j++)
    cDest[uiDestPosition++]=0;
}

void RGBColour::ColourFromText(const char * cData, uint16_t uiLength)
{
    if (uiLength == 0 || uiLength >12) {} else 
    {
        //Assume data is three comma-separated variables: r,g,b
        char cRed[10]="000";
        char cGreen[10]="000";
        char cBlue[10]="000";
        uint8_t uiRed, uiGreen, uiBlue;
        //find first comma
        uint8_t uiCommaPosition=0;
        vStringSearch(cData, uiLength,                   0 , cRed, sizeof(cRed), ',', uiCommaPosition );
        vStringSearch(cData, uiLength, uiCommaPosition + 1 , cGreen, sizeof(cGreen), ',', uiCommaPosition );
        vStringSearch(cData, uiLength, uiCommaPosition + 1 , cBlue, sizeof(cBlue), 0, uiCommaPosition );
        
        Red = std::stoi(cRed);
        Green = std::stoi(cGreen);
        Blue = std::stoi(cBlue);
    }
 }

void RGBColour::vSetColourFromGRB(uint32_t uiColour)
{
  Green = uiColour >> 24;
  Red = uiColour >> 16;
  Blue = uiColour >> 8;
}

void RGBColour::vGetRGBText(char * cText, uint8_t & uiLength) const
{
  sprintf(cText, "%d,%d,%d", Red,Green,Blue);
  uiLength = sizeof(cText);
}