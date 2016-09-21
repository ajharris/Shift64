// Copyright (c) Robarts Research Institute 2006

#ifndef FrameEventH
#define FrameEventH

class FrameEvent
{
public:
	const static int EV_FRAME_COMPLETE = 31001;
	const static int EV_FRAME_TIMEOUT = 31002;
	const static int EV_FG_MOUSE_UP = 31003;

    FrameEvent(){}
    ~FrameEvent(){}

    int GetEventCode() const {return m_eventCode;}
    void SetEventCode(int code) {m_eventCode = code;}
	double GetTimeStamp() {return m_timeStamp;}
	void SetTimeStamp(double time){m_timeStamp = time;}

	void SetBuffer(unsigned char* buffer){m_buffer = buffer;}
    void SetObject( void* object){m_object = object;}
	unsigned char* GetBuffer(){return m_buffer;}
    void* GetObject(){return m_object;}
    
private:
    int m_eventCode;
	double m_timeStamp;
	double m_timerFrequency;
	unsigned char* m_buffer;
    void* m_object;
};
#endif