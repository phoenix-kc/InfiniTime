#ifndef SCREEN_H
#define SCREEN_H


#include <vector>
#include <lvgl/lvgl.h>

#include "components/ComponentContainer.h"

class ScreenGraph;



class Screen
{
public:

        enum class SwipeDirection : uint8_t
        {
                None,
                Left,
                Right,
                Up,
                Down
        };

        Screen(ScreenGraph *screenGraph, ComponentContainer *components);
        virtual ~Screen();

        ScreenGraph *screenGraph() { return _screenGraph; }
        ComponentContainer *components() { return _components; }

        bool isRunning() const { return _isRunning; }

        virtual lv_color_t backgroundColor() const { return lv_color_hex(0x000000); }
        virtual lv_color_t foregroundColor() const { return lv_color_hex(0xffffff); }

        virtual bool isMultiPageScreen() const { return false; }
        virtual uint8_t pageCount() const { return 1; }
        uint8_t currentPage() const { return _currentPage; }
        bool setCurrentPage(uint8_t value);

        virtual void refresh();

        virtual bool handleButtonPress();
        virtual bool handleTap();
        virtual bool handleLongTap();
        virtual bool handleDoubleTap();
        virtual bool handleSwipe(SwipeDirection direction);

protected:

        bool _isRunning;

        lv_obj_t *createLabel(const lv_font_t *font, lv_color_t color, lv_label_align_t align, bool autoAlign);
        lv_obj_t *createLabel(lv_obj_t *parent, const lv_font_t *font, lv_color_t color, lv_label_align_t align, bool autoAlign);

        lv_obj_t *createTitleLabel(const char *title);

        lv_obj_t *createButton(lv_coord_t width = 64, lv_coord_t height = 48);
        lv_obj_t *createButton(lv_obj_t *parent, lv_coord_t width = 64, lv_coord_t height = 48);
        lv_obj_t *createButton(const lv_font_t *iconFont, const char *icon, lv_coord_t width = 64, lv_coord_t height = 48);
        lv_obj_t *createButton(lv_obj_t *parent, const lv_font_t *iconFont, const char *icon, lv_coord_t width = 64, lv_coord_t height = 48);

        virtual void handleCurrentPageChange();
        virtual void addPageNumberWidgets();

private:

        ScreenGraph *_screenGraph;
        ComponentContainer *_components;

        uint8_t _currentPage;

        lv_style_t _buttonStyle;
};

#endif // SCREEN_H
