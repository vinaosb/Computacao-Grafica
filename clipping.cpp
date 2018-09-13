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
                ret.push_back(temp);
            } else {
                Polygon temp = polyClip(*it); 
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
        
        for(int i=0;i<4;i++)
        {
            if(p[i]==0)
            {
                cout<<"line is parallel to one of the clipping boundary";
                if(q[i]>=0)
                {
                    if(i<2)
                    {
                        if(y1<ymin)
                        {
                            y1=ymin;
                        }
                    
                        if(y2>ymax)
                        {
                            y2=ymax;
                        }
                    
                        ret.addPoint(x1,y1);
                        ret.addPoint(x2,y2);
                    }
                    
                    if(i>1)
                    {
                        if(x1<xmin)
                        {
                            x1=xmin;
                        }
                        
                        if(x2>xmax)
                        {
                            x2=xmax;
                        }
                        
                        ret.addPoint(x1,y1);
                        ret.addPoint(x2,y2);
                        return ret;
                    }
                }
            }
        }
        
        float t1 = 0, t2 = 1, temp = 0;
        
        for(int i=0;i<4;i++)
        {
            temp=q[i]/p[i];
            
            if(p[i]<0)
            {
                if(t1<=temp)
                    t1=temp;
            }
            else
            {
                if(t2>temp)
                    t2=temp;
            }
        }
        
        float xx1,xx2,yy1,yy2;
        
        if(t1<t2)
        {
            xx1 = x1 + t1 * p[1];
            xx2 = x1 + t2 * p[1];
            yy1 = y1 + t1 * p[3];
            yy2 = y1 + t2 * p[3];
            ret.addPoint(xx1,yy1);
            ret.addPoint(xx2,yy2);
        }
    
        
        return ret;
    }
    
    
    Polygon polyClip (Polygon p) {
        Polygon ret, lineTemp;
        ret.clear(), lineTemp.clear();
        
        for (vector<Polygon::point>::iterator it = p.getBeginIterator(); it != p.getEndIterator(); ++it) {
            if(it != p.getEndIterator()) {
                lineTemp.addPoint(it->x, it->y);
                lineTemp.addPoint((it+1)->x,(it+1)->y);
            } else  {
                lineTemp.addPoint(p.getBeginIterator()->x, p.getBeginIterator()->y);
                lineTemp.addPoint(it->x,it->y);
            }
            lineTemp = lineClip(lineTemp);
            ret.addPoints(lineTemp.getPoints());
            
            lineTemp.clear();
        }
        
        for (vector<Polygon::point>::iterator it = ret.getBeginIterator(); it != ret.getEndIterator()-1; ++it) {
            if (it->x == (it+1)->x and it->y == (it+1)->y) {
                ret.erase(it+1);
                it--;
            }
        }
        
        return ret;
    }
};
