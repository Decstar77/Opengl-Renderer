#pragma once
#include "Core.h"
#include "Math/CosmicMath.h"
#include "GLFW/glfw3.h"

namespace cm
{
	struct MouseData
	{
		Vec2 prev_mouse_position = Vec2(0);
		Vec2 position = Vec2(0);
		bool prev_mouse_codes[MOUSE_KEY_AMOUNT] = {};
		bool mouse_codes[MOUSE_KEY_AMOUNT] = {};
	};

	struct KeyData
	{
		bool prev_key_codes[KEY_CODE_AMOUNT] = {};
		bool key_codes[KEY_CODE_AMOUNT] = {};
	};

	struct InputModifiers
	{

	};
	   	 	
	class InputCallBacks;
	class Input
	{
	public:
		~Input() = default;
		
		inline static void SetKeyState(int32 index, bool state) {
			Assert(index < KEY_CODE_AMOUNT); 
			key_data.key_codes[index] = state; 
		}
		inline static bool GetKeyJustDown(int32 index) {
			Assert(index < KEY_CODE_AMOUNT); 
			return (key_data.prev_key_codes[index] == false && key_data.key_codes[index] == true) ? true : false;
		}
		inline static bool GetKeyHeldDown(int32 index) {
			Assert(index < KEY_CODE_AMOUNT);
			return key_data.key_codes[index];
		}
		inline static bool GetKeyJustUp(int32 index) { 
			return (key_data.prev_key_codes[index] == true && key_data.key_codes[index] == false) ? true : false;
		}
		inline static void SetMousePosition(const real32 &x, const real32 &y) {
			mouse_data.position = Vec2(x, y);
		}
		inline static void SetMouseKeyState(int32 index, bool state) { 
			Assert(index < MOUSE_KEY_AMOUNT);
			mouse_data.mouse_codes[index] = state;
		}
		inline static bool IsMouseJustDown(int32 index) { 
			Assert(index < MOUSE_KEY_AMOUNT); 
			return (mouse_data.prev_mouse_codes[index] == false && mouse_data.mouse_codes[index] == true) ? true : false;
		}
		inline static bool IsMouseHeldDown(int32 index) { 
			Assert(index < MOUSE_KEY_AMOUNT);
			return mouse_data.mouse_codes[index]; 
		}
		inline static bool IsMouseJustUp(int32 index) {
			Assert(index < MOUSE_KEY_AMOUNT);
			return (mouse_data.prev_mouse_codes[index] == true && mouse_data.mouse_codes[index] == false) ? true : false;
		}
		inline static bool IsMouseUp(int32 index) {
			Assert(index < MOUSE_KEY_AMOUNT);
			return !mouse_data.mouse_codes[index];
		}

		inline static Vec2 GetMousePosition() { return mouse_data.position; }
		inline static Vec2 GetMouseLastPosition() { return mouse_data.prev_mouse_position; }



		inline static void Update()
		{
			mouse_data.prev_mouse_position= mouse_data.position;
			for (uint32 i = 0; i < MOUSE_KEY_AMOUNT; i++)
			{
				mouse_data.prev_mouse_codes[i] = mouse_data.mouse_codes[i];
			}
			for (uint32 i = 0; i < KEY_CODE_AMOUNT; i++)
			{
				key_data.prev_key_codes[i] = key_data.key_codes[i];
			}
		}

		inline static void MousePositionCall(GLFWwindow *widow, double xpos, double ypos) {};
		//inline static RegisterCallBack() {}

		
		static std::vector<InputCallBacks *> msg;



	private:

		static MouseData mouse_data;
		static KeyData key_data;

	private:
		Input() = delete;
		Input(Input &i) = delete;
		Input& operator=(const Input&) = delete;
		
	};

	class InputCallBacks
	{
		InputCallBacks() { Input::msg.push_back(this); }
		
		virtual void KeyCallBack(int32 k) = 0;
		virtual void MousePosition(real x, real y) = 0;
	};

	class Player : public InputCallBacks
	{

		virtual void KeyCallBack(int32 k) override
		{

		}


