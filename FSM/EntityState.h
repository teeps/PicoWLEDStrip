/**
 * @file EntityState.h
 * @author Mark Tuma (marktuma@gmail.com)
 * @brief Generic State class
 * @version 0.1
 * @date 2022-11-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

template <class T>
class EntityState
{
public:
    /** @brief Actions on state entry*/
	virtual void enter(T* entity) = 0;
    /** @brief Advance to next state*/
	virtual void advance(T* entity) = 0;
    /** @brief Action on state exit*/
	virtual void exit(T* entity) = 0;
	virtual ~EntityState() {};
};
