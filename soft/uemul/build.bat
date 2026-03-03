pip install opencv-python
pip install filelock
md bin
cd bin
cmake ..
cmake --build . --config Release
cd ..
copy lcd\lcd.py ..\Release\lcd.py
