#ifndef MENUGFXHEADER
#define MENUGFXHEADER

#include "userface.hpp"

class MenuGfx : public Menu{
        public:

        class ResolutionListModel;

        class DropResActionListener;
        class SoundActionListener;
        class FScreenActionListener;

        DropDown* dropRes;
        CheckBox* checkFullScreen;
        CheckBox* checkSound;
        Button* buttonBack;

        void onEnter();
        void onLeave();

        MenuGfx();
};

#endif // MENUGFXHEADER
