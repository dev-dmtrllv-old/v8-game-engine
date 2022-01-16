export class TestScene extends Scene
{
	protected load(): void
	{
		const gameObject = this.spawn("hyaaaaa!");
		Engine.log("spawned gameobject with name ", gameObject.name);
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
