export class TestScene extends Scene
{
	protected load(): void
	{
		this.spawn("hyaaaaa!");
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
