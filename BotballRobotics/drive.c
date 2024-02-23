\#include "drive.h"
#include <math.h>
#include <kipr/botball.h>
#include <kipr/wombat.h>
#include <stdio.h>
#include <stdlib.h>


float function_start = 0;
float tme_wait = 0;
float tmeout_tme = 0;
float final_cntr = 0;
int ET_count = 0;

void create_battery_check(){
    float num = get_create_battery_charge();
    float dem = get_create_battery_capacity();
    //printf("Create Battery = %f PER\n", (num / dem)*100);
    printf("Create Battery = %f PER\n", num);
    printf("Create Battery = %f PER\n", dem);
}


struct Zone {
  int x_low;
  int x_high;
  int y_low;
  int y_high;
};


// return whether the robot has driven far enough - return 2 for special cases in which slowing down is required
int cntr_comp() {
    // offset drive distance by constant for global tuning
    final_cntr *= DRIVE_OFFSET;
    if (ROBOT_STATUS) {
        // if driving is complete, stop
        if (sqrt(get_create_distance()*get_create_distance()) > sqrt(final_cntr*final_cntr)) return 1;
        // if a certain proportion of the driving is complete, slow down
        if (sqrt(get_create_distance()*get_create_distance()) > sqrt(final_cntr*final_cntr) * SLOW_TOL_DRIVE) return 2;
        // otherwise, there should be no change
        return 0;
    } else {
        // if driving is complete, stop
        if (sqrt(get_motor_position_counter(MOT_RIGHT)*get_motor_position_counter(MOT_RIGHT)) > sqrt(final_cntr*final_cntr)) return 1;
        // if a certain proportion of the driving is complete, slow down
        if (sqrt(get_motor_position_counter(MOT_RIGHT)*get_motor_position_counter(MOT_RIGHT)) > sqrt(final_cntr*final_cntr) * SLOW_TOL_DRIVE) return 2;
        // otherwise, there should be no change
        return 0;
    }
}   

int tme_comp() {
    return (seconds() - function_start) * 1000.0 > tme_wait;
}

// *---------------------------*
// |     Driving functions     |
// *---------------------------*

void drive(int l_speed, int r_speed) {
	create_drive_direct(l_speed, r_speed);
}

// generic drive forwards function b- drives at speed and stops when cond() is fulfilled
void drive_forward_until(int speed, int cond(), int sens) {
    float left_speed = speed;													
    float right_speed = speed;												
    double offset = 0;		
    // record the starting time of the function for timeout
    function_start = seconds();			
    // clear motor position counters							
    clear_motor_position_counter(MOT_LEFT);						
    clear_motor_position_counter(MOT_RIGHT);	
    set_create_distance(0);
    double prev_tme = seconds();
    // keep driving while the condition is not fulfilled (1) and timeout not triggered
    while(cond() != 1) {
        // find the gyro value based on wallaby orientation
        double val = 0;
        val = (gyro_y() - GYRO_DEV);
        // val /= 2;
        // keep a running offset of the gyro value to know how far off the robot is
        offset += val * (seconds() - prev_tme);
        // printf("val: %f \n", val);
        // printf("offset %f \n", offset);
        prev_tme = seconds();
        double sensitivity = sens ? GYRO_SENS : GYRO_SENS_LOW;
        // recalculate speeds based on how far off the robot is from the drive path
        left_speed = speed - ((float){offset} * sensitivity);
        right_speed = speed + ((float){offset} * sensitivity);
        
       // printf("%f ", left_speed);
       // printf("%f\n", right_speed);

        // slow down if slow down condition triggered (2)
        if (cond() == 2) drive(left_speed / 2, right_speed / 2);
        // otherwise, drive at regular speed
        else drive(left_speed, right_speed);    
        
        msleep(15);
    }
    // stop wheels at end to prevent residual drive momentum
	drive(0, 0);
}

// distance drive forwards function
void drive_forward(int speed, float dist) {
    dist /= 20;
    // scale timeout by distance
    tme_wait = dist * TMEOUT_CONST;
    // convert the distance to a counter based on the kind of robot
    if (ROBOT_STATUS) final_cntr = dist * 10;
    else final_cntr = TICKS_PER_ROT_RIGHT * dist / (WHEEL_DIAMETER * PI);
    // use the counter comparison as the stop condition
    drive_forward_until(speed, cntr_comp, 1);
    drive(0, 0);
}

// generic drive backwards function - drives at speed and stops when cond() is fulfilled
void drive_backward_until(int speed, int cond(), int sens) {
    float left_speed = speed;
    float right_speed = speed;
    float offset = 0;
    // record the starting time of the function for timeout
    function_start = seconds();
    // clear motor position counters
    clear_motor_position_counter(MOT_LEFT);
    clear_motor_position_counter(MOT_RIGHT);
    set_create_distance(0);
    double prev_tme = seconds();
    // keep driving while the condition is not fulfilled (1) and timeout not triggered
    while(cond() != 1) {
        // find the gyro value based on wallaby orientation
        double val = 0;
        val = gyro_y() - GYRO_DEV;
        // val /= 10;
        // keep a running offset of the gyro value to know how far off the robot is
        offset += val * (seconds() - prev_tme);
        prev_tme = seconds();
        // recalculate speeds based on how far off the robot is from the drive path
        double sensitivity = sens ? GYRO_SENS : GYRO_SENS_LOW;
        left_speed = speed + ((float){offset} * sensitivity);
        right_speed = speed - ((float){offset} * sensitivity);
        // slow down if slow down condition triggered (2)
        if (cond() == 2) drive(-left_speed / 2, -right_speed / 2);
        // otherwise, drive at regular speed
        else drive(-left_speed, -right_speed);
        
        msleep(15);
    }
    // stop wheels at end to prevent residual drive momentum
}

