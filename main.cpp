#include "mbed.h"
//Later change pins to right settings
DigitalOut Stepper_Motor_1_Step(p5);
DigitalOut Stepper_Motor_1_Direction(p6);
DigitalOut Stepper_Motor_2_Step(p7);
DigitalOut Stepper_Motor_2_Direction(p8);
DigitalIn US_Sensor_Echo(p9);
DigitalOut US_Sensor_Trig(p10);
BusIn Left_Sensors (p11,p12,p13); // Just do busin instead
BusIn Right_Sensors (p14,p15,p16);

Timer t;

bool ECHO = false;
volatile int us_distance_mm = 0;

// North = 0 ,East = 1, South = 2, West = 3
int number_of_maze_walls[8][8];
int mouse_position [8][8]; // location of the mouse
int mouse_position_x = 0; // location of the mouse
int mouse_position_y = 0; // location of the mouse
int goal = 0; // lee numbers goal

int number_of_maze_cells[8][8];
int orientation = 0; // direction of the mouse, assume starting North always

int first_calculate = 0;

double check_us_distance(bool ECHO){
    uint32_t echo_time = 0; //supposed to have values range from 0 - 49000

    //Trigger pulse function 
    US_Sensor_Trig = 0;
    wait_us(2);
    US_Sensor_Trig = 1;
    wait_us(10);
    US_Sensor_Trig = 0;
    //Wait for echo high
    while(!US_Sensor_Echo);
    t.reset();
    t.start();

    //Wait for echo low
    while(US_Sensor_Echo){
        t.stop();
    }
    echo_time = t.read_us();

    //Speed of sound = 0.343 mm/us
    us_distance_mm = (echo_time*343)/2000;
    if(us_distance_mm < 80){
        t.stop();
        }
    return (double)us_distance_mm;
    }

int check_left_sensors(){
    int left_wall_value = Left_Sensors.read();
    if (left_wall_value == 7){
        return 1;
    }
    // can added elseif for allignment issues but need hardware to test
    else {
        return 0;
    }
}

int check_right_sensors(){
    int right_wall_value = Right_Sensors.read();
    if (right_wall_value == 7){
        return 1;
    }
    // can added elseif for allignment issues but need hardware to test
    else {
        return 0;
    }
}

bool wall_in_front(){
    return check_us_distance() < 80;
}
bool wall_to_left(){
    return check_left_sensors() == 1;
}
bool wall_to_right(){
    return check_right_sensors() == 1;
}

void update_walls(int orientation, bool front, bool left, bool right){
    if (orientation == 0){
        if (front) number_of_maze_walls[mouse_position_x][mouse_position_y] |= 1; // Mark North Wall, Front is North (1)
        if (left)  number_of_maze_walls[mouse_position_x][mouse_position_y] |= 8; // Mark West Wall, Left is West (8)
        if (right) number_of_maze_walls[mouse_position_x][mouse_position_y] |= 2; // Mark East Wall, Right is East (2)
        }
    else if (orientation == 1){
        if (front) number_of_maze_walls[mouse_position_x][mouse_position_y] |= 2; // Mark North Wall, Front is East (2)
        if (left)  number_of_maze_walls[mouse_position_x][mouse_position_y] |= 1; // Mark West Wall, Left is North (1)
        if (right) number_of_maze_walls[mouse_position_x][mouse_position_y] |= 4; // Mark East Wall, Right is is South (4)
        }
    else if (orientation == 2){
        if (front) number_of_maze_walls[mouse_position_x][mouse_position_y] |= 4; // Mark North Wall, Front is South (4)
        if (left)  number_of_maze_walls[mouse_position_x][mouse_position_y] |= 2; // Mark West Wall, Left is East (2)
        if (right) number_of_maze_walls[mouse_position_x][mouse_position_y] |= 8; // Mark East Wall, Right is West (8)
        }
    else if (orientation == 3){
        if (front) number_of_maze_walls[mouse_position_x][mouse_position_y] |= 8; // Mark North Wall, Front is West (8)
        if (left)  number_of_maze_walls[mouse_position_x][mouse_position_y] |= 4; // Mark West Wall, Left is South (4)
        if (right) number_of_maze_walls[mouse_position_x][mouse_position_y] |= 1; // Mark East Wall, Right is North (1)
        }
}

