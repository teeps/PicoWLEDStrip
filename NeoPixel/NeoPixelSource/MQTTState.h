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

/**
 * @brief Initial state before WiFi is established.  Also used in case Broker connection is refused.
 * 
 */
class MQTTInitial : public MQTTState
{
    public:    
        void enter(MQTTTaskInterface*) {};
        void advance(MQTTTaskInterface*);
        void exit(MQTTTaskInterface*);
        ~MQTTInitial() {};
        static MQTTState& GetInstance();
    private:
        MQTTInitial () {}
        MQTTInitial (const MQTTInitial&);
        MQTTInitial & operator=(const MQTTInitial&);
};

class MQTTDiscovery : public MQTTState
{
    public:    
        void enter(MQTTTaskInterface*);
        void advance(MQTTTaskInterface*) ;
        void exit(MQTTTaskInterface*) {};
        ~MQTTDiscovery() {};
        static MQTTState& GetInstance();
    private:
        MQTTDiscovery () {}
        MQTTDiscovery (const MQTTDiscovery&);
        MQTTDiscovery & operator=(const MQTTDiscovery&);
};

/**
 * @brief Broker Connection Pending State
 * 
 */
class MQTTConnect : public MQTTState
{
    public:    
        void enter(MQTTTaskInterface*);
        void advance(MQTTTaskInterface*) ;
        void exit(MQTTTaskInterface*) {};
        ~MQTTConnect() {};
        static MQTTState& GetInstance();
    private:
        MQTTConnect () {}
        MQTTConnect (const MQTTConnect&);
        MQTTConnect & operator=(const MQTTConnect&);
};

/**
 * @brief Connection established and discovery message sent, normal transfer state
 * 
 */
class MQTTTransfer : public MQTTState
{
    public:    
        void enter(MQTTTaskInterface*);
        void advance(MQTTTaskInterface*) ;
        void exit(MQTTTaskInterface*) {};
        ~MQTTTransfer() {};
        static MQTTState& GetInstance();
    private:
        MQTTTransfer () {}
        MQTTTransfer (const MQTTTransfer&);
        MQTTTransfer & operator=(const MQTTTransfer&);
};

#endif