# Set-Up guide for this repo
This project is build in a dev-container based on an ubuntu image. Therefore docker desktop is requiered to follow this manual

## Docker Set-Up
Run the following command in the root directory of this project
```bash
docker compose up --build
```
Afterwards a IDE (like VSCode) can be attached to the running container.

## Execute System Monitor

### 1. Remove old build files
```bash
make clean
```

### 2. Build project
```bash
make build
```

### 3. Run executable to see system monitor
```bash
/cpp-project/build/monitor
```