void mouse_location( bool front, bool left, bool right){ 
    // North = 0, East = 1, South = 2, West = 3
     if (orientation == 0){
        if(front == false){
            mouse_position_y++;
        }
        else if (front == true && left == false) {
            orientation = 3;
            mouse_position_x--;
        }
        else if (front == true && left == true && right == false) {
            orientation = 1;
            mouse_position_x++;
        }
        else{
            orientation = 2;
            mouse_position_x++;
        }
    }

    else if (orientation == 1){
        if(front == false){
            mouse_position_x++;
        }
        else if (front == true && left == false) {
            orientation = 0;
            mouse_position_y++;
        }
        else if (front == true && left == true && right == false) {
            orientation = 2;
            mouse_position_y--;
        }
        else{
            orientation = 3;
            mouse_position_x--;
        }
    }

    else if (orientation == 2){
        if(front == false){
            mouse_position_y--;
        }
        else if (front == true && left == false) {
            orientation = 1;
            mouse_position_x++;
        }
        else if (front == true && left == true && right == false) {
            orientation = 3;
            mouse_position_x--;
        }
        else{
            orientation = 0;
            mouse_position_y++;
        }
    }

    else if (orientation == 3){
        if(front == false){
            mouse_position_x--;
        }
        else if (front == true && left == false) {
            orientation = 2;
            mouse_position_y--;
        }
        else if (front == true && left == true && right == false) {
            orientation = 0;
            mouse_position_y++;
        }
        else{
            orientation = 1;
            mouse_position_x++;
        }
    }
}

    void calculate_flood_fill(){
    //lee's assign lee's number to eac
    // Reset all cells to max
    for(int x = 0; x < 8; x++)
        for(int y = 0; y < 8; y++)
            number_of_maze_cells[x][y] = 999; //wipe everthing

    // Goal is centre of maze?
    number_of_maze_cells[3][3] = 0;
    number_of_maze_cells[3][4] = 0;
    number_of_maze_cells[4][3] = 0;
    number_of_maze_cells[4][4] = 0;

    //need to update this section for bfs (I used AI for this bit couldnt figure out myself or find where to look)
    int queue_x[64], queue_y[64];
    int head = 0, tail = 0;
    queue_x[tail] = 3; queue_y[tail] = 3; tail++;
    queue_x[tail] = 3; queue_y[tail] = 4; tail++;
    queue_x[tail] = 4; queue_y[tail] = 3; tail++;
    queue_x[tail] = 4; queue_y[tail] = 4; tail++;

    while(head != tail){
        int cx = queue_x[head];
        int cy = queue_y[head];
        head++;
        int next_val = number_of_maze_cells[cx][cy] + 1;

        if(cy < 7 && !(number_of_maze_walls[cx][cy] & 1) && number_of_maze_cells[cx][cy+1] == 999)
            { number_of_maze_cells[cx][cy+1] = next_val; queue_x[tail] = cx;   queue_y[tail] = cy+1; tail++; }
        if(cx < 7 && !(number_of_maze_walls[cx][cy] & 2) && number_of_maze_cells[cx+1][cy] == 999)
            { number_of_maze_cells[cx+1][cy] = next_val; queue_x[tail] = cx+1; queue_y[tail] = cy;   tail++; }
        if(cy > 0 && !(number_of_maze_walls[cx][cy] & 4) && number_of_maze_cells[cx][cy-1] == 999)
            { number_of_maze_cells[cx][cy-1] = next_val; queue_x[tail] = cx;   queue_y[tail] = cy-1; tail++; }
        if(cx > 0 && !(number_of_maze_walls[cx][cy] & 8) && number_of_maze_cells[cx-1][cy] == 999)
            { number_of_maze_cells[cx-1][cy] = next_val; queue_x[tail] = cx-1; queue_y[tail] = cy;   tail++; }
    }
}

void flood_fill_setup(){ // need to fix, to setup up the 2d array details, of inital lees number
    calculate_flood_fill(); //this will do everthing 
}



