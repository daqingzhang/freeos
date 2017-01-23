#include <fsm.h>

#define FSM_ERROR_DEV_NULL 1
#define FSM_ERROR_OPS_NULL 2
#define FSM_ERROR_INITED   3

int fsm_register(struct fsm_device *dev,
		const char *name,
		struct fsm_operations *ops,
		void *priv)
{
	if(!dev)
		return -FSM_ERROR_DEV_NULL;
	if(!ops)
		return -FSM_ERROR_OPS_NULL;
	if(dev->inited)
		return -FSM_ERROR_INITED;

	dev->inited = 1;
	dev->name = name;
	dev->ops  = ops;
	dev->priv = priv;

	return dev->ops->init(&dev->s);
}

int fsm_unregister(struct fsm_device *dev)
{
	dev->inited = 0;
	return 0;
}

int fsm_init(struct fsm_device *dev)
{
	struct fsm_state *s = fsm_device_to_state(dev);
	return dev->ops->init(s);
}

int fsm_update(struct fsm_device *dev)
{
	struct fsm_state *s = fsm_device_to_state(dev);
	return dev->ops->update(s);
}

int fsm_search(struct fsm_device *dev, int route)
{
	struct fsm_state *s = fsm_device_to_state(dev);
	return dev->ops->search(s,route);
}

int fsm_get_next_state(struct fsm_device *dev)
{
	struct fsm_state *s = fsm_device_to_state(dev);
	return dev->ops->get_next_state(s);
}

int fsm_get_current_state(struct fsm_device *dev)
{
	struct fsm_state *s = fsm_device_to_state(dev);
	return dev->ops->get_current_state(s);
}

int fsm_get_route(struct fsm_device *dev)
{
	struct fsm_state *s = fsm_device_to_state(dev);
	return dev->ops->get_route(s);
}

void fsm_print(struct fsm_device *dev)
{
	struct fsm_state *s = fsm_device_to_state(dev);
	dev->ops->print(s);
}
