/*
 * DO NOT TOUCH THIS FILE!
 *
 * Initializes the engine with required data and starts the main loop
 */

#include <iostream>
#include <cstdarg>

#if defined(__APPLE__) || defined(__linux__)
#include <dlfcn.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

#include "Amphora.h"
#include "Config.h"

struct AmphoraStartup
{
	char *engine_version;
	int api_version;
	int (*StartEngine)();
	void (*RegisterGameData)(const char *, const char *);
	void (*RegisterWindowTitle)(const char *);
	void (*RegisterPrefs)(const char *, const char *, int, int, unsigned int, int);
	void (*RegisterActionData)(uint32_t *, const char **, SDL_Keycode *, SDL_GameControllerButton *, const char **, int);
	void (*RegisterSceneData)(const AmphoraScene *, const char **, int);
	void (*RegisterImageData)(const char **, const char **, int);
	void (*RegisterFontData)(const char **, const char **, int);
	void (*RegisterMapData)(const char **, const char **, int);
	void (*RegisterSFXData)(const char **, const char **, int);
	void (*RegisterMusicData)(const char **, const char **, int);
	void *(*GetAPI)(int);
};

static AmphoraStartup *astart;

namespace Amphora
{
	static AmphoraAPI_V1 *aapi_v1;

#define AMPHORA_VFUNCTION_V1(ret, name, sig_args, p_sig_args, call_args) \
	ret Amphora::name sig_args \
	{ \
		va_list args; \
		ret res; \
		va_start(args, fmt); \
		res = aapi_v1->name call_args; \
		va_end(args); \
		return res; \
	}
#define AMPHORA_FUNCTION_V1(ret, name, sig_args, call_args) ret Amphora::name sig_args { return aapi_v1->name call_args; }
#define AMPHORA_ROUTINE_V1(name, sig_args, call_args) void Amphora::name sig_args { aapi_v1->name call_args; }
	#include "amphora_api.def"
#undef AMPHORA_VFUNCTION_V1
#undef AMPHORA_FUNCTION_V1
#undef AMPHORA_ROUTINE_V1
}

/* Controller data */

enum input_actions {
#define KMAP(action, ...) ACTION_##action,
	#include "Keymap.txt"
#undef KMAP
	ACTION_COUNT
};
_Static_assert(ACTION_COUNT <= 32, "Cannot define more than 32 actions");

union input_state_u {
	InputState state;
	uint32_t bits;
};

static const char *action_names[] = {
#define KMAP(action, key, gamepad) #action,
	#include "Keymap.txt"
#undef KMAP
};

static SDL_Keycode keys[] = {
#define KMAP(action, key, gamepad) SDLK_##key,
	#include "Keymap.txt"
#undef KMAP
};

static SDL_GameControllerButton controller_buttons[] = {
#define KMAP(action, key, gamepad) SDL_CONTROLLER_BUTTON_##gamepad,
	#include "Keymap.txt"
#undef KMAP
};

static const char *controller_button_names[] = {
#define KMAP(action, key, gamepad) #gamepad,
	#include "Keymap.txt"
#undef KMAP
};

static input_state_u input_state;

/* Scene data */

void Scene::init() {}
void Scene::update(unsigned int, const InputState*) {}
void Scene::destroy() {}
Scene::~Scene() = default;

static AmphoraScene scene_structs[] = {
#define SCENE(name) name##_get_c_scene(),
	#include "SceneList.txt"
#undef SCENE
};

enum scene_list_e {
#define SCENE(name) name##_scene,
	#include "SceneList.txt"
#undef SCENE
	SCENES_COUNT
};

static const char *scene_names[] = {
#define SCENE(name) #name,
	#include "SceneList.txt"
#undef SCENE
};

/* Image data */

enum images_e {
#define LOADIMG(name, path) name##_im,
#define LOADFONT(name, path)
#define LOADMAP(name, path)
#define LOADSFX(name, path)
#define LOADMUSIC(name, path)
	#include "Resources.txt"
#undef LOADIMG
#undef LOADFONT
#undef LOADMAP
#undef LOADSFX
#undef LOADMUSIC
	IMAGES_COUNT
};

