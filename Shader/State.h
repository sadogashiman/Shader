#pragma once
class State
{
public:
	virtual ~State() = default;

protected:
	State* state_;
public:
	virtual bool init() = 0;
	virtual State* update() = 0;
	virtual bool render() = 0;
	virtual void destroy() = 0;
};

