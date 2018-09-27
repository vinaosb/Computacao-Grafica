#include "polygon.cpp"
#include <iomanip>

using namespace std;


class Curva2D{
    public:
    Curva2D(){}
    
    Polygon getPointsBezier(Polygon pontos, float param){
        float x,y,t,px[4],py[4], ct[4];
        Polygon ret;
        ret.clear();
		int i = 0;
		for (vector<Polygon::point>::iterator p = pontos.getBeginIterator(); p != pontos.getEndIterator(); p++)
		{

		}
        px[0]=pontos.getPoints().at(0).x;
        px[1]=pontos.getPoints().at(1).x;
        px[2]=pontos.getPoints().at(2).x;
        px[3]=pontos.getPoints().at(3).x;
        py[0]=pontos.getPoints().at(0).y;
        py[1]=pontos.getPoints().at(1).y;
        py[2]=pontos.getPoints().at(2).y;
        py[3]=pontos.getPoints().at(3).y;
        for(t = param; t < 1.0; t = t + param){
            ct[0]=(-pow(t,3) + 3*pow(t,2) -3*t+1);
            ct[1]=(3*pow(t,3) -6*pow(t,2) +3*t);
            ct[2]=(-3*pow(t,3) +3*pow(t,2));
            ct[3]=pow(t,3);
            
            x = px[0]*ct[0] + px[1]*ct[1] + px[3]*ct[2] + px[4]*ct[3];
            y = py[0]*ct[0] + py[1]*ct[1] + py[3]*ct[2] + py[4]*ct[3];
            
            ret.addPoint(x,y);
        }
        return ret;
    }
    
    Polygon getPointsBSpline(Polygon pontos, float t){
        float px[4],py[4],cx1,cx2,cx3,cx4,x,dx,d2x,d3x;
        float cy1,cy2,cy3,cy4,y,dy,d2y,d3y;
        Polygon ret;
        ret.clear();
        px[0]=pontos.getPoints().at(0).x;
        px[1]=pontos.getPoints().at(1).x;
        px[2]=pontos.getPoints().at(2).x;
        px[3]=pontos.getPoints().at(3).x;
        py[0]=pontos.getPoints().at(0).y;
        py[1]=pontos.getPoints().at(1).y;
        py[2]=pontos.getPoints().at(2).y;
        py[3]=pontos.getPoints().at(3).y;
        cout << "px=" << px[0] <<","<<px[1] <<","<<px[2]<<","<<px[3]<<endl;
        //calculo matriz E*M*G para x
        cx1 = -1/6. * px[0] + 1/2. * px[1] -1/2. * px[2] +1/6. * px[3];
        cx2 = 1/2. * px[0] - px[1] + 1/2. * px[2];
        cx3 = -1/2. * px[0]  +1/2. * px[2];
        cx4 = 1/6. * px[0] + 2/3. * px[1] +1/6. * px[2];
        cout << std::fixed << std::setprecision(3) << "cx=["<<cx1<<";"<<cx2<<";"<<cx3<<";"<<cx4<<endl;
        x = cx4;
        dx = pow(t,3)*cx1 + pow(t,2)*cx2 + t*cx3;
        d2x = 6*pow(t,3)*cx1 + 2*pow(t,2)*cx2;
        d3x = 6*pow(t,3)*cx1;
        //calculo matriz E*M*G para y
        cy1 = -1/6. * py[0] + 1/2. * py[1] -1/2. * py[2] +1/6. * py[3];
        cy2 = 1/2. * py[0] -1 * py[1] +1/2. * py[2] ;
        cy3 = -1/2. * py[0]  +1/2. * py[2];
        cy4 = 1/6. * py[0] + 2/3. * py[1] +1/6. * py[2];
        cout << std::fixed << std::setprecision(3) << "cy=["<<cy1<<";"<<cy2<<";"<<cy3<<";"<<cy4<<endl;
        y = cy4;
        dy = pow(t,3)*cy1 + pow(t,2)*cy2 + t*cy3;
        d2y = 6*pow(t,3)*cy1 + 2*pow(t,2)*cy2;
        d3y = 6*pow(t,3)*cy1;
        
        int n=1/t;
        ret.addPoint(x,y);
        for (int i=1; i < n; i++){
            x = x + dx;
            dx = dx + d2x;
            d2x = d2x + d3x;
            y = y + dy;
            dy = dy + d2y;
            d2y = d2y + d3y;
            ret.addPoint(x,y);
        }
        
        
        return ret;
    }
    
    
    
};
