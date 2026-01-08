./setup.sh
docker run --rm -v "../":/work -t --entrypoint bash factory-ds:latest ./scripts/build-toolchained.sh