// distance drive backwards function
void drive_backward(int speed, float dist) {
    dist /= 20;
    // scale timeout by distance
    tme_wait = dist * TMEOUT_CONST;
    // convert the distance to a counter based on the kind of robot
    if (ROBOT_STATUS) final_cntr = dist * 10;
    else final_cntr = TICKS_PER_ROT_RIGHT * dist / (WHEEL_DIAMETER * PI);
    // use the counter comparison as the stop condition
    drive_backward_until(speed, cntr_comp, 1);
    drive(0, 0);
}

void drive_forward_reg(int speed, float distance) { 
    set_create_distance(0);
    while(sqrt(get_create_distance()*get_create_distance()) < distance) {
        drive(speed, speed);
    }
    drive(0, 0);
}

void drive_backward_reg(int speed, float distance) {
    set_create_distance(0);
    while(sqrt(get_create_distance()*get_create_distance()) < distance) {
        drive(-speed, -speed);
    }
    drive(0, 0);
}


void drive_forward_until_touch(int speed) { //none gyro
    while(digital(TOUCH_PORT) == 0) {	
        drive(speed, speed);
    }
    drive(0, 0);
}

int touchsensor_comp() {
    if (digital(TOUCH_PORT) == 1) return 1;
    return 0;
}

int touch_or_distance_comp() {
    // offset drive distance by constant for global tuning
    final_cntr *= DRIVE_OFFSET;
    if ( (sqrt(get_create_distance()*get_create_distance()) > sqrt(final_cntr*final_cntr)) || (digital(TOUCH_PORT) == 1) ) return 1;
    return 0;
}

void drive_backward_until_touch(int speed) {
    drive_backward_until(speed, touchsensor_comp, 1);
    drive(0, 0);
}

void squareup_until_touch_or_dist(int speed, int dist) { //square up function
    dist /= 20;
    // scale timeout by distance
    tme_wait = dist * TMEOUT_CONST;
    final_cntr = dist * 10;
    // use the counter comparison as the stop condition
    drive_backward_until(speed, touch_or_distance_comp, 1);
    drive_backward(speed, 200);
    drive(0, 0);
}

void drive_backward_until_touch_or_dist(int speed, int dist) { //square up function
    dist /= 20;
    // scale timeout by distance
    tme_wait = dist * TMEOUT_CONST;
    final_cntr = dist * 10;
    // use the counter comparison as the stop condition
    drive_backward_until(speed, touch_or_distance_comp, 1);
    drive(0, 0);
}

void turn_until_touch(int speed, int degrees) {
    while(digital(TOUCH_PORT) == 0) {	
    	turn_until_gyro(speed, -speed, degrees, 0);
    }
    drive(0, 0);
}

int touch_comp() {
    if (get_create_lbump() || get_create_rbump()) return 1;
    return 0;
}


void drive_forward_until_bump(int speed) { 		
    drive_forward_until(speed, touch_comp, 1);
    drive(0, 0);
}

/*
void drive_forward_until_bump(int speed) { 		
    while(!get_create_rbump() && !get_create_lbump()) {   
        drive(speed, speed);
        msleep(5);
    }
    drive(0, 0);
}*/

void drive_forward_until_rbump(int speed) { 		
    while(!get_create_rbump()) {   
        drive(speed, speed);
        msleep(5);
    }
    drive(0, 0);
}


/*
void reg_line_follow_left(int speed, int time){  //not andy wang's
    int ms_count = 0;
    while(ms_count < time){
        if(analog(LIGHT_PORT) > GREY_VAL){
            drive(speed, speed*1.5);
        	msleep(10);
        }
        else{
            drive(speed*1.5, speed);
            msleep(10);
        }
        ms_count+=10;
    }
}

void reg_line_follow_right(int speed, int time){  //not andy wang's
    int ms_count = 0;
    while(ms_count < time){
        if(analog(LIGHT_PORT) > GREY_VAL){
            drive(speed*1.5, speed);
        	msleep(10);
        }
        else{
            drive(speed, speed*1.5);
            msleep(10);
        }
        ms_count+=10;
    }
}

void reg_line_follow_left_until_bump(int speed){  //not andy wang's
    set_create_distance(0);
    while(!get_create_lbump() && !get_create_rbump()) {
        if(analog(LIGHT_PORT) > GREY_VAL){
            drive(speed, speed*1.5);
        	msleep(10);
        }
        else{
            drive(speed*1.5, speed);
            msleep(10);
        }
    }
}

void reg_line_follow_right_until_bump(int speed){  //not andy wang's
    set_create_distance(0);
    while(!get_create_lbump() && !get_create_rbump()) {
        if(analog(LIGHT_PORT) > GREY_VAL){
            drive(speed*1.5, speed);
        	msleep(10);
        }
        else{
            drive(speed, speed*1.5);
            msleep(10);
        }
    }
}
*/

void line_follow_left(int speed, float distance) { 
    set_create_distance(0);
    while(get_create_distance() < distance) {  
        //printf("analog 0: %d \n", analog(0));
        float frac = (analog(LIGHT_PORT) - WHITE_VAL) / (BLACK_VAL - WHITE_VAL);
        drive(speed - (frac - 0.5) * speed * LINE_FOLLOW_SENS, speed + (frac - 0.5) * speed * LINE_FOLLOW_SENS);  
        msleep(5);
		//console_clear();
    }
    drive(0, 0);
}

