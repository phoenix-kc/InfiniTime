#include "ScreenGraph.h"

#include "Screen.h"

#include "displayapp/FullRefreshProvider.h"
#include "displayapp/screens/UtilityWatchFace.h"
#include "displayapp/screens/InfographWatchFace.h"
#include "displayapp/screens/BinaryWatchFace.h"
#include "displayapp/screens/FirmwareUpdateScreen.h"
#include "displayapp/screens/FirmwareValidationScreen.h"
#include "displayapp/screens/SystemInfoScreen.h"
#include "displayapp/screens/SettingsScreen.h"
#include "displayapp/screens/BrightnessScreen.h"
#include "displayapp/screens/WakeUpModeScreen.h"
#include "displayapp/screens/StepsGoalScreen.h"


#define BUTTON_DEBOUNCE_TICKS   500
#define MAX_PREVIOUS_SCREENS    4



ScreenGraph::ScreenGraph(FullRefreshProvider *refreshProvider, ComponentContainer *components)
{
        _refreshProvider = refreshProvider;
        _components = components;
        _currentTag = ScreenTag::None;
        _nextTag = ScreenTag::None;
        _nextPageNumber = 1;
        _nextEffect = TransitionEffect::None;
        _currentScreen = nullptr;
        _transitions.clear();
        _lastButtonPressTicks = 0;
        _previousScreens.clear();
}


ScreenGraph::~ScreenGraph()
{
        // delete the current screen
        if (_currentScreen)
                delete _currentScreen;
}


void ScreenGraph::addButtonTransition(ScreenTag fromScreen, ScreenTag toScreen, TransitionEffect effect)
{
        addButtonTransition(fromScreen, toScreen, 1, effect);
}


void ScreenGraph::addButtonTransition(ScreenTag fromScreen, ScreenTag toScreen, uint8_t toPageNumber, TransitionEffect effect)
{
        // guards
        if ((fromScreen == ScreenTag::None) || (fromScreen == ScreenTag::Previous) || (toScreen == ScreenTag::None))
                return;

        // add the transition
        Transition transition;
        transition.trigger = TransitionTrigger::Button;
        transition.swipeDirection = Screen::SwipeDirection::None;
        transition.fromScreen = fromScreen;
        transition.toScreen = toScreen;
        transition.toPageNumber = toPageNumber;
        transition.effect = effect;
        _transitions.push_back(transition);
}


void ScreenGraph::addSwipeTransition(ScreenTag fromScreen, ScreenTag toScreen, Screen::SwipeDirection direction)
{
        addSwipeTransition(fromScreen, toScreen, 1, direction);
}


void ScreenGraph::addSwipeTransition(ScreenTag fromScreen, ScreenTag toScreen, uint8_t toPageNumber, Screen::SwipeDirection direction)
{
        // guards
        if ((fromScreen == ScreenTag::None) || (fromScreen == ScreenTag::Previous) || (toScreen == ScreenTag::None))
                return;
        if (direction == Screen::SwipeDirection::None)
                return;

        // add the transition
        Transition transition;
        transition.trigger = TransitionTrigger::Swipe;
        transition.swipeDirection = direction;
        transition.fromScreen = fromScreen;
        transition.toScreen = toScreen;
        transition.toPageNumber = toPageNumber;
        switch (direction)
        {
        case Screen::SwipeDirection::Left:
                transition.effect = TransitionEffect::MoveLeft;
                break;
        case Screen::SwipeDirection::Right:
                transition.effect = TransitionEffect::MoveRight;
                break;
        case Screen::SwipeDirection::Up:
                transition.effect = TransitionEffect::MoveTop;
                break;
        case Screen::SwipeDirection::Down:
                transition.effect = TransitionEffect::MoveBottom;
                break;
        default:
                transition.effect = TransitionEffect::None;
                break;
        }
        _transitions.push_back(transition);
}


void ScreenGraph::addInactivityTransition(ScreenTag fromScreen, ScreenTag toScreen, TransitionEffect effect)
{
        addInactivityTransition(fromScreen, toScreen, 1, effect);
}


