# Nova 2D Game Engine

Idea:
	I want to make the whole engine in c++ but let the games be creatable in javascript/typescript. This way the heavy computations can be performed natively but games can quickly be made with a higher lever language like js/ts.

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
