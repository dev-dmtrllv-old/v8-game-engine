#pragma once

template<typename ...Args>
class Initializable;

template<>
class Initializable<>
{
private:
	bool isInitialized_ = false;

protected:
	virtual bool onInitialize() = 0;

public:
	bool initialize()
	{
		if (!isInitialized_)
		{
			if(!onInitialize())
				return false;

			isInitialized_ = true;
		}
		return true;
	}

	bool isInitialized() { return this->isInitialized_; }
};


template<typename... Args>
class Initializable
{
	private:
		bool isInitialized_ = false;

	protected:
		virtual bool onInitialize(Args ...args) = 0;

	public:
		bool initialize(Args ...args)
		{
			if(!isInitialized_)
			{
				if(!onInitialize(args...))
					return false;
					
				isInitialized_ = true;
			}
			return true;
		}

		bool isInitialized() { return this->isInitialized_; }
};