void line_follow_left_until_bump(int speed) { 
    set_create_distance(0);
    while(!get_create_lbump() && !get_create_rbump()) {  
        float frac = (analog(LIGHT_PORT) - WHITE_VAL) / (BLACK_VAL - WHITE_VAL);
        drive(speed - (frac - 0.5) * speed * LINE_FOLLOW_SENS, speed + (frac - 0.5) * speed * LINE_FOLLOW_SENS);  
        msleep(5);
    }
    drive(0, 0);
}

void line_follow_right(int speed, float distance) { 
    set_create_distance(0);
    while(get_create_distance() < distance) {     
        float frac = (analog(LIGHT_PORT) - WHITE_VAL) / (BLACK_VAL - WHITE_VAL);
        drive(speed + (frac - 0.5) * speed * LINE_FOLLOW_SENS, speed - (frac - 0.5) * speed * LINE_FOLLOW_SENS);
        msleep(5);
    } 
    drive(0, 0);
}

/*
void line_follow_left_until_grey(int speed) { 	//DOESNT WORK	
    while(analog(LIGHT_PORT) > GREY_VAL - 500 && analog(LIGHT_PORT) > GREY_VAL + 500) {
        //printf("%d\n", 5);
        //printf("%d\n", analog(LIGHT_PORT));
        //printf('hi');
        float frac = (analog(LIGHT_PORT) - WHITE_VAL) / (BLACK_VAL - WHITE_VAL);
        drive(speed - (frac - 0.5) * speed * LINE_FOLLOW_SENS, speed + (frac - 0.5) * speed * LINE_FOLLOW_SENS); 
        msleep(5);
    }
    drive(0, 0);
}
*/
void line_follow_until_bump_high_sens(int speed) { 		
    set_create_distance(0);
    while(!get_create_lbump() && !get_create_rbump()) {     
        float frac = (analog(LIGHT_PORT) - WHITE_VAL) / (BLACK_VAL - WHITE_VAL);
        drive(speed - (frac - 0.5) * speed * LINE_FOLLOW_SENS_HIGH, speed + (frac - 0.5) * speed * LINE_FOLLOW_SENS_HIGH); 
        msleep(5);
    }
    drive(0, 0);
}

void line_follow_right_until_bump(int speed) { 		
    set_create_distance(0);
    while(!get_create_lbump() && !get_create_rbump()) {     
        float frac = (analog(LIGHT_PORT) - WHITE_VAL) / (BLACK_VAL - WHITE_VAL);
        drive(speed + (frac - 0.5) * speed * LINE_FOLLOW_SENS, speed - (frac - 0.5) * speed * LINE_FOLLOW_SENS); 
        msleep(5);
    }
    drive(0, 0);
}

void turn_until_gyro(int l_speed, int r_speed, double deg, int slow_down) {
    // remember start of function for timeout
    function_start = seconds();
    double current_gyro_ticks = 0;
    double final_gyro_ticks = deg * GYRO_PER_ROT / 360.0;
    double prev_tme = seconds();
    // while not enough turning done and timeout of 20 seconds
    while(abs(current_gyro_ticks) < abs(final_gyro_ticks)) {
        if (slow_down == 1 && abs(final_gyro_ticks) - abs(current_gyro_ticks) < (abs(l_speed) + abs(r_speed)) / 2.3) 
            drive(l_speed > 0 ? 60 : -60, r_speed > 0 ? 60 : -60);
        else
            drive(l_speed, r_speed);
        // keep track of a runnning gyro offset
        // determine gyro value based on wallaby orientation
        double val = 0;
        //int i;
        // for (i = 0; i < 10; ++i) 
        val = gyro_y() - GYRO_DEV;
        //val /= 10;
        current_gyro_ticks += val * (seconds() - prev_tme);
        prev_tme = seconds();
        msleep(15);
    }
    // stop motors to prevent residual momentum
    drive(0, 0);
    // wait a bit before next movement to prevent residual momentum from affecting other gyro functions
    msleep(50);
}

// turn right
void turn_right(int speed, float degrees) {
    tme_wait = 10000;
    //degrees = (180/9.6);
    turn_until_gyro(speed, -speed, degrees, 0);
    drive(0, 0);
}

void turn_left_90() {
    turn_left(250, 122); //120 //speed was 200
}

void turn_right_90() {
    turn_right(250, 122);     
}

void turn_left(int speed, float degrees) {
    tme_wait = 10000;
    //degrees = degrees * ( 4.75 / 90);
    turn_until_gyro(-speed, speed, degrees, 0);
    drive(0, 0);
}

void turn_left_reg(int speed, int time) {
    drive(-speed, speed);
    msleep(time);
    drive(0, 0);
}

void turn_right_reg(int speed, int time) {
    drive(speed, -speed);
    msleep(time);
    drive(0, 0);
}

void turn_left_90_old() {
    turn_left(150, 1250); //was 1300 on 6/5
    msleep(100);
}

void turn_right_90_old() {
    turn_right(150, 1250); 
    msleep(100);
}

void turn_left_90_med() {
    turn_left(220, 820); 
    msleep(100);
}

void turn_right_90_med() {
    turn_right(220, 820);
    msleep(100);
}


void turn_left_90_fast() { 
    turn_left(300, 605); //used to be 625 //used to be 600
    msleep(100);
}

