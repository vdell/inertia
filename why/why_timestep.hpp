// Copyright (c) Glenn Fiedler 2004

#pragma once

namespace why
{
	struct State
	{
		float x;
		float v;
	};

	struct Derivative
	{
		float dx;
		float dv;
	};

	State interpolate(const State &previous, const State &current, float alpha);

	float acceleration(const State &state, float t);

	Derivative evaluate(const State &initial, float t);

	Derivative evaluate(const State &initial, float t, float dt, const Derivative &d);

	void integrate(State &state, float t, float dt);
}