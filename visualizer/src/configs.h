#pragma once

//INIT
#define INIT_CAM_RADIUS     10.0f   //초기 카메라 위치 : 중심으로부터 거리
#define INIT_CAM_VANGLE     -15.0f   //초기 카메라 위치 : 수직각도
#define INIT_CAM_HANGLE     -15.0f   //초기 카메라 위치 : 수평각도

//window
#define WINDOW_WIDTH    1200        //창 너비
#define WINDOW_HEIGHT   800         //창 높이

// Constants
#define ORBIT_SPEED         0.01f   // 궤도 회전 속도
#define TRANSLATION_SPEED   0.5f    // 이동 속도
#define ZOOM_SPEED          1.0f    // 줌 속도
#define LINE_THICKNESS      2.0f    // 선 두께
#define CAMERA_SPEED        0.1f

// Scene set
#define GRID_NUM        16
#define GRID_Z_OFFSET   -0.01f
#define GRID_COEFFI     2