/* This file creates an animated visualization app. A firework appears on the screen is a random place on a one second interval (default).
* The speed of these fireworks can be changed using a GLUT menu. The speeds are: Slow (default, 1000ms), Fast (500ms), Max (each frame), and Freeze (stop).
* There are 10 effects that can be utilized to change the look of the application. These are all stackable on top of eachother. Except for inverse/daytime after overlay is already active. These are as follows:
* RandomColour - random colours for fireworks
* Big Fireworks - extra pixels making fireworks larger
* Small Fireworks - less pixels making fireworks smaller
* Overlay Fireworks - doesn't clear screen when drawing a new firework
* Centre Only - fireworks are drawn only in the centre of the screen
* Daytime - sets the background (clear) colour to sky blue, emulating fireworks during the day
* Waterworks - changes the fireworks to droplet shapes
* Tilted - tilts the sky, meaning fireworks are 45 degrees rotated and only show in a trianlge on the left side of the screen. This is because the sky has been tilted.
* Inverse - inverts the colour between the fireworks and the background
* Trails - draw trails below the fireworks
* 
* There is a menu function for resetting the application to its default state, as well as a quit function.
*/
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <freeglut.h>
#include <FreeImage.h>
#include <time.h>

//the global structure
typedef struct 
{
	int w, h, randX, randY; //w and h are width/height of the window. randX and randY are random value within w and h respectively
	int speed; //speed of visualizer in milliseconds, bigger number means slower
	bool frozen; //true when the fireworks are frozen, false otherwise
	bool resetWindow; //true when we want to reset the window, false otherwise

	//bools for variations, true if the variation is being applied, false otherwise
	bool randomColour; //random colours for fireworks
	bool bigFirework; //more pixels around firework, making it bigger
	bool smallFirework; //less pixels around firework, making it smaller
	bool overlayFireworks; //don't clear the window between drawing fireworks
	bool centreOnly; //only create fireworks in the centre of the window
	bool daytime; //set the clear colour to a sky blue, emulating fireworks during the day (nearly impossible to see, as intended. fireworks during the day basically are just noise and smoke)
	bool waterworks; //change the firework to a droplet shape, henceforth called a "waterwork"
	bool tilted; //tilt the matrix drawing the fireworks. using overlayFireworks, you can see this only draws in a trianglular shape on the left of the screen. i thought this looked kinda cool so i kept it that way.
	bool inverse; //invert the colours of the fireworks and the clear colour
	bool trails; //have trails below the fireworks when they fire
} glob;
glob global;

//enums for menu options
enum { MENU_SPEED, MENU_SLOW, MENU_FAST, MENU_MAX, MENU_FREEZE, MENU_VARIATION1, MENU_VARIATION2, MENU_VARIATION3, MENU_VARIATION4, MENU_VARIATION5, MENU_VARIATION6, MENU_VARIATION7, MENU_VARIATION8, MENU_VARIATION9, MENU_VARIATION10, MENU_RESET, MENU_QUIT };

