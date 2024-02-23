//SEEDING CODE

#include <kipr/botball.h>

// adjust GYRO based on the wallaby orientation
//   -1, 1 for gyro_x()
//   -2, 2 for gyro_y()
//   -3, 3 for gyro_z()
#define GYRO 1
#define MOT_LEFT -1
#define MOT_RIGHT -1

// 0 for wallaby, 1 for create
#define ROBOT_STATUS 1

#define GYRO_PER_ROT 1980 //2600 //21000 //1850
double gyro_dev; 
double GYRO_DEV;
#define GYRO_SENS 2.0 //0.8 //TRY 3 or 2.5
#define GYRO_SENS_LOW 0.5  

#define LIGHT_START_PORT 5 

// MOVEMENT OFFSETS
#define LEFT_OFFSET 1
#define RIGHT_OFFSET 1
#define DRIVE_OFFSET 1

// ADJUST BASED ON ROBOT FOR ACCURACY OF MOVEMENT
#define TMEOUT_CONST 600
#define SLOW_TOL_TURN 1   // proportion of turn that should be complete before slowing
#define SLOW_TOL_DRIVE 1  // proportion of drive that should be complete before slowing
#define WHEEL_DIAMETER 7
#define TICKS_PER_ROT_LEFT 1500
#define TICKS_PER_ROT_RIGHT 1500
#define DRIVE_SENS 10.0
#define PI 3.14159265

//servo ports
#define FLICKER_PORT 3 //mini servo
#define ARM_PORT 2 //port 1 broken
#define CONTAINER_PORT 0

// servo values
#define ARM_REST 2000 
#define ARM_RINGBASE ARM_REST-400  
#define ARM_HORIZONTAL ARM_REST - 850
#define ARM_UP 300
#define ARM_MAX 50

#define CONTAINER_OPEN 2000
#define CONTAINER_CLOSE 800
#define CONTAINER_0 CONTAINER_CLOSE + 30
#define CONTAINER_1 CONTAINER_CLOSE + 50
#define CONTAINER_2 CONTAINER_CLOSE + 95
#define CONTAINER_3 CONTAINER_CLOSE + 150
#define CONTAINER_4 CONTAINER_CLOSE + 210

// motor ports
#define ROTATOR_PORT 3 
#define ENCRYPT_ARM_PORT 0
#define SWIPER_PORT 1

// motor values
#define ROTATOR_UPRIGHT 0 
#define ROTATOR_RELEASE -20
#define ROTATOR_HALFWAY ROTATOR_FLIP + 400
#define ROTATOR_FLIP -850  

#define SWIPER_BACK 0 
#define SWIPER_FORWARD 360 
#define SWIPER_ULTRA_FORWARD 430 //450

//flicker: negative moves it toward front of robot
#define FLICKER_STRAIGHT 850 //BASE (horizontal w arm)
#define FLICKER_UP FLICKER_STRAIGHT - 200 
#define FLICKER_LOW_CUBE FLICKER_STRAIGHT + 170 //+130
#define FLICKER_SWISH FLICKER_STRAIGHT + 400   // not used FLICKER_LOW_CUBE - 130

#define FLICKER_PUSH FLICKER_STRAIGHT - 280  //-240
#define FLICKER_DROP FLICKER_PUSH - 180 //-80 //-120

#define ENCRYPT_ARM_DOWN 0 //setup position //10
#define ENCRYPT_ARM_UP 1400 
#define ENCRYPT_ARM_VERT 700
#define ENCRYPT_ARM_CUBE 310 //BASE
#define ENCRYPT_ARM_HIGHCUBE ENCRYPT_ARM_CUBE+130  //+110
#define ENCRYPT_ARM_LEVEL1 ENCRYPT_ARM_CUBE - 150 
#define ENCRYPT_ARM_LEVEL2 ENCRYPT_ARM_CUBE - 70
#define ENCRYPT_ARM_LEVEL3 ENCRYPT_ARM_CUBE - 30

#define ENCRYPT_ARM_BOTGAL 900 //900
#define ENCRYPT_ARM_DROP ENCRYPT_ARM_BOTGAL + 20 //drop needs to make arm go up 

// sensor ports
#define TOUCH_PORT 9
#define LIGHT_PORT 0
#define LIGHT_PORT_2 1
#define WAIT_LIGHT_PORT 3

// line follow
#define WHITE_VAL 3000
#define BLACK_VAL 4000
#define WHITE_VAL_BIG 250
#define THRESHOLD_VAL 2000
//double BLACK_VAL;
//double WHITE_VAL; 600
//double WHITE_VAL_BIG;
#define WIGGLE_ROOM 500 //for drive until grey functions, it will drive until sensor is > GREY_VAL - WIGGLE_ROOM
#define LINE_FOLLOW_SENS 0.4
#define LINE_FOLLOW_SENS_HIGH 1.0



