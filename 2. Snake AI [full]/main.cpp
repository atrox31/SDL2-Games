#include <SDL.h>
#include <time.h>
#include <vector>
#include <iostream>

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const int TITLE_DIMENSIONS = 32;
const int FRAMERATE = 60;
const int TITLE_WIDTH = SCREEN_WIDTH / TITLE_DIMENSIONS;
const int TITLE_HEIGHT = SCREEN_HEIGHT / TITLE_DIMENSIONS;
SDL_Renderer* gRenderer;
SDL_Window* gWindow;
const bool DEBUG_MODE = false;

int get(int x, int y) {
	return x + y * TITLE_WIDTH;
}

void DrawRect(int x, int y, int style) {

	switch (style){
	case -4:
		SDL_SetRenderDrawColor(gRenderer, 100, 100, 255, 160);
		break;
	case -3:
		SDL_SetRenderDrawColor(gRenderer, 100, 255, 100, 10);
		break;
	case -2:
		SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 10);
		break;
	case -1:
		SDL_SetRenderDrawColor(gRenderer, 100, 180, 200, 255);
		break;
	case 0:
		SDL_SetRenderDrawColor(gRenderer, 200, 255, 100, 255);
		break;
	default:
		int s = (252 - style * 10);
		SDL_SetRenderDrawColor(gRenderer, (s<50?50:s), 252, 252, 255);
		break;
	}
	const SDL_Rect r = { x * TITLE_DIMENSIONS, y * TITLE_DIMENSIONS, TITLE_DIMENSIONS, TITLE_DIMENSIONS };
	SDL_RenderFillRect(gRenderer, &r);

	const SDL_Rect r2 = { r.x + 1, r.y + 1, r.w - 2, r.h - 2 };
	const SDL_Rect r3 = { r.x + 2, r.y + 2, r.w - 4, r.h - 4 };
	const SDL_Rect r4 = { r.x + 3, r.y + 3, r.w - 6, r.h - 6 };
	SDL_SetRenderDrawColor(gRenderer, 10, 10, 10, 40);
	SDL_RenderDrawRect(gRenderer, &r);
	SDL_SetRenderDrawColor(gRenderer, 20, 20, 20, 30);
	SDL_RenderDrawRect(gRenderer, &r2);
	SDL_SetRenderDrawColor(gRenderer, 30, 30, 30, 20);
	SDL_RenderDrawRect(gRenderer, &r3);
	SDL_SetRenderDrawColor(gRenderer, 40, 40, 40, 10);
	SDL_RenderDrawRect(gRenderer, &r4);

}
struct Fruit;
struct Snake;
SDL_Point getNextField(Fruit* fruit, Snake* snake, bool diagonal = false);
struct Fruit {
	SDL_Point location;
	int* map;

	Fruit(int* map) {
		this->map = map;
		GetNewLocation();
	}
	void GetNewLocation() {
		std::vector<SDL_Point> newPoints;
		for (int i = 0; i < TITLE_WIDTH; i++) {
			for (int j = 0; j < TITLE_HEIGHT; j++) {
				if (map[get(i, j)] == 0 && (i != location.x && j != location.y)) newPoints.push_back({ i, j });
			}
		}
		//std::cout << newPoints.size();// << std::endl;
		if (newPoints.size() == 0) {
			location = { -1,-1 };
			return;
		}
		location = newPoints[rand() % newPoints.size()];
	}
	bool Spawn() {
	GetNewLocation();
	return (location.x != -1);
	}
};

