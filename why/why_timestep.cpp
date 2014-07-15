#include "pch.hpp"
#include "why_timestep.hpp"

why::State why::interpolate(const why::State &previous, const why::State &current, float alpha)
{
	why::State state;
	state.x = current.x*alpha + previous.x*(1 - alpha);
	state.v = current.v*alpha + previous.v*(1 - alpha);
	return state;
}

float why::acceleration(const why::State &state, float t)
{
	const float k = 10;
	const float b = 1;
	return -k*state.x - b*state.v;
}

why::Derivative why::evaluate(const why::State &initial, float t)
{
	why::Derivative output;
	output.dx = initial.v;
	output.dv = acceleration(initial, t);
	return output;
}

why::Derivative why::evaluate(const why::State &initial, float t, float dt, const why::Derivative &d)
{
	why::State state;
	state.x = initial.x + d.dx*dt;
	state.v = initial.v + d.dv*dt;
	why::Derivative output;
	output.dx = state.v;
	output.dv = acceleration(state, t + dt);
	return output;
}

void why::integrate(why::State &state, float t, float dt)
{
	why::Derivative a = evaluate(state, t);
	why::Derivative b = evaluate(state, t, dt*0.5f, a);
	why::Derivative c = evaluate(state, t, dt*0.5f, b);
	why::Derivative d = evaluate(state, t, dt, c);

	const float dxdt = 1.0f / 6.0f * (a.dx + 2.0f*(b.dx + c.dx) + d.dx);
	const float dvdt = 1.0f / 6.0f * (a.dv + 2.0f*(b.dv + c.dv) + d.dv);

	state.x = state.x + dxdt*dt;
	state.v = state.v + dvdt*dt;
}