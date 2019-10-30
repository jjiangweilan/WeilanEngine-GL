#pragma once
#include <string>
namespace wlEngine
{
class NpcController;
class Entity;
class UserInterface
{
public:
  static Entity *showDialog(NpcController *);
  static void showInfoBoard();
  static void showSettings();
  static void switchToUIMode();
};
} // namespace wlEngine