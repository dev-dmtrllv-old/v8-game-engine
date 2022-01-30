export class TestScene extends Scene
{
	protected async load(): Promise<void>
	{
		const gameObject = new GameObject("hyaaaaa!");
		
		const spriteRenderer = gameObject.addComponent(SpriteRenderer);
		
		Engine.log(spriteRenderer);

		let sprite = AssetManager.load("../assets/sprites/test.jpg");
		
		Engine.log(sprite);
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
