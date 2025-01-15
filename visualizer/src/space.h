#pragma once

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <cmath>
#include <tuple>

class Space {
public:
    void addPoint(const glm::vec3& point);
    void addLine(const glm::vec3& start, const glm::vec3& end,
                const GLfloat R, const GLfloat G, const GLfloat B);
    void addObj(const glm::vec3& point, const glm::vec3& xway, 
                const glm::vec3& yway, const glm::vec3& zway);
    void render() const;

private:
    std::vector<glm::vec3> points;
    //start point, end point, R, G, B
    std::vector<std::tuple<glm::vec3, glm::vec3, GLfloat, GLfloat, GLfloat>> lines; 
    
};