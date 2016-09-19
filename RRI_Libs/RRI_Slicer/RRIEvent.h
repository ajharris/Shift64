// Copyright (c) Robarts Research Institute 2006

//---------------------------------------------------------------------
// RRIEvent.h : interface of the Event class
//---------------------------------------------------------------------
// Purpose: Implementation of the Observer Pattern (GoF)
//---------------------------------------------------------------------

#ifndef RRI_EventH
#define RRI_EventH

class RRIEvent
{
public:


    RRIEvent();//Public constructor
    ~RRIEvent();//Public destructor

    int     GetEventCode() {return m_eventCode;}
    void    SetEventCode(int code) {m_eventCode = code;}

	void    SetControlObject(unsigned char* buffer){m_buffer = buffer;}
	unsigned char*   GetControlObject(){return m_buffer;}



	//new for passing key strokes between objects
	char	GetEventKey(){return m_eventKey;}
	void	SetEventKey(char key){m_eventKey = key;}

	//this could be used to pass an index to a buffer
	long    GetEventValueLong(){return m_eventValueLong;}
	long    SetEventValueLong(long value){m_eventValueLong = value; return 0;}

	void    SetWidth(long width){m_width = width;}
	void    SetHeight(long height){m_height = height;}
	void    SetChannels(long channels){m_channels = channels;}
	long    GetWidth(){return m_width;}
	long    GetHeight(){return m_height;}
	long    GetChannels(){return m_channels;}
    
	//this could be used to pass a pointer to a buffer from the framegrabber
    unsigned char*   m_buffer;
private:
	long    m_eventValueLong;
    int     m_eventCode;
	char	m_eventKey;
	long    m_width;
	long    m_height;
	long    m_channels;
    //NOTE: time stamp should be here


};
#endif