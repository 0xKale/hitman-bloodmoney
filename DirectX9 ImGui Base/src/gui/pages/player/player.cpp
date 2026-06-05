#include "../pages.hpp"
#include "player.hpp"

#include "../../../project/functions.hpp"

namespace gui::pages
{
	void RenderPlayer() noexcept
	{
		ImGui::Checkbox("Debug Menu", &vars::bDebugMenu);
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			functions::cheatEnableMenu();
		}

		ImGui::Checkbox("God Mode", &vars::bGodMode);
		ImGui::SliderInt("FPS", &vars::iFPS, 30, 300);
	}
}
