# Nova 2D Game Engine

Idea:
	I want to make the whole engine in c++ but let the games be creatable in javascript/typescript. This way the heavy computations can be performed natively but games can quickly be made with a higher lever language like js/ts. The game has 3 options on how it is made. The first one is only through scripting where all the game's code and assets will be presented to engine as is. The second option is to make the game through the editor. And as third the engine can be included as static or dynamic library in which the whole game is written in c++.
	I'm thinking to allow some kind of hybrid option where the components can be written in c++ and js/ts and the scenes are made through an editor.

Features:
- Embedded Google's V8 Javascript Engine
- Entity Component System
- Fiber Based Task system (Multi threaded)
- Use OpenGL for accelerated GPU drawing
- Move Parallel computations to Compute Shaders (for example particle systems, liquid systems)
- Custom Log system
- An editor in which the scenes can be created. Scripting is still needed for custom logic (components).

Todo:
- Come up with a good working parallel ECS architecture
	- How should all the components (data) be packed?
	- What is the order the systems should run in?
	- How should frames simultaneous be handled? (Copy of the game state...?)
- Expose C++ Component scripting
- Expose JS/TS Component scripting
- How should the editor and the engine work together? The editor should:
	- check which core systems are available to use.
	- compile the engine with the games configuration. (which systems the game uses, some min/max settings like tick speed, number of threads)
	- compile the game data (textures, audio, scripts) into raw data-chunks which the engine can directly use.
		- the data-chunks should be compiled in such a way that when there is an update for the game the chunk can be replaced easly.
	