void turn_right_90_fast() {
    turn_right(300, 605); //used to be 625 //used to be 600
    msleep(100);
}

void turn_left_180() {
    turn_left(300, 1200);
    msleep(100);
}

void turn_right_180() {
    turn_right(300, 1200);
    msleep(100);
}

int line_comp() {
    if (analog(LIGHT_PORT) > BLACK_VAL - (BLACK_VAL-WHITE_VAL)/3) {
        return 1;
    }
    return 0;   
}

int cliff_comp() {
    if (get_create_lfcliff_amt() < THRESHOLD_VAL) {
        return 1;
    }
    return 0;   
}

int cliff_comp_right() {
    if (get_create_rfcliff_amt() < THRESHOLD_VAL) {
        return 1;
    }
    return 0;   
}

void drive_forward_until_line(int speed) { 
    drive_forward_until(speed, line_comp, 1);
    drive(0, 0);
}
   
void drive_forward_until_cliff(int speed) { 
    drive_forward_until(speed, cliff_comp, 1);
    drive(0, 0);
}

void drive_forward_until_cliff_right(int speed) { 
    drive_forward_until(speed, cliff_comp_right, 1);
    drive(0, 0);
}

void drive_backward_until_cliff(int speed) { 
    drive_backward_until(speed, cliff_comp, 1);
    drive(0, 0);
}

void drive_forward_until_line_old(int speed) { 
    while (analog(LIGHT_PORT) < BLACK_VAL - (BLACK_VAL-WHITE_VAL)/3) {  //while (analog(LIGHT_PORT) < BLACK_VAL-WIGGLE_ROOM)
        drive_forward(speed, speed);
    }
    drive(0, 0);
    msleep(100);
}
void drive_forward_until_white(int speed, int port) {
    while (analog(port) > BLACK_VAL) {
        drive(speed, speed);
    }
}

void drive_backward_until_line(int speed) { //i have light sensor on side of robot
    while (analog(LIGHT_PORT) < BLACK_VAL - (BLACK_VAL-WHITE_VAL)/3) {
        drive(-speed, -speed);
    }
    drive(0, 0);
    msleep(100);
}

void drive_forward_until_grey(int speed) {
    while (analog(LIGHT_PORT_2) < WHITE_VAL_BIG+300) { 
        drive(speed, speed);
    }
    drive(0, 0);
    msleep(100);
}

void drive_backward_until_grey(int speed) {
    while (analog(LIGHT_PORT_2) < WHITE_VAL_BIG+300) { 
        drive(-speed, -speed);
    }
    drive(0, 0);
    msleep(100);
}

void turn_left_until_line(int speed) {
    while (analog(LIGHT_PORT) < BLACK_VAL - (BLACK_VAL-WHITE_VAL)/3) {
        drive(-speed, speed);
    }
    drive(0, 0);
    msleep(100);
}

void turn_left_until_white(int speed) {
    while (analog(LIGHT_PORT) > BLACK_VAL) {
        drive(-speed, speed);
    }
    drive(0, 0);
    msleep(100);
}


void turn_right_until_line(int speed) {
    while (analog(LIGHT_PORT) < BLACK_VAL - (BLACK_VAL-WHITE_VAL)/3) {
        drive(speed, -speed);
    }
    drive(0, 0);
    msleep(100);
}
void turn_right_until_white(int speed) {
    while (analog(LIGHT_PORT) > BLACK_VAL) {
        drive(speed, -speed);
    }
    drive(0, 0);
    msleep(100);
}

void turn_left_until_grey(int speed) {
    while (analog(LIGHT_PORT) < WHITE_VAL_BIG+300) {
        drive(-speed, speed);
    }
    drive(0, 0);
    msleep(100);
}

void turn_right_until_grey(int speed) {
    while (analog(LIGHT_PORT) < WHITE_VAL_BIG+300) {
        drive(speed, -speed);
    }
    drive(0, 0);
    msleep(100);
}

void slow_servo(int port, int end_position, int time) {
    int interval = 10;  // wait between each iteration of the loop
    int time_passed = 0;
    int start_position = get_servo_position(port);
    while (time_passed < time) {
        // calculate the position the servo should be at and move it to that position
        set_servo_position(port, start_position + (end_position - start_position) * time_passed / time);
        msleep(interval);
        time_passed += interval;
    }
    set_servo_position(port, end_position);
}

void double_slow_servo(int left_port, int right_port, int l_end_position, int r_end_position, int time) {
    int interval = 10;  // wait between each iteration of the loop
    int time_passed = 0;
    int l_start_position = get_servo_position(left_port);
    int r_start_position = get_servo_position(right_port);
    //printf("L: %d, R: %d", l_start_position, r_start_position);
    while (time_passed < time) {
        // calculate the position the servo should be at and move it to that position
        set_servo_position(left_port, l_start_position - (l_end_position - l_start_position) * time_passed / time); //left (in the back perspective) 
        set_servo_position(right_port, r_start_position + (r_end_position - r_start_position) * time_passed / time); //right
        msleep(interval);
        time_passed += interval;
    }
    set_servo_position(left_port, l_end_position);
    set_servo_position(right_port, r_end_position);
}

