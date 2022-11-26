/**
 * @file MQTTState.h
 * @author Mark Tuma (marktuma@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef _MQTTSTATE_H
#define _MQTTSTATE_H
#include "EntityState.h"

class MQTTTaskInterface;

class MQTTState : public EntityState<MQTTTaskInterface>
{   
    public:
        virtual void enter(MQTTTaskInterface* MQTT) = 0;
        virtual void advance(MQTTTaskInterface* MQTT) = 0;
        virtual void exit(MQTTTaskInterface* MQTT) = 0;
    	virtual ~MQTTState() {};
};

class MQTTNotConnected : public MQTTState
{
    public:    
        void enter(MQTTTaskInterface*) {};
        void advance(MQTTTaskInterface*);
        void exit(MQTTTaskInterface*);
        ~MQTTNotConnected() {};
        static MQTTState& GetInstance();
    private:
        MQTTNotConnected () {}
        MQTTNotConnected (const MQTTNotConnected&);
        MQTTNotConnected & operator=(const MQTTNotConnected&);
};

class MQTTConnected : public MQTTState
{
    public:    
        void enter(MQTTTaskInterface*) {};
        void advance(MQTTTaskInterface*) ;
        void exit(MQTTTaskInterface*) {};
        ~MQTTConnected() {};
        static MQTTState& GetInstance();
    private:
        MQTTConnected () {}
        MQTTConnected (const MQTTConnected&);
        MQTTConnected & operator=(const MQTTConnected&);
};

#endif