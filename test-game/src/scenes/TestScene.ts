export class TestScene extends Scene
{
	protected load(): void
	{
		const gameObject = this.spawn("hyaaaaa!");
		Engine.log("spawned gameobject with name ", gameObject.name);
		const o2 = new GameObject("test woop");
		Engine.log("spawned gameobject with name ", o2.name);
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
