export class TestScene extends Scene
{
	protected load(): void
	{
		const gameObject = new GameObject("hyaaaaa!");
		const t = gameObject.getComponent(Transform);
		Engine.log(t);
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
