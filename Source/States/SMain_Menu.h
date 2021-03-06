#ifndef SMAIN_MENU_H_INCLUDED
#define SMAIN_MENU_H_INCLUDED

#include "Game_State.h"

#include "../GUI/GUI.h"

namespace State
{
    class Main_Menu : public Game_State
    {
        public:
            Main_Menu(Application& application);

            void input  (sf::Event& e) override;
            void input  (Camera& camera) override;
            void update (Camera& camera, float dt) override;
            void draw   (Renderer::Master& renderer) override;

        private:
            void initMenu();

            GUI::Panel m_frontMenu;
            GUI::Panel m_playMenu;
            GUI::Panel* mp_activeMenu = &m_frontMenu;

            int32_t m_worldSize = 0;
    };
}

#endif // SMAIN_MENU_H_INCLUDED
