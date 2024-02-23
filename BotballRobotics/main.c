#include <kipr/botball.h>
#include <kipr/wombat.h>
#include <drive.h>
#include <kipr/thread.h>

int turn90 = 122;


thread setup_thread;
void thread_setup1() {
    raise_arm_fast();
    encrypt_arm_vert();
}

void thread_setup2() {
    rotator_flip();
    container_open();
}

thread twistnturn_thread;
void thread_twistnturn1() {
    rotator_release();

}

void thread_twistnturn2() {
    turn_left(200, turn90-10);
}

thread ringclose_thread;   
void thread_ringclose1() {
    drive_backward(50, 150);
}

void thread_ringclose2() {
    container_close();
    return;
}

thread backnlower_thread;   
void thread_backnlower1() {
    drive_forward(300, 250);
    msleep(1500);
}

void thread_backnlower2() {
    horizontal_arm();
    return;
}


/////////////////////////////////////////////////////

void tilt_shift() {
    swiper_forward();
    swiper_back();
    move_motor(SWIPER_PORT, 2000, SWIPER_BACK + 1); //added
    msleep(500);
    //cmpc(SWIPER_PORT);
}

void tilt_shift_ultra() { //swiper more forward
    swiper_ultra_forward();
    move_motor(SWIPER_PORT, 2000, SWIPER_BACK + 20);
    msleep(500);
}

void cubeflicker_one() {
    enable_servo(FLICKER_PORT);
    flicker_low_cube();
    msleep(250);
    encrypt_arm_cube();
    msleep(1000);
    squareup_until_touch_or_dist(400, 1300); //drive_backward(400, 1300);

    flicker_up(); //up
    msleep(250);
    encrypt_arm_vert();
    //disable_servo(FLICKER_PORT);
}


void cubeflicker_two() {
    enable_servo(FLICKER_PORT);
    flicker_low_cube();
    msleep(250);
    encrypt_arm_cube();
    msleep(250);
    squareup_until_touch_or_dist(400, 1500);

    flicker_up(); //up
    msleep(250);
    encrypt_arm_vert();
    //disable_servo(FLICKER_PORT);
}

void cubeflicker_high() {
    enable_servo(FLICKER_PORT);
    flicker_low_cube();
    msleep(250);
    encrypt_arm_highcube();
    msleep(250);
    squareup_until_touch_or_dist(300, 1100); //1000

    flicker_up();
    msleep(250);
    encrypt_arm_vert();
    //disable_servo(FLICKER_PORT);
}



void swish_level3() {
    enable_servo(FLICKER_PORT);
    flicker_low_cube();
    encrypt_arm_level3();
    msleep(1000);

    drive_forward(200, 200); //300
    //disable_servo(FLICKER_PORT);

    msleep(100);
    encrypt_arm_vert();
}

void swish_level2() {
    enable_servo(FLICKER_PORT);
    flicker_low_cube();
    encrypt_arm_level2();
    msleep(1000);

    drive_forward(200, 200);
    //disable_servo(FLICKER_PORT);

    msleep(100);
    encrypt_arm_vert();
}

void swish_level1() {
    enable_servo(FLICKER_PORT);
    flicker_low_cube();
    encrypt_arm_level1();
    msleep(1000);

    drive_forward(200, 200);
    //disable_servo(FLICKER_PORT);

    msleep(100);
    encrypt_arm_vert();
}

void botgal_slap() {
    enable_servo(FLICKER_PORT);
    flicker_push();
    encrypt_arm_botgal();
    msleep(500);

    drive_forward_until_bump(100); 
    msleep(100);

    set_servo_position(FLICKER_PORT, FLICKER_PUSH + 300);
    move_motor(ENCRYPT_ARM_PORT, 400, ENCRYPT_ARM_BOTGAL - 50);
    msleep(100);
    drive_backward(300, 400);

    encrypt_arm_botgal(); //encrypt_arm_drop();
    flicker_drop();
    msleep(1000);

    drive_forward_until_bump(100); 

    encrypt_arm_vert();   
    //disable_servo(FLICKER_PORT);
}

/////////////////////////////////////////////////////

void grab_rings() {
    setup_thread = thread_create(&thread_setup2);
    thread_start(setup_thread);
    thread_setup1();

    drive_backward(200, 180);
    msleep(100);
    turn_right(150, 64);
    msleep(100);
    ringbase_arm();
    msleep(500);

    //go to grab
    ringclose_thread = thread_create(&thread_ringclose2);
    thread_start(ringclose_thread);
    thread_ringclose1();
    msleep(500);
    container_close();
    max_arm();
    
    thread_destroy(setup_thread);
    thread_destroy(ringclose_thread);
}

