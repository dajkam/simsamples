/**
 * This is the simple hello world for SDL2.
 *
 * TODO:
 * 

    1. Load Bitmap

	SDL_Surface *bmp = SDL_LoadBMP( fname.c_str() );
	SDL_FreeSurface( ptr );

    2. Create texture
    
	SDL_Texture *tex = SDL_CreateTextureFromSurface( renderer, bitmap );
	SDL_DestroyTexture( tex );

    3. Load texture using lodepng

  	unsigned error = lodepng::decode(image, width, height, fname);
	if(error) throw std::runtime_error( lodepng_error_text(error));

	SDL_Surface* bitmap = SDL_CreateRGBSurfaceFrom( ... )

	4. draw texture to renderer
	
	SDL_RenderCopy( renderer.get(), game_texture_1.get(), NULL, &dstrect );

    4. collisions

	I will show 8 point collision

 */

#include <SDL2/SDL.h>
#include <lodepng.h>
#include <stdexcept>
#include <memory>
#include <string>
#include <set>
#include <tuple>
#include <iostream>
#include <cstdint>
#include <vector>
#include <random>

std::shared_ptr<SDL_Window> init_window(const int width = 320, const int height = 200)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		throw std::runtime_error(SDL_GetError());

	SDL_Window *win = SDL_CreateWindow("ToTheRoof",
									   SDL_WINDOWPOS_UNDEFINED, 
									   SDL_WINDOWPOS_UNDEFINED,
									   width, height, SDL_WINDOW_SHOWN);
	if (win == nullptr)
		throw std::runtime_error(SDL_GetError());
	std::shared_ptr<SDL_Window> window(win, [](SDL_Window *ptr) {
		SDL_DestroyWindow(ptr);
	});
	return window;
}

std::shared_ptr<SDL_Renderer> init_renderer(std::shared_ptr<SDL_Window> window)
{
	SDL_Renderer *ren = SDL_CreateRenderer(window.get(), -1, 
	SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr)
		throw std::runtime_error(SDL_GetError());
	std::shared_ptr<SDL_Renderer> renderer(ren, [](SDL_Renderer *ptr) {
		SDL_DestroyRenderer(ptr);
	});
	return renderer;
}

std::shared_ptr<SDL_Texture> load_texture(const std::shared_ptr<SDL_Renderer> renderer, const std::string fname)
{
	SDL_Surface *bmp = SDL_LoadBMP(fname.c_str());
	SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer.get(), bmp);
	std::shared_ptr<SDL_Texture> texture(tex, [](SDL_Texture *ptr) {
		SDL_DestroyTexture(ptr);
	});
	SDL_FreeSurface(bmp);
	return texture;
}

std::shared_ptr<SDL_Texture> load_png_texture(
	const std::shared_ptr<SDL_Renderer> renderer, 
	const std::string fname)
{
	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, fname); // load png image to vector (image)
	if (error)
		throw std::runtime_error(lodepng_error_text(error));

	SDL_Texture *tex = SDL_CreateTexture(renderer.get(), SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STATIC, width, height);
	if (tex == nullptr)
		throw std::runtime_error(SDL_GetError());
	std::shared_ptr<SDL_Texture> texture(tex, [](SDL_Texture *ptr) { SDL_DestroyTexture(ptr); });
	SDL_SetTextureBlendMode(texture.get(), SDL_BLENDMODE_BLEND); // obslugujemy polprzezroczystosc
	SDL_UpdateTexture(texture.get(), NULL, image.data(), width * sizeof(uint32_t));
	return texture;
}

