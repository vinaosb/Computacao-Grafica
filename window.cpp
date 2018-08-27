#include <vector>
#include <string>
#include "polygon.cpp"

#define Spacing 25

using namespace std;

class Window {
    public:
    Polygon::point min, max;
    
    Window() {
        min.x = 0;
        min.y = 0;
        max.x = 400;
        max.y = 400;
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