void first_drop() {
    twistnturn_thread = thread_create(&thread_twistnturn2);
    thread_start(twistnturn_thread);
    thread_twistnturn1();
    drive_backward(300, 550); //300
    msleep(100);
    turn_left(50, 50); //45
    msleep(100);
    drive_backward(200, 200);
    msleep(100);
    turn_right(50, 30); //33
    msleep(1000);
    cubeflicker_one();
    backnlower_thread = thread_create(&thread_backnlower2);
    thread_start(backnlower_thread);
    thread_backnlower1();
    msleep(500);

    //ring release
    turn_right(150, 4); 
    container_0();
    tilt_shift();
    container_1();

    max_arm();
    
    thread_destroy(twistnturn_thread);
    thread_destroy(backnlower_thread);
}


void first_encrypt() { 
    turn_right(150, 15);  //6
    msleep(100);
    drive_backward_until_touch_or_dist(300, 400); //250
    drive_backward(300, 70);
    msleep(100);
    swish_level2();
}

void second_drop() { //farthest building
    msleep(100);
    //turn_left_90();
    //turn_left(200, 15);
    turn_left(200, turn90 + 15);
    msleep(100);
    squareup_until_touch_or_dist(300, 900); //800 //speed was 300

    drive_forward(500, 800);
    drive_forward_until_cliff(500); 
    drive_forward(350, 350);  //300
    encrypt_arm_vert();

    msleep(250);
    turn_left_90();
    msleep(250);
    drive_backward(300, 150); 
    botgal_slap();

    drive_backward(300, 150);
    msleep(100);
    //turn_left_90();
    //turn_right(100, 3);  
    turn_left(200, turn90-3);

    //msleep(2000); //wait for other robot to clear
    squareup_until_touch_or_dist(450, 3000);

    drive_forward(300, 750); 
    msleep(100);
    //turn_left_90();
    //turn_left(150, 4);
    turn_left(200, turn90+4);
    msleep(100);
    drive_forward(300, 500); //300

    cubeflicker_two();   
    backnlower_thread = thread_create(&thread_backnlower2);
    thread_start(backnlower_thread);
    thread_backnlower1();
    msleep(750);

    turn_right(150, 3);
    tilt_shift();
    container_2();

    max_arm();
    msleep(500);
    
    thread_destroy(backnlower_thread);
}

void second_encrypt() { 
    turn_right(150, 16); //12 //14
    msleep(100);
    drive_backward_until_touch_or_dist(300, 400);
    drive_backward(300, 75);
    //drive_backward(300, 300); //300 //200
    msleep(500);
    swish_level1();
}

void third_drop() { //middle building
    rotator_upright();
    turn_left(150, 12);
    msleep(100);
    squareup_until_touch_or_dist(300, 700); //square up on building pipe
    drive_forward(300, 150); //200
    turn_right_90();
    msleep(500);
    squareup_until_touch_or_dist(400, 1000); //square up on side pipe
    rest_arm();
    drive_forward_until_cliff(500);
    drive_forward(350, 450); 
    max_arm();
    msleep(100);
    turn_left(200, turn90-15);
 
    //third encrypt
    drive_backward_until_touch_or_dist(300, 400);
    drive_backward(300, 20);
    msleep(100);
    drive_forward(300, 50);
    //drive_backward(200, 250); //200 
    swish_level3();   

    //square up
    turn_left(150, 10); //15
    drive_backward(300, 800);

    //third drop
    drive_forward(300, 100);

    tilt_shift_ultra();
    container_3();

    max_arm();
    msleep(500);    

    //square up
    //drive_backward(300, 500);
    //square_up(200, 2000);

}

void third_encrypt() {
    drive_forward(200, 100);  
    turn_right(150, 11);  //14
    msleep(100);
    drive_backward_until_touch_or_dist(300, 400);
    drive_backward(300, 20);
    msleep(100);
    drive_forward(300, 50);
    //drive_backward(200, 250); //200 
    swish_level3();   

    //square up
    turn_left(150, 15);
    drive_backward(300, 800);
}

