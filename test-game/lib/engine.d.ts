
declare class Window
{
	protected constructor();
	public show(): void;
}

/**
 * TODO: implement on c++ side
 */
declare class Vector
{
	public constructor();
}

/**
 * TODO: implement on c++ side
 */
declare class GameObject
{
	public constructor(name?: string);
}

declare abstract class Scene
{
	public constructor();

	/**
	 * Here should all the assets that the current scene needs be loaded.
	 * You can think of all the textures, audio files, shaders, material files etc...
	 */
	protected abstract load(): void;

	/**
	 * When the scene is fully loaded the call method will be called.
	 * Here all the game objects are initialized with their coresponding components and data, so it's safe to access them.
	 */
	protected abstract start(): void;

	/**
	 * When another scene is loaded this scene's stop method will be called just before the other scene is started.
	 */
	protected abstract stop(): void;

	/**
	 * TODO: implement on c++ side
	 */
	protected spawn(name: string): GameObject;
	protected spawn(position: Vector): GameObject;
	protected spawn(name: string, position: Vector): GameObject;
}

type SceneType<T extends Scene = Scene> = new () => T;

declare var exports: { [key: string]: any; };

declare class Exception extends Error
{
	public constructor(name: string, msg: string);

	public toString();
}

declare namespace Engine
{
	class CoreException extends Exception
	{
		constructor(msg: string);
	}

	/**
	 * This holds the main game window.
	 */
	const window: Window;

	/**
	 * Call this to get an onLoadCallback.
	 * The function is used to configure the engine with it's game specific needs.
	 * @param onLoadCallback an async function to configure the engine
	 */
	const onLoad: (onLoadCallback: OnLoadCallback) => void;

	/**
	 * Log to the console
	 * @param args list of objects to log (it can be anything).
	 */
	const log: (...args: any[]) => void;

	/**
	 * Call this method when the game is configured. If not the engine will throw an Engine.CoreException.
	 * @param sceneName The name of the first scene to load.
	 */
	const start: (sceneName: string) => void;

	/**
	 * A method to retreive the current loaded (active) scene.
	 * @returns Scene or null if no scene is loaded.
	 */
	const getActiveScene: () => Scene | null;
}

type OnLoadCallback = (configure: EngineConfigureFunction) => void;

type EngineConfigureFunction = (config: EngineConfiguration) => Promise<void>;

type EngineConfiguration = {
	name: string;
	window?: WindowConfig;
	jobSystem?: JobSystemConfig;
	graphics?: GraphicsConfig;
	scenes?: {
		[name: string]: SceneType
	};
};

type Color = [number, number, number, number];

type GraphicsConfig = {
	clearColor?: Color;
};

type JobSystemConfig = {
	/**
	 * Total number of jobs to be executed.
	*/
	maxJobs?: number;
	/** 
	 * Total number of threads to execute the jobs. 
	 * */
	executionThreads?: number;
};

type WindowConfig = {
	/**
	 * The minimum with of the window.
	 */
	minWidth?: number;
	/**
	 * The minimum height of the window.
	 */
	minHeight?: number;
	/**
	 * The maximum with of the window.
	 */
	maxWidth?: number;
	/**
	 * The maximum height of the window.
	 */
	maxHeight?: number;
	/**
	 * The initial width of the window.
	 */
	width?: number;
	/**
	 * The initial height of the window.
	 */
	height?: number;
	/**
	 * Should the window be resizable?
	 */
	resizable?: boolean;
	/**
	 * Should the window start maximized?
	 */
	maximized?: boolean;
	/**
	 * Should the window start in fullscreen mode?
	 */
	fullscreen?: boolean;
	/**
	 * Should the window initially be hidden? 
	 */
	hidden?: boolean;
};
