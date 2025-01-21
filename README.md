# 0. aeronSW-server 개요
 - 항공전자 서버용 코드 모음
 - ubuntu 20.04 환경
 - cmake 3.26

# 1. aeronSW_visualizer
 - 데모 + 디버깅을 위한 visualizer, opengl로 개발
 - pcap parsing을 위한 third party

## Requirements

1. install glut, glfw, glew, glm
```
$ sudo apt install freeglut3-dev libglu1-mesa-dev mesa-common-dev
$ sudo apt install mesa-utils
$ sudo apt install libglew-dev
$ sudo apt isntall libglfw3-dev libglfw3
$ sudo apt install libglm-dev
```

2. install third_party
 - requirements for ouster : jsoncpp, curl, spd, pcap, libtin, flatbuffers, optional-lite
```
$ sudo apt install libjsoncpp-dev
$ sudo apt install libcurl4-openssl-dev
$ sudo apt install libspdlog-dev
$ sudo apt install libpcap-dev
$ sudo apt install libtins-dev
$ sudo apt install flatbuffers-compiler libflatbuffers-dev
```
 - optional lite
```
$ cd third_party
$ git clone https://github.com/martinmoene/optional-lite.git
$ cd optional-lite
$ mkdir build && cd build
$ cmake ..
$ make
```
 - ouster sdk 
```
$ cd third_party
$ git clone https://github.com/ouster-lidar/ouster_example.git
$ cd ouster_example
$ mkdir build && cd build
$ cmake ..
$ make
$ sudo make install
```
## Build

```
$ cd visualizer
$ mkdir build && cd build
$ cmake ..
$ make
```

## Run

```
$ ./aeronSW_visualizer [option]
```
### Options
1. -h : help
2. -d : data file type : -d [option]</p>
   2.1 -d -p : pcap data : -d -p [pcap file path] [metadata file path]
   ```
   ex)
   $ ./aeronSW_visualizer -d -p \
   $ ../../data/testData/sample_data.pcap \
   $ ../../data/testData/sample_data.json
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
2. 마우스 드래그로 화면회전
3. 키보드 wasd 화면 이동
4. 키보드 q,e 이전 프레임, 다음 프레임
5. p 정지/시작
6. page up / down : 비디오 속도 증가 / 감소