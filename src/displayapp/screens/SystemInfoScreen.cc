#include "SystemInfoScreen.h"

#include <cstdint>
#include <lvgl/lvgl.h>

#include "displayapp/fonts/font_dvsb_ascii_18.h"
#include "displayapp/fonts/font_dvs_ascii_16.h"

#include "Version.h"
#include "BootloaderVersion.h"


#define FG_COLOR_LABEL     0x808080
#define FG_COLOR_VALUE     0xffffff

#define PAGES              4

#define COL_LABEL          16
#define COL_VALUE          120
#define COL_INDENT         32
#define ROW_START          50
#define ROW_DISTANCE       24



SystemInfoScreen::SystemInfoScreen(ScreenGraph *screenGraph, ComponentContainer *components)
        : Screen(screenGraph, components)
{
        _lastUpdateTicks = 0;
        setCurrentPage(1);
}


uint8_t SystemInfoScreen::pageCount() const
{
        return PAGES;
}


void SystemInfoScreen::refresh()
{
        // don't update too often
        uint32_t ticks = lv_tick_get();
        if ((ticks - _lastUpdateTicks) > 500)
                _lastUpdateTicks = ticks;
        else
                return;

        // refresh the widgets of the current page
        switch (currentPage())
        {
        case 1:
                refreshVersionPageWidgets();
                break;
        case 2:
                refreshSystemInfoPageWidgets();
                break;
        case 3:
                refreshLvglPageWidgets();
                break;
        case 4:
                refreshLicensePageWidgets();
                break;
        default:
                break;
        }
}


lv_color_t SystemInfoScreen::foregroundColor() const
{
        return lv_color_hex(FG_COLOR_LABEL);
}


void SystemInfoScreen::handleCurrentPageChange()
{
        // create title label
        lv_obj_t *titleLabel = createTitleLabel("B3yondTime");
        lv_obj_set_style_local_text_color(titleLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xffff00));

        // create the page-specific widgets
        switch (currentPage())
        {
        case 1:
                createVersionPageWidgets();
                break;
        case 2:
                createSystemInfoPageWidgets();
                break;
        case 3:
                createLvglPageWidgets();
                break;
        case 4:
                createLicensePageWidgets();
                break;
        default:
                break;
        }
}


void SystemInfoScreen::createVersionPageWidgets()
{
        addLabel(0, "Version:");
        addValue(0, Pinetime::Version::VersionString());

        addLabel(1, "git commit:");
        addValue(1, Pinetime::Version::GitCommitHash());

        addLabel(2, "Build date:");
        addValue(2, __DATE__);

        addLabel(3, "Build time:");
        addValue(3, __TIME__);

        addLabel(4, "Bootloader:");
        addValue(4, Pinetime::BootloaderVersion::VersionString());
}


void SystemInfoScreen::refreshVersionPageWidgets()
{
        // only static information...
}


void SystemInfoScreen::createSystemInfoPageWidgets()
{
        static constexpr int motionDeviceRow = 0;
        addLabel(motionDeviceRow, "Motion dev.:");
        switch (this->components()->motion()->DeviceType())
        {
        case MotionController::DeviceTypes::BMA421:
                addValue(motionDeviceRow, "BMA421");
                break;
        case MotionController::DeviceTypes::BMA425:
                addValue(motionDeviceRow, "BMA425");
                break;
        default:
                addValue(motionDeviceRow, "unknown");
                break;
        }

        char buffer[256];
        sprintf(buffer,
                "%02x:%02x:%02x:%02x:%02x:%02x",
                this->components()->ble()->Address()[5],
                this->components()->ble()->Address()[4],
                this->components()->ble()->Address()[3],
                this->components()->ble()->Address()[2],
                this->components()->ble()->Address()[1],
                this->components()->ble()->Address()[0]);

        addLabel(1, "BLE MAC address:");
        addValue(2, buffer, true);

        addLabel(3, "Last reset:");
        static constexpr int resetValueRow = 4;
        static constexpr bool resetValueFullRow = true;
        switch (this->components()->watchdog()->GetResetReason())
        {
        case Watchdog::ResetReason::ResetPin:
                addValue(resetValueRow, "Reset pin", resetValueFullRow);
                break;
        case Watchdog::ResetReason::Watchdog:
                addValue(resetValueRow, "Watchdog", resetValueFullRow);
                break;
        case Watchdog::ResetReason::SoftReset:
                addValue(resetValueRow, "Soft reset", resetValueFullRow);
                break;
        case Watchdog::ResetReason::CpuLockup:
                addValue(resetValueRow, "CPU lockup", resetValueFullRow);
                break;
        case Watchdog::ResetReason::SystemOff:
                addValue(resetValueRow, "System off", resetValueFullRow);
                break;
        case Watchdog::ResetReason::LpComp:
                addValue(resetValueRow, "LP comp", resetValueFullRow);
                break;
        case Watchdog::ResetReason::DebugInterface:
                addValue(resetValueRow, "Debug iface", resetValueFullRow);
                break;
        case Watchdog::ResetReason::NFC:
                addValue(resetValueRow, "NFC", resetValueFullRow);
                break;
        case Watchdog::ResetReason::HardReset:
                addValue(resetValueRow, "Hard reset", resetValueFullRow);
                break;
        default:
                break;
        }

        addLabel(5, "Uptime:");
        _uptimeLabel = addValue(6, "00:00:00", true);
        refreshSystemInfoPageWidgets();
}


