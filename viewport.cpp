#include "polygon.cpp"


using namespace std;

class Viewport {
    public:
    Polygon::point min, max;
    int pointWidth = 4;
    
    Viewport() {
        min.x = 0;
        min.y = 0;
        max.x = 400;
        max.y = 400;
    }
};