struct Snake {
	int x, y, vx, vy;
	unsigned int lenght;
	bool alive;
	struct Tail {
		int x, y, age;
		Tail(int x, int y, int age) {
			this->x = x;
			this->y = y;
			this->age = age;
		}
	};
	std::vector<Tail*> tail;
	Snake() {
		x = SCREEN_WIDTH / TITLE_DIMENSIONS / 2;
		y = SCREEN_HEIGHT / TITLE_DIMENSIONS / 2;
		vx = 0;
		vy = 0;
		lenght = 3;
		alive = true;
		
	}
	void Update(Fruit* fruit) {
		if (!alive) return;
		// move
		x += vx;
		y += vy;
		//check boudry
		if (x<0 || x>TITLE_WIDTH - 1 || y<0 || y>TITLE_HEIGHT - 1) {
			alive = false;
			return;
		}
		// check tail
		for (Snake::Tail* t : tail) {
			if (t->x == x && t->y == y) {
				alive = false;
				return;
			}
		}
		// add tail on prev x,y
		if (tail.size() <= lenght) {
			tail.push_back(new Tail(x, y, lenght));
		}
		// eat fruit
		if (fruit->location.x == x && fruit->location.y == y) {
			lenght++;
			if (fruit->Spawn()) {
				//std::cout << " " <<lenght << std::endl;
				//path = getNextField(fruit, this);
				tail.push_back(new Tail(x-vx, y-vy, lenght));
			}
			else {
				alive = false;
			}
		}
		// clean tail
		tail.erase(
			std::remove_if(
				tail.begin(),
				tail.end(),
				[](Tail* const& p) { return --p->age <= 0; }
			),
			tail.end()
		);
	}
};

SDL_Point getNextField(Fruit* fruit, Snake* snake, bool diagonal) {
	// direction table
	int search_index = (diagonal ? 8 : 4);
	SDL_Point search_array[8] = {
		{-1, 0},
		{1 , 0},
		{0 ,-1},
		{0 , 1},

		{-1, -1},
		{-1, 1},
		{1, 1},
		{1, -1}

	};
	// if fruit near
	for (int i = 0; i < search_index; i++) {
		if (fruit->location.x == snake->x + search_array[i].x &&
			fruit->location.y == snake->y + search_array[i].y) {
			return fruit->location;
		}
	}

	// array of fields
	int fields[TITLE_WIDTH * TITLE_HEIGHT];
	std::fill_n(fields, TITLE_WIDTH * TITLE_HEIGHT, 0);

	// array of fields
	int patch[TITLE_WIDTH * TITLE_HEIGHT];
	std::fill_n(patch, TITLE_WIDTH * TITLE_HEIGHT, TITLE_WIDTH * TITLE_HEIGHT);

	// get snake tail
	for (Snake::Tail* t : snake->tail) {
		fields[get(t->x, t->y)] = t->age + 1;
	}

	// varibles
	int current_step = 0;
	int max_step = TITLE_WIDTH * TITLE_HEIGHT;
	bool search_for_food = true;
	bool food_has_found = false;

	std::vector<SDL_Point> current_search_points;
	std::vector<SDL_Point> next_search_points;

	// starting point
	current_search_points.push_back({ snake->x, snake->y });
	while (search_for_food) {
		if (current_step > max_step) break;
		for (SDL_Point p : current_search_points) {
			for (int i = 0; i < search_index; i++) {
				int xx = p.x + search_array[i].x;
				int yy = p.y + search_array[i].y;
				if (DEBUG_MODE) {
					DrawRect(xx, yy, -2);
					SDL_RenderPresent(gRenderer);
					//SDL_Delay(3);
				}

				if (xx >= 0 && xx < TITLE_WIDTH && yy >= 0 && yy < TITLE_HEIGHT) {
					if (fruit->location.x == xx && fruit->location.y == yy) {
						search_for_food = false;
						food_has_found = true;
					}
					else {
						if (patch[get(xx, yy)] > current_step) {
							if (fields[get(xx, yy)] - current_step <= 0) {
								patch[get(xx, yy)] = current_step;
								next_search_points.push_back({ xx,yy });
								if (DEBUG_MODE) {
									DrawRect(xx, yy, -3);
									SDL_RenderPresent(gRenderer);
									SDL_Delay(3);
								}
							}
						}
					}
				}
			}
		}
		current_search_points = next_search_points;
		next_search_points.clear();
		current_step++;
	}

	if (!food_has_found) {
		for (int i = 0; i < search_index; i++) {
			int xx = snake->x + search_array[i].x;
			int yy = snake->y + search_array[i].y;
			if (patch[get(xx, yy)] == 0) {
				return { xx,yy };
			}
		}
			return { snake->x, snake->y };
	}
	else {
		current_step--;
		SDL_Point current_point = fruit->location;
		while (true) {
			for (int i = 0; i < search_index; i++) {
				int xx = current_point.x + search_array[i].x;
				int yy = current_point.y + search_array[i].y;
				if (xx >= 0 && xx < TITLE_WIDTH && yy >= 0 && yy < TITLE_HEIGHT) {
					if (patch[get(xx, yy)] < current_step) {
						if (DEBUG_MODE) {
							DrawRect(xx, yy, -4);
							//SDL_RenderPresent(gRenderer);
							//SDL_Delay(10);
						}
						current_step--;
						if (current_step == 0) {
							return current_point;
						}
						current_point = { xx, yy };
						break;
					}
				}
			}
		}
	}
}