std::shared_ptr<SDL_Texture> create_texture(
	const std::shared_ptr<SDL_Renderer> renderer, 
	const int w, const int h)
{
	SDL_Texture *tex = SDL_CreateTexture(renderer.get(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, w, h);
	if (tex == nullptr)
		throw std::runtime_error(SDL_GetError());
	std::shared_ptr<SDL_Texture> texture(tex, [](SDL_Texture *ptr) { SDL_DestroyTexture(ptr); });
	return texture;
}

using vec_t = std::array<double, 2>;

vec_t operator-(const vec_t &a, const vec_t &b)
{
	return {a[0] - b[0], a[1] - b[1]};
}
vec_t operator+(const vec_t &a, const vec_t &b)
{
	return {a[0] + b[0], a[1] + b[1]};
}
vec_t operator*(const vec_t &a, const vec_t &b)
{
	return {a[0] * b[0], a[1] * b[1]};
}
vec_t operator*(const vec_t &a, const double b)
{
	return {a[0] * b, a[1] * b};
}
vec_t operator/(const vec_t &a, const double b)
{
	return {a[0] / b, a[1] / b};
}
double length(const vec_t &d_)
{
	auto d = d_ * d_;
	return std::sqrt(d[0] + d[1]);
}



class kwadrat
{
private:
	/* std::vector<std::array<unsigned char, 4>> palette {
    {255, 0, 0, 255},  // red
    {0, 255, 0, 255},  // green
	SDL_SetRenderDrawColor(renderer.get(), 0x89, 0x04, 0xB1, 0x00);// fioletowy
}; */
public:

SDL_Rect rect;


struct colour
{
	double r,g,b,a;
};


vec_t position;
vec_t velocity;

enum type 
{
	plus = 1,
	minus = -1, 
	death = 0
};



kwadrat(/* args */)
{

	 std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> los(-1,1);

	this -> type = los(-1,1);

	this -> rect.w = 10; 
	this -> rect.h = 50;

	if (type == -1) {
		this -> colour.r = 255; 
		this -> colour.b = 0; 
		this -> colour.g = 0; 
		this -> colour.a = 255;
		
	}

	if (type == 1) {
		this -> colour.r = 0; 
		this -> colour.b = 255; 
		this -> colour.g = 0; 
		this -> colour.a = 255;
		
	}

	if (type == 0) {
		this -> colour.r = 0x89; 
		this -> colour.b = 0x04; 
		this -> colour.g = 0xB1; 
		this -> colour.a = 0x00;
		
	}
	velocity = {0.0,0.0};
}

void move(int n)
{
	this -> position[0]+=n;
	
	
	

}

};

	







class player_t {
public:
	vec_t position;
	vec_t velocity;
	std::shared_ptr<SDL_Texture> player_image;

	
	std::vector<vec_t> collision_pts;
	std::vector<vec_t> collision_mod;
	void draw(std::shared_ptr<SDL_Renderer> &r,
		const std::vector<int> collisions = {}
		) const {
		SDL_Rect rect = {(int)(position[0]-10),(int)(position[1]-15),20,30};
		SDL_SetRenderDrawColor(r.get(),255,0,0,255);
		//SDL_RenderDrawRect(r.get(), &rect);
		SDL_RenderCopy(r.get(),player_image.get(),NULL,&rect);
		int i =0;
		for (const auto &cp : collision_pts) {
			auto p = position + cp;
			SDL_Rect rect = {(int)(p[0]-3),(int)(p[1]-3),6,6};
			if ((collisions.size() > i) && collisions[i])
				SDL_SetRenderDrawColor(r.get(),255,128,128,255);
			else
				SDL_SetRenderDrawColor(r.get(),0,0,255,255);
			//SDL_RenderDrawRect(r.get(), &rect);
			i++;
		}
	}
	
	std::vector<int> check_collision( std::vector < unsigned char> map_data,
			unsigned map_width,
			unsigned map_height) {
				std::vector<int> ret;
		for (const auto &cp : collision_pts) {
			auto p = position + cp;
			try{
			if(map_data.at(((int)(p[1])*map_width+(int)(p[0]))*4) == 255) {
				ret.push_back(true);
			} else {
				ret.push_back(false);
			}
			} catch (...) {
				ret.push_back(true);
			}
			//std::cout << "p:" << p[0] << ", " << p[1] << ": " << <<  std::endl;
			
		}
		return ret;
	}

	bool apply_collision(const std::vector<int> &collisions) {
		for (unsigned i = 0; i < collisions.size(); i++) {
			auto cmod = collision_mod[i];
			auto c = collisions[i];
			if (c) {
				position = position + cmod;
				return true;
			}
		}
		return false;
	}



	player_t( ) {
		
		collision_pts = {
			{-5,-15},
			{-10, -10},
			{-10, 10},
			{-5,15},
			{5,15},
			{10, 10},
			{10, -10},
			{5,-15}
		};

		collision_mod = {
			{0,1},
			{1, 1},
			{1, -1},
			{0,-1},
			{0,-1},
			{-1, -1},
			{-1, 1},
			{0,1}
		};
		velocity = {0.0,0.0};
	}
};


int main()
{ // int argc, char **argv ) {
	using namespace std;
	auto window = init_window();
	auto renderer = init_renderer(window);


	
   // SDL_Texture *texture_r;
	SDL_Rect r;

	 r.w = 10;
     r.h = 50;

	
	//texture_r = SDL_CreateTexture(renderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 124, 70);

	vector < unsigned char> map_data;
	unsigned map_width;
	unsigned map_height;
	lodepng::decode(map_data, map_width, map_height, "data/map.png"); // load png image to vector (image)
	auto map_image =  load_png_texture(renderer, "data/map.png");
	

	player_t player;
	player.position[0] = 150;
	player.position[1] = 100;
	kwadrat kw;

	player.player_image =  load_png_texture(renderer, "data/bullet.png");
	
	int can_jump =  0;
	for (bool game_active = true; game_active;)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				game_active = false;
				break;
			}
		}
		const Uint8 *kstate = SDL_GetKeyboardState(NULL);


		if ((kstate[SDL_SCANCODE_LEFT])&&(player.position[0]>=10)) player.position[0]--;
		if ((kstate[SDL_SCANCODE_RIGHT])&& (player.position[0]<=305)) player.position[0]++; // na półce p = (291,34.5)
		if (kstate[SDL_SCANCODE_UP]) {
			if (can_jump>0) {
				can_jump = 0;
				player.position[1]--;
				player.velocity[1] = -8;
				//player.velocity[0] = -1;
			}
		}
