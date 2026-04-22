#include "mbed.h"
//Later change pins to right settings
DigitalOut Stepper_Motor_1_Step(p21); // Left Motor
DigitalOut Stepper_Motor_1_Direction(p22); // Left Motor
DigitalOut Stepper_Motor_2_Step(p23); // Right Motor
DigitalOut Stepper_Motor_2_Direction(p24); // Right Motor
DigitalIn US_Sensor_Echo(p12); // Ultrasonic Sensor
DigitalOut US_Sensor_Trig(p13); // Ultrasonic Sensor
BusIn Left_Sensors (p8,p7,p6); // Left Bus Sensor
BusIn Right_Sensors (p11,p10,p9); // Right Bus Sensor

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
void share_walls(int x, int y);                                 // forward declaration made by NICK to help with shar walls function 

double check_us_distance(bool ECHO){
    uint32_t echo_time = 0; //supposed to have values range from 0 - 49000

    //Trigger pulse function 
    US_Sensor_Trig = 0;
    wait_us(2);
    US_Sensor_Trig = 1;
    wait_us(10);
    US_Sensor_Trig = 0;
    //Wait for echo high
    while(!US_Sensor_Echo){};
    t.reset();
    t.start();

    //Wait for echo low
    while(US_Sensor_Echo){
    }
    t.stop(); 

     echo_time = t.elapsed_time().count();

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
    return check_us_distance(bool (ECHO)) < 80;
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
    share_walls(mouse_position_x, mouse_position_y);                            //moved function to call here NICK which just reads the mouse position
}

void share_walls(int x, int y){                                                   //ADDED NEW FUNCTION NICK TO CHECK WALLS and Cells read on otherside too
    // If this cell has a North wall, the cell above gets a South wall
    if((number_of_maze_walls[x][y] & 1) && y < 7)
        number_of_maze_walls[x][y+1] |= 4;
    // If this cell has an East wall, the cell to the right gets a West wall
    if((number_of_maze_walls[x][y] & 2) && x < 7)
        number_of_maze_walls[x+1][y] |= 8;
    // If this cell has a South wall, the cell below gets a North wall
    if((number_of_maze_walls[x][y] & 4) && y > 0)
        number_of_maze_walls[x][y-1] |= 1;
    // If this cell has a West wall, the cell to the left gets an East wall
    if((number_of_maze_walls[x][y] & 8) && x > 0)
        number_of_maze_walls[x-1][y] |= 2;
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

    if (orientation == 1){
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

    if (orientation == 2){
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

    if (orientation == 3){
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

//we could move this whole function into one
    void calculate_flood_fill(){
    // So this initialise all cells to 0
    for(int x = 0; x < 8; x++){
        number_of_maze_cells[x][0] = 0;
        number_of_maze_cells[x][1] = 0;
        number_of_maze_cells[x][2] = 0;
        number_of_maze_cells[x][3] = 0;
        number_of_maze_cells[x][4] = 0;
        number_of_maze_cells[x][5] = 0;
        number_of_maze_cells[x][6] = 0;
        number_of_maze_cells[x][7] = 0;
    }
}

void flood_fill_setup(){ 
// We start by calling the 2d array 
    calculate_flood_fill();
    
    //Calls to see what cell has lee number
    bool visited[8][8] = {};
    //stores cells in queue
    int queue_x[64], queue_y[64];
    int head = 0, tail = 0;

    number_of_maze_cells[7][7] = 0;
    visited[7][7] = true;
    queue_x[tail] = 7; queue_y[tail] = 7; tail++;

    //Process till empty 
    while(head != tail){
        int cx = queue_x[head];
        int cy = queue_y[head];
        head++;
       
    //Kind of explanitory
        int next_val = number_of_maze_cells[cx][cy] + 1;

    //Check North neighbour, if not visited assign Lee number and add to the queue thingy
        if(cy < 7 && !visited[cx][cy+1]){
            number_of_maze_cells[cx][cy+1] = next_val;
            visited[cx][cy+1] = true;
            queue_x[tail] = cx; queue_y[tail] = cy+1; tail++;
        }
        // Check East neighbour, do same
        if(cx < 7 && !visited[cx+1][cy]){
            number_of_maze_cells[cx+1][cy] = next_val;
            visited[cx+1][cy] = true;
            queue_x[tail] = cx+1; queue_y[tail] = cy; tail++;
        }
        //Check South
        if(cy > 0 && !visited[cx][cy-1]){
            number_of_maze_cells[cx][cy-1] = next_val;
            visited[cx][cy-1] = true;
            queue_x[tail] = cx; queue_y[tail] = cy-1; tail++;
        }
        //Check west
        if(cx > 0 && !visited[cx-1][cy]){
            number_of_maze_cells[cx-1][cy] = next_val;
            visited[cx-1][cy] = true;
            queue_x[tail] = cx-1; queue_y[tail] = cy; tail++;
            }
        }
    }

void flood_fill_update(){
    // Reset all cells when unvisited and walls will update this as being discovered 
    for(int x = 0; x < 8; x++){
        number_of_maze_cells[x][0] = 999;
        number_of_maze_cells[x][1] = 999;
        number_of_maze_cells[x][2] = 999;
        number_of_maze_cells[x][3] = 999;
        number_of_maze_cells[x][4] = 999;
        number_of_maze_cells[x][5] = 999;
        number_of_maze_cells[x][6] = 999;
        number_of_maze_cells[x][7] = 999;
    }

    // Set goal cell [7][7] to 0 and add to queue
    number_of_maze_cells[7][7] = 0;
    int queue_x[64], queue_y[64];
    int head = 0, tail = 0;
    queue_x[tail] = 7; queue_y[tail] = 7; tail++;

    // Process queue until empty
    while(head != tail){
        int cx = queue_x[head];
        int cy = queue_y[head];
        head++;
        // NExplanitory again
        int next_val = number_of_maze_cells[cx][cy] + 1;

        // Check North, skips if north wall exists otherwise it will assign a Lee number beforew we add to queue thingy
        if(cy < 7 && !(number_of_maze_walls[cx][cy] & 1) && number_of_maze_cells[cx][cy+1] == 999){
            number_of_maze_cells[cx][cy+1] = next_val;
            queue_x[tail] = cx; queue_y[tail] = cy+1; tail++;
        }
        // Check East 
        if(cx < 7 && !(number_of_maze_walls[cx][cy] & 2) && number_of_maze_cells[cx+1][cy] == 999){
            number_of_maze_cells[cx+1][cy] = next_val;
            queue_x[tail] = cx+1; queue_y[tail] = cy; tail++;
        }
        // Check South 
        if(cy > 0 && !(number_of_maze_walls[cx][cy] & 4) && number_of_maze_cells[cx][cy-1] == 999){
            number_of_maze_cells[cx][cy-1] = next_val;
            queue_x[tail] = cx; queue_y[tail] = cy-1; tail++;
        }
        // Check West
        if(cx > 0 && !(number_of_maze_walls[cx][cy] & 8) && number_of_maze_cells[cx-1][cy] == 999){
            number_of_maze_cells[cx-1][cy] = next_val;
            queue_x[tail] = cx-1; queue_y[tail] = cy; tail++;
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
        update_walls(orientation, front, left, right);                      //Changed this Nick
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
};