void ScreenGraph::addInactivityTransition(ScreenTag fromScreen, ScreenTag toScreen, uint8_t toPageNumber, TransitionEffect effect)
{
        // guards
        if ((fromScreen == ScreenTag::None) || (fromScreen == ScreenTag::Previous) || (toScreen == ScreenTag::None))
                return;

        // add the transition
        Transition transition;
        transition.trigger = TransitionTrigger::Inactivity;
        transition.swipeDirection = Screen::SwipeDirection::None;
        transition.fromScreen = fromScreen;
        transition.toScreen = toScreen;
        transition.toPageNumber = toPageNumber;
        transition.effect = effect;
        _transitions.push_back(transition);
}


void ScreenGraph::activateScreen(ScreenTag tag, TransitionEffect effect)
{
        activateScreen(tag, 1, effect);
}


void ScreenGraph::activateScreen(ScreenTag tag, uint8_t pageNumber, TransitionEffect effect)
{
        // the screen will be switched on the next refresh
        _nextTag = tag;
        _nextPageNumber = pageNumber;
        _nextEffect = effect;
}


void ScreenGraph::setDefaultWatchFace(ScreenTag tag)
{
        // find the number that corresponds to the current watch face
        uint8_t watchFace = 0;
        for (uint8_t i = 0; i < 255; i++)
        {
                if (watchFaceScreenTagByIndex(i) == tag)
                {
                        watchFace = i;
                        break;
                }
        }

        // set the default
        _components->settings()->SetClockFace(watchFace);
}


void ScreenGraph::handleRefresh()
{
        // do we need to switch the screen?
        if (_nextTag != ScreenTag::None)
                switchScreen(_nextTag, _nextPageNumber, _nextEffect);
        else
        {
                // forward to the current screen
                if (_currentScreen)
                        _currentScreen->refresh();
        }
}


bool ScreenGraph::handleButtonPress()
{
        // additional button de-bouncing
        uint32_t nowTicks = lv_tick_get();
        if ((nowTicks - _lastButtonPressTicks) < BUTTON_DEBOUNCE_TICKS)
                return true;
        _lastButtonPressTicks = nowTicks;

        // check if the screen wants to handle the event
        if (_currentScreen)
        {
                if (_currentScreen->handleButtonPress())
                        return true;
        }

        // check if there's a matching transition
        for (Transition transition : _transitions)
        {
                if ((transition.trigger == TransitionTrigger::Button)
                                && (transition.fromScreen == _currentTag))
                {
                        switchScreen(transition.toScreen, transition.toPageNumber, transition.effect);
                        return true;
                }
        }

        // unhandled
        return false;
}


bool ScreenGraph::handleTap()
{
        // check if the screen wants to handle the event
        if (_currentScreen)
        {
                if (_currentScreen->handleTap())
                        return true;
        }

        // check if there's a matching transition
        for (Transition transition : _transitions)
        {
                if ((transition.trigger == TransitionTrigger::Tap)
                                && (transition.fromScreen == _currentTag))
                {
                        switchScreen(transition.toScreen, transition.toPageNumber, transition.effect);
                        return true;
                }
        }

        // unhandled
        return false;
}


bool ScreenGraph::handleLongTap()
{
        // check if the screen wants to handle the event
        if (_currentScreen)
        {
                if (_currentScreen->handleLongTap())
                        return true;
        }

        // check if there's a matching transition
        for (Transition transition : _transitions)
        {
                if ((transition.trigger == TransitionTrigger::LongTap)
                                && (transition.fromScreen == _currentTag))
                {
                        switchScreen(transition.toScreen, transition.toPageNumber, transition.effect);
                        return true;
                }
        }

        // unhandled
        return false;
}


bool ScreenGraph::handleDoubleTap()
{
        // check if the screen wants to handle the event
        if (_currentScreen)
        {
                if (_currentScreen->handleDoubleTap())
                        return true;
        }

        // check if there's a matching transition
        for (Transition transition : _transitions)
        {
                if ((transition.trigger == TransitionTrigger::DoubleTap)
                                && (transition.fromScreen == _currentTag))
                {
                        switchScreen(transition.toScreen, transition.toPageNumber, transition.effect);
                        return true;
                }
        }

        // unhandled
        return false;
}


