#!/bin/sh

echo "Starting Glassomium UI Server..."
killall "ruby" -v
cd server
ruby main.rb &
echo "Starting Glassomium..."
cd ../bin
./Glassomium