void SystemInfoScreen::refreshSystemInfoPageWidgets()
{
        // calculate uptime components
        static constexpr uint32_t secondsInADay = 60 * 60 * 24;
        static constexpr uint32_t secondsInAnHour = 60 * 60;
        static constexpr uint32_t secondsInAMinute = 60;
        uint32_t uptimeSeconds = static_cast<uint32_t>(components()->dateTime()->Uptime().count());
        uint32_t uptimeDays = (uptimeSeconds / secondsInADay);
        uptimeSeconds = uptimeSeconds % secondsInADay;
        uint32_t uptimeHours = uptimeSeconds / secondsInAnHour;
        uptimeSeconds = uptimeSeconds % secondsInAnHour;
        uint32_t uptimeMinutes = uptimeSeconds / secondsInAMinute;
        uptimeSeconds = uptimeSeconds % secondsInAMinute;

        lv_label_set_text_fmt(_uptimeLabel, "%d days, %02d:%02d:%02d", uptimeDays, uptimeHours, uptimeMinutes, uptimeSeconds);
}


void SystemInfoScreen::createLvglPageWidgets()
{
        addLabel(0, "LVGL memory usage");

        addLabel(1, "cur. use:", true);
        _curMemoryUseLabel = addValue(1, "0");
        lv_obj_set_auto_realign(_curMemoryUseLabel, true);

        addLabel(2, "max use:", true);
        _maxMemoryUseLabel = addValue(2, "0");
        lv_obj_set_auto_realign(_maxMemoryUseLabel, true);

        addLabel(3, "fragm.:", true);
        _fragmentationLabel = addValue(3, "0");
        lv_obj_set_auto_realign(_fragmentationLabel, true);

        addLabel(4, "free:", true);
        _freeMemoryLabel = addValue(4, "0");
        lv_obj_set_auto_realign(_freeMemoryLabel, true);

        refreshLvglPageWidgets();
}


void SystemInfoScreen::refreshLvglPageWidgets()
{
        char buffer[256];
        buffer[255] = '\0';

        lv_mem_monitor_t memInfo;
        lv_mem_monitor(&memInfo);

        snprintf(buffer, 255, "%d (%d%%)", static_cast<int>(memInfo.total_size - memInfo.free_size), memInfo.used_pct);
        lv_label_set_text(_curMemoryUseLabel, buffer);

        snprintf(buffer, 255, "%d", static_cast<int>(memInfo.max_used));
        lv_label_set_text(_maxMemoryUseLabel, buffer);

        snprintf(buffer, 255, "%d%%", static_cast<int>(memInfo.frag_pct));
        lv_label_set_text(_fragmentationLabel, buffer);

        snprintf(buffer, 255, "%d", static_cast<int>(memInfo.free_biggest_size));
        lv_label_set_text(_freeMemoryLabel, buffer);
}


void SystemInfoScreen::createLicensePageWidgets()
{
        lv_obj_t *licenseLabel = createLabel(&font_dvs_ascii_16, lv_color_hex(FG_COLOR_LABEL), LV_LABEL_ALIGN_CENTER, false);
        lv_label_set_recolor(licenseLabel, true);
        lv_label_set_text_static(licenseLabel,
                                 "Software licensed\n"
                                 "under the terms of\n"
                                 "the GNU General\n"
                                 "Public License V3\n\n"
                                 "Source code at\n"
                                 "#FFFFFF https://github.com/#\n"
                                 "#FFFFFF B3yondC/B3yondTime#");
        lv_obj_align(licenseLabel, nullptr, LV_ALIGN_CENTER, 0, 8);
}


void SystemInfoScreen::refreshLicensePageWidgets()
{
        // only static information...
}


lv_obj_t *SystemInfoScreen::addLabel(int row, const char *text, bool indent)
{
        lv_obj_t *label = createLabel(&font_dvs_ascii_16, lv_color_hex(FG_COLOR_LABEL), LV_LABEL_ALIGN_LEFT, false);
        lv_label_set_text_static(label, text);
        lv_coord_t x = indent ? COL_INDENT : COL_LABEL;
        lv_coord_t y = static_cast<lv_coord_t>(ROW_START + (row * ROW_DISTANCE));
        lv_obj_align(label, nullptr, LV_ALIGN_IN_TOP_LEFT, x, y);
        return label;
}


lv_obj_t *SystemInfoScreen::addValue(int row, const char *text, bool ownRow)
{
        lv_obj_t *value = createLabel(&font_dvs_ascii_16, lv_color_hex(FG_COLOR_VALUE), LV_LABEL_ALIGN_LEFT, false);
        lv_label_set_text(value, text);
        lv_coord_t x = ownRow ? COL_INDENT : COL_VALUE;
        lv_coord_t y = static_cast<lv_coord_t>(ROW_START + (row * ROW_DISTANCE));
        lv_obj_align(value, nullptr, LV_ALIGN_IN_TOP_LEFT, x, y);
        return value;
}