void slow_servo_motor(int ports[], int e_pos[], int m_ports[], int me_pos[], int tme) {
    int interval = 10;
    int s_pos[4];
    float c_pos[4];
    int i;
    int ms_pos[4];
    int cnt = 0;
    int n = 0;
    int m = 0;
    function_start = seconds();
    // determine how many servos there are to move
    for (i = 0; i < 4; ++i) if (ports[i] != -1) ++n;
    // determine how many motors there are to move
    for (i = 0; i < 4; ++i) if (m_ports[i] != -1) ++m;
    // remember the starting positions of each motor
    for (i = 0; i < m; ++i) ms_pos[i] = get_motor_position_counter(m_ports[i]);
    // remember the starting positions of each servo
    for (i = 0; i < n; ++i) {
        s_pos[i] = get_servo_position(ports[i]);
        c_pos[i] = s_pos[i];
    }
    // calculate the speed the motors should move at to reach the final position in tme milliseconds with move at velocity (mav)
    for (i = 0; i < m; ++i) mav(m_ports[i], (me_pos[i] - ms_pos[i]) * 1000.0 / tme);
    // while not enough time has passed
    while (interval * cnt < tme) {
        // determine if a motor has moved far enough - if it has, stop it
        for (i = 0; i < m; ++i)
            if ((me_pos[i] - get_motor_position_counter(m_ports[i])) * (me_pos[i] - ms_pos[i]) <= 0) {
                mav(m_ports[i], 0);
            }
        // update the position each servo should be at and move it to that position
        for (i = 0; i < n; ++i) {
            c_pos[i] += (float) {(e_pos[i] - s_pos[i])} / (float) {(tme / interval)};
            set_servo_position(ports[i], (int) {c_pos[i]});
        }
        msleep(interval);
        ++cnt;
    }
}

// andy wang's 2019 code
// move list of motors and servos constantly to destination positions over tme milliseconds
// stores the ports of the servos, the final positions they should go to, 
// the ports of the motors, the final positions they should go to, and the time it should take
void slow_servo_motor2(int ports[], int e_pos[], int m_ports[], int me_pos[], int tme) {
  int interval = 10;
  int s_pos[4];
  float c_pos[4];
  int i;
  int ms_pos[4];
  int cnt = 0;
  int n = 0;
  int m = 0;
  function_start = seconds();
  // determine how many servos there are to move
  for (i = 0; i < 4; ++i) if (ports[i] != -1) ++n;
  // determine how many motors there are to move
  for (i = 0; i < 4; ++i) if (m_ports[i] != -1) ++m;
  // remember the starting positions of each motor
  for (i = 0; i < m; ++i) ms_pos[i] = get_motor_position_counter(m_ports[i]);
  // remember the starting positions of each servo
  for (i = 0; i < n; ++i) {
    s_pos[i] = get_servo_position(ports[i]);
    c_pos[i] = s_pos[i];
  }
  //printf("%d, %d\n", n, m);
  // while not enough time has passed
  while (interval * cnt < tme) {
    // determine if a motor has moved far enough - if it has, stop it
    for (i = 0; i < m; ++i) {
      mav(m_ports[i], (me_pos[i] - ms_pos[i]) * 1000.0 / tme);
      if ((me_pos[i] - get_motor_position_counter(m_ports[i])) * (me_pos[i] - ms_pos[i]) <= 0) {
          //printf("here\n");
          mav(m_ports[i], 0);
      }
    }
    // update the position each servo should be at and move it to that position
    for (i = 0; i < n; ++i) {
      c_pos[i] += (float) {(e_pos[i] - s_pos[i])} / (float) {(tme / interval)};
      set_servo_position(ports[i], (int) {c_pos[i]});
    }
    msleep(interval);
    ++cnt;
  }
  // in case some overshoot due to rounding of floats, set to the end positions at the end
  for (i = 0; i < n; ++i) set_servo_position(ports[i], e_pos[i]);

  //printf("Motor supposed to go to position %d, went to %d\n", me_pos[0], get_motor_position_counter(m_ports[0]));
  int cont = 1;
  double init_tme = seconds();
  for (i = 0; i < m; ++i) ms_pos[i] = get_motor_position_counter(m_ports[i]);
  while (cont && seconds() - init_tme < 0.5) {
    cont = 0;
    for (i = 0; i < m; ++i) {
        mav(m_ports[i], 200 * (get_motor_position_counter(m_ports[i]) < me_pos[i] ? 1 : -1));
        if ((me_pos[i] - get_motor_position_counter(m_ports[i])) * (me_pos[i] - ms_pos[i]) <= 0) {
            //printf("here\n");
            mav(m_ports[i], 0);
        } else cont = 1;
    }
    msleep(2);
  }
  for (i = 0; i < m; ++i) mav(m_ports[i], 0);
  //printf("Motor supposed to go to position %d, went to %d\n", me_pos[0], get_motor_position_counter(m_ports[0]));

}


// *---------------------------*
// |     Effector functions    |
// *---------------------------*

void move_motor(int port, int speed, int position) { 
    if (get_motor_position_counter(port) > position) speed *= -1; 
    mav(port, speed); 
    float start_time = seconds();
    while (speed < 0 && get_motor_position_counter(port) > position && seconds() - start_time < 5) msleep(5); 
    while (speed > 0 && get_motor_position_counter(port) < position && seconds() - start_time < 5) msleep(5); 
    mav(port, 0); 
    drive(0, 0);
}

void slow_close_claw() {
    int s_ports[4] = {2, 3, -1, -1}; 
    int s_position[4] = {450, 1650, -1, -1};
    int m_ports[4] = {-1, -1, -1, -1};
    int m_position[4] = {-1, -1, -1, -1};
    slow_servo_motor(s_ports, s_position, m_ports, m_position, 1000);
}

