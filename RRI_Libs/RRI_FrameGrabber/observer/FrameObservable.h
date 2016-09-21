// Copyright (c) Robarts Research Institute 2006
//---------------------------------------------------------------------
// FrameObservable.h : interface of the subject class for notification
//---------------------------------------------------------------------
// Purpose: Implementation of the Observer Pattern specific to message handling
//---------------------------------------------------------------------

#ifndef _FrameObservable_h
#define _FrameObservable_h

//---------------------------------------------------------------------

#include <windows.h>

#include "Observable.h"
#include "FrameObserver.h"

class FrameObservable : public Observable
{
	public :
        FrameObservable()
        {
            InitializeCriticalSection(&m_criticalSection);
        }
        virtual ~FrameObservable()
        {
            DeleteCriticalSection(&m_criticalSection);
        }

		virtual void NotifyMe( FrameObserver *o )
        { 
            EnterCriticalSection(&m_criticalSection);
            Observable::Attach((Observer*)o);  
            LeaveCriticalSection(&m_criticalSection);
        }
		virtual void DisableNotify( FrameObserver *o )
        { 
            EnterCriticalSection(&m_criticalSection);
            Observable::Detach((Observer*)o);  
            LeaveCriticalSection(&m_criticalSection);
        }

		virtual void Notify(FrameEvent& m)
		{
            EnterCriticalSection(&m_criticalSection);
			if( 0 != m_ObserverVector.size() )
			{
				ObserverIterator i = m_ObserverVector.begin();
				FrameObserver* mo = 0;
				for(; i != m_ObserverVector.end(); i++)
				{
					mo = (FrameObserver*)*i;
					mo->Update( this, m );
				}
			}
            LeaveCriticalSection(&m_criticalSection);
		}
	protected: // hide the base classes' generic attach/detach
		virtual void Attach( Observer * o){ Observable::Attach(o);  }
		virtual void Detach( Observer * o){ Observable::Detach(o);  }
        CRITICAL_SECTION m_criticalSection;

};


//---------------------------------------------------------------------

#endif // _FrameObservable_h