bool ScreenGraph::handleSwipe(Screen::SwipeDirection direction)
{
        // check if the screen wants to handle the event
        if (_currentScreen)
        {
                if (_currentScreen->handleSwipe(direction))
                        return true;
        }

        // check if there's a matching transition
        for (Transition transition : _transitions)
        {
                if ((transition.trigger == TransitionTrigger::Swipe)
                                && (transition.swipeDirection == direction)
                                && (transition.fromScreen == _currentTag))
                {
                        switchScreen(transition.toScreen, transition.toPageNumber, transition.effect);
                        return true;
                }
        }

        // unhandled
        return false;
}


bool ScreenGraph::handleTouch(int x, int y)
{
        //TODO
        (void)x;
        (void)y;
        return false;
}


void ScreenGraph::switchScreen(ScreenTag tag, uint8_t pageNumber, TransitionEffect effect)
{
        // clear the "next screen" information
        _nextTag = ScreenTag::None;
        _nextPageNumber = 1;
        _nextEffect = TransitionEffect::None;

        // guards
        if (tag == ScreenTag::None)
                return;

        // handle special case: tag == Previous
        if (tag == ScreenTag::Previous)
        {
                // the screens stack must not be empty
                if (_previousScreens.empty())
                        return;

                // update destination tag and page number
                ScreenInfo prevScreen = _previousScreens.back();
                tag = prevScreen.tag;
                pageNumber = prevScreen.pageNumber;

                // remove that screen from the stack
                _previousScreens.pop_back();
        }
        else
        {
                // make sure that the screens stack doesn't exceed its maximum size
                while (static_cast<int>(_previousScreens.size()) >= MAX_PREVIOUS_SCREENS)
                        _previousScreens.erase(_previousScreens.begin());

                // remember the current screen
                ScreenInfo screen;
                screen.tag = _currentTag;
                screen.pageNumber = _currentScreen ? _currentScreen->currentPage() : 1;
                _previousScreens.push_back(screen);
        }

        // handle another special case: tag == DefaultWatchFace
        if (tag == ScreenTag::DefaultWatchFace)
        {
                // get default watch face from settings
                tag = watchFaceScreenTagByIndex(_components->settings()->GetClockFace());

                // stay on the current screen if no valid tag has been returned
                if (tag == ScreenTag::None)
                {
                        _previousScreens.pop_back();
                        return;
                }
        }

        // set "full refresh direction" in LittleVgl
        switch (effect)
        {
        case TransitionEffect::MoveLeft:
                _refreshProvider->SetFullRefresh(FullRefreshProvider::FullRefreshDirections::Right);
                break;
        case TransitionEffect::MoveRight:
                _refreshProvider->SetFullRefresh(FullRefreshProvider::FullRefreshDirections::Left);
                break;
        case TransitionEffect::MoveTop:
                _refreshProvider->SetFullRefresh(FullRefreshProvider::FullRefreshDirections::Up);
                break;
        case TransitionEffect::MoveBottom:
                _refreshProvider->SetFullRefresh(FullRefreshProvider::FullRefreshDirections::Down);
                break;
        default:
                break;
        }

        (void)effect;

        // delete the old screen and create the new one
        if (_currentScreen)
                delete _currentScreen;
        _currentScreen = createScreen(tag);
        if (_currentScreen)
        {
                _currentTag = tag;
                _currentScreen->setCurrentPage(pageNumber);
        }
}


Screen *ScreenGraph::createScreen(ScreenTag tag)
{
        switch (tag)
        {
        case ScreenTag::UtilityWatchFace:
                return new UtilityWatchFace(this, _components);
        case ScreenTag::InfographWatchFace:
                return new InfographWatchFace(this, _components);
        case ScreenTag::BinaryWatchFace:
                return new BinaryWatchFace(this, _components);
        case ScreenTag::FirmwareUpdate:
                return new FirmwareUpdateScreen(this, _components);
        case ScreenTag::FirmwareValidation:
                return new FirmwareValidationScreen(this, _components);
        case ScreenTag::SystemInfo:
                return new SystemInfoScreen(this, _components);
        case ScreenTag::Settings:
                return new SettingsScreen(this, _components);
        case ScreenTag::Brightness:
                return new BrightnessScreen(this, _components);
        case ScreenTag::WakeUpMode:
                return new WakeUpModeScreen(this, _components);
        case ScreenTag::StepsGoal:
                return new StepsGoalScreen(this, _components);
        default:
                return nullptr;
        }
}
