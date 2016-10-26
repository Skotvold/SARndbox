#pragma once


namespace SARB{

    class TextureManager
    {
    public:
        TextureManager();
        void setX(int& x);
        void setY(int& y);
        void setXY(int& x, int& y);

        int getX();
        int getY();

    private:
        int x;      // Position of x
        int y;      // Position of y

    };
}
