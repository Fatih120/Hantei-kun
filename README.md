# Hantei-chan #
Hantei-chan is a .HA6 moveset editor. This fork focuses on editing Under Night In-Birth, adding much more clarity and pop-ups until source code of better forks (like tabs) is released.

-----------------------
## Hotkeys and stuff ##

Left click and drag in the display to scroll the view.
Drag right click to draw a box.
You can switch frames/patterns with the arrow keys.
You can switch the currently selected type of box with Z and X.
Don't try to press these hotkeys while you're drawing a box. You'll just make a mess.
You can view the all the shortcuts by going into Help->Shortcuts

-----------------------
## How to build ##
Building is DEBATABLY simple, and it works with both MSVC and Mingw-w64.
Clone the git, init the submodules and run cmake.
Assuming you're using ninja:

```
git clone --recursive https://github.com/Fatih120/Hantei-chan.git hanteichan 
cd hanteichan
mkdir build
cmake -S. -Bbuild -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_POLICY_VERSION_MINIMUM=3.5
cmake --build build
build\hanteichan.exe
```