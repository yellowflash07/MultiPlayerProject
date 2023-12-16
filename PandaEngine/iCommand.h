#include <string>

class iCommand
{
public:
	virtual ~iCommand() {};
	virtual bool Execute(double deltaTime) = 0;
	virtual int UniqueId() = 0;
	virtual void SetUniqueId(int id) = 0;
	//virtual std::string Name() = 0;
	virtual void SetName(std::string name) = 0;
};