void move_seesaw_claw_up() {
    int s_ports[4] = {2, 3, -1, -1}; 
    int s_position[4] = {450, 1650, -1, -1};
    int m_ports[4] = {-1, -1, -1, -1};
    int m_position[4] = {-1, -1, -1, -1};
    slow_servo_motor(s_ports, s_position, m_ports, m_position, 1000);
}

void move_seesaw_claw_down() {
    int s_ports[4] = {2, 3, -1, -1}; 
    int s_position[4] = {1720, 30, -1, -1};
    int m_ports[4] = {-1, -1, -1, -1};
    int m_position[4] = {-1, -1, -1, -1};
    slow_servo_motor(s_ports, s_position, m_ports, m_position, 1000);
}

void wrist_reset(int mot_port, int toggle){ //toggle: 1 or -1
    mav(mot_port, toggle * 1125); //used to be 1500
    msleep(100);
    int ticks = 0;
    do{
        ticks = gmpc(mot_port); 
        mav(mot_port, toggle * 750); //used to be 1000
        msleep(50);

        //printf("%d\n", gmpc(mot_port) - ticks);
    } while(abs(gmpc(mot_port) - ticks) > 10);
    //printf("done\n");
    off(mot_port);
    cmpc(mot_port);
    msleep(100);
}

//////// SPECIFIC SERVO MOTIONS ////////
void container_open() {
	set_servo_position(CONTAINER_PORT, CONTAINER_OPEN);   
}   

void container_close() {
    slow_servo(CONTAINER_PORT, CONTAINER_CLOSE, 700); 
	//set_servo_position(CONTAINER_PORT, CONTAINER_CLOSE);   
} 
    
void container_0() {
    set_servo_position(CONTAINER_PORT, CONTAINER_0);
}

void container_1() {
    set_servo_position(CONTAINER_PORT, CONTAINER_1+50);
    msleep(250);
    set_servo_position(CONTAINER_PORT, CONTAINER_1);
}

void container_2() {
    set_servo_position(CONTAINER_PORT, CONTAINER_2+50);
    msleep(250);
    set_servo_position(CONTAINER_PORT, CONTAINER_2);
}

void container_3() {
    set_servo_position(CONTAINER_PORT, CONTAINER_3+70); //25
    msleep(250);
    set_servo_position(CONTAINER_PORT, CONTAINER_3);
}

void container_4() {
    set_servo_position(CONTAINER_PORT, CONTAINER_4+70);//25
    msleep(250);
    set_servo_position(CONTAINER_PORT, CONTAINER_4);
}

void raise_arm() {
    slow_servo(ARM_PORT, ARM_UP, 700); 
}

void raise_arm_fast() {
    slow_servo(ARM_PORT, ARM_UP, 500); 
}


void max_arm() {
    slow_servo(ARM_PORT, ARM_MAX, 700); 
}

void rest_arm() {
    slow_servo(ARM_PORT, ARM_REST, 700);
}

void ringbase_arm() {
    slow_servo(ARM_PORT, ARM_RINGBASE, 700);
}

void horizontal_arm() {
    slow_servo(ARM_PORT, ARM_HORIZONTAL, 700);
}

void flicker_straight() {
	set_servo_position(FLICKER_PORT, FLICKER_STRAIGHT);
}

void flicker_up() {
	set_servo_position(FLICKER_PORT, FLICKER_UP);
}

void flicker_swish() {
	set_servo_position(FLICKER_PORT, FLICKER_SWISH);
}

void flicker_push() {
	set_servo_position(FLICKER_PORT, FLICKER_PUSH);
}

void flicker_drop() {
    set_servo_position(FLICKER_PORT, FLICKER_DROP);
}

void flicker_low_cube() {
    slow_servo(FLICKER_PORT, FLICKER_LOW_CUBE, 150);
}


/*
void raise_arm() { //used to be 1500 for speed, 1000 is faster
	slow_servo_motor2((int []){ARM_LEFT_PORT, ARM_RIGHT_PORT}, (int []){L_ARM_UP, R_ARM_UP}, (int []){3}, (int []){3}, 800);  //3rd and 4th parameter are empty commands
}

void lower_arm() { //used to be 1500 for speed, 1000 is faster
	slow_servo_motor2((int []){ARM_LEFT_PORT, ARM_RIGHT_PORT}, (int []){L_ARM_DOWN, R_ARM_DOWN}, (int []){3}, (int []){3}, 800);  //3rd and 4th parameter are empty commands
}
*/

//////// SPECIFIC MOTOR MOTIONS ////////

void rotator_upright() {
    move_motor(ROTATOR_PORT, 800, ROTATOR_UPRIGHT);
}

void rotator_release() {
    move_motor(ROTATOR_PORT, 800, ROTATOR_RELEASE);
}

void rotator_halfway() {
    move_motor(ROTATOR_PORT, 800, ROTATOR_HALFWAY);
}

void rotator_flip() {
    move_motor(ROTATOR_PORT, 900, ROTATOR_FLIP);
}

void swiper_back() {
    move_motor(SWIPER_PORT, 2000, SWIPER_BACK);
}

void swiper_semi_back() {
    move_motor(SWIPER_PORT, 2000, SWIPER_BACK+10);
}

void swiper_forward() {
    move_motor(SWIPER_PORT, 2000, SWIPER_FORWARD); //1500
}

void swiper_ultra_forward() {
    move_motor(SWIPER_PORT, 2500, SWIPER_ULTRA_FORWARD);
}

void encrypt_arm_down() {
    move_motor(ENCRYPT_ARM_PORT, 800, ENCRYPT_ARM_DOWN);
}