int main(int argc, char* argv[]) {
	srand(time(NULL));
	SDL_Init(SDL_INIT_VIDEO);
	gWindow = SDL_CreateWindow("SelfSnake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);

	SDL_SetWindowFullscreen(gWindow,
		SDL_WINDOW_FULLSCREEN);

	int map[TITLE_WIDTH * TITLE_HEIGHT] = { 0 };
	std::fill_n(map, TITLE_WIDTH * TITLE_HEIGHT, 0);
	Snake snake;
	Fruit fruit(map);
	snake.vx = 1;
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
	SDL_RenderClear(gRenderer);
	SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
	bool game = true;
	while (game) {
		if (snake.alive) {
			// update map
			for (int i = 0; i < TITLE_WIDTH; i++) {
				for (int j = 0; j < TITLE_HEIGHT; j++) {
					map[get(i, j)] = 0;
				}
			}
			for (Snake::Tail* t : snake.tail) {
				map[get(t->x, t->y)] = t->age;
			}
			map[get(snake.x, snake.y)] = 1;
			// update snake
			snake.Update(&fruit);
			SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
			SDL_RenderClear(gRenderer);

			// draw
			if (DEBUG_MODE) {
				for (Snake::Tail* t : snake.tail) {
					DrawRect(t->x, t->y, t->age);
				}

				DrawRect(snake.x, snake.y, 0);
				DrawRect(fruit.location.x, fruit.location.y, -1);
				//SDL_RenderPresent(gRenderer);
			}

			//events
			if (snake.alive) {
				
				SDL_Point nextField = getNextField(&fruit, &snake, false);
				snake.vx = nextField.x - snake.x;
				snake.vy = nextField.y - snake.y;
				if (snake.vx < -1) snake.vx = -1;
				if (snake.vy < -1) snake.vy = -1;
				if (snake.vy > 1) snake.vy = 1;
				if (snake.vx > 1) snake.vx = 1;
				if (abs(snake.vx) + abs(snake.vy) > 1) {
			//snake.vy = 0;
				}
				//std::cout << snake.x << " " << snake.y << "(" << snake.vx << "," << snake.vy << ")"<< std::endl;
			}
		}
		// event
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				game = false;
			}
		}
		// draw
		DrawRect(fruit.location.x, fruit.location.y, -1);
		DrawRect(snake.x, snake.y, 0);
		//SDL_RenderPresent(gRenderer);
		for (Snake::Tail* t : snake.tail) {
			DrawRect(t->x, t->y, t->age);
			//SDL_Delay(10);
		}
		if (DEBUG_MODE) {
			SDL_Delay(10);
		}
		else {
			SDL_Delay(60);
		}
			SDL_RenderPresent(gRenderer);
	}
	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	SDL_Quit();
	return 0;
}