static const char *img_names[] = {
#define LOADIMG(name, path) #name,
#define LOADFONT(name, path)
#define LOADMAP(name, path)
#define LOADSFX(name, path)
#define LOADMUSIC(name, path)
	#include "Resources.txt"
#undef LOADIMG
#undef LOADFONT
#undef LOADMAP
#undef LOADSFX
#undef LOADMUSIC
};
static const char *img_paths[] = {
#define LOADIMG(name, path) #path,
#define LOADFONT(name, path)
#define LOADMAP(name, path)
#define LOADSFX(name, path)
#define LOADMUSIC(name, path)
	#include "Resources.txt"
#undef LOADIMG
#undef LOADFONT
#undef LOADMAP
#undef LOADSFX
#undef LOADMUSIC
};

/* Font data */

enum fonts_e {
#define LOADIMG(name, path)
#define LOADFONT(name, path) name##_ft,
#define LOADMAP(name, path)
#define LOADSFX(name, path)
#define LOADMUSIC(name, path)
	#include "Resources.txt"
#undef LOADIMAGE
#undef LOADFONT
#undef LOADMAP
#undef LOADSFX
#undef LOADMUSIC
	FONTS_COUNT
};

static const char *font_names[] = {
#define LOADIMG(name, path)
#define LOADFONT(name, path) #name,
#define LOADMAP(name, path)
#define LOADSFX(name, path)
#define LOADMUSIC(name, path)
	#include "Resources.txt"
#undef LOADIMAGE
#undef LOADFONT
#undef LOADMAP
#undef LOADSFX
#undef LOADMUSIC
};
static const char *font_paths[] = {
#define LOADIMG(name, path)
#define LOADFONT(name, path) #path,
#define LOADMAP(name, path)
#define LOADSFX(name, path)
#define LOADMUSIC(name, path)
	#include "Resources.txt"
#undef LOADIMAGE
#undef LOADFONT
#undef LOADMAP
#undef LOADSFX
#undef LOADMUSIC
};

/* Map data */

enum tilemaps_e {
#define LOADIMAGE(name, path)
#define LOADFONT(name, path)
#define LOADMAP(name, path) name##_tm,
#define LOADSFX(name, path)
#define LOADMUSIC(name, path)
	#include "Resources.txt"
#undef LOADIMAGE
#undef LOADFONT
#undef LOADMAP
#undef LOADSFX
#undef LOADMUSIC
	MAPS_COUNT
};

static const char *map_names[] = {
#define LOADIMAGE(name, path)
#define LOADFONT(name, path)
#define LOADMAP(name, path) #name,
#define LOADSFX(name, path)
#define LOADMUSIC(name, path)
	#include "Resources.txt"
#undef LOADIMAGE
#undef LOADFONT
#undef LOADMAP
#undef LOADSFX
#undef LOADMUSIC
};
static const char *map_paths[] = {
#define LOADIMAGE(name, path)
#define LOADFONT(name, path)
#define LOADMAP(name, path) #path,
#define LOADSFX(name, path)
#define LOADMUSIC(name, path)
	#include "Resources.txt"
#undef LOADIMAGE
#undef LOADFONT
#undef LOADMAP
#undef LOADSFX
#undef LOADMUSIC
};

/* SFX data */

enum sfx_e {
#define LOADIMAGE(name, path)
#define LOADFONT(name, path)
#define LOADMAP(name, path)
#define LOADSFX(name, path) name##_sf,
#define LOADMUSIC(name, path)
	#include "Resources.txt"
#undef LOADIMAGE
#undef LOADFONT
#undef LOADMAP
#undef LOADSFX
#undef LOADMUSIC
	SFX_COUNT
};

static const char *sfx_names[] = {
#define LOADIMAGE(name, path)
#define LOADFONT(name, path)
#define LOADMAP(name, path)
#define LOADSFX(name, path) #name,
#define LOADMUSIC(name, path)
	#include "Resources.txt"
#undef LOADIMAGE
#undef LOADFONT
#undef LOADMAP
#undef LOADSFX
#undef LOADMUSIC
};
static const char *sfx_paths[] = {
#define LOADIMAGE(name, path)
#define LOADFONT(name, path)
#define LOADMAP(name, path)
#define LOADSFX(name, path) #path,
#define LOADMUSIC(name, path)
	#include "Resources.txt"
#undef LOADIMAGE
#undef LOADFONT
#undef LOADMAP
#undef LOADSFX
#undef LOADMUSIC
};

/* Music data */

