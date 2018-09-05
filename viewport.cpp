#include "polygon.cpp"

#define Spacing 25 //tem q ser igual ao Spacing da window

using namespace std;

class Viewport {
    public:
    Polygon::point min, max;
    int pointWidth = 2;
    
    Viewport() {
        min.x = 10;
        min.y = 10;
        max.x = 390;
        max.y = 390;
    }
    void reset() { //deve ter um jeito melhor
        min.x = 10;
        min.y = 10;
        max.x = 390;
        max.y = 390;
    }
    void zoom(bool z) {
        if (!z)  {
            max.x = max.x + Spacing;
            min.x = min.x - Spacing;
            max.y = max.y + Spacing;
            min.y = min.y - Spacing;
        }
        else {
            max.x = max.x - Spacing;
            min.x = min.x + Spacing;
            max.y = max.y - Spacing;
            min.y = min.y + Spacing;
        }
    }
    
    void moveUp(){
        //move(0,-Spacing);
        min.y = min.y + Spacing;
        max.y = max.y + Spacing;
    }
    void moveLeft (){
        //move(Spacing,0);
        min.x = min.x - Spacing;
        max.x = max.x - Spacing;
    }
    
    void moveRight (){
        //move(-Spacing,0);
        min.x = min.x + Spacing;
        max.x = max.x + Spacing;
    }
    
    void moveDown (){
        //move(0,Spacing);
        min.y = min.y - Spacing;
        max.y = max.y - Spacing;
    }
};
