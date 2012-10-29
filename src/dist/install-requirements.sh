#!/bin/sh

echo "This script will install common requirements for Glassomium using the apt-get command in a Debian based system. Note that some packages might need to be installed manually"

wget http://ftp.br.debian.org/debian/pool/main/g/glew/libglew1.7_1.7.0-3_i386.deb
sudo dpkg -i libglew1.7_1.7.0-3_i386.deb
rm libglew1.7_1.7.0-3_i386.deb
sudo apt-get install ruby1.9.1 git-core cmake g++ freeglut3-dev libjpeg-dev libXss-dev libjpeg62 -y
