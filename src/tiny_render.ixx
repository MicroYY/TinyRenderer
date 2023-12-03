module;

#include <string>

export module renderer;

export
class TinyRender
{
public:
	bool Init(std::string& model);
	bool Render();
};

bool TinyRender::Init(std::string& model)
{
	return 0;
}

bool TinyRender::Render()
{
	return 0;
}