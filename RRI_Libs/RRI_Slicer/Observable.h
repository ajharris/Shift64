// Copyright (c) Robarts Research Institute 2006
//---------------------------------------------------------------------
// Observable.h : interface of the subject class for notification
//
//---------------------------------------------------------------------
// Purpose: Implementation of the Observer Pattern (GoF)
//---------------------------------------------------------------------
#ifndef _Observable
#define _Observable

//---------------------------------------------------------------------
#include "STLIncludes.h"
//---------------------------------------------------------------------

#include <vector>

//---------------------------------------------------------------------

#include "Observer.h"

class Observable
{
	//-----------------------------------------------------------------
	// Destructor
	//-----------------------------------------------------------------
	public :
		 virtual ~Observable();

	//-----------------------------------------------------------------
	// Methods
	//-----------------------------------------------------------------
	public :
		virtual void Attach( Observer * );  // Observers call this to subscribe to notifications
		virtual void Detach( Observer * );  // Observers call this to unsubscribe from notifications
		virtual void Notify();				// Child classes call this when something of interest has been changed

	//-----------------------------------------------------------------
	// Protected constructor and attribute(s)
	//-----------------------------------------------------------------
	protected :
		Observable();

		typedef std::vector<Observer*> ObserverVector;		
		typedef ObserverVector::iterator ObserverIterator;

		ObserverVector m_ObserverVector;	// List of everyone observing us
};


//---------------------------------------------------------------------

#endif // _Observable