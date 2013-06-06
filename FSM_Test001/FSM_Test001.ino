/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
#include "fsm.h"

FSM_HANDLE g_TelFsm;

#define TEL_CMDEVENT_X				\
	X(TELCMD_MAKECALL)				\
	X(TELCMD_SEND_DIGITS)			\
	X(TELCMD_PLAY_FILES)			\
	X(TELCMD_STOP_PLAY_FILES)		\
	X(TELCMD_DROP)					\
	X(TELEVENT_GCEV_UNBLOCKED)		\
	X(TELEVENT_GCEV_RESETLINEDEV)	\
	X(TELEVENT_GCEV_PROCEEDING)		\
	X(TELEVENT_GCEV_ALERTING)		\
	X(TELEVENT_GCEV_CONNECTED)		\
	X(TELEVENT_GCEV_OFFERED)		\
	X(TELEVENT_GCEV_ACCEPT)			\
	X(TELEVENT_GCEV_ANSWERED)		\
	X(TELEVENT_GCEV_DISCONNECTED)	\
	X(TELEVENT_GCEV_DROPCALL)		\
	X(TELEVENT_GCEV_RELEASECALL)	\
	X(TELEVENT_TDX_PLAY)			\
	X(TELEVENT_TDX_RECORD)			\
	X(TELEVENT_TDX_GETDIG)			\
	X(TELEVENT_TDX_DIAL)			\
	X(TELEVENT_TDX_CALLP)			\
	X(TELEVENT_TDX_CST)				\
	X(TELEVENT_TDX_SETHOOK)			\
	X(TELEVENT_TDX_WINK)			\
	X(TELEVENT_TDX_ERROR)			\
	X(TELEVENT_TDX_PLAYTONE)		\
	X(TELEVENT_TDX_GETR2MF)			\
	X(TELEVENT_TDX_BARGEIN)			\
	X(TELEVENT_TDX_NOSTOP)

#define X(a) a,
typedef enum 
{
	TEL_CMDEVENT_X
	TELEVENT_MAX
} st_telfsm_eventid_t;
#undef X


#define TEL_STATE_ID_X					\
	X(TEL_STATE_START)					\
	X(TEL_STATE_DISC_WAIT)				\
	X(TEL_STATE_UNBLOCKED)				\
	X(TEL_STATE_INBOUNDWAITING)			\
	X(TEL_STATE_WAIT_PROCEEDING)		\
	X(TEL_STATE_WAIT_ALERTING)			\
	X(TEL_STATE_WAIT_CONNECT)			\
	X(TEL_STATE_CONNECTED)				\
	X(TEL_STATE_OFFERING)				\
	X(TEL_STATE_ACCEPTED)				\
	X(TEL_STATE_ANSWERED)				\
	X(TEL_STATE_WAIT_PLAY)				\
	X(TEL_STATE_STOPCHANNEL)			\
	X(TEL_STATE_DISCONNECTED)			\
	X(TEL_STATE_CALL_DROPPED)	

#define X(a) a,
typedef enum
{
	TEL_STATE_ID_X
	TEL_STATE_ID_MAX
} st_telfsm_stateid_t;
#undef X



void entryBlah(FSM_APP_CONTEXT a_AppContext);
void exitBlah(FSM_APP_CONTEXT a_AppContext);
FSM_BOOL condBlah(FSM_APP_CONTEXT a_AppContext, void* a_pAppMsg);
FSM_BOOL actionBlah(FSM_APP_CONTEXT a_AppContext, void* a_pAppMsg);

FSM_OBJECT_STATE state;
FSM_APP_CONTEXT context;

// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);     
  buildTelFSM();  
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);               // wait for a second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);               // wait for a second
  fsmExecute(g_TelFsm, &state, EVENT_TIMEOUT, context, NULL);
}

void verifyTelFSMCallback(int iStateId, FSM_VERIFY_ERROR eFsmVerifyErr)
{
}
	
