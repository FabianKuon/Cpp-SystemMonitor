FROM docker-public.docker.devstack.vwgroup.com/ubuntu:22.04

RUN apt update && apt install -y 
RUN apt install -y clang make cmake vim build-essential gcc git openssh-server libncurses5-dev libncursesw5-dev

CMD ["bash"]
