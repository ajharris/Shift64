// Copyright (c) Robarts Research Institute 2006
//---------------------------------------------------------------------
// RRIObservable.h : interface of the subject class for notification
//---------------------------------------------------------------------
// Purpose: Implementation of the Observer Pattern specific to message handling
//---------------------------------------------------------------------

#ifndef _RRI_Observable_h
#define _RRI_Observable_h

//---------------------------------------------------------------------

#include "Observable.h"
#include "RRIObserver.h"

class RRIObservable : public Observable
{
	public :
		virtual void NotifyMe( RRIObserver *o ){ Observable::Attach((Observer*)o);  }
		virtual void DisableNotify( RRIObserver *o ){ Observable::Detach((Observer*)o);  }
 
		virtual void Notify(const RRIEvent& m)
		{
			if( !m_ObserverVector.empty() )
			{
				ObserverIterator i = m_ObserverVector.begin();
				RRIObserver* mo = 0;
				for(; i != m_ObserverVector.end(); i++)
				{
					mo = (RRIObserver*)*i;
					mo->Update( this, m );
				}
			}
		}
	protected: // hide the base classes' generic attach/detach
		virtual void Attach( Observer * o){ Observable::Attach(o);  }
		virtual void Detach( Observer * o){ Observable::Detach(o);  }

};


//---------------------------------------------------------------------

#endif // _RRI_Observable_h