FSM_BOOL buildTelFSM(void)
{
	FSM_HANDLE l_hState;

	if (NULL == (g_TelFsm = fsmCreateFsm()))
	    return FSM_FALSE;

	// ANY STATE
	l_hState = fsmCreateAnyState(g_TelFsm);
	fsmTransition(l_hState,	TELCMD_DROP, 				FSM_NO_CONDITION,	TEL_STATE_STOPCHANNEL,	actionBlah, FSM_END);
	fsmTransition(l_hState,	TELEVENT_GCEV_DISCONNECTED,	FSM_NO_CONDITION,	TEL_STATE_DISC_WAIT,	FSM_END);

	// TEL_STATE_CALL_CONNECTED
	l_hState = fsmCreateState(g_TelFsm, TEL_STATE_DISC_WAIT, entryBlah, exitBlah);
	fsmTransition(l_hState,	EVENT_TIMEOUT,	FSM_NO_CONDITION,	TEL_STATE_STOPCHANNEL,	actionBlah, FSM_END);

	// TEL_STATE_NULL
	l_hState = fsmCreateState(g_TelFsm, TEL_STATE_START, FSM_NULL_ENTRY, FSM_NULL_EXIT);
	fsmTransition(l_hState,	TELEVENT_GCEV_UNBLOCKED, FSM_NO_CONDITION,   TEL_STATE_UNBLOCKED,  actionBlah, FSM_END);
//	fsmCatchTransition(l_hState, TEL_STATE_ERROR, actionBlah, FSM_END);

	// TEL_STATE_UNBLOCKED
	l_hState = fsmCreateState(g_TelFsm, TEL_STATE_UNBLOCKED, FSM_NULL_ENTRY, FSM_NULL_EXIT);
	fsmTransition(l_hState,	TELEVENT_GCEV_RESETLINEDEV, FSM_NO_CONDITION,   TEL_STATE_INBOUNDWAITING,  actionBlah, actionBlah, FSM_END);

	// TEL_STATE_INBOUNDWAITING
	l_hState = fsmCreateState(g_TelFsm, TEL_STATE_INBOUNDWAITING, FSM_NULL_ENTRY, FSM_NULL_EXIT);
	fsmTransition(l_hState,	TELEVENT_GCEV_RESETLINEDEV, FSM_NO_CONDITION,   TEL_STATE_INBOUNDWAITING,  actionBlah, FSM_END);
	fsmTransition(l_hState,	TELEVENT_GCEV_OFFERED, FSM_NO_CONDITION,   TEL_STATE_OFFERING,  actionBlah, actionBlah, FSM_END);
	fsmTransition(l_hState,	TELCMD_MAKECALL, FSM_NO_CONDITION,   TEL_STATE_WAIT_PROCEEDING,  actionBlah, FSM_END);

	// TEL_STATE_WAIT_PROCEEDING
	l_hState = fsmCreateState(g_TelFsm, TEL_STATE_WAIT_PROCEEDING, FSM_NULL_ENTRY, FSM_NULL_EXIT);
	fsmTransition(l_hState,	TELEVENT_GCEV_PROCEEDING, FSM_NO_CONDITION,   TEL_STATE_WAIT_ALERTING,  FSM_END);

	// TEL_STATE_WAIT_ALERTING
	l_hState = fsmCreateState(g_TelFsm, TEL_STATE_WAIT_ALERTING, FSM_NULL_ENTRY, FSM_NULL_EXIT);
	fsmTransition(l_hState,	TELEVENT_GCEV_ALERTING, FSM_NO_CONDITION,   TEL_STATE_WAIT_CONNECT,  FSM_END);
	fsmTransition(l_hState,	TELEVENT_GCEV_CONNECTED, FSM_NO_CONDITION,	TEL_STATE_CONNECTED,  actionBlah, actionBlah, FSM_END);

	// TEL_STATE_WAIT_CONNECT
	l_hState = fsmCreateState(g_TelFsm, TEL_STATE_WAIT_CONNECT, FSM_NULL_ENTRY, FSM_NULL_EXIT);
	fsmTransition(l_hState,	TELEVENT_GCEV_CONNECTED, FSM_NO_CONDITION,	TEL_STATE_CONNECTED,  actionBlah, actionBlah, FSM_END);

	// TEL_STATE_CALL_CONNECTED
	l_hState = fsmCreateState(g_TelFsm, TEL_STATE_CONNECTED, entryBlah, exitBlah);
//	fsmTransition(l_hState,	TELCMD_DROP, FSM_NO_CONDITION,  TEL_STATE_STOPCHANNEL,  actionBlah, FSM_END);
	fsmTransition(l_hState,	EVENT_TIMEOUT, FSM_NO_CONDITION,   TEL_STATE_DISCONNECTED,  actionBlah, FSM_END);
	fsmTransition(l_hState,	TELCMD_PLAY_FILES, FSM_NO_CONDITION,   FSM_ST_SAME,  actionBlah, actionBlah, FSM_END);
	fsmTransition(l_hState,	TELCMD_STOP_PLAY_FILES, FSM_NO_CONDITION,   FSM_ST_SAME,  actionBlah, FSM_END);
	fsmTransition(l_hState,	TELEVENT_TDX_CST, FSM_NO_CONDITION,   FSM_ST_SAME,  actionBlah, FSM_END);
	fsmTransition(l_hState,	TELEVENT_TDX_PLAY, (void*)condBlah,   FSM_ST_SAME,  actionBlah, actionBlah, FSM_END);
	fsmTransition(l_hState,	TELEVENT_TDX_PLAY, (void*)condBlah,   FSM_ST_SAME,  actionBlah, FSM_END);
	fsmTransition(l_hState,	TELEVENT_TDX_PLAY, (void*)condBlah,   TEL_STATE_DISCONNECTED,  actionBlah, actionBlah, FSM_END);

	// TEL_STATE_OFFERING
	l_hState = fsmCreateState(g_TelFsm, TEL_STATE_OFFERING, entryBlah, FSM_NULL_EXIT);
	fsmTransition(l_hState,	TELEVENT_GCEV_ACCEPT, (void *)condBlah,   FSM_ST_SAME,  actionBlah, FSM_END);
	fsmTransition(l_hState,	TELEVENT_GCEV_ACCEPT, (void *)condBlah,   TEL_STATE_DISCONNECTED,  actionBlah, FSM_END);
	fsmTransition(l_hState,	TELEVENT_GCEV_ACCEPT, FSM_NO_CONDITION,   TEL_STATE_ACCEPTED,  actionBlah, FSM_END);

	// TEL_STATE_ACCEPTED
	l_hState = fsmCreateState(g_TelFsm, TEL_STATE_ACCEPTED, FSM_NULL_ENTRY, FSM_NULL_EXIT);
//	fsmTransition(l_hState,	TELEVENT_GCEV_ANSWERED, (void *)condBlah,   TEL_STATE_ANSWERED,  actionBlah, actionBlah, FSM_END);
	fsmTransition(l_hState,	TELEVENT_GCEV_ANSWERED, FSM_NO_CONDITION,   TEL_STATE_ANSWERED,  actionBlah, FSM_END);

	// TEL_STATE_CALL_ANSWERED
	l_hState = fsmCreateState(g_TelFsm, TEL_STATE_ANSWERED, FSM_NULL_ENTRY, FSM_NULL_EXIT);
//	fsmTransition(l_hState,	TELCMD_DROP, FSM_NO_CONDITION,   TEL_STATE_DISCONNECTED,  actionBlah, FSM_END);
	fsmTransition(l_hState,	TELCMD_SEND_DIGITS, FSM_NO_CONDITION,   FSM_ST_SAME,  actionBlah, FSM_END);
	fsmTransition(l_hState,	TELCMD_PLAY_FILES, FSM_NO_CONDITION,   FSM_ST_SAME,  actionBlah, FSM_END);
	fsmTransition(l_hState,	TELCMD_STOP_PLAY_FILES, FSM_NO_CONDITION,   FSM_ST_SAME,  actionBlah, FSM_END);
	fsmTransition(l_hState,	TELEVENT_TDX_DIAL, FSM_NO_CONDITION,   FSM_ST_SAME,  actionBlah, FSM_END);
	fsmTransition(l_hState,	TELEVENT_TDX_CST, FSM_NO_CONDITION,   FSM_ST_SAME,  actionBlah, FSM_END);
	fsmTransition(l_hState,	TELEVENT_TDX_PLAY, (void*)condBlah,   FSM_ST_SAME,  actionBlah, actionBlah, FSM_END);
	fsmTransition(l_hState,	TELEVENT_TDX_PLAY, (void*)condBlah,   FSM_ST_SAME,  actionBlah, FSM_END);
	fsmTransition(l_hState,	TELEVENT_TDX_PLAY, (void*)condBlah,   TEL_STATE_DISCONNECTED,  actionBlah, actionBlah, FSM_END);

	// TEL_STATE_WAIT_PLAY
//	l_hState = fsmCreateState(g_TelFsm, TEL_STATE_WAIT_PLAY, FSM_NULL_ENTRY, FSM_NULL_EXIT);
//	fsmTransition(l_hState,	TELEVENT_TDX_PLAY, (void*)condBlah,   FSM_ST_SAME,  actionBlah, actionBlah, FSM_END);
//	fsmTransition(l_hState,	TELEVENT_TDX_PLAY, FSM_NO_CONDITION,   TEL_STATE_DISCONNECTED,  actionBlah, actionBlah, FSM_END);

	// TEL_STATE_STOPCHANNEL
	l_hState = fsmCreateState(g_TelFsm, TEL_STATE_STOPCHANNEL, FSM_NULL_ENTRY, FSM_NULL_EXIT);
	fsmTransition(l_hState,	TELEVENT_TDX_NOSTOP, FSM_NO_CONDITION,   TEL_STATE_DISCONNECTED,  actionBlah, actionBlah, FSM_END);
	fsmTransition(l_hState,	TELEVENT_TDX_PLAY,   FSM_NO_CONDITION,   TEL_STATE_DISCONNECTED,  actionBlah, actionBlah, FSM_END);


	// TEL_STATE_DISCONNECTED
	l_hState = fsmCreateState(g_TelFsm, TEL_STATE_DISCONNECTED, FSM_NULL_ENTRY, FSM_NULL_EXIT);
	fsmTransition(l_hState,	TELEVENT_GCEV_DROPCALL, FSM_NO_CONDITION,   TEL_STATE_CALL_DROPPED,  actionBlah, actionBlah, FSM_END);

	// TEL_STATE_CALL_DROPPED
	l_hState = fsmCreateState(g_TelFsm, TEL_STATE_CALL_DROPPED, FSM_NULL_ENTRY, FSM_NULL_EXIT);
	fsmTransition(l_hState,	TELEVENT_GCEV_RELEASECALL, FSM_NO_CONDITION,   TEL_STATE_INBOUNDWAITING,  actionBlah, FSM_END);


	if (fsmHasCreateErrorOccurred(g_TelFsm))
	{
		return FSM_FALSE;
	}
	if (!fsmVerifyFsm(g_TelFsm,verifyTelFSMCallback))
	{
		return FSM_FALSE;
	}

	return FSM_TRUE;
}

FSM_BOOL actionBlah(FSM_APP_CONTEXT a_AppContext, void* a_pAppMsg)
{
	return FSM_TRUE;
}

void entryBlah(FSM_APP_CONTEXT a_AppContext)
{
}

void exitBlah(FSM_APP_CONTEXT a_AppContext)
{
}

FSM_BOOL condBlah(FSM_APP_CONTEXT a_AppContext, void* a_pAppMsg)
{
	return FSM_TRUE;
}

