#ifndef DEFAULTSCREENGRAPH_H
#define DEFAULTSCREENGRAPH_H


#include "ScreenGraph.h"

class ComponentContainer;



class DefaultScreenGraph : public ScreenGraph
{
public:

        DefaultScreenGraph(FullRefreshProvider *refreshProvider, ComponentContainer *components);

protected:

        ScreenTag watchFaceScreenTagByIndex(uint8_t watchFaceIndex) override;
};

#endif // DEFAULTSCREENGRAPH_H
