#ifndef __MOTOR_H__
#define __MOTOR_H__

void motor_init(void);
void motor_brake(void);
void motor_rotate(int step8, int clockwise);
void motor_test(void);

struct motor_operations {

};

enum motor_dir {
	MTR_DIR_FORWARD = 0,
	MTR_DIR_BACKWARD,
};

struct motor_dev {
	u8 id;
	u8 dir;
	u16 speed;
	const char *name;
	struct motor_operations *ops;
};


#endif /* __MOTOR_H__ */
