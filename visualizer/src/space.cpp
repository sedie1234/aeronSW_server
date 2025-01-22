#pragma once
#include "space.h"
#include "configs.h"
#include <cmath>  // for sin, cos, M_PI
#include <iostream>
#include <tuple>

extern float orbitRadius;

void Space::addPoint(const glm::vec3& point) {
    std::pair<glm::vec3, glm::vec3> _point;
    _point.first = point;
    _point.second = glm::vec3(1.0f, 0.0f, 0.0f); //default = red
    points.push_back(_point);
}

void Space::addPoint(const glm::vec3& point, const glm::vec3& color){
    std::pair<glm::vec3, glm::vec3> _point;
    _point.first = point;
    _point.second = color;
    points.push_back(_point);
}

void Space::clearPoints(){
    points.clear();
    std::vector <std::pair<glm::vec3, glm::vec3>>().swap(points);
}

void Space::addLine(const glm::vec3& start, const glm::vec3& end,
            const GLfloat R, const GLfloat G, const GLfloat B) {
    lines.emplace_back(start, end, R, G, B);
}

void Space::clearLines(){
    lines.clear();
    std::vector <std::tuple<glm::vec3, glm::vec3, GLfloat, GLfloat, GLfloat>>().swap(lines);
}

void Space::addObj(const glm::vec3& point, const glm::vec3& xway, 
                const glm::vec3& yway, const glm::vec3& zway){

    lines.emplace_back(point, point + xway, 1.0f, 0.0f, 0.0f);
    lines.emplace_back(point, point + yway, 0.0f, 1.0f, 0.0f);
    lines.emplace_back(point, point + zway, 1.0f, 1.0f, 0.0f);
}

// Helper function to draw a circle
void drawCircle(const glm::vec3& center, float radius, int segments, const glm::vec3& color) {
    glBegin(GL_TRIANGLE_FAN);
    glColor3f((GLfloat)color.x, (GLfloat)color.y, (GLfloat)color.z);  // 동그라미 색상 (빨간색)

    glVertex3f(center.x, center.y, center.z);  // 중심점

    for (int i = 0; i <= segments; ++i) {
        float theta = 2.0f * M_PI * float(i) / float(segments);  // 각도
        float x = radius * cosf(theta);
        float y = radius * sinf(theta);
        glVertex3f(center.x + x, center.y + y, center.z);
    }
    glEnd();
}

void Space::drawGrid(){
    std::vector<std::pair<glm::vec3, glm::vec3>> gridpoints;

    for(int i=-GRID_NUM; i<GRID_NUM+1; i++){
        std::pair<glm::vec3, glm::vec3> line0_points;
        line0_points.first = glm::vec3(GRID_COEFFI*orbitRadius*i/GRID_NUM, -GRID_COEFFI*orbitRadius, GRID_Z_OFFSET);
        line0_points.second = glm::vec3(GRID_COEFFI*orbitRadius*i/GRID_NUM, GRID_COEFFI*orbitRadius, GRID_Z_OFFSET);
        gridpoints.push_back(line0_points);

        std::pair<glm::vec3, glm::vec3> line1_points;
        line1_points.first = glm::vec3(-GRID_COEFFI*orbitRadius, GRID_COEFFI*orbitRadius*i/GRID_NUM, GRID_Z_OFFSET);
        line1_points.second = glm::vec3(GRID_COEFFI*orbitRadius, GRID_COEFFI*orbitRadius*i/GRID_NUM, GRID_Z_OFFSET);
        gridpoints.push_back(line1_points);
    }

    for(const auto &points : gridpoints){
        addLine(points.first, points.second, 0.6f, 0.6f, 0.6f); //grid:: blue line
    }
}

void Space::render() const {
    // Draw circles for points
    float radius = 0.05f;  // 동그라미 크기 설정
    int segments = 30;     // 동그라미 세그먼트 (정확도)
    for (const auto& point : points) {
        drawCircle(point.first, radius, segments, point.second);
    }

    // Draw lines
    glLineWidth(LINE_THICKNESS);  // 선 두께 설정
    glBegin(GL_LINES);
    for (const auto& line : lines) {
        glColor3f(std::get<2>(line), std::get<3>(line), std::get<4>(line));  // 선 색상
        glVertex3f(std::get<0>(line).x, std::get<0>(line).y, std::get<0>(line).z);
        glVertex3f(std::get<1>(line).x, std::get<1>(line).y, std::get<1>(line).z);
    }
    glEnd();
}
