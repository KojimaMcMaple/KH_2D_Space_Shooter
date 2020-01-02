//***CODES BY TRUNG LE (KYLE)***
#include <iostream>
#include <ctime>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include <algorithm>
using namespace std;

// MACROS
#define FPS 60
#define WIDTH 1024
#define HEIGHT 768

class PlrBullet {
public:
	bool is_active = true;
	SDL_Rect src, dst;
	static const int speed = 13;
	Mix_Chunk* spawn_sfx = nullptr;
	int spawn_sfx_channel;

	void Update() {
		dst.x += speed;
		// DESTROY BULLET THAT GOES OFF-SCREEN
		if (dst.x > WIDTH) {
			is_active = false;
		}
	}
	void PlaySpawnSfx(Mix_Chunk* spawn_sfx, int spawn_sfx_channel) {
		this->spawn_sfx = spawn_sfx;
		this->spawn_sfx_channel = spawn_sfx_channel;
		if (Mix_Playing(spawn_sfx_channel)) {
			Mix_HaltChannel(spawn_sfx_channel);
		}
		Mix_PlayChannel(spawn_sfx_channel, spawn_sfx, 0);
	}

	PlrBullet() {
		src = { 0, 0, 20, 24 };
		dst = { 0, 0, src.w, src.h };
	}
	PlrBullet(int x, int y) {
		src = { 0, 0, 20, 24 };
		dst = { x, y, src.w, src.h };
	}
};
class EnmBullet {
public:
	bool is_active = true;
	SDL_Rect src, dst;
	static const int speed = 9;
	Mix_Chunk* spawn_sfx = nullptr;
	int spawn_sfx_channel;

	void Update() {
		dst.x -= speed;
		// DESTROY BULLET THAT GOES OFF-SCREEN
		if (dst.x + dst.w < 0) {
			is_active = false;
		}
	}
	void PlaySpawnSfx(Mix_Chunk* spawn_sfx, int spawn_sfx_channel) {
		this->spawn_sfx = spawn_sfx;
		this->spawn_sfx_channel = spawn_sfx_channel;
		if (Mix_Playing(spawn_sfx_channel)) {
			Mix_HaltChannel(spawn_sfx_channel);
		}
		Mix_PlayChannel(spawn_sfx_channel, spawn_sfx, 0);
	}

	EnmBullet() {
		src = { 0, 0, 19, 19 };
		dst = { src.x, src.y, src.w, src.h };
	}
};

class PlrXplos {
public:
	bool is_active = false;
	bool is_created = false;
	SDL_Rect src, dst;
	static const int speed = 2;
	int sprite_x = 0;
	static const int sprite_x_max = 8;
	int sprite_y = 0;
	static const int sprite_y_max = 8;
	int sprite_frame_count = 0;
	static const int sprite_frame_count_max = 1;
	Mix_Chunk* spawn_sfx = nullptr;
	int spawn_sfx_channel;

	void Update() {
		dst.x -= speed;

		src.x = sprite_x * src.w;
		src.y = sprite_y * src.h;
		sprite_frame_count++;
		if (sprite_frame_count == sprite_frame_count_max) {
			sprite_frame_count = 0;
			sprite_x++;
			if (sprite_x == sprite_x_max) {
				sprite_x = 0;
				sprite_y++;
				if (sprite_y > sprite_y_max) {
					src.x = 0;
					src.y = 0;
					is_active = false;
				}
			}
		}
	}
	void PlaySpawnSfx(Mix_Chunk* spawn_sfx, int spawn_sfx_channel) {
		this->spawn_sfx = spawn_sfx;
		this->spawn_sfx_channel = spawn_sfx_channel;
		Mix_PlayChannel(spawn_sfx_channel, spawn_sfx, 0);
	}

	PlrXplos() {
		src = { 0,0,512,512 };
		dst = { 0,0,src.w/3,src.h/3 };
	}
};
class EnmXplos {
public:
	bool is_active = true;
	SDL_Rect src, dst;
	static const int speed = 5;
	int sprite = 0;
	static const int sprite_max = 15;
	int sprite_frame_count = 0;
	static const int sprite_frame_count_max = 2;
	Mix_Chunk* spawn_sfx;
	int spawn_sfx_channel;

	void Update() {
		dst.x -= speed;
		if (dst.x + dst.w < 0) {
			is_active = false;
		}

		src.x = sprite * src.w;
		sprite_frame_count++;
		if (sprite_frame_count == sprite_frame_count_max) {
			sprite_frame_count = 0;
			sprite++;
			if (sprite == sprite_max) {
				is_active = false;
			}
		}
	}
	void PlaySpawnSfx(Mix_Chunk* spawn_sfx, int spawn_sfx_channel) {
		this->spawn_sfx = spawn_sfx;
		this->spawn_sfx_channel = spawn_sfx_channel;
		Mix_PlayChannel(spawn_sfx_channel, spawn_sfx, 0);
	}

