sudo apt-get install docker
sudo apt install docker.io


sudo docker run --name db -d mongo:3.0 --smallfiles
sudo docker run --name rocketchat -p 8080:3000 --env ROOT_URL=http://localhost --link db -d rocket.chat
