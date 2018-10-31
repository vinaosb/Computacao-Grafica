#include "polygon.cpp"
#include <algorithm>



class Clipping {
    public:
    
    Clipping(float x1, float x2, float y1, float y2) {
        xmin = x1;
        xmax = x2;
        ymin = y1;
        ymax = y2;
    }
    
    vector<Polygon> clip (vector<Polygon> ppc) {
        vector<Polygon> ret;
        ret.clear();
        for (vector<Polygon>::iterator it = ppc.begin(); it != ppc.end(); ++it) {
            if (it->getType() == "Point") {
                Polygon temp = pointClip(*it);
                cout << "temp_size:" << temp.getPoints().size() << endl;
                if (temp.getPoints().size() > 0)
                    ret.push_back(temp);
            } else if (it->getType() == "Line") {
                Polygon temp = lineClip(*it); 
                if (temp.getPoints().size() > 0)
                    ret.push_back(temp);
            } else if (it->getType() == "Curve"){
                Polygon temp = curveClip(*it); 
                temp.setType(it->getType());
                if (temp.getPoints().size() > 0)
                    ret.push_back(temp);
            }else {
                Polygon temp = polyClip(*it); 
                temp.setType(it->getType());
                if (temp.getPoints().size() > 0)
                    ret.push_back(temp);
            }
        }
        //cout << "size:" << ret[0].getPoints().size() << endl;
        return ret;
    }
    
    
    private:
    float xmin, xmax, ymin, ymax;
    vector<Polygon> listClipped;
    
    Polygon pointClip (Polygon p) {
        float x = p.getPoints().at(0).x;
        float y = p.getPoints().at(0).y;
        float z = p.getPoints().at(0).z;
        Polygon ret;
        cout << x << endl << y << endl << z << endl;
        cout << xmin << endl << ymin << endl << xmax << endl << ymax << endl;
        if ((x > xmin && x < xmax) && (y > ymin && y < ymax)) {
            ret.addPoint(x,y,z);
        }
        cout << "OK" << endl;
        return ret;
    }
    
    Polygon lineClip (Polygon l) {
        Polygon ret;
        ret.clear();
        
        float x1 = l.getPoints().at(0).x,
            x2 = l.getPoints().at(1).x,
            y1 = l.getPoints().at(0).y,
            y2 = l.getPoints().at(1).y;
        
        float dx = l.getPoints().at(1).x - l.getPoints().at(0).x,
            dy = l.getPoints().at(1).y - l.getPoints().at(0).y,
            p[4], q[4];
        
        p[0] = -dx;
        p[1] = dx;
        p[2] = -dy;
        p[3] = dy;
        q[0] = l.getPoints().at(0).x - xmin;
        q[1] = xmax - l.getPoints().at(0).x;
        q[2] = l.getPoints().at(0).y - ymin;
        q[3] = ymax - l.getPoints().at(0).y;
        
        float valor1=0, valor2=1, r, x, y;
        for(int i=0;i<4;i++)
        {
            cout << "p[i]="<<p[1]<<endl;
            if(p[i]==0){
                cout << "entrou igual a 0"<<endl;
                if(q[i] >= 0)
                    continue;
                else{
                    cout << "linha esta completamente fora" << endl;
                    return ret;
                }
            }
            else{
                cout <<"nao entrou"<<endl;
                r = q[i]/p[i];
                if(p[i] < 0){
                    valor1 = std::max(valor1, r);
                }else if(p[i] > 0){
                    valor2 = std::min(valor2, r);
                }
            }   
        }
        x = x1;
        y = y1;
        if (valor1 > valor2){
            cout << "linha esta completamente fora" << endl;
        }
        else{
            if(valor1 != 0){
                x = x1 + valor1 * dx;
                y = y1 + valor1 * dy;
            }
            if(valor2 != 1){
                x2 = x1 + valor2 * dx;
                y2 = y1 + valor2 * dy;
            }
            ret.addPoint(x, y);
            ret.addPoint(x2, y2);
        }
        return ret;
    }
    
    
    Polygon polyClip (Polygon p) {
        Polygon ret, lineTemp;
        ret.clear(), lineTemp.clear();
        cout << "Polygon before clipping"<<endl;
        p.print();
        for (vector<Polygon::point>::iterator it = p.getBeginIterator(); it != p.getEndIterator(); ++it) {
            cout <<"iterator"<<endl;
            if(it != p.getEndIterator()) {
                lineTemp.addPoint(it->x, it->y);
                lineTemp.addPoint((it+1)->x,(it+1)->y);
                cout <<"if"<<endl;
            } else  {
                lineTemp.addPoint(p.getBeginIterator()->x, p.getBeginIterator()->y);
                lineTemp.addPoint(it->x,it->y);
                cout << "else"<<endl;
            }
            lineTemp.print();
            lineTemp = lineClip(lineTemp);
            if(lineTemp.getPoints().size()>0)
                ret.addPoints(lineTemp.getPoints());
            
            lineTemp.clear();
        }
        ret.print();
        // cout<<"polyClip: points added"<<endl;
        // for (vector<Polygon::point>::iterator it = ret.getBeginIterator(); it != ret.getEndIterator(); ++it) {
        //     if (it->x == (it+1)->x and it->y == (it+1)->y) {
        //         ret.erase(it+1);
        //         it--;
        //     }
        // }
        // cout<<"polyClip: erase point"<<endl;
        return ret;
    }
    
    Polygon curveClip(Polygon p){
        Polygon ret, temp;
        ret.clear();temp.clear();
        for (vector<Polygon::point>::iterator it = p.getBeginIterator(); it != p.getEndIterator()-1; ++it) {
            temp.addPoint(it->x, it->y);
            temp.addPoint((it+1)->x, (it+1)->y);
            temp = lineClip(temp);
            ret.addPoints(temp.getPoints());
            temp.clear();
        } 
        return ret;
    }
};
