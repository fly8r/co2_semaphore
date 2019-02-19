/*
 * messages.c
 *
 * Created: 23.01.2019 10:44:25
 *  Author: fly8r
 */
#include "include/messages.h"

/************************************************************************/
/* VARS                                                                 */
/************************************************************************/
typedef struct {
	uint8_t Msg;
	void * pParams;
} MSG_DATA_t;

volatile	MSG_DATA_t	Messages[MAX_MESSAGES];

#ifdef USE_BC_MESSAGES
volatile uint8_t BroadcastMessages[MAX_BC_MESSAGES];
#endif



/************************************************************************/
/* FUNCTIONS                                                            */
/************************************************************************/
void InitMessages(void)
{
	for(uint8_t i=0;i<MAX_MESSAGES;i++) {
		Messages[i].Msg = MSG_STATE_INACTIVE;
	}
	#ifdef USE_BC_MESSAGES
	for (uint8_t i=0; i<MAX_BC_MESSAGES; i++) {
		BroadcastMessages[i] = MSG_STATE_INACTIVE;
	}
	#endif
}

void SendMessageWParam(uint8_t Msg, void * pParams)
{
	if(Messages[Msg].Msg == MSG_STATE_INACTIVE) {
		Messages[Msg].Msg = MSG_STATE_SET;
		Messages[Msg].pParams = pParams;
	}
}

void SendMessageWOParam(uint8_t Msg)
{
	if(Messages[Msg].Msg == MSG_STATE_INACTIVE) {
		Messages[Msg].Msg = MSG_STATE_SET;
	}
}

uint8_t GetMessage(uint8_t Msg)
{
	if(Messages[Msg].Msg == MSG_STATE_ACTIVE) {
		Messages[Msg].Msg = MSG_STATE_INACTIVE;
		return 1;
	}
	return 0;
}

void * GetMessageParam(uint8_t Msg)
{
	return Messages[Msg].pParams;
}

#ifdef USE_BC_MESSAGES
void SendBCMessage(uint8_t Msg)
{
	if (BroadcastMessages[Msg] == MSG_STATE_INACTIVE) {
		BroadcastMessages[Msg] = MSG_STATE_SET;
	}
}

uint8_t GetBCMessage(uint8_t Msg)
{
	if (BroadcastMessages[Msg] == MSG_STATE_ACTIVE) {
		return 1;
	}
	return 0;
}
#endif


void ProcessMessages(void)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		for (uint8_t i=0; i<MAX_MESSAGES; i++) {
			if (Messages[i].Msg == MSG_STATE_ACTIVE) Messages[i].Msg = MSG_STATE_INACTIVE;
			if (Messages[i].Msg == MSG_STATE_SET) Messages[i].Msg = MSG_STATE_ACTIVE;
		}
		#ifdef USE_BC_MESSAGES
		for (uint8_t i=0; i<MAX_BC_MESSAGES; i++) {
			if (BroadcastMessages[i] == MSG_STATE_ACTIVE) BroadcastMessages[i] = MSG_STATE_INACTIVE;
			if (BroadcastMessages[i] == MSG_STATE_SET) BroadcastMessages[i] = MSG_STATE_ACTIVE;
		}
		#endif
	}
}
