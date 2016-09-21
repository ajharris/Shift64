// Copyright (c) Robarts Research Institute 2006

//---------------------------------------------------------------------
// FrameObserver.h : interface of the Observer class
//---------------------------------------------------------------------
// Purpose: Implementation of the Observer Pattern specific to messages
//---------------------------------------------------------------------

#if !defined(_FrameObserver_h)
#define _FrameObserver_h

//---------------------------------------------------------------------
// Forward declare the class LISMessageObservable
//---------------------------------------------------------------------

class FrameObservable;
class FrameEvent;

//---------------------------------------------------------------------
// Class declaration for Observer
//---------------------------------------------------------------------
class FrameObserver
{
	//-----------------------------------------------------------------
	// Destructor
	//-----------------------------------------------------------------
	public :
		virtual ~FrameObserver(){}

	//-----------------------------------------------------------------
	// Methods
	//-----------------------------------------------------------------
	public :
		virtual bool Update( FrameObservable* op, FrameEvent& m) = 0;

	//-----------------------------------------------------------------
	// Protected constructor
	//-----------------------------------------------------------------
	protected :
		FrameObserver(){}
};

//---------------------------------------------------------------------

#endif // _FrameObserver_h
