for /f "delims=" %%i in ('docker build -q .') do set output=%%i
docker run --rm -v "../":/work -it --entrypoint bash %output% ./scripts/build-toolchained.sh