#ifndef FULLREFRESHPROVIDER_H
#define FULLREFRESHPROVIDER_H

class FullRefreshProvider {
public:

  enum class FullRefreshDirections { None, Up, Down, Left, Right, LeftAnim, RightAnim };

  virtual ~FullRefreshProvider() { }

  virtual void SetFullRefresh(FullRefreshDirections direction) = 0;
};

#endif // FULLREFRESHPROVIDER_H