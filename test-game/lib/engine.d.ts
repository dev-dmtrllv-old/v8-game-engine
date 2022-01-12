
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

	protected abstract load(): void;
	protected abstract start(): void;
	protected abstract stop(): void;

	/**
	 * TODO: implement on c++ side
	 */
	protected spawn(name: string): GameObject;
	protected spawn(position: Vector): GameObject;
	protected spawn(name: string, position: Vector): GameObject;
}

type SceneType<T extends Scene = Scene> = new (...args: any) => T;

declare var exports: any;

declare namespace Engine
{
	const window: Window;
	const onLoad: <T extends EngineConfiguration>(callback: OnLoadCallback<T>) => void;
	const log: (...args: any[]) => void;
	const start: (sceneName: string) => void;
	const getActiveScene: () => Scene | null;
}

type OnLoadCallback<T extends EngineConfiguration> = (configure: EngineConfigureFunction<T>) => void;

type EngineConfigureFunction<T extends EngineConfiguration> = (config: T) => Promise<void>;

type EngineConfiguration = {
	name: string;
	window?: WindowConfig;
	jobSystem?: JobSystemConfig;
	scenes?: {
		[name: string]: SceneType
	};

};

type Color = [number, number, number, number];

type GraphicsConfig = {
	clearColor?: Color;
};

type JobSystemConfig = {
	/** Total number of jobs to be executed */
	maxJobs?: number;
	/** Total number of threads to execture the jobs. */
	executionThreads?: number;
};

type WindowConfig = {
	minWidth?: number;
	minHeight?: number;
	maxWidth?: number;
	maxHeight?: number;
	width?: number;
	height?: number;
	resizable?: boolean;
	maximized?: boolean;
	fullscreen?: boolean;
	hidden?: boolean;
	graphics?: GraphicsConfig;
};
