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
			if (vars::bGodMode && !vars::cheatMenu)
			{
				vars::cheatMenu = true;
				game::SetCheatsMenuEnabled(true);
			}
			game::SetGodMode(vars::bGodMode);
		}

		if (vars::cheatMenu)
		{
			vars::bIsInvisible = game::IsInvisibleMode();
		}

		if (ImGui::Checkbox("Invisible Mode", &vars::bIsInvisible))
		{
			if (vars::bIsInvisible && !vars::cheatMenu)
			{
				vars::cheatMenu = true;
				game::SetCheatsMenuEnabled(true);
			}
			game::SetInvisibleMode(vars::bIsInvisible);
		}

		if (ImGui::Checkbox("Infinite Clips", &vars::bInfClips))
		{
			game::SetInfClips(vars::bInfClips);
		}

		if (ImGui::Button("Give All Items"))
		{
			game::RequestGiveAllItems();
		}

		if (ImGui::Button("Complete Mission"))
		{
			if (!vars::cheatMenu)
			{
				vars::cheatMenu = true;
				game::SetCheatsMenuEnabled(true);
			}
			game::RequestCompleteLevel();
		}

		ImGui::SliderInt("FPS", &vars::iFPS, 30, 300);
	}
}