		virtual void MousePosition(real x, real y) override
		{

		}

	};

#pragma region KEY_CODES
	//************************************
	// Copied From glfw. 
	//************************************
	#define KEY_UNKNOWN            -1
	#define KEY_SPACE              32
	#define KEY_APOSTROPHE         39 
	#define KEY_COMMA              44 
	#define KEY_MINUS              45 
	#define KEY_PERIOD             46 
	#define KEY_SLASH              47 
	#define KEY_0                  48
	#define KEY_1                  49
	#define KEY_2                  50
	#define KEY_3                  51
	#define KEY_4                  52
	#define KEY_5                  53
	#define KEY_6                  54
	#define KEY_7                  55
	#define KEY_8                  56
	#define KEY_9                  57
	#define KEY_SEMICOLON          59 
	#define KEY_EQUAL              61 
	#define KEY_A                  65
	#define KEY_B                  66
	#define KEY_C                  67
	#define KEY_D                  68
	#define KEY_E                  69
	#define KEY_F                  70
	#define KEY_G                  71
	#define KEY_H                  72
	#define KEY_I                  73
	#define KEY_J                  74
	#define KEY_K                  75
	#define KEY_L                  76
	#define KEY_M                  77
	#define KEY_N                  78
	#define KEY_O                  79
	#define KEY_P                  80
	#define KEY_Q                  81
	#define KEY_R                  82
	#define KEY_S                  83
	#define KEY_T                  84
	#define KEY_U                  85
	#define KEY_V                  86
	#define KEY_W                  87
	#define KEY_X                  88
	#define KEY_Y                  89
	#define KEY_Z                  90
	#define KEY_LEFT_BRACKET       91 
	#define KEY_BACKSLASH          92 
	#define KEY_RIGHT_BRACKET      93 
	#define KEY_GRAVE_ACCENT       96 
	#define KEY_WORLD_1            161
	#define KEY_WORLD_2            162

	#define KEY_ESCAPE             256
	#define KEY_ENTER              257
	#define KEY_TAB                258
	#define KEY_BACKSPACE          259
	#define KEY_INSERT             260
	#define KEY_DELETE             261
	#define KEY_RIGHT              262
	#define KEY_LEFT               263
	#define KEY_DOWN               264
	#define KEY_UP                 265
	#define KEY_PAGE_UP            266
	#define KEY_PAGE_DOWN          267
	#define KEY_HOME               268
	#define KEY_END                269
	#define KEY_CAPS_LOCK          280
	#define KEY_SCROLL_LOCK        281
	#define KEY_NUM_LOCK           282
	#define KEY_PRINT_SCREEN       283
	#define KEY_PAUSE              284
	#define KEY_F1                 290
	#define KEY_F2                 291
	#define KEY_F3                 292
	#define KEY_F4                 293
	#define KEY_F5                 294
	#define KEY_F6                 295
	#define KEY_F7                 296
	#define KEY_F8                 297
	#define KEY_F9                 298
	#define KEY_F10                299
	#define KEY_F11                300
	#define KEY_F12                301
	#define KEY_F13                302
	#define KEY_F14                303
	#define KEY_F15                304
	#define KEY_F16                305
	#define KEY_F17                306
	#define KEY_F18                307
	#define KEY_F19                308
	#define KEY_F20                309
	#define KEY_F21                310
	#define KEY_F22                311
	#define KEY_F23                312
	#define KEY_F24                313
	#define KEY_F25                314
	#define KEY_KP_0               320
	#define KEY_KP_1               321
	#define KEY_KP_2               322
	#define KEY_KP_3               323
	#define KEY_KP_4               324
	#define KEY_KP_5               325
	#define KEY_KP_6               326
	#define KEY_KP_7               327
	#define KEY_KP_8               328
	#define KEY_KP_9               329
	#define KEY_KP_DECIMAL         330
	#define KEY_KP_DIVIDE          331
	#define KEY_KP_MULTIPLY        332
	#define KEY_KP_SUBTRACT        333
	#define KEY_KP_ADD             334
	#define KEY_KP_ENTER           335
	#define KEY_KP_EQUAL           336
	#define KEY_LEFT_SHIFT         340
	#define KEY_LEFT_CONTROL       341
	#define KEY_LEFT_ALT           342
	#define KEY_LEFT_SUPER         343
	#define KEY_RIGHT_SHIFT        344
	#define KEY_RIGHT_CONTROL      345
	#define KEY_RIGHT_ALT          346
	#define KEY_RIGHT_SUPER        347
	#define KEY_MENU               348

	#define MOUSE_BUTTON_1         0
	#define MOUSE_BUTTON_2         1
	#define MOUSE_BUTTON_3         2
	#define MOUSE_BUTTON_4         3
	#define MOUSE_BUTTON_5         4
	#define MOUSE_BUTTON_6         5
	#define MOUSE_BUTTON_7         6
	#define MOUSE_BUTTON_8         7
	#define MOUSE_BUTTON_LAST      MOUSE_BUTTON_8
	#define MOUSE_BUTTON_LEFT      MOUSE_BUTTON_1
	#define MOUSE_BUTTON_RIGHT     MOUSE_BUTTON_2
	#define MOUSE_BUTTON_MIDDLE    MOUSE_BUTTON_3
	//************************************
#pragma endregion



}