void encrypt_arm_up() {
    move_motor(ENCRYPT_ARM_PORT, 800, ENCRYPT_ARM_UP);
}

void encrypt_arm_level1() {
    move_motor(ENCRYPT_ARM_PORT, 800, ENCRYPT_ARM_LEVEL1);
}

void encrypt_arm_level2() {
    move_motor(ENCRYPT_ARM_PORT, 800, ENCRYPT_ARM_LEVEL2);
}

void encrypt_arm_level3() {
    move_motor(ENCRYPT_ARM_PORT, 800, ENCRYPT_ARM_LEVEL3);
}

void encrypt_arm_cube() {
    move_motor(ENCRYPT_ARM_PORT, 800, ENCRYPT_ARM_CUBE);
}

void encrypt_arm_highcube() {
    move_motor(ENCRYPT_ARM_PORT, 800, ENCRYPT_ARM_HIGHCUBE);
}

void encrypt_arm_vert() {
    move_motor(ENCRYPT_ARM_PORT, 800, ENCRYPT_ARM_VERT);
}

void encrypt_arm_botgal() {
    move_motor(ENCRYPT_ARM_PORT, 800, ENCRYPT_ARM_BOTGAL);
}

void encrypt_arm_drop() {
    move_motor(ENCRYPT_ARM_PORT, 800, ENCRYPT_ARM_DROP);
}

/*
void move_motor_until_bump(int port, int speed, int position) { 
    if (get_motor_position_counter(port) > position) speed *= -1; 
    mav(port, speed); 
    float start_time = seconds();
    while (speed < 0 && get_motor_position_counter(port) > position && seconds() - start_time < 5 && touch_comp() != 1) msleep(50); 
    while (speed > 0 && get_motor_position_counter(port) < position && seconds() - start_time < 5 && touch_comp() != 1) msleep(50); 
    mav(port, 0); 
}

void lower_arm() {
    while(touch_comp() == 0){
        mav(ARM_PORT, -800);
        msleep(10);
    }
    motor_power(ARM_PORT, 0);
    msleep(100);
    cmpc(ARM_PORT);
}

void lower_arm_grabs() {
    int claw_pos = gmpc(CLAW_PORT);
    //semi_claw();
	while(touch_comp() == 0){
        mav(ARM_PORT, -800);
        msleep(10);
    }
    motor_power(ARM_PORT, 0);
    msleep(100);
    cmpc(ARM_PORT);
    move_motor(CLAW_PORT, 800, claw_pos);
    //msleep(500);
}
*/

// drive backwards for a set amount of time, mainly used for squaring up
/*void square_up(int speed, float time) {
  // set the time for the timeout
  tme_wait = time;
  // use the time comparison as the stop condition
  drive_backward_until(speed, tme_comp, 0);
}*/


// *---------------------------*
// |     Startup functions     |
// *---------------------------*

int get_light() {
    return analog(LIGHT_PORT);
}

int get_light2() {
    return analog(LIGHT_PORT_2);
}
/*
void calibrate_sensor(char* message, int sensor_val(), double *var) {
  int time = 1000;
  int interval = 50;
  double sum = 0;
  double i;
  while (!right_button()) {
    display_clear();
    printf("%s\n press r_button when ready\nCurrent values seen: %d\n", message, sensor_val());
    msleep(50);
  }
  for (i = 0; i < time / interval; ++i) {
    display_clear();
    printf("Calculating...\n");
    printf("Current values seen: %d\n", sensor_val());
    sum += sensor_val();
    msleep(interval);
  }
  // gyro deviation is average of all values - sum / count
  *var = sum / i;
}
void calibrate_light() {
  calibrate_sensor("Put light sensor on black", get_light, &BLACK_VAL);
  calibrate_sensor("Put light sensor on white", get_light, &WHITE_VAL);
  calibrate_sensor("Put light sensor on white (for side tophat)", get_light2, &WHITE_VAL_BIG);
}
*/

// Calculate the deviation of the gyro sensor
void calc_dev() {
    drive(0, 0);
    printf("please keep robot still for 7 seconds\n press r_button when ready\n");
    while (!right_button()) {
        msleep(50);
        //printf("%f \n", gyro_z());
    }
    msleep(250);
    printf("calculating...\n");
    int time = 5000;
    int interval = 15;
    double sum = 0;
    double i;
    for (i = 0; i < time / interval; ++i) {
        double val = gyro_y();
        sum += val;
        msleep(interval);
    }
    GYRO_DEV = sum / i;
    printf("average deviation of %f \n", GYRO_DEV);
    drive(0, 0);
}

