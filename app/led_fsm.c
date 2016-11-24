#include <led_task.h>

struct LedMsgStateType LedMsgStateTable[] = 
{
	{.current = STATE_DISP, .r = DISP_TO_DISP, .next = STATE_DISP},
	{.current = STATE_DISP, .r = DISP_TO_DLY,  .next = STATE_DLY},
	{.current = STATE_DLY,  .r = DLY_TO_DLY,   .next = STATE_DLY},
	{.current = STATE_DLY,  .r = DLY_TO_RSP,   .next = STATE_RSP},
	{.current = STATE_RSP,  .r = RSP_TO_RSP,   .next = STATE_RSP},
	{.current = STATE_RSP,  .r = RSP_TO_DISP,  .next = STATE_DISP},
	{.current = STATE_RSP,  .r = RSP_TO_IDLE,  .next = STATE_IDLE},
	{.current = STATE_IDLE, .r = IDLE_TO_IDLE, .next = STATE_IDLE},
	{.current = STATE_IDLE, .r = IDLE_TO_DISP, .next = STATE_DISP},
};

int xLedMsgStateUpdate(struct LedMsgStateType *state, enum LedFsmRouteType route)
{
	struct LedMsgStateType *p;
	int i, err = -1;

	for(i = 0;i < ARRAY_SIZE(LedMsgStateTable);i++) {
		p = &LedMsgStateTable[i];
		if((p->current == state->current) && (p->r == route)) {
			state->next = p->next;
			state->r = route;
			err = 0;
			break;
		}
	}
	return err;
}

int xLedMsgStateSwitch(struct LedMsgStateType *state)
{
	if(state->next < 0)
		return -1;

	state->current = state->next;
	return 0;
}

int xLedMsgCurStateGet(struct LedMsgStateType *state)
{
	return state->current;
}

int xLedMsgNextStateGet(struct LedMsgStateType *state)
{
	return state->next;
}

int xLedMsgStateInit(struct LedMsgStateType *p)
{
	if(p == NULL)
		return -1;

	p->current = STATE_DISP;
	p->r	   = DISP_TO_DISP;
	p->next	   = STATE_DISP;

	return 0;
}

int xLedMsgStatePrint(struct LedMsgStateType *p)
{
	rprintf("\r\nstate: current: %d, r: %d, next: %d\r\n",
			p->current,p->r,p->next);
	return 0;
}
