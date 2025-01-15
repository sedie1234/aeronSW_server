# 0. aeronSW-server 개요
 - 항공전자 서버용 코드 모음
 - ubuntu 20.04 환경

# 1. aeronSW_visualizer
 - 데모 + 디버깅을 위한 visualizer, opengl로 개발

## Requirements

```
$ sudo apt install freeglut3-dev libglu1-mesa-dev mesa-common-dev
$ sudo apt install mesa-utils
$ sudo apt install libglew-dev
$ sudo apt isntall libglfw3-dev libglfw3
$ sudo apt install libglm-dev
```

## Build

```
$ cd visualizer
$ mkdir build && cd build
$ cmake ..
$ make
$ ./aeronSW_visualizer
```

## Work

1. main의  scene 부분에 detecting되는 정보들을 점이나 선으로 등록
2. 메인좌표는 (0,0,0)으로 고정
3. 메인보드 및 서브보드는 addObj를 통해 각자의 좌표계가 보이도록 할 것 </p>
   -> 필요할 때만 켜서 볼 수 있도록 설정, 후에 사용 안할 수도 있음
4. 객체가 탐지되는 모양은 별개의 모양으로 생성
5. 시중에 모델링 된 데이터를 인용해 가시성을 높히기
6. 선택에 따라 각 보드 별 시점 변경 기능

## 동작
1. 마우스 휠로 확대축소 (메인보드 위치와 카메라 사이 거리 조절)
2. 드래그로 가로축, 세로축으로 카메라 이동