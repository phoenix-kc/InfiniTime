#ifndef SCREENGRAPH_H
#define SCREENGRAPH_H


#include <cstdint>
#include <vector>

#include "Screen.h"

class FullRefreshProvider;
class ComponentContainer;



class ScreenGraph
{
public:

        enum class ScreenTag
        {
                None,
                Previous,
                DefaultWatchFace,
                UtilityWatchFace,
                InfographWatchFace,
                BinaryWatchFace,
                FirmwareUpdate,
                FirmwareValidation,
                SystemInfo,
                Settings,
                Brightness,
                WakeUpMode,
                StepsGoal
        };

        enum class TransitionTrigger : uint8_t
        {
                Button,
                Swipe,
                Tap,
                LongTap,
                DoubleTap,
                Inactivity
        };

        enum class TransitionEffect : uint8_t
        {
                None,
                MoveLeft,
                MoveRight,
                MoveTop,
                MoveBottom
        };

        Screen *currentScreen() const { return _currentScreen; }

        ScreenGraph(FullRefreshProvider *refreshProvider, ComponentContainer *components);
        virtual ~ScreenGraph();

        void addButtonTransition(ScreenTag fromScreen, ScreenTag toScreen, TransitionEffect effect = TransitionEffect::None);
        void addButtonTransition(ScreenTag fromScreen, ScreenTag toScreen, uint8_t toPageNumber, TransitionEffect effect = TransitionEffect::None);
        void addSwipeTransition(ScreenTag fromScreen, ScreenTag toScreen, Screen::SwipeDirection direction);
        void addSwipeTransition(ScreenTag fromScreen, ScreenTag toScreen, uint8_t toPageNumber, Screen::SwipeDirection direction);
        void addInactivityTransition(ScreenTag fromScreen, ScreenTag toScreen, TransitionEffect effect = TransitionEffect::None);
        void addInactivityTransition(ScreenTag fromScreen, ScreenTag toScreen, uint8_t toPageNumber, TransitionEffect effect = TransitionEffect::None);

        void activateScreen(ScreenTag tag, TransitionEffect effect = TransitionEffect::None);
        void activateScreen(ScreenTag tag, uint8_t pageNumber, TransitionEffect effect = TransitionEffect::None);

        void setDefaultWatchFace(ScreenTag tag);

        virtual void handleRefresh();
        virtual bool handleButtonPress();
        virtual bool handleTap();
        virtual bool handleLongTap();
        virtual bool handleDoubleTap();
        virtual bool handleSwipe(Screen::SwipeDirection direction);
        virtual bool handleTouch(int x, int y);

protected:

        struct Transition
        {
        public:
                TransitionTrigger trigger;
                Screen::SwipeDirection swipeDirection;
                ScreenTag fromScreen;
                ScreenTag toScreen;
                uint8_t toPageNumber;
                TransitionEffect effect;
        };

        struct ScreenInfo
        {
        public:
                ScreenTag tag;
                uint8_t pageNumber;
        };

        FullRefreshProvider *_refreshProvider;
        ComponentContainer *_components;
        ScreenTag _currentTag;
        ScreenTag _nextTag;
        uint8_t _nextPageNumber;
        TransitionEffect _nextEffect;
        Screen *_currentScreen;
        std::vector<Transition> _transitions;
        uint32_t _lastButtonPressTicks;
        std::vector<ScreenInfo> _previousScreens;

        virtual ScreenTag watchFaceScreenTagByIndex(uint8_t watchFaceIndex) = 0;

        void switchScreen(ScreenTag tag, uint8_t pageNumber, TransitionEffect effect);
        Screen *createScreen(ScreenTag tag);
};

#endif // SCREENGRAPH_H
