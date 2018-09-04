#include "polygon.cpp"


using namespace std;

class Viewport {
    public:
    Polygon::point min, max;
    int pointWidth = 4;
    
    Viewport() {
        min.x = 10;
        min.y = 10;
        max.x = 390;
        max.y = 390;
    }
};
