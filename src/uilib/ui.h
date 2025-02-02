#ifndef _UILIB_UI_H
#define _UILIB_UI_H

#include "window.h"
#include <map>
#include <string>

namespace Ui {

class Ui {
protected:
    std::map<Window::ID,Window*> _windows;
    std::string _name;
    unsigned _lastRenderDuration = 0;
    uint64_t _lastFrameMicroTimestamp = 0;
    unsigned _globalMouseButton = 0;
public:
    Ui(const char *name);
    virtual ~Ui();
    template <class T>
    T *createWindow(const char *title, SDL_Surface* icon=nullptr, const Position& pos={-1,-1}, const Size& size={0,0})
    {
        T *win = new T(title,icon,pos,size);
        this->_windows[win->getID()] = win;
        return win;
    }
    void destroyWindow(Window *win);
    bool render();
    
    Signal<Window*> onWindowDestroyed;
};

} // namespace Ui

#endif // _UILIB_UI_H
