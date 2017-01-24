#ifndef __FSM_H__
#define __FSM_H__

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

enum fsm_comm_route
{
	FSM_COMM_R0 = 0,
	FSM_COMM_R1,
	FSM_COMM_R2,
	FSM_COMM_R3,
	FSM_COMM_R4,
	FSM_COMM_R5,
	FSM_COMM_R6,
	FSM_COMM_R7,
	FSM_COMM_R8,
	FSM_COMM_R9,
	FSM_COMM_R10,
	FSM_COMM_R11,
	FSM_COMM_R12,
	FSM_COMM_R13,
	FSM_COMM_R14,
	FSM_COMM_R15,
};

enum fsm_comm_state
{
	FSM_COMM_S0 = 0,
	FSM_COMM_S1,
	FSM_COMM_S2,
	FSM_COMM_S3,
	FSM_COMM_S4,
	FSM_COMM_S5,
	FSM_COMM_S6,
	FSM_COMM_S7,
	FSM_COMM_S8,
	FSM_COMM_S9,
	FSM_COMM_S10,
	FSM_COMM_S11,
	FSM_COMM_S12,
	FSM_COMM_S13,
	FSM_COMM_S14,
	FSM_COMM_S15,
};

struct fsm_state
{
	int current;
	int route;
	int next;
};

struct fsm_operations
{
	int (*init)(struct fsm_state *s);
	int (*update)(struct fsm_state *s);
	int (*search)(struct fsm_state *s,int route);
	int (*get_next_state)(struct fsm_state *s);
	int (*get_current_state)(struct fsm_state *s);
	int (*get_route)(struct fsm_state *s);
	void (*print)(struct fsm_state *s);
};

struct fsm_device
{
	int inited;
	const char *name;
	void *priv;
	struct fsm_state s;
	struct fsm_operations *ops;
};

#define fsm_device_to_state(dev) (&(dev->s))
#define fsm_device_to_priv(dev) (dev->priv)

int fsm_register(struct fsm_device *dev,
		const char *name,
		struct fsm_operations *ops,
		void *priv);
int fsm_unregister(struct fsm_device *dev);
int fsm_init(struct fsm_device *dev);
int fsm_update(struct fsm_device *dev);
int fsm_search(struct fsm_device *dev,int route);
int fsm_get_next_state(struct fsm_device *dev);
int fsm_get_current_state(struct fsm_device *dev);
int fsm_get_route(struct fsm_device *dev);
void fsm_print(struct fsm_device *dev);

#endif
