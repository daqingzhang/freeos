#ifndef __MOTOR_H__
#define __MOTOR_H__

void motor_init(void);
void motor_brake(void);
void motor_rotate(int step8, int clockwise);
void motor_test(void);

#endif /* __MOTOR_H__ */
