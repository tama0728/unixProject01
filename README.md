# unixProject01
## 숫자 야구 게임
## 1. GTK 설치
   GTK를 사용하려면 먼저 시스템에 설치해야 합니다.

#### Linux (Ubuntu LTS (22.04) 이상 기준)
```bash
sudo apt update
sudo apt install libgtk-4-dev
```
### macOS
Homebrew를 사용해 설치할 수 있습니다:
```bash
brew install gtk4
```

## 2. 빌드 및 실행
### Server 빌드
```bash
cd server
rm -rf build
mkdir build
cd build
cmake ..
make
```

### Client 빌드
```bash
cd client
rm -rf build
mkdir build
cd build
cmake ..
make
```

## 3. 실행
```bash
./server/build/server
```
```bash
./client/build/client
```
