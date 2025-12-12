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
#define AMPHORA_METHOD_V1(ret, name, sig_args, call_args) ret (*name) sig_args;
	#include "amphora_api.def"
#undef AMPHORA_VFUNCTION_V1
#undef AMPHORA_FUNCTION_V1
#undef AMPHORA_ROUTINE_V1
#undef AMPHORA_METHOD_V1
};

namespace Amphora
{
#define AMPHORA_VFUNCTION_V1(ret, name, sig_args, p_sig_args, call_args) ret name sig_args;
#define AMPHORA_FUNCTION_V1(ret, name, sig_args, call_args) ret name sig_args;
#define AMPHORA_ROUTINE_V1(name, sig_args, call_args) void name sig_args;
#define AMPHORA_METHOD_V1(ret, name, sig_args, call_args);
	#include "amphora_api.def"
#undef AMPHORA_VFUNCTION_V1
#undef AMPHORA_FUNCTION_V1
#undef AMPHORA_ROUTINE_V1
#undef AMPHORA_METHOD_V1
	namespace Colors
	{
#define COLOR(name, r, g, b) inline AmphoraColor name = { r, g, b, 0xff };
		#include "Colors.txt"
#undef COLOR
	}

	class Sprite
	{
		AmphoraImage *imageHandle = nullptr;
	public:
		Sprite(const char *image_name, float x, float y, float scale, bool flip, bool stationary, int order);
		~Sprite();

		Vector2f Position();
		Vector2f Center();
		bool Flipped();
		int AddFrameset(const char *name, const char *override_img, int sx, int sy,
			int w, int h, float off_x, float off_y, int num_frames, int delay);
		void SetFrameset(const char *name);
		void PlayOneshot(const char *name, void (*callback)());
		int SetFramesetAnimationTime(const char *name, int delay);
		AmphoraImage *Reorder(int order);
		int SetLocation(float x, float y);
		int Move(float delta_x, float delta_y);
		int Flip();
		int Unflip();
		int Show();
		int Hide();
		bool CheckCollision(const Sprite& other);
		bool CheckCollision(Sprite *other);
		AmphoraCollision CheckObjectGroupCollision(const char *name);
		bool MouseOver();
		bool Clicked(int button, void (*callback)());
		void ApplyFX(void (*fx)(AmphoraSurface *));
		void MakeCameraTarget();
		void Reset();
	};

	class Emitter
	{
		AmphoraEmitter *emitterHandle = nullptr;

	public:
		Emitter(float x, float y, float w, float h, float start_x, float start_y,
			int spread_x, int spread_y, int count, float p_w, float p_h, AmphoraColor color, bool stationary, int order,
			void (*update_fn)(int, int, AmphoraParticle *, AmphoraParticleExt *, const AmphoraFRect *));
		~Emitter();
	};

	class String
	{
		AmphoraString *stringHandle = nullptr;

	public:
		String(const char *font_name, int pt, float x, float y, int order, AmphoraColor color, bool stationary, const char *fmt, ...);
		~String();

		void Show();
		void Hide();
		size_t Length();
		size_t NumCharsDisplayed();
		const char *Text();
		char CharAtIndex(int idx);
		Vector2 Dimensions();
		bool MouseOver();
		bool Clicked(int button, void (*callback)());
		AmphoraString *UpdateText(const char *fmt, ...);
		AmphoraString *SetCharsDisplayed(size_t n);
		AmphoraString *SetPosition(float x, float y);
		TypewriterStatus Typewriter(int ms, void (*callback)(int, char));
		TypewriterStatus SetTypewriterSpeed(int ms);
	};
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
