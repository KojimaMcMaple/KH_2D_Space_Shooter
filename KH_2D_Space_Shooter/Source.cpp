#include <algorithm>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#define WIDTH 1024
#define HEIGHT 768
#define FPS 60
using namespace std;

struct Sprite
{
	SDL_Rect m_dst;
};

class Bullet : public Sprite // Inherits m_dst from Sprite.
{
public:
	bool m_active = true;
	Bullet(SDL_Rect d)
	{
		m_dst = d;
	}
	void update()
	{
		m_dst.x -= 4; // Just a literal speed. You may want a variable.
		if (m_dst.x < 0 - m_dst.w) // Off-screen.
			m_active = false;
	}
};

class Enemy : public Sprite
{
public:
	bool m_playSpawn = true;
	int m_bulletTimer = 0, m_timerMax, m_channel;
	Mix_Chunk* m_pSound;
	vector<Bullet*>* bulletVec; // A pointer to a vector. NOT a vector on its own.

public:
	Enemy(SDL_Rect d = { 0,0,0,0 }, vector<Bullet*>* bVec = nullptr,
		Mix_Chunk* s = nullptr, int c = 0, int bt = 0, int t = 120) // Note the default parameters.
	{
		m_dst = d;
		bulletVec = bVec;
		m_pSound = s;
		m_channel = c;
		m_timerMax = t;
		m_bulletTimer = bt;
	}
	void update()
	{
		if (m_bulletTimer++ == m_timerMax)
		{
			m_bulletTimer = 0;
			spawnBullet();
		}
	}
	void spawnBullet()
	{ // Note the -> because we have a POINTER to a vector.
		bulletVec->push_back(new Bullet({ m_dst.x, m_dst.y + 8, 8, 8 }));
		if (m_playSpawn)
		{
			if (Mix_Playing(m_channel))
				Mix_HaltChannel(m_channel);
			Mix_PlayChannel(m_channel, m_pSound, 0);
		}

	}
};

// Global engine variables.
bool g_bRunning = false; // Loop control flag.
int g_iSpeed = 5; // Speed of box.
const Uint8* g_iKeystates; // Keyboard state container.
Uint32 g_start, g_end, g_delta, g_fps; // Fixed timestep variables.
SDL_Window* g_pWindow; // This represents the SDL window.
SDL_Renderer* g_pRenderer; // This represents the buffer to draw to.
Mix_Music* g_pMusic;

Enemy eArray[4]; // Enemy array. You will want a vector of them. 
vector<Bullet*> enemyBullets;

void updatePanning()
{
	Mix_SetPanning(0, 0, 255); // Full right.
	Mix_SetPanning(1, 64, 128); // Partly right.
	Mix_SetPanning(2, 128, 64); // Partly left.
	Mix_SetPanning(3, 255, 0); // Full left.
}

bool init(const char* title, int xpos, int ypos, int width, int height, int flags)
{
	cout << "Initializing game." << endl;
	// Attempt to initialize SDL.
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		// Create the window.
		g_pWindow = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
		if (g_pWindow != nullptr) // Window init success.
		{
			g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, 0);
			if (g_pRenderer != nullptr) // Renderer init success.
			{
				// Now mixer subsystem.
				if (Mix_Init(MIX_INIT_MP3) != 0) // Mixer init success.
				{
					Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 8192); // Good for most games.
					Mix_AllocateChannels(16); // We don't need this many channels for this though.
					g_pMusic = Mix_LoadMUS("DiGiorno.mp3");
					Mix_Volume(-1, MIX_MAX_VOLUME); // Volume for all channels.
					Mix_VolumeMusic(MIX_MAX_VOLUME / 2); // Volume for channel 0 to 64.
					updatePanning();
				}
				else return false;
			}
			else return false; // Renderer init fail.
		}
		else return false; // Window init fail.
	}
	else return false; // SDL init fail.
	g_fps = (Uint32)round((1 / (double)FPS) * 1000); // Sets FPS in milliseconds and rounds.
	g_iKeystates = SDL_GetKeyboardState(nullptr);
	eArray[0] = Enemy({ 896, 128, 16, 16 }, &enemyBullets, Mix_LoadWAV("death.wav"), 0, 99, 100);
	eArray[1] = Enemy({ 896, 320, 16, 16 }, &enemyBullets, Mix_LoadWAV("jump.wav"), 1, 50, 100);
	eArray[2] = Enemy({ 896, 512, 16, 16 }, &enemyBullets, Mix_LoadWAV("power.wav"), 2, 25, 50);
	eArray[3] = Enemy({ 896, 704, 16, 16 }, &enemyBullets, Mix_LoadWAV("laser.wav"), 3, 49, 50);
	Mix_PlayMusic(g_pMusic, -1);
	g_bRunning = true; // Everything is okay, start the engine.
	cout << "Success!" << endl;
	return true;
}

