#include "DefaultScreenGraph.h"

#include "components/ComponentContainer.h"

#include "Screen.h"



DefaultScreenGraph::DefaultScreenGraph(FullRefreshProvider *refreshProvider, ComponentContainer *components)
        : ScreenGraph(refreshProvider, components)
{
        // set initial screen
        switchScreen(ScreenTag::DefaultWatchFace, 1, TransitionEffect::None);

        // transitions from UtilityWatchFace
        addSwipeTransition(ScreenTag::UtilityWatchFace, ScreenTag::Settings, Screen::SwipeDirection::Up);
        addSwipeTransition(ScreenTag::UtilityWatchFace, ScreenTag::InfographWatchFace, Screen::SwipeDirection::Left);

        // transitions from InfographWatchFace
        addSwipeTransition(ScreenTag::InfographWatchFace, ScreenTag::Settings, Screen::SwipeDirection::Up);
        addSwipeTransition(ScreenTag::InfographWatchFace, ScreenTag::BinaryWatchFace, Screen::SwipeDirection::Left);

        // transitions from BinaryWatchFace
        addSwipeTransition(ScreenTag::BinaryWatchFace, ScreenTag::Settings, Screen::SwipeDirection::Up);
        addSwipeTransition(ScreenTag::BinaryWatchFace, ScreenTag::UtilityWatchFace, Screen::SwipeDirection::Left);

        // transitions from FirmwareValidation
        addButtonTransition(ScreenTag::FirmwareValidation, ScreenTag::DefaultWatchFace);
        addSwipeTransition(ScreenTag::FirmwareValidation, ScreenTag::Previous, Screen::SwipeDirection::Right);

        // transitions from SystemInfo
        addButtonTransition(ScreenTag::SystemInfo, ScreenTag::DefaultWatchFace);
        addSwipeTransition(ScreenTag::SystemInfo, ScreenTag::Previous, Screen::SwipeDirection::Right);

        // transitions from Settings
        addButtonTransition(ScreenTag::Settings, ScreenTag::DefaultWatchFace);
        addSwipeTransition(ScreenTag::Settings, ScreenTag::DefaultWatchFace, Screen::SwipeDirection::Down);

        // transitions from Brightness
        addButtonTransition(ScreenTag::Brightness, ScreenTag::DefaultWatchFace);
        addSwipeTransition(ScreenTag::Brightness, ScreenTag::Previous, Screen::SwipeDirection::Right);

        // transitions from WakeUpMode
        addButtonTransition(ScreenTag::WakeUpMode, ScreenTag::DefaultWatchFace);
        addSwipeTransition(ScreenTag::WakeUpMode, ScreenTag::Previous, Screen::SwipeDirection::Right);

        // transitions from StepsGoal
        addButtonTransition(ScreenTag::StepsGoal, ScreenTag::DefaultWatchFace);
        addSwipeTransition(ScreenTag::StepsGoal, ScreenTag::Previous, Screen::SwipeDirection::Right);
}


ScreenGraph::ScreenTag DefaultScreenGraph::watchFaceScreenTagByIndex(uint8_t watchFaceIndex)
{
        switch (watchFaceIndex)
        {
        case 1:
                return ScreenTag::InfographWatchFace;
        case 2:
                return ScreenTag::BinaryWatchFace;
        default:
                return ScreenTag::UtilityWatchFace;
        }
}
