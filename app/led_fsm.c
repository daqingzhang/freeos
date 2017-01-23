#include <led_task.h>

struct fsm_state led_fsm_map[] =
{
	{.current = STATE_DISP, .route = DISP_TO_DISP, .next = STATE_DISP},
	{.current = STATE_DISP, .route = DISP_TO_DLY,  .next = STATE_DLY},
	{.current = STATE_DLY,  .route = DLY_TO_DLY,   .next = STATE_DLY},
	{.current = STATE_DLY,  .route = DLY_TO_RSP,   .next = STATE_RSP},
	{.current = STATE_RSP,  .route = RSP_TO_RSP,   .next = STATE_RSP},
	{.current = STATE_RSP,  .route = RSP_TO_DISP,  .next = STATE_DISP},
	{.current = STATE_RSP,  .route = RSP_TO_IDLE,  .next = STATE_IDLE},
	{.current = STATE_IDLE, .route = IDLE_TO_IDLE, .next = STATE_IDLE},
	{.current = STATE_IDLE, .route = IDLE_TO_DISP, .next = STATE_DISP},
};

static int led_fsm_search(struct fsm_state *state, int route)
{
	struct fsm_state *p;
	int i, err = -1;

	for(i = 0;i < ARRAY_SIZE(led_fsm_map);i++) {
		p = &led_fsm_map[i];
		if((p->current == state->current) && (p->route == route)) {
			state->next = p->next;
			state->route = route;
			err = 0;
			break;
		}
	}
	return err;
}

static int led_fsm_update(struct fsm_state *state)
{
	if(state->next < 0)
		return -1;

	state->current = state->next;
	return 0;
}

static int led_fsm_get_current_state(struct fsm_state *state)
{
	return state->current;
}

static int led_fsm_get_next_state(struct fsm_state *state)
{
	return state->next;
}

static int led_fsm_get_route(struct fsm_state *state)
{
	return state->route;
}

static int led_fsm_init(struct fsm_state *p)
{
	if(p == NULL)
		return -1;

	p->current = STATE_DISP;
	p->route   = DISP_TO_DISP;
	p->next	   = STATE_DISP;

	return 0;
}

static void led_fsm_print(struct fsm_state *s)
{
	rprintf("\r\nstate: current: %d, r: %d, next: %d\r\n",
			s->current,s->route,s->next);
}

static struct fsm_operations led_fsm_ops =
{
	.init = led_fsm_init,
	.update = led_fsm_update,
	.search = led_fsm_search,
	.get_next_state = led_fsm_get_next_state,
	.get_current_state = led_fsm_get_current_state,
	.get_route = led_fsm_get_route,
	.print = led_fsm_print,
};

static struct fsm_device led_fsm_dev =
{
	.inited = 1,
	.name = "led_fsm_device",
	.priv = (void *)led_fsm_map,
	.ops = &led_fsm_ops,
};

struct fsm_device *led_fsm_get_device(void)
{
	return &led_fsm_dev;
}

