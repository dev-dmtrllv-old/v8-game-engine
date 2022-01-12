import { TestScene } from "scenes/TestScene";

Engine.onLoad(async (configure) => 
{
	Engine.log("Configuring engine...");

	await configure({
		name: "Nova Test Game",
		window: {
			minWidth: 640,
			minHeight: 480,
			resizable: true,
			maximized: true,
			width: 1920,
			height: 1080,
			hidden: true,
			graphics: {
				clearColor: [1, 0.2, 0.2, 1]
			},
		},
		jobSystem: {
			executionThreads: 3,
			maxJobs: 1000
		},
		scenes: {
			"test scene":  TestScene
		}
	});

	Engine.log("Engine configured!");

	Engine.window.show();

	Engine.start("test scene");
});

