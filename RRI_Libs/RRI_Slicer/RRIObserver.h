// Copyright (c) Robarts Research Institute 2006

//---------------------------------------------------------------------
// RRIObserver.h : interface of the Observer class
//---------------------------------------------------------------------
// Purpose: Implementation of the Observer Pattern specific to messages
//---------------------------------------------------------------------

#if !defined(_RRI_Observer_h)
#define _RRI_Observer_h

#include "Observer.h"

//---------------------------------------------------------------------
// Forward declare the class RRIObservable
//---------------------------------------------------------------------

class RRIObservable;
class RRIEvent;

//---------------------------------------------------------------------
// Class declaration for Observer
//---------------------------------------------------------------------
class RRIObserver
{
	//-----------------------------------------------------------------
	// Destructor
	//-----------------------------------------------------------------
	public :
        virtual ~RRIObserver();

	//-----------------------------------------------------------------
	// Methods
	//-----------------------------------------------------------------
	public :
		virtual bool Update( RRIObservable* op, const RRIEvent& m) = 0;

	//-----------------------------------------------------------------
	// Protected constructor
	//-----------------------------------------------------------------
	protected :
        RRIObserver();
};

//---------------------------------------------------------------------

#endif // _RRI_Observer_h