void flood_fill_update(){
    // update lees number, and to control priority for t junctions (Also done with AI to fit the other pasted code)
    int queue_x[64], queue_y[64];
    int head = 0, tail = 0;
    queue_x[tail] = mouse_position_x;
    queue_y[tail] = mouse_position_y;
    tail++;

    while(head != tail){
        int cx = queue_x[head];
        int cy = queue_y[head];
        head++;

        int min_n = 999;
        if(cy < 7 && !(number_of_maze_walls[cx][cy] & 1)) if(number_of_maze_cells[cx][cy+1] < min_n) min_n = number_of_maze_cells[cx][cy+1];
        if(cx < 7 && !(number_of_maze_walls[cx][cy] & 2)) if(number_of_maze_cells[cx+1][cy] < min_n) min_n = number_of_maze_cells[cx+1][cy];
        if(cy > 0 && !(number_of_maze_walls[cx][cy] & 4)) if(number_of_maze_cells[cx][cy-1] < min_n) min_n = number_of_maze_cells[cx][cy-1];
        if(cx > 0 && !(number_of_maze_walls[cx][cy] & 8)) if(number_of_maze_cells[cx-1][cy] < min_n) min_n = number_of_maze_cells[cx-1][cy];

        if(number_of_maze_cells[cx][cy] != min_n + 1){
            number_of_maze_cells[cx][cy] = min_n + 1;
            if(cy < 7 && !(number_of_maze_walls[cx][cy] & 1)){ queue_x[tail] = cx;   queue_y[tail] = cy+1; tail++; }
            if(cx < 7 && !(number_of_maze_walls[cx][cy] & 2)){ queue_x[tail] = cx+1; queue_y[tail] = cy;   tail++; }
            if(cy > 0 && !(number_of_maze_walls[cx][cy] & 4)){ queue_x[tail] = cx;   queue_y[tail] = cy-1; tail++; }
            if(cx > 0 && !(number_of_maze_walls[cx][cy] & 8)){ queue_x[tail] = cx-1; queue_y[tail] = cy;   tail++; }
        }
    }
}



void steps(){ // expand on loops when testing hardware
    Stepper_Motor_1_Step = 1; 
    Stepper_Motor_2_Step = 1;
    wait_us(1000);
    Stepper_Motor_1_Step = 0; 
    Stepper_Motor_2_Step = 0;
    wait_us(1000);
}

void forwards(){
    Stepper_Motor_1_Direction = 1;
    Stepper_Motor_2_Direction = 1;
    for (int i = 0; i < 400; i++){
        steps();
    }
}
void turn_left(){  
    Stepper_Motor_1_Direction = 0;
    Stepper_Motor_2_Direction = 1;
    for (int i = 0; i < 400; i++){
        steps();
    }
}

void turn_right(){
    Stepper_Motor_1_Direction = 1;
    Stepper_Motor_2_Direction = 0;
    for (int i = 0; i < 400; i++){
        steps();
    }
}
void u_turn(){
    Stepper_Motor_1_Direction = 1;
    Stepper_Motor_2_Direction = 0;
    for (int i = 0; i < 400; i++){
        steps();
    }
}

int main(){
   
    flood_fill_setup();

    while (1){
        wait_us(10000);
        bool front = wall_in_front();
        bool left = wall_to_left();
        bool right = wall_to_right();
        wait_us(10000);
        update_walls(front, left, right);
        wait_us(10000);
    
        if((mouse_position_x == 3 || mouse_position_x == 4) &&
           (mouse_position_y == 3 || mouse_position_y == 4)){
            break;
        // end program
        }
        //else if are priority based
        else if(front == false){
            wait_us(10000);
            mouse_location(front, left, right); //update mouse_location
            wait_us(10000);
            forwards();
            wait_us(10000);
            flood_fill_update();
            }
        else if (front == true && left == false) {
            wait_us(10000);
            mouse_location(front, left, right);
            wait_us(10000);
            turn_left();
            wait_us(10000);
            flood_fill_update();
            }
        else if (front == false && left == false && right == true) {
            wait_us(10000);
            mouse_location(front, left, right);
            wait_us(10000);
            turn_right();
            wait_us(10000);
            flood_fill_update();
            }
        else if (front == true && left == true && right == true) { // uturn
            wait_us(10000);
            mouse_location(front, left, right);
            wait_us(10000);
            u_turn();
            wait_us(10000);
            calculate_flood_fill(); // deadend, recalculate needs number
        }
    }
}