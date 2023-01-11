#include "Gardien.h"
#include "Labyrinthe.h"
#include <math.h>
#include <iostream>
using namespace std;

struct vector
{
    float x,y;
    
    void init_vector(float x1,float y1, float x2, float y2)
    {
        x = x2 - x1;
        y = y2 - y1;
    }

    float norm()
    {
        return sqrt(x*x + y*y);
    }

    int angle_to(vector v)
    {
        return 180*(atan2(v.y,v.x) - atan2(y,x))/M_PI;
    }

    int angle()
    {
        return 180*(atan2(y,x))/M_PI + 180;
    }
};



bool check_collision(char** _data, float x1, float y1, float x2, float y2)
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

bool Gardien::move(double dx, double dy)
{
    

    if(((Labyrinthe*)_l)->data((_x+dx)/Environnement::scale,(_y+dy)/Environnement::scale) != EMPTY)
    {
    
        return false;

    }else{
        _x = (_x+dx);
        _y = (_y+dy);
        return true;
    }
}

void Gardien::update()
{
    float p_x = (_l->_guards[0])->_x;///_l->scale;
    float p_y = (_l->_guards[0])->_y;///_l->scale;

    float x = _x;///_l->scale;
    float y = _y;///_l->scale;

    float _angle_rad = 180*_angle/M_PI;

    vector dist{};
    dist.init_vector(x,y,p_x,p_y);

    // cout<<"------------------------------------------------------------------------"
    //     <<endl;
        
    //     cout<<"scale = "<<_l->scale<<endl;
    //     cout<<"pos monstre = ("<<x<<","<<y<<") | pos joueur = ("<<p_x<<","<<p_y<<")"<<endl;
    //     cout<<"dist: "<<dist.x<<";"<<dist.y<<" | angle = "<<dist.angle()<<endl;
    //     cout<<"look: "<<look.x<<";"<<look.y<<" | angle = "<<look.angle()<<endl;
            


    // cout<<"angle actuel: "<<_angle
    //     <<" | angle futur: "<<dist.angle()<<endl;
    // cout<<"------------------------------------------------------------------------"
    //     <<endl;

    if(dist.norm() <= FOV)
        _angle_cible = dist.angle();

    if(_angle_cible != _angle)
        _angle += (_angle_cible-_angle)*_vitesse_rotation/100;

    if(!move(cos(_angle_rad),sin(_angle_rad)))
    {
        _angle += (rand()%5)-5;
    }

}