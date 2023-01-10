#include <Gardien.h>
#include <Labyrinthe.h>
#include <math.h>
#include <iostream>
using namespace std;

struct vector
{
    float x,y;

    int angle()
    {
        return 180*(atan2(y,x))/M_PI;
    }

    float norm()
    {
        return sqrt(x*x + y*y);
    }
};

bool check_collision(char* _data, float x1, float y1, float x2, float y2)
{
    float dx = abs(x2 - x1);
    float sx;
    if(x1 < x2)
    {
        sx = 1;
    }else{
        sx = -1;
    }

    float dy = abs(y2 - y1);
    float sy;

    float error = dx + dy;

    if(y1 < y2)
    {
        sy = 1;
    }else{
        sy = -1;
    }

    while(true)
    {
        if(x1 == x2 && y1 == y2)
        {
            return true;
        }
        float e2 = 2*error;
        if(e2 >= dy)
        {
            if(x1 == x2 && y1 == y2)
            {
                return true;
            }
            error = error + dy;
            x1 = x1 + sx;
        }
        if (e2 <= dx)
        {
            if (y1 == y2)
            {
                return true;
            }
            error = error + dx;
            y1 = y1 + sy;
        }

        if(_data[(int)x1][(int)y1] == 1)
        {
            return false;
        }
    }
}

void Gardien::update()
{

    float x = _x/_l->scale;
    float y = _y/_l->scale;

    float angle_rad = 180*_angle/M_PI;
    vector look = vector{cos(angle_rad),sin(angle_rad)};

    if(((Labyrinthe*)_l)->_data[x+look.x][y+look.y] == 0)
    {
        _angle += 15;

    }else{
        _x = (x+look.x)*_l->scale;
        _y = (y+look.y)*_l->scale;
    }


}