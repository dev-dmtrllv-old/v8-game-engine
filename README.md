# Nova 2D Game Engine

Idea:
	I want to make the whole engine in c++ but let the games be creatable in javascript/typescript while still be able to create the whole game in c/c++. This way the heavy computations can be performed natively but games can quickly be made with a higher lever language like js/ts.

Features:
- Embedded Google's V8 Javascript Engine
- Entity Component System
- Fiber Based Task system (Multi threaded)
- Use OpenGL for accelerated GPU drawing
- Move Parallel computations to Compute Shaders (for example particle systems, liquid systems)
- Custom Log system
- An editor in which the scenes can be created. Scripting is still needed for custom logic (components).
