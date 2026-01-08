docker run --rm -v "../":/work -it --entrypoint bash $(docker build -q .) ./scripts/build-toolchained.sh