//this method draws the firework to the screen based on bools for effects
void draw_firework(void)
{
	//set the clear colour and colour of the drawn pixels
	GLfloat r, g, b; //RGB for the drawn pixels

	//randomly set a colour if the random colour effect is on
	if (global.randomColour)
	{
		r = rand() / static_cast <float> (RAND_MAX) - 0.1;
		g = rand() / static_cast <float> (RAND_MAX) - 0.1;
		b = rand() / static_cast <float> (RAND_MAX) - 0.1;
	}
	//if we are inverting the colours, set the fire/waterworks to the clear colour and vice versa
	if (global.inverse)
	{
		//set clearcolor to the random colour
		if (global.randomColour)
			glClearColor(r, g, b, 1.0);
		//else set clearcolor to the waterworks default colour
		else if (global.waterworks)
			glClearColor(0.0, 0.0, 1.0, 1.0);
		//else set clearcolor to the fireworks default colour
		else
			glClearColor(1.0, 0.0, 0.0, 1.0);

		//if daytime, make the fireworks skyblue
		if (global.daytime)
		{
			r = 0.529;
			g = 0.808;
			b = 0.922;
		}
		//else the fireworks are black
		else
		{
			r = 0;
			g = 0;
			b = 0;
		}
	}
	else //the default colours
	{
		//if not random, set the colours
		if (!global.randomColour)
		{
			//if we are creating "waterworks", make the default blue
			if (global.waterworks)
			{
				r = 0;
				g = 0;
				b = 1;
			}
			else//else set the default colour as red
			{
				r = 1;
				g = 0;
				b = 0;
			}
		}

		//if daytime mode is active, set the clear colour to sky blue. else, set it to black
		if (global.daytime)
			glClearColor(0.529, 0.808, 0.922, 1.0);
		else
			glClearColor(0.0, 0.0, 0.0, 1.0);
	}

	glColor3f(r, g, b); //set the colour of the drawn pixels

	//if we want the fireworks to only appear in the middle quarter of the window
	if (global.centreOnly)
	{
		global.randX = rand() % (global.w / 4) + (global.w / 2.66);
		global.randY = rand() % (global.h / 4) + (global.h / 2.66);
	}

	//else have fireworks across the entire window
	else
	{
		global.randX = rand() % global.w;
		global.randY = rand() % global.h;
	}

	//if we want to rotate to be tilted, set the rotate angle to 45 degrees around the z-axis
	if (global.tilted)
	{
		glPushMatrix();
		glRotatef(45, 0, 0, 1);
	}

	//if we are not overlaying fireworks, ensure the color buffer is cleared each time a firework is drawn
	if (!global.overlayFireworks)
		glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_POINTS); //begin drawing points

	//this section contains code for possible variations. each if statement represents one variation that changes the look of the firework

	//if we are creating "waterworks" instead of fireworks. draws a tear shape
	if (global.waterworks)
	{
		//draw the droplet of the "waterworks"
		//middle line
		glVertex2i(global.randX, global.randY);
		glVertex2i(global.randX, global.randY + 1);
		glVertex2i(global.randX, global.randY + 2);
		glVertex2i(global.randX, global.randY + 3);
		glVertex2i(global.randX, global.randY - 1);
		glVertex2i(global.randX, global.randY - 2);
		glVertex2i(global.randX, global.randY - 3);

		//left lines
		glVertex2i(global.randX - 1, global.randY + 2);
		glVertex2i(global.randX - 1, global.randY + 1);
		glVertex2i(global.randX - 1, global.randY);
		glVertex2i(global.randX - 1, global.randY - 1);
		glVertex2i(global.randX - 1, global.randY - 2);
		glVertex2i(global.randX - 1, global.randY - 3);

		glVertex2i(global.randX - 2, global.randY + 1);
		glVertex2i(global.randX - 2, global.randY);
		glVertex2i(global.randX - 2, global.randY - 1);
		glVertex2i(global.randX - 2, global.randY - 2);

		//right lines
		glVertex2i(global.randX + 1, global.randY + 2);
		glVertex2i(global.randX + 1, global.randY + 1);
		glVertex2i(global.randX + 1, global.randY);
		glVertex2i(global.randX + 1, global.randY - 1);
		glVertex2i(global.randX + 1, global.randY - 2);
		glVertex2i(global.randX + 1, global.randY - 3);

		glVertex2i(global.randX + 2, global.randY + 1);
		glVertex2i(global.randX + 2, global.randY);
		glVertex2i(global.randX + 2, global.randY - 1);
		glVertex2i(global.randX + 2, global.randY - 2);
	}

	//else we are creating regular fireworks
	else
	{
		//always draw the 5 middle pixels of the firework
		glVertex2i(global.randX, global.randY);
		glVertex2i(global.randX + 5, global.randY + 5);
		glVertex2i(global.randX - 5, global.randY - 5);
		glVertex2i(global.randX + 5, global.randY - 5);
		glVertex2i(global.randX - 5, global.randY + 5);
	}

	//this makes the fireworks smaller/larger. if we create a "waterwork", the smallFirework means we don't create a splash around it, and the bigFirework creates a larger splash around the "waterwork"
	if (!global.smallFirework)
	{
		glVertex2i(global.randX + 10, global.randY + 10);
		glVertex2i(global.randX - 10, global.randY - 10);
		glVertex2i(global.randX + 10, global.randY - 10);
		glVertex2i(global.randX - 10, global.randY + 10);
	}

	if (global.bigFirework)
	{
		glVertex2i(global.randX + 20, global.randY + 20);
		glVertex2i(global.randX - 20, global.randY - 20);
		glVertex2i(global.randX + 20, global.randY - 20);
		glVertex2i(global.randX - 20, global.randY + 20);
	}

	//if we are creating trails, draw the pixels representing them
	if (global.trails)
	{
		int direction = rand() + 1 % 10;
		for (int i = 0; i < 10; i++)
			glVertex2i(global.randX - (10 * i) * (direction % 2 == 0 ? 1 : -1), global.randY - (50 * i));
	}

	glEnd(); //we are done drawing points

	//ensure we pop the matrix after drawing
	if (global.tilted)
		glPopMatrix();

	glFlush(); //always call glFlush() at the end of drawing to ensure the buffer is flushed to display
}

//reset all variables to their defaults
void reset()
{
	global.speed = 1000;
	global.frozen = false;
	global.randomColour = false;
	global.smallFirework = false;
	global.bigFirework = false;
	global.overlayFireworks = false;
	global.centreOnly = false;
	global.daytime = false;
	global.waterworks = false;
	global.tilted = false;
	global.inverse = false;
	global.trails = false;

	global.resetWindow = false;
}

