# allegro_5_billiard_game
hello everyone 
I am Mustafa Gülsoy. now, I am student on Çukurova Univesty 
and this billiard game is our semester project
if someone need to write allegro game, I load it to reach anybody

I left my teammater's username below 
We builed our game with functions, every function takes one mission.

and here are the heart functions of the game:


void UpdateBall(Ball* ball); // update our balls when collision happens
This function is responsible for approxmitely everything, it updates the position, velocity, acceleration of all balls; we include to kinds of collosion: the static collosion and the dynamic collosion.

_____________________________________


void CheckIfIn(Ball* ball);

This function check every ball that they are in hole or not if they are in hole,it change draw parameter from true to false
and set coordinat as x = 10000, y = 10000. 
when a ball in hole for firts time ,its kind sets target to player who throw into as target parameter,
when a player throw  all target ball into hole its blackfree parameter turn true,

if white ball in hole then turn pass to opponent player

if 8 ball(black ball) in hole,it does not matter that who throw into, it change player's winner parameter to true

if a player could not throw ball  into any hole then its turn get pass


it takes ball as parameter  

_________________________________________

bool checkBallStop(Ball* ball);
if every ball stop the function retrun true.

it take ball as parameter

________________________________________
void InitBall(Ball* ball);
This function set default value for balls 


it take ball as parameter

_________________________________________
void DrawBall(Ball* ball);
This function draw image of balls depend on their direction and frame.


it takes ball as parameter


_________________________________________
void loadImage(Ball* ball);
This function load image from drivers to game


it takes ball as parameter

___________________________________________
void setDirection(Ball* ball);
This function set direction based on coordinate of hitting ball as sourceY parameters


it takes ball as parameter

__________________________________________
void setFrame(Ball* ball);
This function set frame depend on how long ball rolling and it keep values as sourceX parameters.

it takes ball as parameter

_____________________________________________
bool playertarget1(Ball* ball);
bool playertarget2(Ball* ball);
these  two function check any player reach its gola except 8 ball(black ball),
if one of two  does then, retrun true if not then return false
_____________________________________________
