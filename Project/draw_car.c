volatile int pixel_buffer_start;
volatile char* character_buffer = (char*) 0xC9000000;
#include <stdio.h>
#include <stdlib.h>
typedef int bool;
#define true 1
#define false 0

extern short MYIMAGE [50][60];

void plot_pixel(int x, int y, short int line_color);
void clear_screen();
void draw_background();
void draw_line(int x0, int y0, int x1, int y1, short int line_color);
void draw_car(int x, int y, short int colour);
void vsync();
int check_car(int x, int y);
void DrawFilledCircle(int x0, int y0, int radius, short int color);
void display_score();
int abs();
void wait_for_enter();
void write_start_message();
void clear_car(int x, int y, short int colour);

int xp[6],r[6],yp[6],dy[6];
int score = 0;
int sixtycount = 0;

int main(void) {
    volatile int *pixel_ctrl_ptr = (int *) 0xFF203020;
    volatile int *PS2_ptr = (int *) 0xFF200100;  // PS/2 port address
    volatile int *HEX_ptr = (int *) 0xFF200020; //for score
    int car_speed = 3; //3 pixels
    int PS2_data, RVALID;
    unsigned char b1 = 0;

    *(pixel_ctrl_ptr + 1) = 0xC8000000;
    vsync();
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen();
    // declare other variables(not shown)
    int x = 140;
    int y = 190;


    int k;
    for (k = 0; k < 6; k++){
        yp[k] = 20;
        r[k] = rand()%3+3;
    }
    xp[0] =  20;
    xp[1] =  70;
    xp[2] =  120;
    xp[3] = 170;
    xp[4] = 220;
    xp[5] = 290;

    /*(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1);*/

    int play = 1;
    int scorecount = 0;
    wait_for_enter();
	clear_screen();
    while(play){
        display_score();

        dy[0] = rand()%1+1;
        dy[1] = rand()%2;
        dy[2] = rand()%4;
        dy[3] = rand()%2;
        dy[4] = rand()%3;
        dy[5] = rand()%1+1;
        draw_background();

        PS2_data = *(PS2_ptr);	// read the Data register in the PS/2 port
        RVALID = (PS2_data & 0x8000);	// extract the RVALID field

        if (RVALID != 0)
        {
            b1 = PS2_data & 0xFFFF; //convert to hex
        }

        if(b1 == 0x1C && x >= 4){ //a key
            x = x - car_speed;

        }
        else if(b1 == 0x23 && x <= 270){ //d key
            x = x + car_speed;

        }
		else if(b1 == 0x29 && x <= 270){ //space key
            x = x;
		}
		
        int j;
        for (j = 0; j < 6; ++j){
            if(yp[j] >= 210){
                yp[j] = 20;
            }
            else{
                yp[j] = yp[j] + dy[j];
            }
            DrawFilledCircle(xp[j],yp[j],r[j], 0x001F);
        }

        draw_car(x,y,0xF800);
        vsync();
        pixel_buffer_start = *(pixel_ctrl_ptr + 1);
        DrawFilledCircle(xp[0],yp[0], r[0], 0);
        DrawFilledCircle(xp[1],yp[1], r[1], 0);
        DrawFilledCircle(xp[2],yp[2], r[2], 0);
        DrawFilledCircle(xp[3],yp[3], r[3], 0);
        DrawFilledCircle(xp[4],yp[4], r[4], 0);
        DrawFilledCircle(xp[5],yp[5], r[5], 0);
        clear_car(x,y,0x0000);

        play = check_car(x,y);
        if(play == 0){
            wait_for_enter();
            play = 1;
            score = 0;
        }
    }
}
void write_char(int x, int y, char c) {

    *(char *) (character_buffer + (y <<7) + x) = c;
}
void clear_car(int x, int y, short int colour){
	int i,j;
    int count = 0;
  for (i = 0; i < 50; ++i){
		for ( j = 0; j < 60; ++j){
			plot_pixel(j+x,i+y,colour);
		}
	}
}
void write_start_message(){
	int i;
	int x = 2;
	int y = 2;
	
	char title[] = "CRAZY TAXI";
	char instructions[] = "Press Enter to Start";
	char mechanics[] = "A = Left,D = Right";
	char mechanics1[] = "Space to Stop";


	//TITLE
	for (i = 0; i < sizeof(title); i++) {
		write_char(x, y, title[i]);
		x++;
	}
	
	x = 2;
	y = 4;
	//INSTRUCTIONS
	for (i = 0; i < sizeof(instructions); i++) {
		write_char(x, y, instructions[i]);
		x++;
	}
	
	x = 2;
	y = 6;
	//MECHANICS
	for (i = 0; i < sizeof(mechanics); i++) {
		write_char(x, y, mechanics[i]);
		x++;
	}
	x = 2;
	y = 8;
	for (i = 0; i < sizeof(mechanics1); i++) {
		write_char(x, y, mechanics1[i]);
		x++;
	}
	
}