// tweak area size
int wait_for_light_read(int channel) {
  int checks = 15; 
  int interval = 200;
  int off_val;
  int on_val;
  int tme = 0;
  while (1) {
    while (!right_button()) {
      display_clear();
      printf("Please turn the light on.\nPress right button when ready.\nCurrent value of %d seen\n", analog(LIGHT_START_PORT));
      msleep(30);
    }
    while (right_button()) msleep(30);
    int sm = 0;
    int i = 0;
    for (i = 0; i < 20; ++i) {
      display_clear();
      printf("Calibrating.... Current value of %d.\n", analog(LIGHT_START_PORT));
      sm += analog(LIGHT_START_PORT);
      msleep(50);
    }
    while (!right_button()) {
      display_clear();
      printf("On value of %d\n", on_val);
      printf("Please turn the light off.\nPress right button when ready.\nCurrent value of %d seen\n", analog(LIGHT_START_PORT));
      msleep(30);
    }
    while (right_button()) msleep(30);
    on_val = sm / i;
    sm = 0;
    for (i = 0; i < 20; ++i) {
      display_clear();
      printf("Calibrating.... Current value of %d.\n", analog(LIGHT_START_PORT));
      sm += analog(LIGHT_START_PORT);
      msleep(50);
    }
    off_val = sm / i;
    display_clear();
    printf("Off value of %d\nOn value of %d.\n", off_val, on_val);
    if (on_val > 500 || off_val < 2500 || abs(on_val - off_val) < 2000) {
      printf("Bad calibration! Press right button to redo, A button to override\n");
      while (!a_button() && !right_button()) msleep(50);
      if (right_button()) {
        while (right_button()) msleep(50);
        printf("Redoing calibration!\n");
        msleep(1500);
      } else {
        while (a_button()) msleep(50);
        printf("Overriding the calibration!\n");
        msleep(2000);
        break;
      }
    } else {
      printf("Good calibration!\n");
      msleep(2000);
      break;
    }
  }
    
  int readings[15];
  int i = 0;
  for(i=0; i<15; i++) readings[i] = 0;
  int reading_index = 0;
  int new_detect_count = 0;
  //int i = 0;
  int light_check = 0; //checks for light on
  while (1) {
    if (tme % interval == 0) { //interval = how frequent it updates (ms)
      camera_update(); //gets you the boxes
      int object_cnt = get_object_count(channel);
      new_detect_count = 0;
      int j;
      for (j = 0; j < object_cnt; ++j) { //iterates through the boxes
        printf("area: %d\n", get_object_area(channel, j));
        if (get_object_area(channel, j) < 150) continue; //box size threshold
        //int x_center = get_object_center(channel, j).x;
        //int y_center = get_object_center(channel, j).y;
        //printf("for check %d, for object %d: %d, %d\n", i, j, x_center, y_center);
        ++new_detect_count; //found box that is big enough
      }  
      readings[reading_index] = new_detect_count;
      reading_index = (reading_index + 1) % checks;
    }
    display_clear();
    printf("Currently reading %d\n", analog(LIGHT_START_PORT));
    if (analog(LIGHT_START_PORT) < (double) {off_val - on_val} * 0.7 + on_val) {
      printf("Found a value exceeding %f\n", (double) {off_val - on_val} * 0.7 + on_val);
      ++light_check;
    } else {
      light_check = 0;
    }
    int k = 0;
    int detect_sum = 0;
  	for(k=0; k<15; k++) {
    	detect_sum += readings[k];
    }
    printf("detect sum: %d\n", detect_sum);
    if (light_check >= 3) break;  
    tme += 20;
    msleep(20);
  }
  int k = 0;
  int detect_sum = 0;
  for(k=0; k<15; k++) {
  	detect_sum += readings[k];
  }
  if (detect_sum >= 8) return 1; //detected blue
  else return 0;
}

/*
void all_ring_release() {
    container_0();
    swiper_swipe();
    msleep(2000);
    container_1();
    swiper_swipe();
    msleep(2000);
    container_2();
    swiper_swipe();
    msleep(2000);
    container_3();
    swiper_swipe();
    msleep(2000);
    swiper_swipe();
    msleep(2000);
}
*/

void camera_test() {
  int channel = 0;
  int interval = 200;
  int detect_count = 0; 
  //int i = 0;
  int tme = 0;
  int time_tracker = 0;
  while (time_tracker < 30) { //3 seconds
    if (tme % interval == 0) { //interval = how frequent it updates (ms)
      camera_update(); //gets you the boxes
      int object_cnt = get_object_count(channel);
      //detect_count = 0;
      int j;
      for (j = 0; j < object_cnt; ++j) { //iterates through the boxes
        //printf("area: %d\n", get_object_area(channel, j));
        if (get_object_area(channel, j) < 150) continue; //box size threshold
        //int x_center = get_object_center(channel, j).x;
        //int y_center = get_object_center(channel, j).y;
        //printf("for check %d, for object %d: %d, %d\n", i, j, x_center, y_center);
        ++detect_count; //found box that is big enough
       }
  	  msleep(100);
      time_tracker ++;
    }
  }  
  if (detect_count >= 5) {printf("DETECTED BLUE\n");}
  else {printf("DETECTED GREEN\n");}
}


void start_up() {
    create_connect();
    create_full();
    enable_servo(CONTAINER_PORT);
    rest_arm(); //so it's down once enabled
    enable_servo(ARM_PORT);
    cmpc(SWIPER_PORT);
    cmpc(ROTATOR_PORT); 
    rotator_upright();
    rest_arm();
    msleep(1000);
    container_close();
    calc_dev();
    
    // POST
	drive_backward(100, 100);
    msleep(250);
    turn_right_90();
    msleep(250);
   
    printf("press r_button to test mini servo\n");
    while (!right_button()) {
        msleep(50);
    }
    cmpc(ENCRYPT_ARM_PORT);
    enable_servo(FLICKER_PORT);
    flicker_low_cube();
    msleep(2000);
    disable_servo(FLICKER_PORT);
    msleep(2000);
    enable_servo(FLICKER_PORT);
    flicker_low_cube();
    msleep(2000);
    disable_servo(FLICKER_PORT);

    printf("press r_button to manually reset encrypt arm\n");
    while (!right_button()) {
        msleep(50);
    }
    cmpc(ENCRYPT_ARM_PORT);
    
    //calibrate_light();
    //wait_for_light(WAIT_LIGHT_PORT);
}