//----- FUNCTIONS -----//
double create_forward_gyro(int speed, double offset);
int back_line_comp();
void create_battery_check();

// driving
void drive(int l_speed, int r_speed);
void drive_forward(int speed, float distance);
void drive_backward(int speed, float distance);
void drive_forward_reg(int speed, float distance);
void drive_backward_reg(int speed, float distance);
void drive_forward_gyro(int speed, float distance);
void drive_forward_until_line(int speed);
void drive_forward_until_line_old(int speed);
void drive_forward_until_grey(int speed);
void drive_forward_until_white(int speed, int port);
void drive_backward_until_line(int speed);
void drive_forward_until_bump(int speed);
void drive_forward_until_rbump(int speed);
void drive_forward_until_touch(int speed);
void drive_backward_until_touch(int speed);
void back_drive_backward_until_line(int speed);
void drive_forward_until_cliff(int speed);
void drive_backward_until_cliff(int speed);
void drive_forward_until_cliff_right(int speed);

void drive_forward_until_grey(int speed);
void drive_backward_until_grey(int speed);

void square_up(int speed, float time);
void drive_backward_until_touch_or_dist(int speed, int dist);
void squareup_until_touch_or_dist(int speed, int dist);

// turning
void turn_until_gyro(int l_speed, int r_speed, double deg, int slow_down);
void turn_left(int speed, float degrees);
void turn_right(int speed, float degrees);
void turn_left_reg(int speed, int time);
void turn_right_reg(int speed, int time);
void turn_left_90();
void turn_right_90();
void turn_left_90_old();
void turn_right_90_old();
void turn_left_90_fast();
void turn_right_90_fast();
void turn_right_90_med();
void turn_left_90_med();
void turn(int l_speed, int r_speed, double degrees);
void turn_left_gyro(int speed, float degrees);
void turn_right_gyro(int speed, float degrees);
void turn_left_stupid(int speed, int time);
void turn_right_stupid(int speed, int time);
void turn_left_until_line(int speed);
void turn_left_until_white(int speed);
void turn_right_until_line(int speed);
void turn_right_until_white(int speed);
void back_turn_right_until_line(int speed);
void back_turn_right_until_white(int speed);
void back_turn_left_until_line(int speed);
void back_turn_left_until_white(int speed);
void turn_left_until_grey(int speed);
void turn_right_until_grey(int speed);
void turn_left_180();
void turn_right_180();
void turn_until_touch(int speed, int degrees);

// line
void line_follow_left(int speed, float distance);
void line_follow_right(int speed, float distance);
void reg_line_follow_left(int speed, int time);
void reg_line_follow_right(int speed, int time);
void reg_line_follow_left_until_bump(int speed);
void reg_line_follow_right_until_bump(int speed);
void backward_line_follow_left_until_bump(int speed);
void line_follow_right_until_bump(int speed);
void line_follow_left_until_bump(int speed);
void line_follow_until_bump_high_sens(int speed);
void backward_line_follow_left(int speed, float distance);
void backward_line_follow_right(int speed, float distance);
void line_follow_left_until_grey(int speed);

// general servos and motors
void move_motor(int port, int speed, int pos);
void move_motor_until_bump(int port, int speed, int position);
void slow_servo(int port, int end_position, int time);
void slow_servo_position(int pos, int inc, int port);
void slow_servo_motor(int ports[], int e_pos[], int m_ports[], int me_pos[], int tme);
void double_slow_servo(int left_port, int right_port, int l_end_position, int r_end_position, int time);
void slow_servo_motor2(int ports[], int e_pos[], int m_ports[], int me_pos[], int tme); // i use this
void wrist_reset(int mot_port, int toggle);

// specific servo motions
void short_building_arm();
void tall_building_arm();
void rest_arm();
void horizontal_arm();
void ringbase_arm();
void raise_arm();
void raise_arm_fast();
void max_arm();

void container_open();
void container_close();
void container_0();
void container_1();
void container_2();
void container_3();
void container_4();

void flicker_straight();
void flicker_up();
void flicker_low_cube();
void flicker_push();
void flicker_drop();
void flicker_swish();

// specific motor motions
void swiper_semi_back();
void swiper_back();
void swiper_forward();
void swiper_ultra_forward();

void rotator_upright();
void rotator_flip();
void rotator_release();
void rotator_halfway(); 

void encrypt_arm_down();
void encrypt_arm_up();
void encrypt_arm_swish();
void encrypt_arm_level1();
void encrypt_arm_level2();
void encrypt_arm_level3();
void encrypt_arm_vert();
void encrypt_arm_cube();
void encrypt_arm_highcube();
void encrypt_arm_botgal();
void encrypt_arm_drop();

// start up
void calc_dev();
void start_up();
int get_zone_start();
int get_zone_centers();
int wait_for_light_read(int channel);
void camera_test();