	EnmXplos() {
		src = { 0,0,321,321 };
		dst = { 0,0,src.w / 2,src.h / 2 };
	}
};

class Enemy {
public:
	bool is_active = true;
	bool is_hit = false;
	SDL_Rect src, dst;
	static const int speed = 5;
	bool spawns_blt = false;
	int blt_frame_count = 0;
	static const int kEnmBltSpawnFrame = 60;

	void SetDstY(int in_y) {
		dst.y = in_y;
	}
	void Update() {
		dst.x -= speed;
		blt_frame_count++;
		spawns_blt = false;
		if (blt_frame_count == kEnmBltSpawnFrame) {
			blt_frame_count = 0;
			spawns_blt = true;
		}

		if (dst.x + dst.w < 0) {
			is_active = false;
		}
	}

	Enemy() {
		src = { 0, 0, 182, 248 };
		dst = { WIDTH, HEIGHT, src.w / 3, src.h / 3 };
		//[Q] I need dst.h to calculate the maximum allowed y coord, is it a good practice to use a default constructor & change the member values outside of class ? 
	}
};

class Asteroid {
public:
	SDL_Texture* texture = nullptr;
	bool is_active = true;
	SDL_Rect src, dst;
	static const int speed = 4;
	int sprite_x = 0;
	static const int sprite_x_max = 4;
	int sprite_y = 0;
	static const int sprite_y_max = 4;
	int sprite_frame_count = 0;
	int sprite_frame_count_max = 1;

	void Update() {
		dst.x -= speed;
		if (dst.x + dst.w < 0) {
			is_active = false;
		}

		src.x = sprite_x * src.w;
		src.y = sprite_y * src.h;
		sprite_frame_count++;
		if (sprite_frame_count == sprite_frame_count_max) {
			sprite_frame_count = 0;
			sprite_x++;
			if (sprite_x == sprite_x_max) {
				sprite_x = 0;
				sprite_y++;
				if (sprite_y == sprite_y_max) {
					sprite_y = 0;
				}
			}
		}
	}

	Asteroid(SDL_Texture* texture, int sprite_frame_count_max) {
		this->texture = texture;
		this->sprite_frame_count_max = sprite_frame_count_max;

		src = { 0, 0, 488/4, 488/4 };
		dst = { WIDTH, 0, HEIGHT/16, HEIGHT/16 };
	}
};

class Engine {
private:
	// SYSTEM VARS
	bool gRunning = false;
	Uint32 gStart, gEnd, gDelta, gFps;
	const Uint8* gKeystates;
	SDL_Window* gWindow = nullptr;
	SDL_Renderer* gRenderer = nullptr; //buffer to draw to

	// BKG VARS
	SDL_Texture* gBkgTexture = nullptr;
	SDL_Rect gBkgSrc, gBkgDst;
	SDL_Texture* gBkgNextTexture = nullptr;
	SDL_Rect gBkgNextSrc, gBkgNextDst;
	int gBkgSpeed = 4;
	SDL_Texture* gBkgStarTexture = nullptr;
	SDL_Rect gBkgStar1Src, gBkgStar1Dst; //3 randomized star textures will run on top of bkg
	SDL_Rect gBkgStar2Src, gBkgStar2Dst;
	SDL_Rect gBkgStar3Src, gBkgStar3Dst;
	static const int kBkgStarSrcCoordsSize = 4;
	int gBkgStarSrcCoords[kBkgStarSrcCoordsSize];
	bool gBkgStar1SrcNeedsToChange = true;
	bool gBkgStar2SrcNeedsToChange = true;
	bool gBkgStar3SrcNeedsToChange = true;
	int gBkgStarSpeed = 2;

	// ASTEROID VARS
	SDL_Texture* gAstr1Texture = nullptr;
	SDL_Texture* gAstr2Texture = nullptr;
	SDL_Texture* gAstr3Texture = nullptr;
	static const int kAstrTextureArrSize = 3;
	SDL_Texture* gAstrTextureArr[kAstrTextureArrSize];
	SDL_Rect gAstrSrc, gAstrDst;
	vector<Asteroid*> gAstrVtr;
	int gAstrRotateSpeedMax = 10;
	static const int kAstrGridYSize = 16;
	int gAstrGridY[kAstrGridYSize];
	int gAstrNumCount = 0;
	int gAstrNumCountMax = kAstrGridYSize - 1; //maximum number of astr per spawn
	int gAstrFrameCount = 0;
	int gAstrFrameCountMax = 150; //spawn frame

	// PLAYER VARS
	SDL_Texture* gPlrTexture = nullptr; //holds pixels, 1 source image is stored in 1 texture (1-1)
	SDL_Rect gPlrSrc, gPlrDst, gPlrColl;
	int gPlrSpeed = 11;
	bool gPlrIsActive = true;
	SDL_Texture* gPlrBltTexture = nullptr;
	vector<PlrBullet*> gPlrBltVtr; //holds pointers to dynamically-created bullets
	Mix_Chunk* gPlrBltSfx = nullptr;
	int gPlrBltSfxChannel = 1;
	SDL_Texture* gPlrXplosTexture = nullptr;
	PlrXplos gPlrXplos = PlrXplos();
	Mix_Chunk* gPlrXplosSfx = nullptr;
	int gPlrXplosSfxChannel = 3;