//		if (kstate[SDL_SCANCODE_DOWN]) player.position[1]++;

		player.position = player.position + player.velocity;
		player.velocity = player.velocity + vec_t{0.0, 0.5};
		if (can_jump > 0) can_jump--;

		auto collisions = player.check_collision(map_data, map_width, map_height);
		bool colided;
		auto collisions_check = collisions;
		do {
			colided = player.apply_collision(collisions_check);
			if (colided) {
				player.velocity = {0,0};
				collisions_check = player.check_collision(map_data, map_width, map_height);
			}
		} while (colided);

		if ((collisions[3] == 1) || (collisions[4] == 1)) {
			can_jump = 3;
		}
		SDL_RenderClear(renderer.get());
		
		SDL_RenderCopy(renderer.get(), map_image.get(), NULL, NULL);
		

		// ładowanie textury nie powinno być co klatke czyli nie powinoo być w pętli
		

		// poczatek z tutoriala

		//SDL_SetRenderTarget(renderer.get(), texture_r);
               SDL_RenderDrawRect(renderer.get(),&r);
              // SDL_SetRenderDrawColor(renderer.get(), 0xFF, 0x00, 0x00, 0x00);
			   SDL_SetRenderDrawColor(renderer.get(), 0x89, 0x04, 0xB1, 0x00);
               SDL_RenderFillRect(renderer.get(), &r);

			   //8904B1 fioletowy html
		// koniec z tutoriala */
		kw.move(20);
		SDL_RenderDrawRect(renderer.get(),&kw.rect);
		SDL_SetRenderDrawColor(renderer.get(), kw.colour.r, kw.colour.g, kw.colour.b, kw.colour.a);
		SDL_RenderFillRect(renderer.get(), &kw.rect);

		
		



		player.draw(renderer, collisions);
		std::cout << "p: " << player.position[0] << " " << player.position[1] <<"         \r";
		std::cout.flush(); 
		
		SDL_RenderPresent(renderer.get());
		SDL_Delay(10);
	}
	return 0;
}
