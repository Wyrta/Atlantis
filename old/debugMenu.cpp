#include "debugMenu.hpp"
#include "Entity.hpp"

extern SDL_Renderer *render;

TTF_Font *DebugMenu::debugFont = NULL;

DebugMenu::DebugMenu(string font)
{
	debugFont = createFont(font.c_str(), 16);

	fpsTxt  = write(render, &rectFpsTxt, debugFont, "Render time:");
		rectFpsTxt.x = 0; rectFpsTxt.y = 0;
	fpsValue = write(render, &rectFpsValue, debugFont, "0");
		rectFpsValue.x = rectFpsTxt.w + 3;
		rectFpsValue.y = 0;
}

void DebugMenu::print(int fps)
{
	if (!this->active)
		return;
		
	/* FPS */
	SDL_DestroyTexture(fpsValue);
	fpsValue = write(render, &rectFpsValue, debugFont, to_string(fps).c_str());

	rectFpsValue.x = rectFpsTxt.w + 3;
	rectFpsValue.y = 0;

	SDL_RenderCopy(render, fpsTxt,   NULL, &rectFpsTxt);
	SDL_RenderCopy(render, fpsValue, NULL, &rectFpsValue);

	/* Entity */

	for(int i = 0; i < MAX_ENTITY; i++)
	{
		if (Entity::entityTab[i] != NULL)
		{
			SDL_SetRenderDrawColor(render, 255, 0, 0, 128);
			SDL_RenderFillRect(render, Entity::entityTab[i]->getHitbox_ptr());

			/* print extra data */

			char coo_txt[64];   SDL_memset(coo_txt, 0, 64);
			snprintf(coo_txt, 64, "Coo (X,Y): %d %d", 
									Entity::entityTab[i]->getPosition().x,
									Entity::entityTab[i]->getPosition().y
					);

			char atk_txt[64];   SDL_memset(atk_txt, 0, 64);
			snprintf(atk_txt, 64, "Atk : %d ( %d + %d)", 
									Entity::entityTab[i]->getBase_atk() + Entity::entityTab[i]->item_atk_bonus(),
									Entity::entityTab[i]->getBase_atk(),
									Entity::entityTab[i]->item_atk_bonus()
					);

			char hp_txt[64];   SDL_memset(hp_txt, 0, 64);
			snprintf(hp_txt, 64, "Hp : %d ( %d + %d)", 
									Entity::entityTab[i]->getBase_hp() + Entity::entityTab[i]->item_hp_bonus(),
									Entity::entityTab[i]->getMax_hp(),
									Entity::entityTab[i]->item_hp_bonus()
					);

			char speed_txt[64];   SDL_memset(speed_txt, 0, 64);
			snprintf(speed_txt, 64, "Speed : %d ( %d + %d)", 
									Entity::entityTab[i]->getSpeed() + Entity::entityTab[i]->item_speed_bonus(),
									Entity::entityTab[i]->getSpeed(),
									Entity::entityTab[i]->item_speed_bonus()
					);

			this->name_texture	= write(render, &this->rect_name,  this->debugFont, Entity::entityTab[i]->getName().c_str());
			this->coo_texture	= write(render, &this->rect_coo,   this->debugFont, coo_txt);
			this->atk_texture	= write(render, &this->rect_atk,   this->debugFont, atk_txt);
			this->hp_texture	= write(render, &this->rect_hp,	this->debugFont, hp_txt);
			this->speed_texture	= write(render, &this->rect_speed, this->debugFont, speed_txt);

			this->rect_name.x	= Entity::entityTab[i]->getHitbox().x;
			this->rect_coo.x	= Entity::entityTab[i]->getHitbox().x;
			this->rect_hp.x		= Entity::entityTab[i]->getHitbox().x;
			this->rect_atk.x	= Entity::entityTab[i]->getHitbox().x;
			this->rect_speed.x	= Entity::entityTab[i]->getHitbox().x;

			this->rect_name.y	= Entity::entityTab[i]->getHitbox().y - this->rect_speed.h - this->rect_atk.h - this->rect_hp.h - this->rect_coo.h - this->rect_name.h;
			this->rect_coo.y	= Entity::entityTab[i]->getHitbox().y - this->rect_speed.h - this->rect_atk.h - this->rect_hp.h - this->rect_coo.h;
			this->rect_hp.y		= Entity::entityTab[i]->getHitbox().y - this->rect_speed.h - this->rect_atk.h - this->rect_hp.h;
			this->rect_atk.y	= Entity::entityTab[i]->getHitbox().y - this->rect_speed.h - this->rect_atk.h;
			this->rect_speed.y	= Entity::entityTab[i]->getHitbox().y - this->rect_speed.h;

			SDL_RenderCopy(render, this->name_texture,	NULL, &this->rect_name);
			SDL_RenderCopy(render, this->coo_texture,	NULL, &this->rect_coo);
			SDL_RenderCopy(render, this->hp_texture,	NULL, &this->rect_hp);
			SDL_RenderCopy(render, this->atk_texture,	NULL, &this->rect_atk);
			SDL_RenderCopy(render, this->speed_texture,	NULL, &this->rect_speed);

			SDL_DestroyTexture(this->name_texture);
			SDL_DestroyTexture(this->coo_texture);
			SDL_DestroyTexture(this->atk_texture);
			SDL_DestroyTexture(this->hp_texture);
			SDL_DestroyTexture(this->speed_texture);
		}
	}

}

