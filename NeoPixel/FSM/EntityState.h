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
	virtual void enter(T* entity) = 0;
	virtual void advance(T* entity) = 0;
	virtual void exit(T* entity) = 0;
	virtual ~EntityState() {};
};
