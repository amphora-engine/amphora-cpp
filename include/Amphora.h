#pragma once

#include <cstdarg>
#include <cstddef>
#include <cstdint>

#include "Amphora_SDL.h"

#define fullscreen SDL_WINDOW_FULLSCREEN_DESKTOP
#define fixed SDL_WINDOW_SHOWN
#define resizable SDL_WINDOW_RESIZABLE

#define Amphora_Max(a,b) ((a) > (b) ? (a) : (b))
#define Amphora_Min(a,b) ((a) < (b) ? (a) : (b))
#define Amphora_IsOdd(a) ((a) & 1)
#define Amphora_IsEven(a) (!Amphora_IsOdd((a)))

enum AmphoraCollision {
	AMPHORA_COLLISION_NONE,
	AMPHORA_COLLISION_LEFT,
	AMPHORA_COLLISION_RIGHT,
	AMPHORA_COLLISION_TOP,
	AMPHORA_COLLISION_BOTTOM
};

enum AmphoraStatusCode {
	AMPHORA_STATUS_OK,
	AMPHORA_STATUS_ALLOC_FAIL,
	AMPHORA_STATUS_CORE_FAIL,
	AMPHORA_STATUS_FAIL_UNDEFINED
};

struct Vector2f
{
	float x, y;
};
typedef Vector2f Camera;
struct Vector2
{
	int x, y;
};
typedef struct sprite_t AmphoraImage;
typedef struct amphora_message_t AmphoraString;
typedef struct emitter_t AmphoraEmitter;

struct AmphoraFRect
{
	float x, y, w, h;
};

struct AmphoraColor
{
	uint8_t r, g, b, a;
};

union AmphoraPixelDataABGR
{
	AmphoraColor color_data;
	uint32_t pixel;
};

struct AmphoraSurface
{
	AmphoraPixelDataABGR *pixels;
	int w, h;
};

struct InputState
{
#define KMAP(action, ...) bool action : 1;
	#include "Keymap.txt"
#undef KMAP
};

enum MouseButton
{
	MouseLeftButton = 1,
	MouseMiddleButton,
	MouseRightButton
};

struct AmphoraParticle
{
	float x, y, w, h, vx, vy;
	AmphoraColor color;
};

struct AmphoraParticleExt {
	float data1, data2;
	int data3, data4;
	bool hidden;
};

enum TypewriterStatus {
	TYPEWRITER_ERROR,
	TYPEWRITER_NOSTRING,
	TYPEWRITER_CREATED,
	TYPEWRITER_WAITING,
	TYPEWRITER_ATTRIB_UPDATE,
	TYPEWRITER_ADVANCE,
	TYPEWRITER_DONE
};

struct AmphoraAPI_V1
{
#define AMPHORA_VFUNCTION_V1(ret, name, sig_args, p_sig_args, call_args) ret (*name) p_sig_args;
#define AMPHORA_FUNCTION_V1(ret, name, sig_args, call_args) ret (*name) sig_args;
#define AMPHORA_ROUTINE_V1(name, sig_args, call_args) void (*name) sig_args;
	#include "amphora_api.def"
#undef AMPHORA_VFUNCTION_V1
#undef AMPHORA_FUNCTION_V1
#undef AMPHORA_ROUTINE_V1
};

namespace Amphora
{
#define AMPHORA_VFUNCTION_V1(ret, name, sig_args, p_sig_args, call_args) ret name sig_args;
#define AMPHORA_FUNCTION_V1(ret, name, sig_args, call_args) ret name sig_args;
#define AMPHORA_ROUTINE_V1(name, sig_args, call_args) void name sig_args;
	#include "amphora_api.def"
#undef AMPHORA_VFUNCTION_V1
#undef AMPHORA_FUNCTION_V1
#undef AMPHORA_ROUTINE_V1
	namespace Colors
	{
#define COLOR(name, r, g, b) inline AmphoraColor name = { r, g, b, 0xff };
		#include "Colors.txt"
#undef COLOR
	}
}

struct AmphoraScene {
	void (*init_func)();
	void (*update_func)(unsigned int, const InputState *);
	void (*destroy_func)();
};

class Scene {
public:
	virtual void init();
	virtual void update(unsigned int frame, const InputState* input);
	virtual void destroy();

	virtual ~Scene();
};

#define SCENE(name) class name;
#include "SceneList.txt"
#undef SCENE

#define Amphora_RegisterScene(name) \
name& name::instance() \
{ \
static name inst; \
return inst; \
} \
AmphoraScene name##_get_c_scene() { \
return { \
[]{ name::instance().init(); }, \
[](unsigned int f, const InputState* i){ name::instance().update(f, i); }, \
[]{ name::instance().destroy(); } \
}; \
}

#define SCENE(name) AmphoraScene name##_get_c_scene();
#include "SceneList.txt"
#undef SCENE