	// ENEMY VARS
	SDL_Texture* gEnmTexture = nullptr;
	vector<Enemy*> gEnmVtr;
	SDL_Texture* gEnmBltTexture = nullptr;
	vector<EnmBullet*> gEnmBltVtr;
	Mix_Chunk* gEnmBltSfx = nullptr;
	int gEnmBltSfxChannel = 2;
	SDL_Texture* gEnmXplosTexture = nullptr;
	vector<EnmXplos*> gEnmXplosVtr;
	Mix_Chunk* gEnmXplosSfx = nullptr;
	int gEnmXplosSfxChannel = 4;

	// ANIMATION VARS
	/*SDL_Texture* gSanicTexture = nullptr;
	SDL_Rect gSanicSrc, gSanicDst;
	SDL_RendererFlip gSanicOrientation = SDL_FLIP_NONE;*/
	int gFrame = 0; //delays animation
	const int gEnmSpawnFrame = 70;
	int gSprite = 0;
	int gSpriteMax = 8;

	// MUSIC VARS
	Mix_Music* gMusicTrack = nullptr;

	// DEBUG VARS
	SDL_Rect gDebugBox;
	SDL_Rect gDebugBox2;

	bool Init(const char* title, int xpos, int ypos, int width, int height, int flags) {
		// INIT
		cout << "Initializing game..." << endl;
		if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
			gWindow = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
			if (gWindow != nullptr) {
				gRenderer = SDL_CreateRenderer(gWindow, -1, 0);
				if (gRenderer != nullptr) {
					if (IMG_Init(IMG_INIT_PNG) != 0) {
						cout << "Loading texture..." << endl;
						gBkgTexture = IMG_LoadTexture(gRenderer, "_assets/_bkg/Nebula Red.png");
						gBkgNextTexture = IMG_LoadTexture(gRenderer, "_assets/_bkg/Nebula Red.png");
						gBkgStarTexture = IMG_LoadTexture(gRenderer, "_assets/_bkg/Stars_Sheet.png");
						gAstr1Texture = IMG_LoadTexture(gRenderer, "_assets/_asteroids/med_a10000_spritesheet.png");
						gAstr2Texture = IMG_LoadTexture(gRenderer, "_assets/_asteroids/med_c10000_spritesheet.png");
						gAstr3Texture = IMG_LoadTexture(gRenderer, "_assets/_asteroids/med_d10000_spritesheet.png");
						gAstrTextureArr[0] = gAstr1Texture;
						gAstrTextureArr[1] = gAstr2Texture;
						gAstrTextureArr[2] = gAstr3Texture;
						gPlrTexture = IMG_LoadTexture(gRenderer, "_assets/_ships/SF01.png");
						gPlrBltTexture = IMG_LoadTexture(gRenderer, "_assets/_vfxs/PlayProjectile.png");
						gPlrXplosTexture = IMG_LoadTexture(gRenderer, "_assets/_vfxs/explosion 4.png");
						gEnmTexture = IMG_LoadTexture(gRenderer, "_assets/_ships/3B.png");
						gEnmBltTexture = IMG_LoadTexture(gRenderer, "_assets/_vfxs/EnemyProjectile1.png");
						gEnmXplosTexture = IMG_LoadTexture(gRenderer, "_assets/_vfxs/xplosion_purple_sprite.png");

						//gSanicTexture = IMG_LoadTexture(gRenderer, "PlayerRun.png");
					}
					else return false;

					if (Mix_Init(MIX_INIT_MP3) != 0) {
						//Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 8192); // Good for most games.
						Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 2048); // Good for most games.
						Mix_AllocateChannels(16); // We don't need this many channels for this though.
						
						gMusicTrack = Mix_LoadMUS("_assets/_soundtracks/MGS_Encounter.mp3");
						Mix_VolumeMusic(MIX_MAX_VOLUME/4); // Volume for channel 0 to 64.

						gPlrBltSfx = Mix_LoadWAV("_assets/_sfxs/7.wav");
						gPlrXplosSfx = Mix_LoadWAV("_assets/_sfxs/explosion.wav");
						gEnmBltSfx = Mix_LoadWAV("_assets/_sfxs/laser3.mp3");
						gEnmXplosSfx = Mix_LoadWAV("_assets/_sfxs/rumble.wav");
						
						Mix_Volume(-1, MIX_MAX_VOLUME); // Volume for all channels.
						Mix_Volume(gPlrBltSfxChannel, MIX_MAX_VOLUME);
						Mix_Volume(gPlrXplosSfxChannel, MIX_MAX_VOLUME);
						Mix_Volume(gEnmBltSfxChannel, MIX_MAX_VOLUME);
						Mix_Volume(gEnmXplosSfxChannel, MIX_MAX_VOLUME);

						//Mix_SetPanning(0, 0, 255); // Full right.
						//Mix_SetPanning(3, 255, 0); // Full left.
						Mix_SetPanning(gPlrBltSfxChannel, 128, 64); // Partly left.
						Mix_SetPanning(gPlrXplosSfxChannel, 128, 64); // Partly left.
						Mix_SetPanning(gEnmBltSfxChannel, 64, 128); // Partly right.
						Mix_SetPanning(gEnmXplosSfxChannel, 128, 128); // Partly right.
					}
					else return false;
				}
				else return false;
			}
			else return false;
		}
		else return false;

		gFps = (Uint32)round((1 / (double)FPS) * 1000);
		gKeystates = SDL_GetKeyboardState(nullptr);
		gBkgSrc = { 0, 0, 4096, 4096 };
		gBkgDst = { 0, 0, WIDTH, HEIGHT };
		gBkgNextSrc = { gBkgSrc.x, gBkgSrc.y, gBkgSrc.w, gBkgSrc.h };
		gBkgNextDst = { gBkgDst.w, gBkgDst.y, gBkgDst.w, gBkgDst.h };
		gBkgStar1Src = { 0, 0, 2048, 2048 };
		gBkgStar1Dst = { 0, 0, HEIGHT, HEIGHT };
		gBkgStar2Src = { 0, 0, gBkgStar1Src.w, gBkgStar1Src.h };
		gBkgStar2Dst = { gBkgStar1Dst.w, gBkgStar1Dst.y, gBkgStar1Dst.w, gBkgStar1Dst.h };
		gBkgStar3Src = { 0, 0, gBkgStar1Src.w, gBkgStar1Src.h };
		gBkgStar3Dst = { gBkgStar2Dst.w, gBkgStar2Dst.y, gBkgStar2Dst.w, gBkgStar2Dst.h };
		for (int i = 0; i < kBkgStarSrcCoordsSize; i++) {
			gBkgStarSrcCoords[i] = i * gBkgStar1Src.w; //multiply is better than addition
			//cout << gBkgStarSrcCoords[i] << endl;
		}
		for (int i = 0; i < kAstrGridYSize; i++) {
			gAstrGridY[i] = i * (HEIGHT/16);
		}
		gPlrSrc = { 0, 0, 256, 256 };
		gPlrDst = { gPlrSrc.w/2, HEIGHT/2 - gPlrSrc.h/4, gPlrSrc.w/3, gPlrSrc.h/3 };
		gPlrColl = { gPlrDst.x, gPlrDst.y, gPlrDst.w/3, gPlrDst.h/3 };
		/*gSanicSrc = { 0, 0, 128, 128 };
		gSanicDst = { gSanicSrc.w / 2, HEIGHT / 2 - gSanicSrc.h / 4, 128, 128 };*/
		gDebugBox = { gPlrColl.x,gPlrColl.y,gPlrColl.w,gPlrColl.h };
		gDebugBox2 = { gDebugBox.w,0,HEIGHT,HEIGHT };

		srand(time(NULL));
		Mix_PlayMusic(gMusicTrack, -1);
		gRunning = true;

		cout << "Initialization complete." << endl;
		return true;
	}

	void Wake() {
		// START
		gStart = SDL_GetTicks();
	}

	void Sleep() {
		// END
		gEnd = SDL_GetTicks();
		gDelta = gEnd - gStart;
		if (gDelta < gFps) {
			SDL_Delay(gFps - gDelta);
		}
	}

	void HandleEvents() {
		// PROCESS KEYS, HAPPENS ONCE PER KEY PRESS
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				gRunning = false;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					gRunning = false;
				}
				else if (event.key.keysym.sym == SDLK_SPACE) {
					gPlrBltVtr.push_back(new PlrBullet());
					gPlrBltVtr.back()->dst.x = gPlrDst.x + gPlrDst.w / 1.5;
					gPlrBltVtr.back()->dst.y = (gPlrDst.y + gPlrDst.h / 2) - gPlrBltVtr.back()->dst.h/2;
					gPlrBltVtr.back()->PlaySpawnSfx(gPlrBltSfx, gPlrBltSfxChannel);
				}
				break;
			}
		}
	}

	bool KeyDown(SDL_Scancode code) {
		// KEY EVENT
		if (gKeystates != nullptr) {
			if (gKeystates[code]) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			return false;
		}
	}

	void Animate() {
		// BKG ANIM
		// MAIN BKG
		gBkgDst.x -= gBkgSpeed;
		gBkgNextDst.x -= gBkgSpeed;
		if (gBkgDst.x + gBkgDst.w < 0) {
			gBkgDst.x = gBkgNextDst.x + gBkgNextDst.w;
		}
		if (gBkgNextDst.x + gBkgNextDst.w < 0) {
			gBkgNextDst.x = gBkgDst.x + gBkgDst.w;
		}
		// STAR BKG
		gBkgStar1Dst.x -= gBkgStarSpeed;
		gBkgStar2Dst.x -= gBkgStarSpeed;
		gBkgStar3Dst.x -= gBkgStarSpeed;
		/*gDebugBox.x -= gBkgStarSpeed;
		gDebugBox2.x -= gBkgStarSpeed;*/
		if (gBkgStar1Dst.x + gBkgStar1Dst.w < 0) {
			gBkgStar1Dst.x = gBkgStar3Dst.x + gBkgStar3Dst.w;
			//gDebugBox.x = gDebugBox2.x + gDebugBox2.w;
			gBkgStar1SrcNeedsToChange = true;
		}
		if (gBkgStar2Dst.x + gBkgStar2Dst.w < 0) {
			gBkgStar2Dst.x = gBkgStar1Dst.x + gBkgStar1Dst.w;
			//gDebugBox2.x = gDebugBox.x + gDebugBox.w;
			gBkgStar2SrcNeedsToChange = true;
		}
		if (gBkgStar3Dst.x + gBkgStar3Dst.w < 0) {
			gBkgStar3Dst.x = gBkgStar2Dst.x + gBkgStar2Dst.w;
			//gDebugBox3.x = gDebugBox.x + gDebugBox.w;
			gBkgStar3SrcNeedsToChange = true;
		}
		if (gBkgStar1SrcNeedsToChange) {
			gBkgStar1Src.x = gBkgStarSrcCoords[rand() % kBkgStarSrcCoordsSize];
			gBkgStar1SrcNeedsToChange = false;
		}
		if (gBkgStar2SrcNeedsToChange) {
			gBkgStar2Src.x = gBkgStarSrcCoords[rand() % kBkgStarSrcCoordsSize];
			gBkgStar2SrcNeedsToChange = false;
		}
		if (gBkgStar3SrcNeedsToChange) {
			gBkgStar3Src.x = gBkgStarSrcCoords[rand() % kBkgStarSrcCoordsSize];
			gBkgStar3SrcNeedsToChange = false;
		}


		// SANIC ANIM W/ DELAY
		/*if (gFrame == gEnmSpawnFrame) {
			gFrame = 0;
			gSprite++;
			if (gSprite == gSpriteMax) {
				gSprite = 0;
			}
			gSanicSrc.x = gSanicSrc.w * gSprite;
		}
		gFrame++;*/
	}

	bool HaveCollided(const SDL_Rect& obj1, const SDL_Rect& obj2) {
		// alternatively, use SDL_HasIntersection
		/*if (SDL_HasIntersection(&p, &b))
		{
			g_isCol = true;
		}*/
		if (obj1.x + obj1.w < obj2.x) {
			return false;
		}
		if (obj1.x > obj2.x + obj2.w) {
			return false;
		}
		if (obj1.y + obj1.h < obj2.y) {
			return false;
		}
		if (obj1.y > obj2.y + obj2.h) {
			return false;
		}
		return true;
	}

	void Update() {
		// PROCESSING, HAPPENS WHEN KEYS ARE HELD DOWN
		if (KeyDown(SDL_SCANCODE_W) || KeyDown(SDL_SCANCODE_UP)) {
			if (gPlrDst.y + gPlrDst.h / 2 > 0 + gPlrDst.h / 3) {
				gPlrDst.y -= gPlrSpeed; //using gPlrSrc will cause weirdness
			}
			/*if (gSanicDst.y > 0) {
				gSanicDst.y -= gPlrSpeed;
			}*/
		}
		if (KeyDown(SDL_SCANCODE_S) || KeyDown(SDL_SCANCODE_DOWN)) {
			if (gPlrDst.y + gPlrDst.h / 2 < HEIGHT - gPlrDst.h / 3) {
				gPlrDst.y += gPlrSpeed;
			}
			/*if (gSanicDst.y + gSanicDst.h < HEIGHT) {
				gSanicDst.y += gPlrSpeed;
			}*/
		}
		if (KeyDown(SDL_SCANCODE_A) || KeyDown(SDL_SCANCODE_LEFT)) {
			if (gPlrDst.x > 0 - gPlrDst.h / 4) {
				gPlrDst.x -= gPlrSpeed;
			}
			/*if (gSanicDst.x > 0) {
				gSanicDst.x -= gPlrSpeed;
				gSanicOrientation = SDL_FLIP_HORIZONTAL;
			}*/
		}
		if (KeyDown(SDL_SCANCODE_D) || KeyDown(SDL_SCANCODE_RIGHT)) {
			if (gPlrDst.x < WIDTH/2- gPlrDst.w) { //do not go past half the screen
				gPlrDst.x += gPlrSpeed;
			}
			/*if (gSanicDst.x + gSanicDst.w < WIDTH) {
				gSanicDst.x += gPlrSpeed;
				gSanicOrientation = SDL_FLIP_NONE;
			}*/
		}

		// PLAYER COLLISION UPDATE
		gPlrColl.x = gPlrDst.x + gPlrDst.w / 2 - gPlrColl.w / 2;
		gPlrColl.y = gPlrDst.y + gPlrDst.h / 2 - gPlrColl.h / 2;
		gDebugBox.x = gPlrColl.x + gPlrColl.w / 2 - gPlrColl.w / 2;
		gDebugBox.y = gPlrColl.y + gPlrColl.h / 2 - gPlrColl.h / 2;

		// ASTEROID HANDLING
		gAstrFrameCount++;
		if (gAstrFrameCount == gAstrFrameCountMax) {
			gAstrFrameCount = 0;
			int random_astr_num_max = 3 + rand() % (gAstrNumCountMax - 6);
			do {
				gAstrNumCount++;
				int random_texture_index = rand() % kAstrTextureArrSize;
				int random_speed = 2 + rand() % (gAstrRotateSpeedMax - 2);
				int random_y_index = rand() % kAstrGridYSize;
				gAstrVtr.push_back(new Asteroid(gAstrTextureArr[random_texture_index], random_speed));
				gAstrVtr.back()->dst.y = gAstrGridY[random_y_index];
			} while (gAstrNumCount < random_astr_num_max + 1);//maximum inclusive
			gAstrNumCount = 0;
		}
		for (int i = 0; i < (int)gAstrVtr.size(); i++) {
			gAstrVtr[i]->Update();
			if (!gAstrVtr[i]->is_active) {
				delete gAstrVtr[i];
				gAstrVtr[i] = nullptr;
			}
		}
		if (!gAstrVtr.empty()) {
			gAstrVtr.erase(remove(gAstrVtr.begin(), gAstrVtr.end(), nullptr), gAstrVtr.end());
			gAstrVtr.shrink_to_fit();
		}

		// BULLET HANDLING JUST IN CASE MULTIPLE BULLETS SET TO ACTIVE @ SAME TIME
		for (int i = 0; i < (int)gPlrBltVtr.size(); i++) {
			gPlrBltVtr[i]->Update(); //-> combines dereferences w/ member access
			if (gPlrBltVtr[i]->is_active == false) {
				delete gPlrBltVtr[i];
				gPlrBltVtr[i] = nullptr;
			}
		}
		if (!gPlrBltVtr.empty()) {
			gPlrBltVtr.erase(remove(gPlrBltVtr.begin(), gPlrBltVtr.end(), nullptr), gPlrBltVtr.end());
			gPlrBltVtr.shrink_to_fit();
		}
		//cout << gPlrBltVtr.capacity() << endl;
		//cout << gPlrBltVtr.size() << endl;

		// ENEMY HANDLING
		gFrame++;
		if (gFrame == gEnmSpawnFrame) {
			gFrame = 0;
			gEnmVtr.push_back(new Enemy());
			int rand_range = HEIGHT - gEnmVtr.back()->dst.h;
			int y_pos = rand() % rand_range;
			gEnmVtr.back()->SetDstY(y_pos); //y position must account for enemy height, cannot input y at creation because there is no dst.h yet
		}
		// CLEAN ENEMY IF INACTIVE, SPAWN ENEMY BULLETS IF NOT
		for (int i = 0; i < (int)gEnmVtr.size(); i++) {
			gEnmVtr[i]->Update();
			if (gEnmVtr[i]->is_active == false) {
				delete gEnmVtr[i];
				gEnmVtr[i] = nullptr;
			}
			else {
				if (gEnmVtr[i]->spawns_blt == true) {
					gEnmBltVtr.push_back(new EnmBullet());
					gEnmBltVtr.back()->dst.x = gEnmVtr[i]->dst.x - gEnmBltVtr.back()->dst.w;
					gEnmBltVtr.back()->dst.y = gEnmVtr[i]->dst.y + gEnmVtr[i]->dst.h / 2 - gEnmBltVtr.back()->dst.h / 2;
					gEnmBltVtr.back()->PlaySpawnSfx(gEnmBltSfx, gEnmBltSfxChannel);
				}
			}
		}
		if (!gEnmVtr.empty()) {
			gEnmVtr.erase(remove(gEnmVtr.begin(), gEnmVtr.end(), nullptr), gEnmVtr.end());
			gEnmVtr.shrink_to_fit();
		}
		// CLEAN ENEMY BULLETS
		for (int i = 0; i < (int)gEnmBltVtr.size(); i++) {
			gEnmBltVtr[i]->Update();
			if (gEnmBltVtr[i]->is_active == false) {
				delete gEnmBltVtr[i];
				gEnmBltVtr[i] = nullptr;
			}
		}
		if (!gEnmBltVtr.empty()) {
			gEnmBltVtr.erase(remove(gEnmBltVtr.begin(), gEnmBltVtr.end(), nullptr), gEnmBltVtr.end());
			gEnmBltVtr.shrink_to_fit();
		}
		/*cout << gEnmVtr.capacity() << endl;
		cout << gEnmVtr.size() << endl;*/

		// ENEMY COLLISION HANDLING
		for (int i = 0; i < (int)gPlrBltVtr.size(); i++) {
			if (gPlrBltVtr[i]->is_active == true) {
				for (int j = 0; j < (int)gEnmVtr.size(); j++) {
					if (gEnmVtr[j]->is_active == true) {
						if (HaveCollided(gPlrBltVtr[i]->dst, gEnmVtr[j]->dst)) {
							gPlrBltVtr[i]->is_active = false;
							gEnmVtr[j]->is_hit = true;
							gEnmVtr[j]->is_active = false;
						}
					}
				}
			}
		}
		// ENEMY XPLOSION HANDLING
		for (int i = 0; i < (int)gEnmVtr.size(); i++) {
			// SPAWN XPLOSION IF ENEMY IS HIT
			if (gEnmVtr[i]->is_hit) {
				gEnmXplosVtr.push_back(new EnmXplos());
				gEnmXplosVtr.back()->dst.x = gEnmVtr[i]->dst.x;
				gEnmXplosVtr.back()->dst.y = gEnmVtr[i]->dst.y + gEnmVtr[i]->dst.h / 2 - gEnmXplosVtr.back()->dst.h / 2;
				gEnmXplosVtr.back()->PlaySpawnSfx(gEnmXplosSfx, gEnmXplosSfxChannel);
			}
		}
		// CLEAN ENEMY XPLOSION
		for (int i = 0; i < (int)gEnmXplosVtr.size(); i++) {
			gEnmXplosVtr[i]->Update();
			if (!gEnmXplosVtr[i]->is_active) {
				delete gEnmXplosVtr[i];
				gEnmXplosVtr[i] = nullptr;
			}
		}
		if (!gEnmXplosVtr.empty()) {
			gEnmXplosVtr.erase(remove(gEnmXplosVtr.begin(), gEnmXplosVtr.end(), nullptr), gEnmXplosVtr.end());
			gEnmXplosVtr.shrink_to_fit();
		}

		// PLAYER COLLISION HANDLING
		for (int i = 0; i < (int)gAstrVtr.size(); i++) {
			if (gAstrVtr[i]->is_active && gPlrIsActive) {
				if (HaveCollided(gPlrColl, gAstrVtr[i]->dst)) {
					gPlrIsActive = false;
				}
			}
		}
		for (int i = 0; i < (int)gPlrBltVtr.size(); i++) {
			for (int j = 0; j < (int)gAstrVtr.size(); j++) {
				if (gPlrBltVtr[i]->is_active && gAstrVtr[j]->is_active) {
					if (HaveCollided(gPlrBltVtr[i]->dst, gAstrVtr[j]->dst)) {
						gPlrBltVtr[i]->is_active = false;
					}
				}
			}
		}
		for (int i = 0; i < (int)gEnmBltVtr.size(); i++) {
			if (gEnmBltVtr[i]->is_active && gPlrIsActive) {
				if (HaveCollided(gPlrColl, gEnmBltVtr[i]->dst)) {
					gPlrIsActive = false;
				}
			}
		}
		for (int i = 0; i < (int)gEnmVtr.size(); i++) {
			if (gEnmVtr[i]->is_active && gPlrIsActive) {
				if (HaveCollided(gPlrColl, gEnmVtr[i]->dst)) {
					gPlrIsActive = false;
				}
			}
		}

		// PLAYER XPLOSION HANDLING
		if (!gPlrIsActive && !gPlrXplos.is_created) {
			gPlrXplos.is_created = true;
			gPlrXplos.is_active = true;
			gPlrXplos.dst.x = gPlrDst.x;
			gPlrXplos.dst.y = gPlrDst.y + gPlrDst.h / 2 - gPlrXplos.dst.h / 2;
			gPlrXplos.PlaySpawnSfx(gPlrXplosSfx, gPlrXplosSfxChannel);
		}
		if (gPlrXplos.is_active) {
			gPlrXplos.Update();
		}

		// ANIMATION HANDLING
		Animate();
	}

	void Render() {
		// CLEAR
		SDL_SetRenderDrawColor(gRenderer, 50, 50, 50, 255);
		SDL_RenderClear(gRenderer);

		// RENDER NEW
		SDL_RenderCopy(gRenderer, gBkgTexture, &gBkgSrc, &gBkgDst);
		SDL_RenderCopy(gRenderer, gBkgTexture, &gBkgNextSrc, &gBkgNextDst);
		SDL_RenderCopy(gRenderer, gBkgStarTexture, &gBkgStar1Src, &gBkgStar1Dst);
		SDL_RenderCopy(gRenderer, gBkgStarTexture, &gBkgStar2Src, &gBkgStar2Dst);
		SDL_RenderCopy(gRenderer, gBkgStarTexture, &gBkgStar3Src, &gBkgStar3Dst);
		for (int i = 0; i < (int)gAstrVtr.size(); i++) {
			SDL_RenderCopyEx(gRenderer, gAstrVtr[i]->texture, &gAstrVtr[i]->src, &gAstrVtr[i]->dst, 0.0, nullptr, SDL_FLIP_NONE);
			//SDL_RenderCopyEx(gRenderer, gAstrTextureArr[0], &gAstrVtr[i]->src, &gAstrVtr[i]->dst, 0.0, nullptr, SDL_FLIP_NONE);
		}
		for (int i = 0; i < (int)gEnmBltVtr.size(); i++) {
			SDL_RenderCopyEx(gRenderer, gEnmBltTexture, &gEnmBltVtr[i]->src, &gEnmBltVtr[i]->dst, 0.0, nullptr, SDL_FLIP_NONE);
		}
		for (int i = 0; i < (int)gEnmVtr.size(); i++) {
			SDL_RenderCopyEx(gRenderer, gEnmTexture, &gEnmVtr[i]->src, &gEnmVtr[i]->dst, 270.0, nullptr, SDL_FLIP_NONE);
		}
		for (int i = 0; i < (int)gPlrBltVtr.size(); i++) {
			SDL_RenderCopyEx(gRenderer, gPlrBltTexture, &gPlrBltVtr[i]->src, &gPlrBltVtr[i]->dst, 90.0, nullptr, SDL_FLIP_NONE);
		}
		for (int i = 0; i < (int)gEnmXplosVtr.size(); i++) {
			SDL_RenderCopyEx(gRenderer, gEnmXplosTexture, &gEnmXplosVtr[i]->src, &gEnmXplosVtr[i]->dst, 0.0, nullptr, SDL_FLIP_NONE);
		}
		if (gPlrIsActive) {
			SDL_RenderCopyEx(gRenderer, gPlrTexture, &gPlrSrc, &gPlrDst, 90.0, nullptr, SDL_FLIP_NONE);
		}
		else {
			SDL_RenderCopyEx(gRenderer, gPlrXplosTexture, &gPlrXplos.src, &gPlrXplos.dst, 0.0, nullptr, SDL_FLIP_NONE);
		}
		// BULLET RENDER (NO TEXTURE)
		/*SDL_SetRenderDrawColor(gRenderer, 255, 200, 0, 255);
		for (int i = 0; i < (int)gPlrBltVtr.size(); i++) {
			SDL_RenderFillRect(gRenderer, &gPlrBltVtr[i]->dst);
		}*/
		
		// SANIC RENDER
		//SDL_RenderCopyEx(gRenderer, gSanicTexture, &gSanicSrc, &gSanicDst, 0.0, nullptr, gSanicOrientation);
		
		// DEBUG RENDER
		//SDL_SetRenderDrawColor(gRenderer, 0, 255, 255, 150);
		//SDL_RenderFillRect(gRenderer, &gDebugBox);
		//SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 150);
		//SDL_RenderFillRect(gRenderer, &gDebugBox2);

		// SHOW
		SDL_RenderPresent(gRenderer);
	}

	void Clean() {
		// DESTROY
		cout << "Cleaning game..." << endl;
		Mix_CloseAudio();
		Mix_FreeChunk(gPlrBltSfx);
		Mix_FreeChunk(gPlrXplosSfx);
		Mix_FreeChunk(gEnmBltSfx);
		Mix_FreeChunk(gEnmXplosSfx);
		Mix_FreeMusic(gMusicTrack);
		SDL_DestroyTexture(gPlrTexture);
		SDL_DestroyTexture(gPlrXplosTexture);
		SDL_DestroyTexture(gPlrBltTexture);
		SDL_DestroyTexture(gEnmTexture);
		SDL_DestroyTexture(gEnmBltTexture);
		SDL_DestroyTexture(gAstr1Texture);
		SDL_DestroyTexture(gAstr2Texture);
		SDL_DestroyTexture(gAstr3Texture);
		SDL_DestroyTexture(gBkgStarTexture);
		SDL_DestroyTexture(gBkgNextTexture);
		SDL_DestroyTexture(gBkgTexture);
		SDL_DestroyRenderer(gRenderer);
		SDL_DestroyWindow(gWindow);
		SDL_Quit();
	}

public:
	bool GameStart() {
		if (!Init("Metal Gear College", SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0)) {
			return 1;
		}
		while (gRunning) {
			Wake();
			HandleEvents();
			Update();
			Render();
			if (gRunning) {
				Sleep();
			}
		}
		Clean();
		return 0;
	}
};

int main(int argc, char* argv[]) {
	Engine engine_obj;

	engine_obj.GameStart();

	return 0;
}
//***CODES BY TRUNG LE (KYLE)***