// Copyright (c) Robarts Research Institute 2006

// Observable.cpp
//---------------------------------------------------------------------
// Purpose: Implementation of the Observer Pattern (GoF)
//---------------------------------------------------------------------
//#include "stdafx.h"
#include "Observable.h"
//#include <afxwin.h>

//---------------------------------------------------------------------

Observable::Observable()
{
}

//---------------------------------------------------------------------

Observable::~Observable()
{
	m_ObserverVector.clear();
}

//---------------------------------------------------------------------

void Observable::Attach( Observer * pObserver )
{
	//TRACE("Observable::Attach(%p)", pObserver);
	//-----------------------------------------------------------------
	// Add the observer to the list
	// Only adds it once, as subsequent adds are pointless, as far as
	// I can figure...
	//-----------------------------------------------------------------
	bool alreadyInVector = false;

	if( !m_ObserverVector.empty() )
	{
		ObserverIterator i = m_ObserverVector.begin();
		for(; i != m_ObserverVector.end(); i++)
		{
			if( *i == pObserver )
				alreadyInVector = true;
		}
	}


	if( !alreadyInVector )
		m_ObserverVector.push_back( pObserver );
}

//---------------------------------------------------------------------

void Observable::Detach( Observer * pObserver )
{
	//TRACE("Observable::Detach(%p)", pObserver);

	//-----------------------------------------------------------------
	// Find and remove the observer from the list
	//-----------------------------------------------------------------
	if( !m_ObserverVector.empty() )
	{
		
		int j = (int)m_ObserverVector.size();
		ObserverIterator i = m_ObserverVector.begin();
		bool done = false;
		for(; i != m_ObserverVector.end(); i++)
			{
				if( *i == pObserver )
				{
					m_ObserverVector.erase(i);
					break;
				}
			}	
		
	}
}

//---------------------------------------------------------------------

void Observable::Notify()
{
	if( !m_ObserverVector.empty() )
	{
		ObserverIterator i = m_ObserverVector.begin();
		for(; i != m_ObserverVector.end(); i++)
		{
			//TRACE("Observable::Notify(%p)", *i);
			(*i)->Update( this );
		}
	}
}

//---------------------------------------------------------------------
