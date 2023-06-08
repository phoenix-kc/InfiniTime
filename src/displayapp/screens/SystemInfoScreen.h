#ifndef SYSTEMINFOSCREEN_H
#define SYSTEMINFOSCREEN_H


#include "Screen.h"



class SystemInfoScreen : public Screen
{
public:

        SystemInfoScreen(ScreenGraph *screenGraph, ComponentContainer *components);

        bool isMultiPageScreen() const override { return true; }
        uint8_t pageCount() const override;

        void refresh() override;

protected:

        lv_color_t foregroundColor() const override;

        void handleCurrentPageChange() override;

private:

        lv_obj_t *_uptimeLabel;

        lv_obj_t *_curMemoryUseLabel;
        lv_obj_t *_maxMemoryUseLabel;
        lv_obj_t *_fragmentationLabel;
        lv_obj_t *_freeMemoryLabel;

        uint32_t _lastUpdateTicks;

        void createVersionPageWidgets();
        void refreshVersionPageWidgets();

        void createSystemInfoPageWidgets();
        void refreshSystemInfoPageWidgets();

        void createLvglPageWidgets();
        void refreshLvglPageWidgets();

        void createLicensePageWidgets();
        void refreshLicensePageWidgets();

        lv_obj_t *addLabel(int row, const char *text, bool indent = false);
        lv_obj_t *addValue(int row, const char *text, bool ownRow = false);
};

#endif // SYSTEMINFOSCREEN_H
