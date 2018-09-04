#ifndef POLYGON_CPP
#define POLYGON_CPP
#include <vector>
#include <iostream>
#include <string>
#include <math.h>

#define PI 3.14159265

using namespace std;

class Polygon {
    public:
    
    typedef struct {
        float x = 0;
        float y = 0;
        float z = 0;
    } point;
    
    
    Polygon () {
        type = "Point";
        points.clear();
    }
    
    
    Polygon (string n) {
        name = n;
        type = "Point";
        points.clear();
    }
    
    
    Polygon (string n,float x, float y) {
        name = n;
        type = "Point";
        points.clear();
        addPoint(x,y);
    }	 	  	 	    	   	      	   	 	    	    	 	

    
    point findCenter () {
        int c = 0;
        float auxx=0,auxy=0;
        
        for (vector<point>::iterator it = points.begin(); it != points.end(); ++it) {
            auxx += it->x;
            auxy += it->y;
            c++;
        }
        point r;
        r.x = auxx/c;
        r.y = auxy/c;
        return r;
    }
    
    void rotation (float a) {
        a = a*PI/180;
        float auxx, auxy;
        for (vector<point>::iterator it = points.begin(); it != points.end(); ++it) {
            auxx = it->x * cos ( a ) + it->y * sin (a);
            auxy = it->y * cos ( a ) - it->x * sin (a);
            it->x = auxx;
            it->y = auxy;
        }
    }
    
    void translation (float dx, float dy) {
        for (vector<point>::iterator it = points.begin(); it != points.end(); ++it) {
            it->x += dx;
            it->y += dy;
        }
    }
    
    void translation (point p) {	 	  	 	    	   	      	   	 	    	    	 	
        float dx = p.x, dy = p.y;
        for (vector<point>::iterator it = points.begin(); it != points.end(); ++it) {
            it->x += dx;
            it->y += dy;
        }
    }
    
    void scaling (float e) {
        point center = findCenter();
        point auxc;
        auxc.x = -center.x;
        auxc.y = -center.y;
        translation (auxc);
        
        
        for (vector<point>::iterator it = points.begin(); it != points.end(); ++it) {
            it->x = it->x * e;
            it->y = it->y * e;
        }
        
        translation(center);
    }
    
    vector<point> getPoints() {
        return points;
    }
    
    vector<point>::iterator getBeginIterator() {
        return points.begin();
    }
    vector<point>::iterator getEndIterator() {
        return points.end();
    }
    
    void addPoint(float x, float y, float z = 0) {
        point aux;
        aux.x = x;
        aux.y = y;
        aux.z = z;
        
        points.push_back(aux);
    }	 	  	 	    	   	      	   	 	    	    	 	
    
    void clear() {
        points.clear();
    }
    
    string getName() {
        return name;
    }
    
    void setName (string s) {
        name = s;
    }
    
    void setType (string s) {
        type = s;
    }
    string getType () {
        return type;
    }
    
    void print() {
        cout << name << endl << type << endl;
        for (vector<point>::iterator it = points.begin(); it != points.end(); ++it)
            cout << "X " << it->x << endl << "Y " << it->y << endl << endl; 
    }
    
    private:
    vector<point> points;
    string name,type;
};

#endif	 	  	 	    	   	      	   	 	    	    	 	
