#include "Input.h"


namespace cm
{
	MouseData Input::mouse_data = {};

	KeyData Input::key_data = {};

	std::vector<InputCallBacks *> Input::msg = {};



}