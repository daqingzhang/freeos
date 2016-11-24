enum fsm_route
{
	FSM_ROUTE_R0 = 0,
	FSM_ROUTE_R1,
	FSM_ROUTE_R2,
	FSM_ROUTE_R3,
	FSM_ROUTE_R4,
	FSM_ROUTE_R5,
	FSM_ROUTE_R6,
	FSM_ROUTE_R7,
	FSM_ROUTE_R8,
	FSM_ROUTE_R9,
	FSM_ROUTE_R10,
	FSM_ROUTE_R11,
	FSM_ROUTE_R12,
};

enum fsm_state_val
{
	FSM_STATE_S0 = 0,
	FSM_STATE_S1,
	FSM_STATE_S2,
	FSM_STATE_S3,
	FSM_STATE_S4,
	FSM_STATE_S5,
	FSM_STATE_S6,
	FSM_STATE_S7,
	FSM_STATE_S8,
	FSM_STATE_S9,
	FSM_STATE_S10,
	FSM_STATE_S11,
	FSM_STATE_S12,

};

struct fsm_state
{
	int current;
	int route;
	int next;
};

struct fsm_operations
{
	int (*update)(struct fsm_state *s,int route);
	int (*init)(struct fsm_state *s, struct fsm_state *table[]);
	int (*do_switch)(struct fsm_state *s);
	int (*get_next_state)(struct fsm_state *s);
	int (*get_current_state)(struct fsm_state *s);
	int (*get_route)(struct fsm_state *s);
	int (*print)(struct fsm_state *s);
};

struct fsm_driver
{
	struct fsm_state *s;
	struct fsm_state *table[];
	struct fsm_operations *op;
};

int fsm_driver_register(struct fsm_driver *pdrv,
				struct fsm_state *s,
				struct fsm_state *tbl[],
				struct fsm_operations *op);