void display_score(){

    volatile int *HEX_ptr0 = (int *) 0xFF200020; //for score

	unsigned char seg_table[] = {
        0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
        0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
	unsigned char hex_segs[] = {0, 0};
	
	
	
    unsigned char s0 = 0;
    unsigned char s1 = 0;
	
    int count = score;
    int remainder,quotient;
    remainder = count % 10;
    quotient = count / 10;

	
    switch(remainder){
        case 0 : hex_segs[0] = 0x3F; break;
        case 1 : hex_segs[0] = 0x06; break;
        case 2 : hex_segs[0] = 0x5B; break;
        case 3 : hex_segs[0] = 0x4F; break;
        case 4 : hex_segs[0] = 0x66; break;
        case 5 : hex_segs[0] = 0x6D; break;
        case 6 : hex_segs[0] = 0x7D; break;
        case 7 : hex_segs[0] = 0x07; break;
        case 8 : hex_segs[0] = 0x7F; break;
        case 9 : hex_segs[0] = 0x67; break;
    }
    switch(quotient){
        case 0 : hex_segs[1] = 0x3F; break;
        case 1 : hex_segs[1] = 0x06; break;
        case 2 : hex_segs[1] = 0x5B; break;
        case 3 : hex_segs[1] = 0x4F; break;
        case 4 : hex_segs[1] = 0x66; break;
        case 5 : hex_segs[1] = 0x6D; break;
        case 6 : hex_segs[1] = 0x7D; break;
        case 7 : hex_segs[1] = 0x07; break;
        case 8 : hex_segs[1] = 0x7F; break;
        case 9 : hex_segs[1] = 0x67; break;
    }
	 *HEX_ptr0 =  *(int *)(hex_segs);

}


void wait_for_enter(){
    volatile int *PS2_ptr = (int *) 0xFF200100;  // PS/2 port address
    unsigned char b1 = 0;
    int PS2_data, RVALID;
	write_start_message();
    while (1){
        PS2_data = *(PS2_ptr);	// read the Data register in the PS/2 port
        RVALID = (PS2_data & 0x8000);	// extract the RVALID field

        if (RVALID != 0)
        {
            b1 = PS2_data & 0xFFFF; //convert to hex
        }
        if (b1 == 0x5A){
            int x = 140;
            int y = 198;


            int k;
            for (k = 0; k < 6; k++){
                yp[k] = 20;
                r[k] = rand()%3+3;
            }
            xp[0] =  20;
            xp[1] =  70;
            xp[2] =  120;
            xp[3] = 170;
            xp[4] = 220;
            xp[5] = 290;
            break;



        }
    }
}
void draw_car(int x, int y, short int colour){
    int i,j;
    int count = 0;
  for (i = 0; i < 50; ++i){
		for ( j = 0; j < 60; ++j){
			plot_pixel(j+x,i+y,MYIMAGE[i][j]);
		}
	}
}
int check_car(int x, int y){
    int i,j;
    for (i = 0; i < 6; ++i){
        for (j = 0; j < 25; j++){
            if(yp[i] >= 190){
                if (x+15+j == xp[i])
                    return 0;
            }
        }
    }
    return 1;

}
void draw_background(){

    draw_line(100,0,100,190,0xffff);
    draw_line(101,0,101,190,0xffff);
    draw_line(200,0,200,190,0xffff);
    draw_line(201,0,201,190,0xffff);

    draw_line(50,60,50,100,0xffff);
    draw_line(51,60,51,100,0xffff);
    draw_line(50,120,50,160,0xffff);
    draw_line(51,120,51,160,0xffff);

    draw_line(150,60,150,100,0xffff);
    draw_line(151,60,151,100,0xffff);
    draw_line(150,120,150,160,0xffff);
    draw_line(151,120,151,160,0xffff);

    draw_line(250,60,250,100,0xffff);
    draw_line(251,60,251,100,0xffff);
    draw_line(250,120,250,160,0xffff);
    draw_line(251,120,251,160,0xffff);

}
// code for subroutines (not shown)
void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

void clear_screen(){
    int i, j;
    for(i = 0; i < 319; i++) {
        for (j = 0; j < 239; j++) {
            plot_pixel(i, j, 0x0000);
        }
    }
}
void vsync(){
    volatile int* status_register =  0xFF203020;
    register int status;

    *status_register = 1; // set B to 1
    status = *(status_register + 3);
    while((status & 0x001) != 0)
    {
        status = *(status_register + 3);
    }
	sixtycount++;
	if(sixtycount >=60){
		score++;
		sixtycount = 0;
	}
}
void draw_line(int x0, int y0, int x1, int y1, short int line_color){
    bool is_steep = abs(y1 - y0) > abs(x1 - x0);
    if(is_steep){
        int a = x0;
        x0 = y0;
        y0 = a;

        a = x1;
        x1 = y1;
        y1 = a;
    }
    if(x0 > x1){
        int a = x0;
        x0 = x1;
        x1 = a;

        a = y0;
        y0 = y1;
        y1 = a;
    }
    int deltaX = x1 - x0;
    int deltaY = abs(y1 - y0);
    int error = (-deltaX)/2;
    int x;
    int y = y0;
    int y_step;
    if(y0 < y1){
        y_step = 1;
    } else{
        y_step = -1;
    }

    for(x = x0; x < x1; ++x){
        if(is_steep)
            plot_pixel(y,x , line_color);
        else
            plot_pixel(x,y , line_color);

        error += deltaY;

        if(error >= 0){
            y += y_step;
            error -= deltaX;
        }
    }
}

void DrawFilledCircle(int x0, int y0, int radius, short int color)
{
    int x = radius;
    int y = 0;
    int xChange = 1 - (radius << 1);
    int yChange = 0;
    int radiusError = 0;

    while (x >= y)
    {
        int i;
        for (i = x0 - x; i <= x0 + x; i++)
        {
            plot_pixel(i, y0 + y, color);
            plot_pixel(i, y0 - y, color);
        }
        for (i = x0 - y; i <= x0 + y; i++)
        {
            plot_pixel(i, y0 + x, color);
            plot_pixel(i, y0 - x, color);
        }

        y++;
        radiusError += yChange;
        yChange += 2;
        if (((radiusError << 1) + xChange) > 0)
        {
            x--;
            radiusError += xChange;
            xChange += 2;
        }
    }
}

int abs(int x)
{
    if(x < 0) return (-1 * x);
    else return x;
}