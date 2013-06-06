/*
	Blink
	Turns on an LED on for one second, then off for one second, repeatedly.
 
	This example code is in the public domain.
 */

#include "fsm.h"


#define EVENT_X		\
	X(EVENT1)		\
	X(EVENT2)		\
	X(EVENT3)		\
	X(EVENT4)		\
	X(EVENT5)		\

#define X(a) a,
typedef enum 
{
	EVENT_X
	EVENT_MAX
} st_eventid_t;
#undef X


#define STATE_ID_X	\
	X(STATE_ANY)		\
	X(STATE_1)			\
	X(STATE_2)			\

#define X(a) a,
typedef enum
{
	STATE_ID_X
	STATE_ID_MAX
} st_stateid_t;
#undef X



void entryAction0(FSM_APP_CONTEXT a_AppContext);
void entryAction1(FSM_APP_CONTEXT a_AppContext);

void exitAction0(FSM_APP_CONTEXT a_AppContext);
void exitAction2(FSM_APP_CONTEXT a_AppContext);

FSM_BOOL condTrue(FSM_APP_CONTEXT a_AppContext, void* a_pAppMsg);
FSM_BOOL condFalse(FSM_APP_CONTEXT a_AppContext, void* a_pAppMsg);

FSM_BOOL actionTrue(FSM_APP_CONTEXT a_AppContext, void* a_pAppMsg);
FSM_BOOL actionFalse(FSM_APP_CONTEXT a_AppContext, void* a_pAppMsg);

FSM_HANDLE g_Fsm;
FSM_OBJECT_STATE state;
FSM_APP_CONTEXT context;

int _deb = 1;
unsigned int z = 0;

char* l_pObject = NULL;
int led = 13;
int ledVal = LOW;
// the setup routine runs once when you press reset:
void setup() {
	  pinMode(led, OUTPUT);     
  Serial.begin(9600);

	buildFSM();
	fsmSetStartState(&state, STATE_1, STATE_1);
}

// the loop routine runs over and over again forever:
void loop() {
	//if (_deb)
	//	Serial.println("loop");
#if 0
	fsmExecute(g_Fsm, &state, EVENT1, context, NULL);
	fsmExecute(g_Fsm, &state, EVENT3, context, NULL);
	fsmExecute(g_Fsm, &state, EVENT1, context, NULL);
	fsmExecute(g_Fsm, &state, EVENT3, context, NULL);
	fsmExecute(g_Fsm, &state, EVENT1, context, NULL);
	fsmExecute(g_Fsm, &state, EVENT3, context, NULL);
	fsmExecute(g_Fsm, &state, EVENT1, context, NULL);
	fsmExecute(g_Fsm, &state, EVENT3, context, NULL);
	fsmExecute(g_Fsm, &state, EVENT1, context, NULL);
	fsmExecute(g_Fsm, &state, EVENT3, context, NULL);
#else
	fsmExecute(g_Fsm, &state, EVENT1, context, NULL);
  delay(100);
	fsmExecute(g_Fsm, &state, EVENT3, context, NULL);
  delay(100);
#endif
}

void verifyFSMCallback(int iStateId, FSM_VERIFY_ERROR eFsmVerifyErr)
{
}
	
FSM_BOOL buildFSM(void)
{
	FSM_HANDLE l_hState;

	if (NULL == (g_Fsm = fsmCreateFsm()))
	{
		Serial.println("CreateError");
		return FSM_FALSE;
	}

	// ANY STATE
//	l_hState = fsmCreateAnyState(g_Fsm);
//	fsmTransition(l_hState,	EVENT4,		(void*)condTrue,	STATE_2,	actionTrue, actionTrue, FSM_END);

	// STATE_1
	l_hState = fsmCreateState(g_Fsm, STATE_1, entryAction0, exitAction0);
	fsmTransition(l_hState,	EVENT1,	(void*)condFalse,	STATE_2,	actionTrue, FSM_END);
	fsmTransition(l_hState,	EVENT1,	(void*)condTrue,	STATE_2,	actionTrue, FSM_END);
	fsmTransition(l_hState,	EVENT2,	(void*)condTrue,	STATE_2,	actionTrue, FSM_END);
	fsmTransition(l_hState,	EVENT3,	(void*)condFalse,	STATE_2,	actionTrue, FSM_END);
	fsmTransition(l_hState,	EVENT4,	(void*)condFalse,	STATE_2,	actionTrue, FSM_END);
	fsmCatchTransition(l_hState, STATE_2, actionTrue, FSM_END);

	// STATE_2
	l_hState = fsmCreateState(g_Fsm, STATE_2, entryAction1, exitAction1);
	fsmTransition(l_hState,	EVENT3,	(void*)condTrue,			STATE_1,		actionTrue, FSM_END);
	fsmTransition(l_hState,	EVENT4,	FSM_NO_CONDITION,	FSM_ST_SAME,	actionTrue,  FSM_END);
	fsmTransition(l_hState,	EVENT4,	FSM_NO_CONDITION,	STATE_1,		actionTrue, FSM_END);
	fsmCatchTransition(l_hState, STATE_1, actionTrue, FSM_END);

	if (fsmHasCreateErrorOccurred(g_Fsm))
	{
		Serial.println("CreateErrorOccurred");
		return FSM_FALSE;
	}
	if (!fsmVerifyFsm(g_Fsm,verifyFSMCallback))
	{
		Serial.println("VerifyError");
		return FSM_FALSE;
	}

//	Serial.println("Success");
	return FSM_TRUE;
}

FSM_BOOL actionTrue(FSM_APP_CONTEXT a_AppContext, void* a_pAppMsg)
{
  if (z++ == 10000)
{
  ledVal = !ledVal;
  digitalWrite(led, ledVal);   // turn the LED on (HIGH is the voltage level)

  z = 0;
}
	if (_deb)
		Serial.println("actionTrue");
	return FSM_TRUE;
}

FSM_BOOL actionFalse(FSM_APP_CONTEXT a_AppContext, void* a_pAppMsg)
{
	if (_deb)
		Serial.println("actionFalse");
	return FSM_FALSE;
}

void entryAction0(FSM_APP_CONTEXT a_AppContext)
{
	if (_deb)
		Serial.println("entryAction0");
}

void entryAction1(FSM_APP_CONTEXT a_AppContext)
{
	if (_deb)
		Serial.println("entryAction1");
}

void exitAction0(FSM_APP_CONTEXT a_AppContext)
{
	if (_deb)
		Serial.println("exitAction0");
}

void exitAction1(FSM_APP_CONTEXT a_AppContext)
{
	if (_deb)
		Serial.println("exitAction1");
}

FSM_BOOL condTrue(FSM_APP_CONTEXT a_AppContext, void* a_pAppMsg)
{
	if (_deb)
		Serial.println("condTrue");
	return FSM_TRUE;
}

FSM_BOOL condFalse(FSM_APP_CONTEXT a_AppContext, void* a_pAppMsg)
{

	if (_deb)
		Serial.println("condFalse");
	return FSM_FALSE;
}


