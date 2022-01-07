export class TestScene extends Scene
{
	public constructor()
	{
		super();
		Engine.log("Test scene initialized!");
	}

	protected load(): void
	{
		Engine.log("Test Scene load function called!");
	}

	protected start(): void
	{
		throw new Error("Method not implemented.");
	}

	protected stop(): void
	{
		throw new Error("Method not implemented.");
	}
}
