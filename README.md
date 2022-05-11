# Seam Carving C++ Final Project

## About
### Project Description and Proposal can be found [here](https://docs.google.com/document/d/1sUdq5QHb0U0yQCnDj7lcP6z-H4K9Jmua2MpBQdVeuwc)
### Demonstration Video (In Icelandic) can be found [here](https://www.youtube.com/watch?v=aDHIt2jeR0U)

## Compiling and running
Since the OpenCV library has quite complicated requirements I have created a Dockerfile which allows you to run the program as a docker container and not have to worry about installing anything (except docker of course).

To build and run the docker image, use the following commands:

    docker build -t gf-image .
    docker run -d -v {absolute path to current directory}/results:/main/results --name=gf-cont gf-image tail -f /dev/null

Now we a have running docker container which contains an environment we can use our program in. The
program uses the results directory to fetch and save images, so you place the images you want to use
in there and check the results in there as well.

To enter the docker container use the command:

    docker exec -it gf-cont /bin/bash

Once inside the docker container you can run the program:

    ./gf results/{input filename} results/{output filename} {optional arguments}

To exit the continer, simply close the bash session:

    exit

To stop and remove the running container:

    docker rm -f gf-cont

To delete the image:

    docker rmi -f gf-image

You may want to delete the archlinux image as well:

    docker rmi -f archlinux

## Using the program
The expects the following runtime arguments:

    <> = required
    [] = optional
    a  = argument being discussed
    ./gf <infile> <outfile> [xEndScale] [yEndScale] [xStartScale] [yStartScale] [loops] [dupes] [xResize] [yResize]
    infile: The filename of the input image
    outfile: The filename of the output image
    xEndScale: The scale of the x axis in the final frame of the image, 0<a<=1, default 1.
    yEndScale: The scale of the y axis in the final frame of the image, 0<a<=1, default 1.
    xStartScale: The scale of the x axis in the first frame of the final image, 0<a<=1, default 1.
    yStartScale: The scale of the y axis in the first frame of the final image, 0<a<=1, deafult 1.
    loops: How often to loop the input image/video while seam carving, 1<=a, default 1
    dupes: How often to duplicate each frame in the input image/video while seam carving, 1<=a, default 1.
    xResize: The scale to resize the x axis of the input image by, 0<a, default 1.
    yResize: The scale to resize the y axis of the input image by, 0<a, default 1.
