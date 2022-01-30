export class TestScene extends Scene
{
	protected async load(): Promise<void>
	{
		const gameObject = new GameObject("hyaaaaa!");
		// const spriteRenderer = gameObject.addComponent(SpriteRenderer);
		let sprite = AssetManager.load("../assets/sprites/test.jpg");
		Engine.log(sprite);
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
