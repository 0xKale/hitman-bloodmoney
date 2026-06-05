#include "../pages.hpp"
#include "player.hpp"

#include "../../../game/cheats.hpp"

namespace gui::pages
{
	void RenderPlayer() noexcept
	{
		ImGui::Checkbox("Enable Cheat Menu (C)", &vars::cheatMenu);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			game::SetCheatsMenuEnabled(vars::cheatMenu);
		}

		ImGui::Checkbox("God Mode", &vars::bGodMode);
		ImGui::SliderInt("FPS", &vars::iFPS, 30, 300);
	}
}