enum music_e {
#define LOADIMAGE(name, path)
#define LOADFONT(name, path)
#define LOADMAP(name, path)
#define LOADSFX(name, path)
#define LOADMUSIC(name, path) name##_mu,
	#include "Resources.txt"
#undef LOADIMAGE
#undef LOADFONT
#undef LOADMAP
#undef LOADSFX
#undef LOADMUSIC
	MUSIC_COUNT
};

static const char *music_names[] = {
#define LOADIMAGE(name, path)
#define LOADFONT(name, path)
#define LOADMAP(name, path)
#define LOADSFX(name, path)
#define LOADMUSIC(name, path) #name,
	#include "Resources.txt"
#undef LOADIMAGE
#undef LOADFONT
#undef LOADMAP
#undef LOADSFX
#undef LOADMUSIC
};
static const char *music_paths[] = {
#define LOADIMAGE(name, path)
#define LOADFONT(name, path)
#define LOADMAP(name, path)
#define LOADSFX(name, path)
#define LOADMUSIC(name, path) #path,
	#include "Resources.txt"
#undef LOADIMAGE
#undef LOADFONT
#undef LOADMAP
#undef LOADSFX
#undef LOADMUSIC
};

static int
load_engine()
{
	void *engine;
	AmphoraStartup *(*connect)();

	/* TODO: update placeholder library paths with real detection logic */
#if defined(__APPLE__) || defined(__linux__)
#ifdef DEBUG
	engine = dlopen("/Users/caleb/src/amphora/amphora-engine/cmake-build-debug/src/libamphora-vanilla-v0.2.3-alpha-dbg.dylib", RTLD_NOW);
#else
	engine = dlopen("/Users/caleb/src/amphora/amphora-engine/cmake-build-release/src/libamphora-vanilla-v0.2.3-alpha.dylib", RTLD_NOW);
#endif

	if (engine == nullptr)
	{
		std::cerr << "Could not load libamphora: " << dlerror() << std::endl;

		return -1;
	}
	connect = (AmphoraStartup *(*)())dlsym(engine, "Amphora_Connect");
	if (connect == nullptr)
	{
		std::cerr << "Failed to locate Amphora_Connect symbol: " << dlerror() << std::endl;

		return -1;
	}
#elif defined(_WIN32)
#else
#error Unsupported Operating System
#endif

	astart = connect();
	if (astart->api_version < API_VERSION)
	{
		std::cerr << "This game requires Amphora API V" << API_VERSION << " or greater, but the engine only supports V" << astart->api_version << std::endl;

#if defined(__APPLE__) || defined(__linux__)
		dlclose(engine);
#elif defined(_WIN32)
#endif

		return -1;
	}
	for (int i = 1; i <= API_VERSION; i++)
	{
		switch (i)
		{
			case 1:
				Amphora::aapi_v1 = static_cast<AmphoraAPI_V1 *>(astart->GetAPI(1));
				break;
			default:
				std::cerr << "Bad API version: " << i << std::endl;
				break;
		}

	}
	if (astart->engine_version != nullptr)
	{
		std::cout << "Connected to libamphora-" << astart->engine_version << ": using API V"
			<< API_VERSION << " (this engine supports up to API V" << astart->api_version << ")" << std::endl;
		free(astart->engine_version); /* astart->engine_version is malloc-ed by the engine */
	}
	else
	{
		std::cout << "Connected to libamphora-unknown: using API V"
			<< API_VERSION << " (this engine supports up to API V" << astart->api_version << ")" << std::endl;
	}

	return 0;
}

int
main()
{

	if (load_engine() == -1) return -1;

	astart->RegisterGameData(GAME_AUTHOR, GAME_TITLE);
	astart->RegisterWindowTitle(GAME_TITLE);
	astart->RegisterPrefs(GAME_AUTHOR, GAME_TITLE, WINDOW_X, WINDOW_Y, WINDOW_MODE, FRAMERATE);
	astart->RegisterActionData(&input_state.bits, action_names, keys, controller_buttons, controller_button_names, ACTION_COUNT);
	astart->RegisterSceneData(scene_structs, scene_names, SCENES_COUNT);
	astart->RegisterImageData(img_names, img_paths, IMAGES_COUNT);
	astart->RegisterFontData(font_names, font_paths, FONTS_COUNT);
	astart->RegisterMapData(map_names, map_paths, MAPS_COUNT);
	astart->RegisterSFXData(sfx_names, sfx_paths, SFX_COUNT);
	astart->RegisterMusicData(music_names, music_paths, MUSIC_COUNT);

	astart->StartEngine();
}
