#include "Screen.h"

#include <lvgl/lvgl.h>

#include "displayapp/fonts/font_dvsb_ascii_18.h"
#include "displayapp/fonts/font_dvs_ascii_12.h"
#include "displayapp/fonts/font_symbols_14.h"
#include "components/ComponentContainer.h"



Screen::Screen(ScreenGraph *screenGraph, ComponentContainer *components)
{
        // copy the references
        _screenGraph = screenGraph;
        _components = components;

        // clear the screen, the constructor of the derived class will refill it
        lv_obj_clean(lv_scr_act());

        //lv_obj_set_scrollbar_mode(lv_scr_act(), LV_SCROLLBAR_MODE_OFF);

        // set the default background color
        lv_obj_set_style_local_bg_color(lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, backgroundColor());

        // consider the screen "running" until told otherwise
        _isRunning = true;

        // current page has not been set
        _currentPage = 0;

        // set up the style of the default button
        lv_style_init(&_buttonStyle);
        lv_style_set_bg_opa(&_buttonStyle, LV_STATE_DEFAULT, LV_OPA_COVER);
        lv_style_set_bg_color(&_buttonStyle, LV_STATE_DEFAULT, lv_color_hex(0x606060));
        lv_style_set_bg_grad_color(&_buttonStyle, LV_STATE_DEFAULT, lv_color_hex(0x202020));
        lv_style_set_bg_grad_dir(&_buttonStyle, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
        lv_style_set_radius(&_buttonStyle, LV_STATE_DEFAULT, 4);
        lv_style_set_border_opa(&_buttonStyle, LV_STATE_DEFAULT, LV_OPA_COVER);
        lv_style_set_border_color(&_buttonStyle, LV_STATE_DEFAULT, lv_color_hex(0x505050));
        lv_style_set_border_width(&_buttonStyle, LV_STATE_DEFAULT, 1);
}


Screen::~Screen()
{
        // delete the lvgl child objects
        lv_obj_clean(lv_scr_act());
}


bool Screen::setCurrentPage(uint8_t value)
{
        // guards
        if (!isMultiPageScreen() || (value < 1) || (value > pageCount()))
                return false;

        // remove the widgets of the previous page
        lv_obj_clean(lv_scr_act());

        // create the new page
        _currentPage = value;
        if (pageCount() > 1)
                addPageNumberWidgets();
        handleCurrentPageChange();
        return true;
}


void Screen::refresh()
{
}


bool Screen::handleButtonPress()
{
        return false;
}


bool Screen::handleTap()
{
        return false;
}


bool Screen::handleLongTap()
{
        return false;
}


bool Screen::handleDoubleTap()
{
        return false;
}


bool Screen::handleSwipe(SwipeDirection direction)
{
        // swiping up and down is automatically handled in multi-page screens
        if (isMultiPageScreen())
        {
                switch (direction)
                {
                case SwipeDirection::Up:     // next page
                        if (_currentPage < pageCount())
                        {
                                setCurrentPage(_currentPage + 1);
                                return true;
                        }
                        break;
                case SwipeDirection::Down:   // previous page
                        if (_currentPage > 1)
                        {
                                setCurrentPage(_currentPage - 1);
                                return true;
                        }
                        break;
                default:
                        break;
                }
        }

        return false;
}


lv_obj_t *Screen::createLabel(const lv_font_t *font, lv_color_t color, lv_label_align_t align, bool autoAlign)
{
        return createLabel(lv_scr_act(), font, color, align, autoAlign);
}


lv_obj_t *Screen::createLabel(lv_obj_t *parent, const lv_font_t *font, lv_color_t color, lv_label_align_t align, bool autoAlign)
{
        lv_obj_t *label = lv_label_create(parent, nullptr);
        lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, font);
        lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, color);
        lv_label_set_align(label, align);
        lv_obj_set_auto_realign(label, autoAlign);
        return label;
}


lv_obj_t *Screen::createTitleLabel(const char *title)
{
        lv_obj_t *titleLabel = createLabel(&font_dvsb_ascii_18, foregroundColor(), LV_LABEL_ALIGN_CENTER, false);
        lv_label_set_text_static(titleLabel, title);
        lv_obj_align(titleLabel, nullptr, LV_ALIGN_IN_TOP_MID, 0, 4);
        return titleLabel;
}


lv_obj_t *Screen::createButton(lv_coord_t width, lv_coord_t height)
{
        return createButton(lv_scr_act(), width, height);
}


lv_obj_t *Screen::createButton(lv_obj_t *parent, lv_coord_t width, lv_coord_t height)
{
        lv_obj_t *button = lv_btn_create(parent, nullptr);
        lv_obj_set_size(button, width, height);
        lv_obj_add_style(button, LV_BTN_PART_MAIN, &_buttonStyle);
        return button;
}


lv_obj_t *Screen::createButton(const lv_font_t *iconFont, const char *icon, lv_coord_t width, lv_coord_t height)
{
        return  createButton(lv_scr_act(), iconFont, icon, width, height);
}


lv_obj_t *Screen::createButton(lv_obj_t *parent, const lv_font_t *iconFont, const char *icon, lv_coord_t width, lv_coord_t height)
{
        lv_obj_t *button = createButton(parent, width, height);
        lv_obj_t *label = createLabel(button, iconFont, foregroundColor(), LV_LABEL_ALIGN_CENTER, false);
        lv_label_set_text_static(label, icon);
        return button;
}


void Screen::handleCurrentPageChange()
{
}


void Screen::addPageNumberWidgets()
{
        lv_color_t color = lv_color_hex(0x808080);

        // up arrow
        if (_currentPage > 1)
        {
                lv_obj_t *upSymbol = createLabel(&font_symbols_14, color, LV_LABEL_ALIGN_CENTER, false);
                lv_label_set_text_static(upSymbol, SYMBOL_SORT_UP);
                lv_obj_align(upSymbol, nullptr, LV_ALIGN_IN_TOP_RIGHT, -6, 0);
        }

        // down arrow
        if (_currentPage < pageCount())
        {
                lv_obj_t *downSymbol = createLabel(&font_symbols_14, color, LV_LABEL_ALIGN_CENTER, false);
                lv_label_set_text_static(downSymbol, SYMBOL_SORT_DOWN);
                lv_obj_align(downSymbol, nullptr, LV_ALIGN_IN_TOP_RIGHT, -6, 14);
        }

        // page number
        lv_obj_t *pageLabel = createLabel(&font_dvs_ascii_12, color, LV_LABEL_ALIGN_RIGHT, false);
        lv_label_set_text_fmt(pageLabel, "%d/%d", _currentPage, pageCount());
        lv_obj_align(pageLabel, nullptr, LV_ALIGN_IN_TOP_RIGHT, 0, 7);
}
