cd ../
pyinstaller --onefile --add-data "../lib/ditherlib.so:./build" --specpath ./build ./src/dthr.py --name dthr.exe
