#include "pch.hpp"
#include "why_program.hpp"
#include "why_app.hpp"

int why::Program::main ( const std::vector<std::string> &args )
{
	using namespace clan;
	using namespace std;
	try
	{
		SetupCore setup_core;

		SetupDisplay setup_display;

		SetupD3D setup_d3d;
		SetupGL setup_gl;

		SetupSound setup_sound;

		why::Application app;
		const int retval = app.start(args);
		return retval;
	}
	catch (const std::exception &e)
	{
		WHY_LOG() << "Fatal error: " << e.what();
		MessageBox(NULL, string(string("Fatal error: ") + e.what()).c_str(), "Fatal Error", MB_OK);
	}
	return -1;
}

clan::Application app ( &why::Program::main );