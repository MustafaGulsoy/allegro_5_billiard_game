#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <math.h>
#include <stdio.h>
#include<allegro5/allegro_audio.h>
#include<allegro5/allegro_acodec.h>

int WBenter = 0;

struct Ball
{
	float px, py; // positin
	float vx, vy; // velocity
	float ax, ay; // accerelation
	float radius; // R/2 == r
	float mass = radius * 10.0f; // mass
	float sourX, sourY; // bitmap coordinat into image 
	ALLEGRO_BITMAP* image;
	int dir = 0;
	bool draw = true;
	int target;
	int Kind;
	ALLEGRO_TIMER* frame;

};
struct Players
{
	int target;
	bool blackFree;
	bool winner = false;

};

// =======================================
			//Globals
// =======================================
const int WIDTH = 910;
const int HEIGHT = 520;
const int BALLNUM = 16; // balls number
const float velocityRate = 60.0; // speed rate
enum KEYS { UP, DOWN, LEFT, RIGHT, SPACE };
enum KEYSMOUSE { PRESS1, PRESS2 };
bool KEYSMOUSE[2] = { false, false };
bool KEYS[5] = { false,false,false,false,false };
enum mainBalls { WHITE, BLACK };
Players player[2];
int turn = 1;
int moving = 0;

ALLEGRO_SAMPLE* soundeffect = NULL;
ALLEGRO_SAMPLE* pots = NULL;
ALLEGRO_SAMPLE* win = NULL;

// =======================================
		//prototypes
