/**
 * @file RGBColour.h
 * @author Mark Tuma (marktuma@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once
#include <stdint.h>

/**
 * 
*/
class RGBColour
{
private:
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
public:
    RGBColour(/* args */){
    Red = 0;
    Green = 0;
    Blue = 0;
    };

    void vGetColour(uint8_t & RedDest, uint8_t & GreenDest, uint8_t & BlueDest) const
    {
        RedDest = Red;
        GreenDest = Green;
        BlueDest = Blue;
    };
    void vSetColour (uint8_t RedSrc, uint8_t GreenSrc, uint8_t BlueSrc)
    {
        Red = RedSrc;
        Green = GreenSrc;
        Blue = BlueSrc;
    };
    void ColourFromText(const char *, uint16_t);
    void vSetColourFromGRB(uint32_t);
    /** @brief Get the stored colour as RGB text, rrr,ggg,bbb*/
    void vGetRGBText(char *, uint8_t &) const;
    ~RGBColour() {};
};
