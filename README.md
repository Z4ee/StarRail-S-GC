# Simple cheat for Honkai Star Rail
This repository provides code for a simple cheat for the game Honkai Star Rail. For informational purposes only. Use at your own risk.

# Instruction:
What you need to compile:
- minhook

After compilation, you will receive a dll that you need to inject into the game immediately after opening

You can use Extreme Injector (auto inject) to inject the dll into the game at startup

Now you can use speedhack

# Functional:
- speedhack (hotkey: CAPSLOCK) (works everywhere)
- peeking (hotkey: F5)
- fps unlock (hotkey: F6)

![image](https://user-images.githubusercontent.com/113752393/235335594-638626bf-b598-4c60-ab05-852968dfe0a3.png)

if it does not work for the Chinese version of the game, then create github issue

# cheat engine:
BATTLE SPEED:

address: [["GameAssembly.dll" + 0x8CAA6A0]+ 0xC0] + 0x1DC

type: float

GLOBAL SPEED: (thx @Gktwo)

address: [unityplayer.dll + 0x1D21D78] + 0xFC

type: float