// =======================================
void CheckIfIn(Ball* ball);
bool checkBallStop(Ball* ball);
void InitBall(Ball* ball); //Initialize out balls
void DrawBall(Ball* ball);// drawing our balls
void UpdateBall(Ball* ball); // update our balls when collision happens
void CheckIfHit(Ball* ball);// check if the balls reach and hit the screen ends
bool DoCircleOverLab(Ball& ball, Ball& target); // colloision ?
bool IsPointInCircle(Ball& ball, float mX, float mY); // mouse points
void loadImage(Ball* ball);
void DestroyFrames(Ball* ball);
void setDirection(Ball* ball);
void setFrame(Ball* ball);
bool playertarget1(Ball* ball);
bool playertarget2(Ball* ball);
void SetWhiteBall(Ball* ball);
bool ses(Ball* ball);
int main() {

	// ========================================
			//primative variables
	// ========================================
	bool done = false;
	bool play = false;
	int FBS = 60; //timer
	int FBSframes = 10;
	Ball* pSelectedBall = nullptr; // the selected ball by mouse
	bool release = false; // if the mouse botton was realesed
	int m_x = 0;
	int m_y = 0;


	// ========================================
			//object variables
	// ========================================
	Ball ball[BALLNUM];// balls array




// =========================================
		//Allegro variabels
// =========================================
	ALLEGRO_DISPLAY* display = NULL;
	ALLEGRO_EVENT_QUEUE* event_queue = NULL;
	ALLEGRO_TIMER* timer = NULL, * ballTimer = NULL;
	ALLEGRO_BITMAP* table, * login, * cue, * player1Won, * player2Won;


	// ========================================
			//Init	
	// ========================================
	if (!al_init())
		return -1;
	display = al_create_display(WIDTH, HEIGHT);
	if (!display)
		return -2;





	InitBall(ball);

	al_install_audio();
	al_init_acodec_addon();

	al_install_keyboard();
	al_install_mouse();
	al_init_image_addon();
	al_init_primitives_addon();
	timer = al_create_timer(1.0 / FBS);
	ballTimer = al_create_timer(1.0 / FBSframes);

	event_queue = al_create_event_queue();
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_timer_event_source(ballTimer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_mouse_event_source());

	soundeffect = al_load_sample("collide.wav");
	pots = al_load_sample("pot.wav");
	win = al_load_sample("honor.wav");
	al_reserve_samples(5);

	table = al_load_bitmap("table.jpg");
	login = al_load_bitmap("login screen.jpg");
	cue = al_load_bitmap("billiard cue.png");
	player1Won = al_load_bitmap("Player 1 won.png");
	player2Won = al_load_bitmap("Player 2 won.png");
	loadImage(ball);

	al_start_timer(timer);
	al_start_timer(ballTimer);
	//void al_draw_rotated_bitmap(ALLEGRO_BITMAP * bitmap,float cx, float cy, float dx, float dy, float angle, int flags)
	while (!done)
	{




		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);




		al_draw_bitmap(table, 0, 0, NULL);

		if (ev.type == ALLEGRO_EVENT_KEY_DOWN) { // not important

			switch (ev.keyboard.keycode)
			{
			case ALLEGRO_KEY_DOWN:
				KEYS[DOWN] = true;
				break;
			case ALLEGRO_KEY_UP:
				KEYS[UP] = true;
				break;
			case ALLEGRO_KEY_LEFT:
				KEYS[LEFT] = true;
				break;
			case ALLEGRO_KEY_RIGHT:
				KEYS[RIGHT] = true;
				break;
			case ALLEGRO_KEY_ESCAPE:
				done = true;
				break;
			}
		}

		else if (ev.type == ALLEGRO_EVENT_KEY_UP) { // not important
			switch (ev.keyboard.keycode)
			{
			case ALLEGRO_KEY_DOWN:
				KEYS[DOWN] = false;
				break;
			case ALLEGRO_KEY_UP:
				KEYS[UP] = false;
				break;
			case ALLEGRO_KEY_LEFT:
				KEYS[LEFT] = false;
				break;
			case ALLEGRO_KEY_RIGHT:
				KEYS[RIGHT] = false;
				break;
			}
		}

		else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {


			if (ev.mouse.button & 2 && checkBallStop(ball))
				KEYSMOUSE[PRESS2] = true;


		}

		else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
			if (ev.mouse.button & 1) {
				pSelectedBall = nullptr;
				KEYSMOUSE[PRESS1] = false;
			}

			if (ev.mouse.button & 2) {
				if (pSelectedBall) {
					pSelectedBall->vx = 5.0f * ((pSelectedBall->px) - (float)ev.mouse.x);
					pSelectedBall->vy = 5.0f * ((pSelectedBall->py) - (float)ev.mouse.y);
					al_play_sample(soundeffect, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);

					moving = 1;
				}
				KEYSMOUSE[PRESS2] = false;
				pSelectedBall = nullptr;

			}
		}

		else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) {
			m_x = ev.mouse.x;
			m_y = ev.mouse.y;

			if (KEYSMOUSE[PRESS1] || KEYSMOUSE[PRESS2]) {
				for (size_t i = 0; i < BALLNUM; i++)
				{
					if (IsPointInCircle(ball[0], ev.mouse.x, ev.mouse.y)) {

						pSelectedBall = &ball[0];
						break;
					}
				}
			}



			if (pSelectedBall && KEYSMOUSE[PRESS1] && !KEYSMOUSE[PRESS2]) {
				pSelectedBall->px = ev.mouse.x;
				pSelectedBall->py = ev.mouse.y;
			}
		}

		else if (ev.type == ALLEGRO_EVENT_TIMER) {

			if (ev.timer.source == ballTimer)
			{
				for (int i = 1; i < 16; i++) {

					setFrame(ball);
				}
			}
			else
				if (ev.timer.source == timer) {
					UpdateBall(ball); // al our game is in this function which is defined below
					CheckIfHit(ball);

					setDirection(ball);
					CheckIfIn(ball);
					if (WBenter) {
						SetWhiteBall(ball);
					}
					if (pSelectedBall && KEYSMOUSE[PRESS2]) {
						float x_mm = pSelectedBall->px + (pSelectedBall->px - m_x);
						float y_mm = pSelectedBall->py + (pSelectedBall->py - m_y);
						al_draw_line(m_x, m_y, pSelectedBall->px, pSelectedBall->py, al_map_rgb(255, 0, 0), 5);

						al_draw_line(pSelectedBall->px, pSelectedBall->py, x_mm, y_mm, al_map_rgb(255, 0, 0), 5);
						al_draw_circle(x_mm, y_mm, 5, al_map_rgb(0, 0, 255), 1);




					}
					DrawBall(ball); // draw our array balls

					if (player[0].blackFree == false && ball[8].draw == false && player[0].winner == true)
					{
						al_play_sample(win, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
						al_draw_bitmap(player2Won, 200, 170, NULL);
						done = true;


					}
					if (player[1].blackFree == false && ball[8].draw == false && player[1].winner == true)
					{
						al_draw_bitmap(player1Won, 300, 170, NULL);
						al_play_sample(win, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
						done = true;



					}


					if (player[0].blackFree == true && ball[8].draw == false && player[0].winner == true)
					{
						al_draw_bitmap(player1Won, 300, 170, NULL);
						al_play_sample(win, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
						done = true;


					}
					else

						if (player[1].blackFree == true && ball[8].draw == false && player[1].winner == true)
						{
							al_draw_bitmap(player2Won, 300, 170, NULL);
							al_play_sample(win, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
							done = true;


						}
					al_flip_display();
				}

		}





	}

	al_rest(2.0);

	delete pSelectedBall;
	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
	al_destroy_timer(ballTimer);
	al_destroy_display(display);
	al_destroy_bitmap(player1Won);
	al_destroy_bitmap(player2Won);
	al_destroy_bitmap(cue);
	al_destroy_bitmap(login);
	al_destroy_bitmap(table);
	DestroyFrames(ball);


	return 0;
}

/// ================================
		// Functions Implementation 
//=================================
void InitBall(Ball* ball) {

	ball[0].px = 300;
	ball[0].py = 250;

	ball[1].px = 487;
	ball[1].py = 250;

	ball[2].px = 534;
	ball[2].py = 250;

	ball[3].px = 533;
	ball[3].py = 297;

	ball[4].px = 531;
	ball[4].py = 202;

	ball[5].px = 588;
	ball[5].py = 150;


	ball[6].px = 588;
	ball[6].py = 344;

	ball[7].px = 588;
	ball[7].py = 295;

	ball[8].px = 588;
	ball[8].py = 250;

	ball[9].px = 588;
	ball[9].py = 198;

	ball[10].px = 612;
	ball[10].py = 233;

	ball[11].px = 612;
	ball[11].py = 187;

	ball[12].px = 612;
	ball[12].py = 140;

	ball[13].px = 612;
	ball[13].py = 284;

	ball[14].px = 612;
	ball[14].py = 328;

	ball[15].px = 622;
	ball[15].py = 375;




	for (size_t i = 0; i < BALLNUM; i++)
	{

		ball[i].vx = 0;
		ball[i].vy = 0;

		ball[i].ax = 0;
		ball[i].ay = 0;

		ball[i].sourX = 0;

		ball[i].radius = 22;

		ball[i].target = 0;
		if (i < 8 && i> 0) {
			ball[i].Kind = 1;
		}
		else
			if (i != 8)
				ball[i].Kind = 2;
	}
	ball[8].Kind = 8;
}
void DrawBall(Ball* ball) {

	for (size_t i = 0; i < BALLNUM; i++)
	{
		if (ball[i].draw == true) {



			if (i == WHITE)
				al_draw_filled_circle(ball[i].px, ball[i].py, ball[i].radius, al_map_rgb(255, 255, 255));
			else
			{
				if (ball[i].dir < 2)

					al_draw_bitmap_region(ball[i].image, ball[i].sourX, ball[i].sourY, al_get_bitmap_width(ball[i].image) / 4, al_get_bitmap_height(ball[i].image) / 4, ball[i].px - 28, ball[i].py - 28, NULL);

				else if (ball[i].dir < 4) {
					al_draw_bitmap_region(ball[i].image, 3 * al_get_bitmap_width(ball[i].image) / 4 - ball[i].sourX, (ball[i].sourY), al_get_bitmap_width(ball[i].image) / 4, al_get_bitmap_height(ball[i].image) / 4, ball[i].px - 28, ball[i].py - 28, NULL);
				}
				else
					if (ball[i].dir < 5)
						al_draw_bitmap_region(ball[i].image, ball[i].sourX, (ball[i].sourY), al_get_bitmap_width(ball[i].image) / 4, al_get_bitmap_height(ball[i].image) / 4, ball[i].px - 28, ball[i].py - 28, NULL);

					else if (ball[i].dir < 6) {
						al_draw_bitmap_region(ball[i].image, 3 * al_get_bitmap_width(ball[i].image) / 4 - ball[i].sourX, ball[i].sourY, al_get_bitmap_width(ball[i].image) / 4, al_get_bitmap_height(ball[i].image) / 4, ball[i].px - 28, ball[i].py - 28, NULL);

					}
					else if (ball[i].dir == 7)
						al_draw_bitmap_region(ball[i].image, ball[i].sourX, (ball[i].sourY), al_get_bitmap_width(ball[i].image) / 4, al_get_bitmap_height(ball[i].image) / 4, ball[i].px - 28, ball[i].py - 28, NULL);
					else
						al_draw_bitmap_region(ball[i].image, 3 * al_get_bitmap_width(ball[i].image) / 4 - ball[i].sourX, 3 * al_get_bitmap_height(ball[i].image) / 4, al_get_bitmap_width(ball[i].image) / 4, al_get_bitmap_height(ball[i].image) / 4, ball[i].px - 28, ball[i].py - 28, NULL);

			}


		}
	}
}
void UpdateBall(Ball* ball) {
	// static collsion
	for (size_t cball = 0; cball < BALLNUM; cball++)
	{

		for (size_t ctarget = 0; ctarget < BALLNUM; ctarget++)
		{
			if (cball != ctarget && DoCircleOverLab(ball[cball], ball[ctarget])) {
				if (ses(ball))
					al_play_sample(soundeffect, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
				float fDistance = sqrtf((ball[cball].px - ball[ctarget].px) * (ball[cball].px - ball[ctarget].px) +
					(ball[cball].py - ball[ctarget].py) * (ball[cball].py - ball[ctarget].py));
				// normal vector
				float nx = (ball[cball].px - ball[ctarget].px) / fDistance;
				float ny = (ball[cball].py - ball[ctarget].py) / fDistance;

				// tangent vector
				float tx = -ny;
				float ty = nx;

				// Dot Product tangent
				float dpTan1 = ball[cball].vx * tx + ball[cball].vy * ty;
				float dpTan2 = ball[ctarget].vx * tx + ball[ctarget].vy * ty;

				// Dot Product Normal
				float dpNorm1 = ball[cball].vx * nx + ball[cball].vy * ny;
				float dpNorm2 = ball[ctarget].vx * nx + ball[ctarget].vy * ny;

				// Conservation of momentum in 1D
				float m1 = (dpNorm1 * (ball[cball].mass - ball[ctarget].mass) + 2.0f * ball[ctarget].mass * dpNorm2) / (ball[cball].mass + ball[ctarget].mass);
				float m2 = (dpNorm2 * (ball[cball].mass - ball[cball].mass) + 2.0f * ball[cball].mass * dpNorm1) / (ball[cball].mass + ball[ctarget].mass);




				float foverlap = 0.5f * (fDistance - ball[cball].radius - ball[ctarget].radius);

				ball[cball].px -= foverlap * (ball[cball].px - ball[ctarget].px) / fDistance;
				ball[cball].py -= foverlap * (ball[cball].py - ball[ctarget].py) / fDistance;


				ball[ctarget].px += foverlap * (ball[cball].px - ball[ctarget].px) / fDistance;
				ball[ctarget].py += foverlap * (ball[cball].py - ball[ctarget].py) / fDistance;

				ball[cball].vx = tx * dpTan1 + nx * m1;
				ball[cball].vy = ty * dpTan1 + ny * m1;
				ball[ctarget].vx = tx * dpTan2 + nx * m2;
				ball[ctarget].vy = ty * dpTan2 + ny * m2;



			}


		}


		//if (ball[cball].px < 0) ball[cball].px += (float)WIDTH;
		//if (ball[cball].px > WIDTH) ball[cball].px -= (float)WIDTH;
		//if (ball[cball].py < 0) ball[cball].py += (float)HEIGHT;
		//if (ball[cball].py > HEIGHT) ball[cball].py -= (float)HEIGHT;
	}

	//decreasing velocity
	for (size_t i = 0; i < BALLNUM; i++)
	{
		ball[i].ax = -ball[i].vx * 0.01f;
		ball[i].ay = -ball[i].vy * 0.01f;

		ball[i].vx += ball[i].ax;
		ball[i].vy += ball[i].ay;

		ball[i].px += ball[i].vx / velocityRate;
		ball[i].py += ball[i].vy / velocityRate;

		if (fabs((ball[i].vx * ball[i].vx) + (ball[i].vy * ball[i].vy)) < 100.0f) {
			ball[i].vx = 0.0;
			ball[i].vy = 0.0;
		}

	}

	//for (size_t cball = 0; cball < BALLNUM; cball++)
	//{
	//	if (ball[cball].px <= 41  + ball[cball].radius || ball[cball].px >= (float)WIDTH - 41 - ball[cball].radius)
	//		ball[cball].vx *= -1;
	//	if (ball[cball].py <= 41 + ball[cball].radius || ball[cball].py >= (float)HEIGHT - 41 - ball[cball].radius)
	//		ball[cball].vy *= -1;

	//}



}
bool DoCircleOverLab(Ball& ball, Ball& target) {
	return fabs((ball.px - target.px) * (ball.px - target.px) +
		(ball.py - target.py) * (ball.py - target.py) <=
		(ball.radius + target.radius) * (ball.radius + target.radius));
}
bool IsPointInCircle(Ball& ball, float mX, float mY) {
	return fabs((ball.px - mX) * (ball.px - mX) + (ball.py - mY) * (ball.py - mY) < ball.radius * ball.radius);
}
void CheckIfHit(Ball* ball) {
	for (size_t cball = 0; cball < BALLNUM; cball++)
	{
		if (ball[cball].draw == true) {
			if (ball[cball].px <= 0.0f + ball[cball].radius + 32) {
				ball[cball].px = 1.0f + ball[cball].radius + 32;
				ball[cball].vx *= -1.0f;
				al_play_sample(soundeffect, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
			}
			if (ball[cball].px >= (float)WIDTH - ball[cball].radius - 31) {
				ball[cball].px = (float)WIDTH - 1.0f - ball[cball].radius - 31;
				ball[cball].vx *= -1.0f;
				al_play_sample(soundeffect, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
			}
			if (ball[cball].py <= 0.0f + ball[cball].radius + 27) {
				ball[cball].py = 1.0f + ball[cball].radius + 27;
				ball[cball].vy *= -1.0f;
				al_play_sample(soundeffect, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
			}

			if (ball[cball].py >= (float)HEIGHT - ball[cball].radius - 27) {
				ball[cball].py = (float)HEIGHT - 1.0f - ball[cball].radius - 27;
				ball[cball].vy *= -1.0f;
				al_play_sample(soundeffect, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
			}
		}
	}
}
void loadImage(Ball* ball) {


	ball[1].image = al_load_bitmap("billiard ball 1.png");
	ball[2].image = al_load_bitmap("billiard ball 2.png");
	ball[3].image = al_load_bitmap("billiard ball 3.png");
	ball[4].image = al_load_bitmap("billiard ball 4.png");
	ball[5].image = al_load_bitmap("billiard ball 5.png");
	ball[6].image = al_load_bitmap("billiard ball 6.png");
	ball[7].image = al_load_bitmap("billiard ball 7.png");
	ball[8].image = al_load_bitmap("billiard ball 8.png");
	ball[9].image = al_load_bitmap("billiard ball 9.png");
	ball[10].image = al_load_bitmap("billiard ball 10.png");
	ball[11].image = al_load_bitmap("billiard ball 11.png");
	ball[12].image = al_load_bitmap("billiard ball 12.png");
	ball[13].image = al_load_bitmap("billiard ball 13.png");
	ball[14].image = al_load_bitmap("billiard ball 14.png");
	ball[15].image = al_load_bitmap("billiard ball 15.png");




}
void setDirection(Ball* ball) {

	for (int i = 1; i < 16; i++) {
		if (ball[i].vx > 0 && ball[i].vy < 0)
			ball[i].dir = 0;
		if (ball[i].vx < 0 && ball[i].vy == 0)
			ball[i].dir = 2;
		if (ball[i].vx == 0 && ball[i].vy < 0)
			ball[i].dir = 1;
		if (ball[i].vx == 0 && ball[i].vy > 0)
			ball[i].dir = 3;
		if (ball[i].vx > 0 && ball[i].vy > 0)
			ball[i].dir = 4;
		if (ball[i].vx > 0 && ball[i].vy < 0)
			ball[i].dir = 6;
		if (ball[i].vx < 0 && ball[i].vy < 0)
			ball[i].dir = 5;
		if (ball[i].vx < 0 && ball[i].vy > 0)
			ball[i].dir = 7;



		if (ball[i].dir < 2) {
			ball[i].sourY = 0;
		}
		else
			if (ball[i].dir < 4) {
				ball[i].sourY = al_get_bitmap_height(ball[i].image) / 4;
			}
			else
				if (ball[i].dir < 6) {
					ball[i].sourY = al_get_bitmap_height(ball[i].image) / 4 * 2;
				}
				else
					if (ball[i].dir < 8) {
						ball[i].sourY = al_get_bitmap_height(ball[i].image) / 4 * 3;
					}



	}
}

void setFrame(Ball* ball) {


	for (int i = 1; i < 16; i++) {
		if (ball[i].vx != 0 || ball[i].vy != 0)
		{


			ball[i].sourX += al_get_bitmap_width(ball[i].image) / 4;
			if (ball[i].sourX >= al_get_bitmap_width(ball[i].image) * 3 / 4)
				ball[i].sourX = 0;
		}
	}

}

void CheckIfIn(Ball* ball) {

	player[0].blackFree = false;
	player[1].blackFree = false;
	static bool itisIn;


	for (int i = 0; i < 16; i++) {

		if (ball[i].draw == false)

			continue;
		else
		{
			if (((ball[i].px - 40) * (ball[i].px - 40) + (ball[i].py - 40) * (ball[i].py - 40)) <= (22 * 22))
			{
				al_play_sample(pots, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
				ball[i].draw = false;
				ball[i].px = 10000;
				ball[i].py = 10000;
				ball[i].vx = 0;
				ball[i].vy = 0;
				itisIn = true;

				if (player[0].target == NULL)
					player[0].target = ball[i].Kind;
				if (player[1].target == NULL)
					player[1].target = ball[i].Kind;

				if (i == 0) {
					WBenter = true;
					switch (turn)
					{
					case 1:
						turn = 2;
						break;
					case 2:
						turn = 1;
						break;

					}
				}
				if (i == 8)
					switch (turn)
					{
					case 1:
						player[0].winner = true;
						break;
					case 2:
						player[1].winner = true;
					default:
						break;
					}
			}


			else
				if (((ball[i].px - 450) * (ball[i].px - 450) + (ball[i].py - 40) * (ball[i].py - 40)) <= (22 * 22))
				{
					al_play_sample(pots, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
					ball[i].draw = false;
					ball[i].px = 10000;
					ball[i].py = 10000;
					ball[i].vx = 0;
					ball[i].vy = 0;
					itisIn = true;



					if (player[0].target == NULL)
						player[0].target = ball[i].Kind;
					if (player[1].target == NULL)
						player[1].target == ball[i].Kind;

					if (i == 0) {
						WBenter = true;

						switch (turn)
						{
						case 1:
							turn = 2;
							break;
						case 2:
							turn = 1;
							break;

						}
					}
					if (i == 8)
						switch (turn)
						{
						case 1:
							player[0].winner = true;
							break;
						case 2:
							player[1].winner = true;
						default:
							break;
						}

				}
				else
					if (((ball[i].px - 870) * (ball[i].px - 870) + (ball[i].py - 40) * (ball[i].py - 40)) <= (22 * 22))
					{
						al_play_sample(pots, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
						ball[i].draw = false;
						ball[i].px = 10000;
						ball[i].py = 10000;
						ball[i].vx = 0;
						ball[i].vy = 0;

						itisIn = true;

						if (player[0].target == NULL)
							player[0].target = ball[i].Kind;
						if (player[1].target == NULL)
							player[1].target == ball[i].Kind;

						if (i == 0) {
							WBenter = true;

							switch (turn)
							{
							case 1:
								turn = 2;
								break;
							case 2:
								turn = 1;
								break;

							}
						}
						if (i == 8)
							switch (turn)
							{
							case 1:
								player[0].winner = true;
								break;
							case 2:
								player[1].winner = true;
							default:
								break;
							}

					}
					else
						if (((ball[i].px - 40) * (ball[i].px - 40) + (ball[i].py - 480) * (ball[i].py - 480)) <= (22 * 22))
						{
							al_play_sample(pots, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
							ball[i].draw = false;
							ball[i].px = 10000;
							ball[i].py = 10000;
							ball[i].vx = 0;
							ball[i].vy = 0;

							itisIn = true;

							if (player[0].target == NULL)
								player[0].target = ball[i].Kind;
							if (player[1].target == NULL)
								player[1].target == ball[i].Kind;

							if (i == 0) {
								WBenter = true;
								switch (turn)
								{
								case 1:
									turn = 2;
									break;
								case 2:
									turn = 1;
									break;

								}
							}
							if (i == 8)
								switch (turn)
								{
								case 1:
									player[0].winner = true;
									break;
								case 2:
									player[1].winner = true;
								default:
									break;
								}
						}
						else
							if (((ball[i].px - 450) * (ball[i].px - 450) + (ball[i].py - 480) * (ball[i].py - 480)) <= (22 * 22))
							{
								al_play_sample(pots, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
								ball[i].draw = false;
								ball[i].px = 10000;
								ball[i].py = 10000;
								ball[i].vx = 0;
								ball[i].vy = 0;

								itisIn = true;


								if (player[0].target == NULL)
									player[0].target = ball[i].Kind;
								if (player[0].target == NULL)
									player[0].target == ball[i].Kind;

								if (i == 0) {
									WBenter = true;

									switch (turn)
									{
									case 1:
										turn = 2;
										break;
									case 2:
										turn = 1;
										break;

									}
								}
								if (i == 8)
									switch (turn)
									{
									case 1:
										player[0].winner = true;
										break;
									case 2:
										player[1].winner = true;
									default:
										break;
									}

							}
							else
								if (((ball[i].px - 870) * (ball[i].px - 870) + (ball[i].py - 480) * (ball[i].py - 480)) <= (22 * 22))
								{
									al_play_sample(pots, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
									ball[i].draw = false;
									ball[i].px = 10000;
									ball[i].py = 10000;
									ball[i].vx = 0;
									ball[i].vy = 0;

									itisIn = true;

									if (player[0].target == NULL)
										player[0].target = ball[i].Kind;
									if (player[1].target == NULL)
										player[1].target == ball[i].Kind;

									if (i == 0) {
										WBenter = true;

										switch (turn)
										{
										case 1:
											turn = 2;
											break;
										case 2:
											turn = 1;
											break;

										}
									}
									if (i == 8)
										switch (turn)
										{
										case 1:
											player[0].winner = true;
											break;
										case 2:
											player[1].winner = true;
										default:
											break;
										}
								}









		}






	}



	if (player[0].target == 1)
	{
		if (playertarget1(ball))
			player[0].blackFree = true;
	}
	else if (playertarget2(ball))
		player[0].blackFree = true;

	if (player[1].target == 2)
	{
		if (playertarget2(ball))
			player[1].blackFree = true;
	}
	else if (playertarget1(ball))
		player[0].blackFree = true;


	if (checkBallStop(ball) && moving == 1)
	{
		moving = 0;

		switch (turn)
		{
		case 1:
			if (itisIn)

				turn = 1;
			else
				turn = 2;
			break;
		case 2:

			if (itisIn)
				turn = 2;
			else

				turn = 1;
			break;



		default:
			break;
		}
		itisIn = false;
	}
	if (ball[0].draw == false && checkBallStop(ball)) { // rewrting whilte ball
		ball[0].draw = true;
		ball[0].px = 300;
		ball[0].py = 250;
	}
}


bool checkBallStop(Ball* ball) {

	bool sum = true;
	for (int i = 0; i < 16; i++)
	{
		if (ball[i].draw == true)
			if (ball[i].vx != 0 && ball[i].vy != 0) {
				sum = false;
				break;
			}
	}
	return sum;



}


bool playertarget1(Ball* ball) {

	bool sum = true;
	for (int i = 1; i < 8; i++)
	{

		if (ball[i].draw == true)
			sum = false;


	}

	return sum;
}
bool ses(Ball* ball) {
	for (int cball = 0; cball < BALLNUM; cball++) {


		for (int ctarget = 0; ctarget < BALLNUM; ctarget++) {
			if (sqrt((ball[cball].px - ball[ctarget].px) * (ball[cball].px - ball[ctarget].px)) +

				sqrt((ball[cball].py - ball[ctarget].py) * (ball[cball].py - ball[ctarget].py)) <= 2 * sqrt(ball[cball].radius * ball[cball].radius)) {
				return true;

			}


		}

	}
}

bool playertarget2(Ball* ball) {

	bool sum = true;
	for (int i = 9; i < 16; i++)
	{

		if (ball[i].draw == true)
			sum = false;


	}

	return sum;
}
void SetWhiteBall(Ball* ball) {
	WBenter = false;
	ball[0].draw = true;
	ball[0].px = 200;
	ball[0].py = 200;
}
void DestroyFrames(Ball* ball) {
	for (size_t i = 1; i < BALLNUM; i++)
	{
		al_destroy_bitmap(ball[i].image);
	}
}