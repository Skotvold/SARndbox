#include "textureManager.hpp"

SARB::TextureManager::TextureManager() : x(0), y(0)
{

}

void SARB::TextureManager::setX(int& x)
{
    this->x = x;
}

void SARB::TextureManager::setY(int& y)
{
    this->y = y;
}

void SARB::TextureManager::setXY(int& x, int& y)
{
    setX(x);
    setY(y);
}

int SARB::TextureManager::getX()
{
    return this->x;
}

int SARB::TextureManager::getY()
{
    return this->y;
}
