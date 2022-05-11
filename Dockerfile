FROM archlinux:latest
RUN pacman -Syyu --noconfirm
RUN pacman-db-upgrade
RUN pacman -S pkgconf --noconfirm
RUN pacman -S gcc --noconfirm
RUN pacman -S qt5-base --noconfirm
RUN pacman -S jsoncpp --noconfirm
RUN pacman -S glew --noconfirm
RUN pacman -S openmpi --noconfirm
RUN pacman -S fmt --noconfirm
RUN pacman -S opencv --noconfirm
RUN pacman -S hdf5 --noconfirm
RUN pacman -S vtk --noconfirm
WORKDIR /main
COPY . .
# RUN g++ -o gf gf.cpp -Ofast
# CMD ["./gf"]
RUN g++ -std=c++17 -Ofast -o gf *.cpp egif/GifEncoder.cpp egif/*/*.cpp `pkg-config --cflags --libs /usr/lib/pkgconfig/opencv4.pc`
#CMD ["./gf", "test.png", "test2.png", "0.5", "0.5", "1", "1", "1", "25", "2", "1"]
CMD ["echo", "Gf container running"]

