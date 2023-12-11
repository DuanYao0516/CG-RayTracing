#ifndef READOBJ_H
#define READOBJ_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "vec3.h" 

class ReadOBJ {
public:
    //外部的faces被更改，存入读取的值
    static bool loadOBJ(const std::string & filename, std::vector<point3>&vertices, std::vector<vec3>&normals, std::vector<vec3>&texCoords, std::vector<std::vector<int>>&faces) {

        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filename << std::endl;
            return false;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string type;
            iss >> type;

            if (type == "v") {
                point3 vertex;
                iss >> vertex[0] >> vertex[1] >> vertex[2];
                vertices.push_back(vertex);
            }
            else if (type == "vn") {
                vec3 normal;
                iss >> normal[0] >> normal[1] >> normal[2];
                normals.push_back(normal);
            }
            else if (type == "vt") {
                vec3 texCoord;
                iss >> texCoord[0] >> texCoord[1];
                texCoords.push_back(texCoord);
            }
            else if (type == "f") {
                std::vector<int> face;
                std::string vertexStr;
                while (iss >> vertexStr) {
                    std::istringstream vss(vertexStr);
                    std::string vertexIndexStr;
                    std::getline(vss, vertexIndexStr, '/');
                    int vertexIndex = std::stoi(vertexIndexStr) - 1; // Convert 1-based indices to 0-based
                    face.push_back(vertexIndex);
                }
                faces.push_back(face);
            }
        }
        file.close();

        return true;
    }
};

#endif