void leftside_path () {     
    // LEFTSIDE DROP
    drive_forward(300, 400); //500
    msleep(100);
    turn_right_90(); 
    msleep(100);
    drive_forward(300, 900);  //870
    msleep(100);
    turn_left(200, turn90+8);
    msleep(100);

    cubeflicker_high();   
    drive_forward(300, 220);
    msleep(100);
    turn_right(150, 12); //15
    msleep(100);

    tilt_shift_ultra();
    container_4();

    drive_forward(300, 100);
    max_arm();  

    // LEFTSIDE ENCRYPT
    turn_right(150, 7); //7
    drive_backward_until_touch_or_dist(300, 200);
    drive_backward(300, 180); 
    //drive_backward(300, 500); //400
    msleep(100);
    swish_level3();

    // LEFTSIDE GO BACK
    drive_forward(300, 100);
    msleep(100);
    turn_right(150, 97); //85 //87
    msleep(100);
    drive_backward_until_cliff(300);

    // RIGHTSIDE DROP
    drive_backward(250, 200); //230
    encrypt_arm_vert();

    msleep(100);
    //turn_left_90();
    //turn_left(150, 3);
    turn_left(200, turn90+7); //+5
    msleep(100);
    drive_forward(300, 100);

    cubeflicker_high();   
    drive_forward(300, 220);
    msleep(100);
    turn_right(150, 15); 
    msleep(100);

    container_open();
    tilt_shift_ultra();
    swiper_forward();

    drive_forward(300, 100);
    max_arm();   

    // RIGHTSIDE ENCRYPT 
    turn_right(150, 6); //7
    drive_backward_until_touch_or_dist(300, 200);
    drive_backward(300, 200); //180
    //drive_backward(300, 500); //400
    msleep(100);

    enable_servo(FLICKER_PORT); //swish level 2 without the raise encrypt arm at the end
    flicker_low_cube();
    encrypt_arm_level2();
    msleep(100);

    drive_forward(200, 200);
}

void rightside_path () { 
    // RIGHTSIDE DROP
    drive_forward(300, 500);
    msleep(100);
    turn_right_90(); 
    msleep(100);
    drive_backward_until_cliff(300); 

    drive_backward(300, 140); 
    encrypt_arm_vert();

    msleep(100);
    turn_left(200, turn90+7); //+3
    msleep(100);
    drive_forward(300, 100);
    max_arm();

    cubeflicker_high();   
    drive_forward(300, 220); 
    msleep(100);
    turn_right(150, 15);
    msleep(100);

    tilt_shift_ultra();
    container_4();

    drive_forward(300, 100);
    max_arm();   

    // RIGHTSIDE ENCRYPT
    turn_right(150, 15); //16
    drive_backward_until_touch_or_dist(300, 400);
    drive_backward(300, 180); //130
    //drive_backward(300, 500); //400
    msleep(100);

    enable_servo(FLICKER_PORT); 
    flicker_low_cube();
    encrypt_arm_level2();
    msleep(100);   
    drive_forward(200, 200);

    // RIGHTSIDE GO BACK
    drive_forward(300, 300); 
    encrypt_arm_vert();
    turn_right(150, 93); //90 
    msleep(100);
    drive_backward(200, 200); //to avoid turning over the line
    msleep(100);
    drive_forward_until_cliff_right(300); 
    drive_forward(300, 600); //adjust as needed (goes to position for leftside drop)

    // LEFTSIDE DROP
    drive_forward(300, 880);  
    msleep(100);
    turn_left(200, turn90+10); //+3
    msleep(100);
    drive_forward(200, 200); 

    cubeflicker_high();   
    drive_forward(300, 220);
    msleep(100);
    turn_right(150, 15); 
    msleep(100);

    container_open();
    tilt_shift_ultra();
    swiper_forward();

    drive_forward(300, 100);
    max_arm();  

    // LEFTSIDE ENCRYPT
    turn_right(150, 8); //5
    drive_backward_until_touch_or_dist(300, 400);
    drive_backward(300, 180);
    //drive_backward(300, 500); //400
    msleep(100);

    enable_servo(FLICKER_PORT); //swish level 2 without the raise encrypt arm at the end
    flicker_low_cube();
    encrypt_arm_level3();
    msleep(100);

    drive_forward(200, 200);

}

void blue_right_path() {
    grab_rings();
    first_drop();
    first_encrypt();
    second_drop();
    second_encrypt();
    third_drop();
    //third_encrypt();
    leftside_path();
}

void blue_left_path() {
    grab_rings();
    first_drop();
    first_encrypt();
    second_drop();
    second_encrypt();
    third_drop();
    //third_encrypt();
    rightside_path();
}

void miniservo_test() {
    enable_servo(FLICKER_PORT);
    flicker_low_cube();
    msleep(2000);
    disable_servo(FLICKER_PORT);
}

int main()  {

    printf("Starting...");
    start_up();
    
    printf("press r_button to continue to camera test\n");
    while (!right_button()) {
        msleep(50);
    }
    
    camera_test();
    
    printf("SETUP RINGS!!!\n");
    
    printf("press r_button to continue to light start\n");
    while (!right_button()) {
        msleep(50);
    }
    
    int leftside = wait_for_light_read(0); //1 if leftside building is blue
    printf("After startup...");
    
    camera_close();

    //while (!right_button()) {msleep(100);}
    double start_time = seconds();
    shut_down_in(119.5);

    if (leftside == 1) { 
        printf("blue found!\n");
        blue_left_path();
    }
    else {
        printf("green found!\n");
        blue_right_path();
    }

    disable_servos();
    create_disconnect();
    printf("end time: %f\n", seconds() - start_time);
    return 0;
}
