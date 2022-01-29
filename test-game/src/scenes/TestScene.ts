export class TestScene extends Scene
{
	protected load(): void
	{
		const gameObject = new GameObject("hyaaaaa!");
		// const transform = gameObject.getComponent(Transform);
		// Engine.log(transform.position);
		// Engine.log(transform.scale);
		// Engine.log(transform.rotation);
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
