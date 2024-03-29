/**
 * @file MQTTAttribute.h
 * @author Mark Tuma (marktuma@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <stdint.h>
#include <cstring>
#include "ToText.h"

/** @brief Templated class for handling attributes from MQTT comms*/
template <class T> class MQTTAttribute
{
    public:
        /**
         * @brief Set the attribute by reference
         * @return uint8_t 
         */
        //uint8_t uiSetAtt(T &);
        /** @brief Set the attribute by copy
         * 
         * @return uint8_t 
         */
        uint8_t uiSetAtt(T const & xNewAtt) {xAtt = xNewAtt; return 0;};
        /**
         * @brief Get a copy of the attribute as text
         * @param[out] char * - buffer to store text
         * @param[out] uint8_t & - length of text string
         */
        void vGetAttText(char * cData, uint8_t & uiLen) const
        {
            strcpy (cData, ToText (xAtt, uiLen));
        }
        
        /**
         * @brief Return a pointer to the attribute data
         * 
         * @return T* 
         */
        T* pGetPtr()
        {
            return &xAtt;
        };
        /** @brief Publishing modes*/
        enum ePublishingModes {
            PUB_CYCLIC=0,
            PUB_DELTA,
            PUB_DELTA_CYCLIC,
        };
        
        //default constructor
        MQTTAttribute() {
            bSubscribe = false;
            bPublish = false;
            uiPublishInterval = 60;
            eMode = PUB_CYCLIC;
            uiLastPublishedTime = 0;
            xAtt = T(); //initialiser for templated type
            xDelta = T(); //initialiser for templated type
        };  
        /** @brief NOT IMPLEMENTED*/
        void vSetPublish (bool);
        /** @brief NOT IMPLEMENTED*/
        void vSetSubscribe (bool);
        /** @brief NOT IMPLEMENTED*/
        void vSetInterval (uint32_t);
        /** @brief NOT IMPLEMENTED*/
        void vSetMode(ePublishingModes);
        /** @brief NOT IMPLEMENTED*/
        void vSetDelta(T);
        /** @brief NOT IMPLEMENTED*/
        bool bGetSubscribe() const;
        /** @brief NOT IMPLEMENTED*/
        bool bGetPublish() const;
        /** @brief NOT IMPLEMENTED*/
        uint32_t uiGetInterval();
        /** @brief NOT IMPLEMENTED*/
        bool bCheckPublishCondition(uint64_t) const;
        /** @brief NOT IMPLEMENTED*/
        void vSetPublishedTime(uint64_t);
    private:
        /** @brief The attribute data*/
        T xAtt;
        /** @brief The attribute delta value for delta publishing*/
        T xDelta;
        /** @brief Set if the attribute is to be published*/
        bool bPublish;
        /** @brief Set if the attribute is to be subscribed*/
        bool bSubscribe;
        /** @brief Interval in seconds at which the attribute should be published*/
        uint32_t uiPublishInterval;
        /** @brief System clock time of last publishing*/
        uint64_t uiLastPublishedTime;
        /** @brief Publishng mode*/
        ePublishingModes eMode;
        
};