//handles drawing fireworks and resetting the application
void visualizer()
{
	if (global.resetWindow)
	{
		reset();
		glClear(GL_COLOR_BUFFER_BIT);
	}

	if (!global.frozen)
		draw_firework();
}

//implements glutTimerFunc() to create a consistent time regardless of computer
//this calls glutPostRedisplay() to redraw the window on the time interval of global.speed
void timer(int)
{
	glutPostRedisplay();
	glutTimerFunc(global.speed, timer, 0);
}

 /*glut keyboard function*/
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 0x1B:
	case'q':
	case 'Q':
		exit(0);
		break;
	case 'r':
	case 'R':
		global.resetWindow = true;
		break;
	case 'f':
	case 'F':
		global.frozen = !global.frozen;
	}
}//keyboard

 //Glut menu callback function
void menuFunc(int value)
{
	switch (value) {
	case MENU_QUIT:
		exit(0);
		break;
	case MENU_SLOW:
		global.frozen = false;
		global.speed = 1000;
		break;
	case MENU_FAST:
		global.frozen = false;
		global.speed = 500;
		break;
	case MENU_MAX:
		global.frozen = false;
		global.speed = 0;
		break;
	case MENU_FREEZE:
		global.frozen = !global.frozen;
		break;
	case MENU_VARIATION1:
		global.randomColour = !global.randomColour;
		break;
	case MENU_VARIATION2:
		global.smallFirework = !global.smallFirework;
		break;
	case MENU_VARIATION3:
		global.bigFirework = !global.bigFirework;
		break;
	case MENU_VARIATION4:
		global.overlayFireworks = !global.overlayFireworks;
		break;
	case MENU_VARIATION5:
		global.centreOnly = !global.centreOnly;
		break;
	case MENU_VARIATION6:
		global.daytime = !global.daytime;
		break;	
	case MENU_VARIATION7:
		global.waterworks = !global.waterworks;
		break;
	case MENU_VARIATION8:
		global.tilted = !global.tilted;
		break;
	case MENU_VARIATION9:
		global.inverse = !global.inverse;
		break;
	case MENU_VARIATION10:
		global.trails = !global.trails;
		break;
	case MENU_RESET:
		global.resetWindow = true;
		break;
	}
}//menuFunc

void show_keys()
{
	printf("Q:quit\nR:reset\nF:freeze\n");
}

//Glut menu set up
void init_menu()
{
	//menu for speed options
	int speed_menu = glutCreateMenu(&menuFunc);
	glutAddMenuEntry("Slow", MENU_SLOW);
	glutAddMenuEntry("Fast", MENU_FAST);
	glutAddMenuEntry("Max", MENU_MAX);
	glutAddMenuEntry("Freeze", MENU_FREEZE);

	//menu for effect options
	int effect_menu = glutCreateMenu(&menuFunc);
	glutAddMenuEntry("Random Colour", MENU_VARIATION1);
	glutAddMenuEntry("Small Fireworks", MENU_VARIATION2);
	glutAddMenuEntry("Big Fireworks", MENU_VARIATION3);
	glutAddMenuEntry("Overlay Fireworks", MENU_VARIATION4);
	glutAddMenuEntry("Centre Only", MENU_VARIATION5);
	glutAddMenuEntry("Daytime", MENU_VARIATION6);
	glutAddMenuEntry("Waterworks", MENU_VARIATION7);
	glutAddMenuEntry("Tilted", MENU_VARIATION8);
	glutAddMenuEntry("Inverted", MENU_VARIATION9);
	glutAddMenuEntry("Trails", MENU_VARIATION10);

	int main_menu = glutCreateMenu(&menuFunc);
	glutAddSubMenu("Speed", speed_menu);
	glutAddSubMenu("Effect", effect_menu);
	glutAddMenuEntry("Reset", MENU_RESET);
	glutAddMenuEntry("Quit", MENU_QUIT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

//what runs the whole show and stuff
int main(int argc, char** argv)
{
	srand(static_cast <unsigned> (time(0))); //set a new random seed

	global.w = 500; //window size 500 x 500
	global.h = 500;
	global.speed = 1000; //default speed of 1000 ms

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowSize(global.w, global.h);
	glutCreateWindow("SUPER VISUALIZATION APP");
	glShadeModel(GL_SMOOTH);
	glutKeyboardFunc(keyboard);
	glMatrixMode(GL_MODELVIEW);
	glOrtho(0, global.w, 0, global.h, 0, 1);

	init_menu();
	show_keys();

	glutDisplayFunc(visualizer);
	glutTimerFunc(global.speed, timer, 0);

	glutMainLoop();
}