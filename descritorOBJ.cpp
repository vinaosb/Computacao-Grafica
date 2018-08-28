#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include "polygon.cpp"

using namespace std;

class DescritorOBJ {
    public:
    
    DescritorOBJ (string fn) {
        filename = fn;
    }
    
    void saver(Polygon pol) {
        fstream file(filename, ios::ate);
        
        if (file.is_open()) {
            file << pol.getName();
            cout << pol.getName();
            
            for (vector<Polygon::point>::iterator it = pol.getBeginIterator(); it != pol.getEndIterator(); ++it) {
                file << "v " << it->x << " " << it->y << " " << it->z << endl;
                cout << "v " << it->x << " " << it->y << " " << it->z << endl;
            }
            
            file.close();
        }
        
    }
    
    void clearData() {
        fstream file(filename, ios::trunc);
        
        file.close();
    }
    
    vector<Polygon> loader() {
        fstream file(filename);
        string line;
        vector<string> tokenVec;
        vector<Polygon> ret;
        ret.clear();
        int i,j,k;
        bool test = false;
        
        Polygon pol;
        
        if (file.is_open()) {
            while (getline(file,line)) {
                tokenVec = tokenizer(line);
                string name(line,1,string::npos);
                
                if(tokenVec.at(0).compare("#") == 0) {
                    if (test) {
                        if (pol.getType().compare("Point") == 0) {
                            if (pol.getPoints().size() == 2)
                                pol.setType("Line");
                            if (pol.getPoints().size() == 3)
                                pol.setType("Polygon");
                        }
                        ret.push_back(pol);
                    }
                    test = true;
                    pol.clear();
                    pol.setName(name);
                } else {
                    if (tokenVec.at(0).compare("v") == 0) {
                        float z = 0;
                        if (tokenVec.size() > 3)
                            z = stof(tokenVec.at(3));
                        pol.addPoint(stof(tokenVec.at(1)),stof(tokenVec.at(2)),z);
                    }
                }
            }
            ret.push_back(pol);
            file.close();
        } else {
            cout << "Error loading file" << endl;
        }
        
        return ret;
    }
    
    private:
    string filename;
    
    vector<string> tokenizer(string s) {
        char* token;
        char* temp = new char[s.length() +1];
        strcpy(temp,s.c_str());
        vector<string> vec;
        vec.clear();
        
        token = strtok(temp," ,.-");
        while (token != 0) {
            string tok (token);
            vec.push_back(tok);
            token = strtok(NULL," ,.-");
        }
        
        return vec;
    }
};
