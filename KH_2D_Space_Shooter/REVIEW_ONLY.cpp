#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#define FPS 60
#define WIDTH 1024
#define HEIGHT 768

// SYSTEM VARS
bool gRunning = false;
Uint32 gStart, gEnd, gDelta, gFps;
const Uint8* gKeystates;
SDL_Window* gWindow = nullptr;
SDL_Renderer* gRenderer = nullptr;

SDL_Texture* gPlrTexture = nullptr; //holds pixels, 1 source image is stored in 1 texture (1-1)
SDL_Rect gPlrSrc, gPlrDst, gPlrColl;
SDL_Texture* gPlrBltTexture = nullptr;
vector<PlrBullet*> gPlrBltVtr; //holds pointers to dynamically-created bullets
Mix_Chunk* gPlrBltSfx = nullptr;
/*SDL_Texture* gSanicTexture = nullptr;
SDL_Rect gSanicSrc, gSanicDst;
SDL_RendererFlip gSanicOrientation = SDL_FLIP_NONE;*/
int gFrame = 0; //delays animation
const int gEnmSpawnFrame = 70;
int gSprite = 0;
int gSpriteMax = 8;
Mix_Music* gMusicTrack = nullptr;
bool Init(const char* title, int xpos, int ypos, int width, int height, int flags) {
	// INIT
	cout << "Initializing game..." << endl;
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
		gWindow = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
		if (gWindow != nullptr) {
		gRenderer = SDL_CreateRenderer(gWindow, -1, 0);
		if (gRenderer != nullptr) {
		if (IMG_Init(IMG_INIT_PNG) != 0) {
		gBkgTexture = IMG_LoadTexture(gRenderer, "_assets/_bkg/Nebula Red.png");
		}
		else return false;
		if (Mix_Init(MIX_INIT_MP3) != 0) {
		//Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 8192); // Good for most games.
		Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 2048); // Good for most games.
		Mix_AllocateChannels(16); // We don't need this many channels for this though.
		gMusicTrack = Mix_LoadMUS("_assets/_soundtracks/MGS_Encounter.mp3");
		Mix_VolumeMusic(MIX_MAX_VOLUME / 4); // Volume for channel 0 to 64.
		gPlrBltSfx = Mix_LoadWAV("_assets/_sfxs/7.wav");
		// MIX_MAX_VOLUME = 128
		Mix_Volume(-1, MIX_MAX_VOLUME); // Volume for all channels.
		Mix_SetPanning(0, 0, 255); // Full right.
		Mix_SetPanning(3, 255, 0); // Full left.
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
	gDebugBox = { gPlrColl.x,gPlrColl.y,gPlrColl.w,gPlrColl.h };
	Mix_PlayMusic(gMusicTrack, -1);
	gRunning = true;
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
		gPlrBltVtr.back()->dst.y = (gPlrDst.y + gPlrDst.h / 2) - gPlrBltVtr.back()->dst.h / 2;
		/*if (Mix_Playing(spawn_sfx_channel)) {
			Mix_HaltChannel(spawn_sfx_channel);
		}
		Mix_PlayChannel(spawn_sfx_channel, spawn_sfx, 0);*/
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
	if (gFrame == gEnmSpawnFrame) {
		gFrame = 0;
		gSprite++;
		if (gSprite == gSpriteMax) {
			gSprite = 0;
		}
		gSanicSrc.x = gSanicSrc.w * gSprite;
	}
	gFrame++;
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
		if (gPlrDst.x < WIDTH / 2 - gPlrDst.w) { //do not go past half the screen
			gPlrDst.x += gPlrSpeed;
		}
		/*if (gSanicDst.x + gSanicDst.w < WIDTH) {
			gSanicDst.x += gPlrSpeed;
			gSanicOrientation = SDL_FLIP_NONE;
		}*/
	}
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
	Animate();
}
void Render() {
	SDL_RenderClear(gRenderer);
	SDL_RenderCopy(gRenderer, gBkgTexture, &gBkgSrc, &gBkgDst);
	for (int i = 0; i < (int)gPlrBltVtr.size(); i++) {
		SDL_RenderCopyEx(gRenderer, gPlrBltTexture, &gPlrBltVtr[i]->src, &gPlrBltVtr[i]->dst, 90.0, nullptr, SDL_FLIP_NONE);
	}
	SDL_SetRenderDrawColor(gRenderer, 0, 255, 255, 150);
	SDL_RenderFillRect(gRenderer, &gDebugBox);
	SDL_RenderPresent(gRenderer);
}
void Clean() {
	Mix_CloseAudio();
	Mix_FreeChunk(gSfx);
	Mix_FreeMusic(gMusicTrack);
	SDL_DestroyTexture(gBkgTexture);
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	SDL_Quit();
}
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
int main(int argc, char* argv[]) {
	Engine engine_obj;
	engine_obj.GameStart();
	return 0;
}