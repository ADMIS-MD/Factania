call setup.bat
docker run -d -v "../":/work -it --entrypoint bash --name persist-ds factory-ds:latest 