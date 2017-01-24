#include <fsm.h>

#define FSM_ERROR_DEV_NULL 1
#define FSM_ERROR_OPS_NULL 2
#define FSM_ERROR_INITED   3

/**
 * fsm_register() - Register a new fsm device
 * @dev :	Device pointer will be registered
 * @name:	Name string of device
 * @ops :	Operations pointer to the fsm device
 * @priv:	Pointer to private data
 *
 * @return:	0(success) or -1(failed)
 */
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

	dev->inited = TRUE;
	dev->name = name;
	dev->ops  = ops;
	dev->priv = priv;

	return dev->ops->init(&dev->s);
}

/**
 * fsm_unregister() - Unregister a fsm device
 * @dev :	Device pointer will be unregistered
 *
 * @return:	0(success) or -1(failed)
 */
int fsm_unregister(struct fsm_device *dev)
{
	dev->inited = FALSE;
	return 0;
}

/**
 * fsm_init() - Initialize the original state of fsm device
 * @dev :	Device pointer will be initialized
 *
 * @return:	0(success) or -1(failed)
 */
int fsm_init(struct fsm_device *dev)
{
	struct fsm_state *s = fsm_device_to_state(dev);
	return dev->ops->init(s);
}

/**
 * fsm_update() - Change the current state into a new state
 * @dev :	Device pointer will be updated
 *
 * @return:	0(success) or -1(failed)
 */
int fsm_update(struct fsm_device *dev)
{
	struct fsm_state *s = fsm_device_to_state(dev);
	return dev->ops->update(s);
}

/**
 * fsm_search() - Search the next state by the route
 * @dev :	Device pointer
 * @route:	Route value for finding next state
 *
 * @return:	0(success) or -1(failed)
 */
int fsm_search(struct fsm_device *dev, int route)
{
	struct fsm_state *s = fsm_device_to_state(dev);
	return dev->ops->search(s,route);
}

/**
 * fsm_get_next_state() - Get the next state value
 * @dev :	Device pointer
 *
 * @return:	Next state value
 */
int fsm_get_next_state(struct fsm_device *dev)
{
	struct fsm_state *s = fsm_device_to_state(dev);
	return dev->ops->get_next_state(s);
}

/**
 * fsm_get_current_state() - Get the current state value
 * @dev :	Device pointer
 *
 * @return:	Current state value
 */
int fsm_get_current_state(struct fsm_device *dev)
{
	struct fsm_state *s = fsm_device_to_state(dev);
	return dev->ops->get_current_state(s);
}

/**
 * fsm_get_route() - Get the route value
 * @dev :	Device pointer
 *
 * @return:	Route value
 */
int fsm_get_route(struct fsm_device *dev)
{
	struct fsm_state *s = fsm_device_to_state(dev);
	return dev->ops->get_route(s);
}

/**
 * fsm_print() - Print fsm state value
 * @dev :	Device pointer
 */
void fsm_print(struct fsm_device *dev)
{
	struct fsm_state *s = fsm_device_to_state(dev);
	dev->ops->print(s);
}
