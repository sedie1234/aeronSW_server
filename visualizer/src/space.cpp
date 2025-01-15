#pragma once
#include "space.h"
#include "configs.h"
#include <cmath>  // for sin, cos, M_PI
#include <iostream>
#include <tuple>

void Space::addPoint(const glm::vec3& point) {
    points.push_back(point);
}

void Space::addLine(const glm::vec3& start, const glm::vec3& end,
            const GLfloat R, const GLfloat G, const GLfloat B) {
    lines.emplace_back(start, end, R, G, B);
}

void Space::addObj(const glm::vec3& point, const glm::vec3& xway, 
                const glm::vec3& yway, const glm::vec3& zway){

    lines.emplace_back(point, xway, 1.0f, 0.0f, 0.0f);
    lines.emplace_back(point, yway, 0.0f, 1.0f, 0.0f);
    lines.emplace_back(point, zway, 1.0f, 1.0f, 0.0f);
}

// Helper function to draw a circle
void drawCircle(const glm::vec3& center, float radius, int segments) {
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1.0f, 0.0f, 0.0f);  // 동그라미 색상 (빨간색)
    glVertex3f(center.x, center.y, center.z);  // 중심점

    for (int i = 0; i <= segments; ++i) {
        float theta = 2.0f * M_PI * float(i) / float(segments);  // 각도
        float x = radius * cosf(theta);
        float y = radius * sinf(theta);
        glVertex3f(center.x + x, center.y + y, center.z);
    }
    glEnd();
}

void Space::render() const {
    // Draw circles for points
    float radius = 0.05f;  // 동그라미 크기 설정
    int segments = 30;     // 동그라미 세그먼트 (정확도)
    for (const auto& point : points) {
        drawCircle(point, radius, segments);
    }

    // Draw lines
    glLineWidth(LINE_THICKNESS);  // 선 두께 설정
    glBegin(GL_LINES);
    for (const auto& line : lines) {
        glColor3f(std::get<2>(line), std::get<3>(line), std::get<4>(line));  // 선 색상 (초록색)
        glVertex3f(std::get<0>(line).x, std::get<0>(line).y, std::get<0>(line).z);
        glVertex3f(std::get<1>(line).x, std::get<1>(line).y, std::get<1>(line).z);
    }
    glEnd();
}
