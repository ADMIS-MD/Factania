call setup.bat
docker run --rm -v "../":/work -it --entrypoint bash factory-ds:latest ./scripts/build-toolchained.sh
