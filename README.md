# Hantei-kun #

Hantei-kun is a .HA6 moveset editor, and is forked off from Hantei-chan as the amount of versions from different people keep piling up. This fork focuses on editing Under Night In-Birth, adding much more clarity and pop-ups until source code of better forks (for things like tabs and less crashes) is released.

### WHY IS THERE CODE??? MAKE A GRDDING .EXE FILE AND GIVE IT TO ME

The tool is always going to be [bundled on this repo](https://github.com/Fatih120/undernightinbirth/tree/master/tools) instead of having releases here.

# Important and Changes

This fork and the last has problems with using the Copy Pattern/Paste Pattern features in which saving might cause the program to crash, or, worse, straight-up 0-byte your HA6 file. Please make backups frequently when using this fork until the exact problem is found and fixed - otherwise, the program is stable for typical editing.

Hantei-kun is a slow-burn, spine-tingling, cheap, vibe coded, jumpscare-free, bone-chilling, genre-redefining blah blah, and contains a few QoL changes.

### More Hotkeys

This fork adds plenty of new hotkeys, many of which are related to Box manipulation. You can move and resize boxes using hotkeys, found in About->Shortcuts.

### Mass Resizing, but good

You can now mass resize sprites only without resizing boxes, and do this only per-pattern. Also, you can use a *set* value instead of having to *multiply* what's currently present, so this will make character creation much easier.

### Better Frame Editing

Hantei-kun adds more human-readable Effects dropdowns for you to use, cleans up some useless fields, and has a few additional tools for copying and pasting individual layers, offsets, and range tools are consolidated into the main view. Scroll zooming can be done for now by scrolling over the box pane at the bottom.

# How to build

Building is DEBATABLY simple, and it works with both MSVC and Mingw-w64.
Clone the git, init the submodules and run cmake.
Assuming you're using ninja:

```
git clone --recursive https://github.com/Fatih120/Hantei-chan.git hanteichan 
cd hanteichan
mkdir build
cmake -S. -Bbuild -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_POLICY_VERSION_MINIMUM=3.5
cmake --build build
```

exe obtained