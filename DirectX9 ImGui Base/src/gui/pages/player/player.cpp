#include "../pages.hpp"
#include "player.hpp"

#include "../../../game/actions.hpp"
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

		if (ImGui::Checkbox("Infinite Ammo", &vars::bInfAmmo))
		{
			game::SetInfAmmo(vars::bInfAmmo);
		}

		if (ImGui::Checkbox("Infinite Clips", &vars::bInfClips))
		{
			game::SetInfClips(vars::bInfClips);
		}

		if (ImGui::Button("Give All Items"))
		{
			game::RequestGiveAllItems();
		}

		ImGui::SliderInt("FPS", &vars::iFPS, 30, 300);
	}
}
