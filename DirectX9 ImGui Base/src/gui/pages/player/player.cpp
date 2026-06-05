#include "../pages.hpp"
#include "player.hpp"

#include "../../../game/cheats.hpp"

namespace gui::pages
{
	void RenderPlayer() noexcept
	{
		if (ImGui::Checkbox("Enable Cheat Menu (C)", &vars::cheatMenu))
		{
			game::SetCheatsMenuEnabled(vars::cheatMenu);
		}

		if (ImGui::Checkbox("God Mode", &vars::bGodMode))
		{
			game::SetGodMode(vars::bGodMode);
		}
		ImGui::SliderInt("FPS", &vars::iFPS, 30, 300);
	}
}