void wake()
{
	g_start = SDL_GetTicks();
}

void sleep()
{
	g_end = SDL_GetTicks();
	g_delta = g_end - g_start;
	if (g_delta < g_fps) // Engine has to sleep.
		SDL_Delay(g_fps - g_delta);
}

void handleEvents()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT: // User pressed window's 'x' button.
			g_bRunning = false;
			break;
		case SDL_KEYDOWN: // Try SDL_KEYUP instead.
			if (event.key.keysym.sym == SDLK_ESCAPE)
				g_bRunning = false;
			break;
		case SDL_KEYUP:
			if (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_4)
			{
				int index = event.key.keysym.sym - 49;
				eArray[index].m_playSpawn = !eArray[index].m_playSpawn;
			}
			break;

		}
	}
}

// Keyboard utility function.
bool keyDown(SDL_Scancode c)
{
	if (g_iKeystates != nullptr)
	{
		if (g_iKeystates[c] == 1)
			return true;
		else
			return false;
	}
	return false;
}

void update()
{
	updatePanning();
	for (int i = 0; i < 4; i++)
		eArray[i].update();
	for (int i = 0; i < (int)enemyBullets.size(); i++)
	{ // Same code as the player bullet example.
		enemyBullets[i]->update();
		if (enemyBullets[i]->m_active == false)
		{
			delete enemyBullets[i];
			enemyBullets[i] = nullptr;
		}
	}
	if (!enemyBullets.empty())
	{
		enemyBullets.erase(remove(enemyBullets.begin(), enemyBullets.end(), nullptr), enemyBullets.end());
		enemyBullets.shrink_to_fit();
	}
}

void render()
{
	SDL_SetRenderDrawColor(g_pRenderer, 0, 0, 0, 255);
	SDL_RenderClear(g_pRenderer); // Clear the screen with the draw color.
	// Render stuff.

	for (int i = 0; i < 4; i++)
	{
		SDL_SetRenderDrawColor(g_pRenderer, 255, 0, 0, 255);
		if (eArray[i].m_playSpawn)
			SDL_SetRenderDrawColor(g_pRenderer, 0, 255, 0, 255);
		SDL_RenderFillRect(g_pRenderer, &eArray[i].m_dst);
	}
	SDL_SetRenderDrawColor(g_pRenderer, 255, 255, 0, 255);
	for (int i = 0; i < (int)enemyBullets.size(); i++)
		SDL_RenderFillRect(g_pRenderer, &enemyBullets[i]->m_dst);
	// Draw anew.
	SDL_RenderPresent(g_pRenderer);
}

void clean()
{
	cout << "Cleaning game." << endl;
	for (int i = 0; i < (int)enemyBullets.size(); i++)
	{ // Proper way to empty a vector of pointers.
		delete enemyBullets[i];
		enemyBullets[i] = nullptr;
	}
	enemyBullets.clear(); // Empty all elements.
	enemyBullets.shrink_to_fit(); // Shrink down capacity to 0.
	Mix_CloseAudio();
	Mix_FreeMusic(g_pMusic);
	for (int i = 0; i < 4; i++)
		Mix_FreeChunk(eArray[i].m_pSound);
	SDL_DestroyRenderer(g_pRenderer);
	SDL_DestroyWindow(g_pWindow);
	SDL_Quit();
}

// Main function.
//int main(int argc, char* args[]) // Main MUST have these parameters for SDL.
//{
//	if (init("GAME1007_SDL_Setup", SDL_WINDOWPOS_CENTERED,
//		SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0) == false)
//		return 1;
//	while (g_bRunning)
//	{
//		wake();
//		handleEvents();
//		update();
//		render();
//		if (g_bRunning)
//			sleep();
//	}
//	clean();
//	return 0;
//}